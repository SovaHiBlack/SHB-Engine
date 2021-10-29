#include "stdafx.h"

#include "..\..\PHShellHolder.h"
#include "TelekineticObject.h"
#include "../../PhysicsShell.h"
#include "../../PHInterpolation.h"
#include "../../PHElement.h"
#include "..\..\Level.h"
#include "../../GameObject.h"

#define KEEP_IMPULSE_UPDATE 200
#define FIRE_TIME			3000
#define RAISE_MAX_TIME		5000

CTelekineticObject::CTelekineticObject( )
{
	state						= TS_None;
	object						= nullptr;
	telekinesis					= nullptr;
	m_rotate					= false;
}

CTelekineticObject::~CTelekineticObject( )
{ }

bool CTelekineticObject::init(CTelekinesis* tele, CPHShellHolder* obj, float s, float h, u32 ttk, bool rot)
{
	if (!can_activate(obj))
	{
		return false;
	}

	switch_state(TS_Raise);
	object						= obj;

	target_height				= obj->Position( ).y + h;

	time_keep_started			= 0;
	time_keep_updated			= 0;
	time_to_keep				= ttk;

	strength					= s;

	time_fire_started = 0;

	m_rotate = rot;

	if (object->m_pPhysicsShell)
	{
		object->m_pPhysicsShell->set_ApplyByGravity(FALSE);
	}

	return true;
}

void CTelekineticObject::set_sound(const ref_sound& snd_hold, const ref_sound& snd_throw)
{
	sound_hold.clone(snd_hold, st_Effect, sg_SourceType);
	sound_throw.clone(snd_throw, st_Effect, sg_SourceType);
}

void CTelekineticObject::raise_update( )
{
	if (check_height( ) || check_raise_time_out( ))
	{	// начать удержание предмета
		prepare_keep( );
	}
	else
	{
		if (m_rotate)
		{
			rotate( );
		}
	}
}

void CTelekineticObject::keep_update( )
{
	if (time_keep_elapsed( ))
	{
		release( );
	}
}

void CTelekineticObject::fire_update( )
{
	if (time_fire_elapsed( ))
	{
		release( );
	}
}

void CTelekineticObject::update_state( )
{
	switch (get_state( ))
	{
		case TS_Raise:
		{
			raise_update( );
		}
		break;
		case TS_Keep:
		{
			keep_update( );
		}
		break;
		case TS_Fire:
		{
			fire_update( );
		}
		break;
		case TS_None:
		{
		}
		break;
	}
}

void CTelekineticObject::switch_state(ETelekineticState new_state)
{
	unsigned int time			= Device.dwTimeGlobal;

	switch (new_state)
	{
		case TS_Raise:
		{
			time_raise_started = time;
		}
		break;
		case TS_Keep:
		{
			time_keep_started = time;
		}
		break;
		case TS_Fire:
		{
			time_fire_started = time;
		}
		break;
		case TS_None:
		{
		}
		break;
	}

	state = new_state;
}

void CTelekineticObject::raise(float step)
{
	if (!object || !object->m_pPhysicsShell || !object->m_pPhysicsShell->isActive( ))
	{
		return;
	}

	step *= strength;

	Fvector3 dir;
	dir.set(0.0f, 1.0f, 0.0f);

	float elem_size = float(object->m_pPhysicsShell->Elements( ).size( ));
	dir.mul(elem_size * elem_size * strength);

	if (OnServer( ))
	{
		(object->m_pPhysicsShell->Elements( )[0])->applyGravityAccel(dir);
	}

	update_hold_sound( );
}

void CTelekineticObject::prepare_keep( )
{
	switch_state(TS_Keep);
	time_keep_updated = 0;
}

bool CTelekineticObject::time_keep_elapsed( )
{
	if (time_keep_started + time_to_keep < Device.dwTimeGlobal)
	{
		return true;
	}

	return false;
}

bool CTelekineticObject::time_fire_elapsed( )
{
	if (time_fire_started + FIRE_TIME < Device.dwTimeGlobal)
	{
		return true;
	}

	return false;
}

void CTelekineticObject::keep( )
{
	if (!object || !object->m_pPhysicsShell || !object->m_pPhysicsShell->isActive( ))
	{
		return;
	}

	// проверить высоту
	float cur_h = object->Position( ).y;

	// установить dir в соответствие с текущей высотой
	Fvector3 dir;
	if (cur_h > target_height + 0.6f)
	{
		dir.set(0.0f, -1.0f, 0.0f);
	}
	else if (cur_h < target_height + 0.6f)
	{
		dir.set(0.0f, 1.0f, 0.0f);
	}
	else
	{
		dir.set(Random.randF(-1.0f, 1.0f), Random.randF(-1.0f, 1.0f), Random.randF(-1.0f, 1.0f));
		dir.normalize_safe( );
	}

	dir.mul(5.0f);

	if (OnServer( ))
	{
		(object->m_pPhysicsShell->Elements( )[0])->applyGravityAccel(dir);
	}

	// установить время последнего обновления
	time_keep_updated = Device.dwTimeGlobal;

	update_hold_sound( );
}

void CTelekineticObject::release( )
{
	if (!object || !object->m_pPhysicsShell || !object->m_pPhysicsShell->isActive( ))
	{
		return;
	}

	Fvector3 dir_inv;
	dir_inv.set(0.0f, -1.0f, 0.0f);

	// включить гравитацию
	object->m_pPhysicsShell->set_ApplyByGravity(TRUE);
	if (OnServer( ))
	{	// приложить небольшую силу для того, чтобы объект начал падать
		object->m_pPhysicsShell->applyImpulse(dir_inv, 0.5f * object->m_pPhysicsShell->getMass( ));
	}

	switch_state(TS_None);
}

void CTelekineticObject::fire_t(const Fvector3& target, float time)
{
	switch_state(TS_Fire);

	if (!object || !object->m_pPhysicsShell || !object->m_pPhysicsShell->isActive( ))
	{
		return;
	}

	// включить гравитацию
	object->m_pPhysicsShell->set_ApplyByGravity(TRUE);

	Fvector3 transference;
	transference.sub(target, object->Position( ));
	TransferenceToThrowVel(transference, time, object->EffectiveGravity( ));
	object->m_pPhysicsShell->set_LinearVel(transference);

	if (sound_throw._handle( ))
	{
		sound_throw.play_at_pos(object, object->Position( ));
	}

	if (sound_hold._handle( ) && sound_hold._feedback( ))
	{
		sound_hold.stop( );
	}
}

void CTelekineticObject::fire(const Fvector3& target, float power)
{
	switch_state(TS_Fire);

	if (!object || !object->m_pPhysicsShell || !object->m_pPhysicsShell->isActive( ))
	{
		return;
	}

	// вычислить направление
	Fvector3 dir;
	dir.sub(target, object->Position( ));
	dir.normalize( );

	// включить гравитацию
	object->m_pPhysicsShell->set_ApplyByGravity(TRUE);

	if (OnServer( ))
	{	// выполнить бросок
		for (u32 i = 0; i < object->m_pPhysicsShell->Elements( ).size( ); i++)
		{
			object->m_pPhysicsShell->Elements( )[i]->applyImpulse(dir, power * 20.0f * object->m_pPhysicsShell->getMass( ) / object->m_pPhysicsShell->Elements( ).size( ));
		}
	}
}

bool CTelekineticObject::check_height( )
{
	if (!object)
	{
		return true;
	}

	return (object->Position( ).y > target_height);
}

bool CTelekineticObject::check_raise_time_out( )
{
	if (time_raise_started + RAISE_MAX_TIME < Device.dwTimeGlobal)
	{
		return true;
	}

	return false;
}

void CTelekineticObject::enable( )
{
	if (object->m_pPhysicsShell)
	{
		object->m_pPhysicsShell->Enable( );
	}
}

void CTelekineticObject::rotate( )
{
	if (!object || !object->m_pPhysicsShell || !object->m_pPhysicsShell->isActive( ))
	{
		return;
	}

	// вычислить направление
	Fvector3 dir;
	dir.random_dir( );
	dir.normalize( );

	if (OnServer( ))
	{
		object->m_pPhysicsShell->applyImpulse(dir, 2.5f * object->m_pPhysicsShell->getMass( ));
	}
}

bool CTelekineticObject::can_activate(CPHShellHolder* obj)
{
	return (obj && obj->m_pPhysicsShell);
}

void CTelekineticObject::update_hold_sound( )
{
	if (!sound_hold._handle( ))
	{
		return;
	}

	if (sound_hold._feedback( ))
	{
		sound_hold.set_position(object->Position( ));
	}
	else
	{
		sound_hold.play_at_pos(object, object->Position( ));
	}
}

#include "stdafx.h"

#include "PhysicsShellHolder.h"
#include "PhysicsShell.h"
#include "xrMessages.h"
#include "ph_shell_interface.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "script_callback_ex.h"
#include "Level.h"
#include "PHCommander.h"
#include "PHScriptCall.h"
#include "CustomRocket.h"
#include "Grenade.h"
#include "phworld.h"
#include "PHActivationShape.h"
#include "PHValide.h"

CPhysicsShellHolder::CPhysicsShellHolder( )
{
	init( );
}

void CPhysicsShellHolder::net_Destroy( )
{
	//remove calls
	CPHSriptReqGObjComparer cmpr(this);
	Level( ).ph_commander_scripts( ).remove_calls(&cmpr);
	//������� �������� �� ParticlePlayer
	CParticlesPlayer::net_DestroyParticles( );
	inherited::net_Destroy( );
	b_sheduled = false;
	deactivate_physics_shell( );
	xr_delete(m_pPhysicsShell);
}

static enum EEnableState
{
	stEnable = 0,
	stDisable,
	stNotDefitnite
};
static u8 st_enable_state = (u8)stNotDefitnite;
BOOL CPhysicsShellHolder::net_Spawn(CSE_Abstract* DC)
{
	CParticlesPlayer::net_SpawnParticles( );
	st_enable_state = (u8)stNotDefitnite;
	b_sheduled = true;
	BOOL ret = inherited::net_Spawn(DC);//load
		//create_physic_shell			();
	if (PPhysicsShell( ) && PPhysicsShell( )->isFullActive( ))
	{
		PPhysicsShell( )->GetGlobalTransformDynamic(&XFORM( ));
		switch (EEnableState(st_enable_state))
		{
			case stEnable:	PPhysicsShell( )->Enable( ); break;
			case stDisable:	PPhysicsShell( )->Disable( ); break;
			case stNotDefitnite:; break;
		}
		ApplySpawnIniToPhysicShell(pSettings, PPhysicsShell( ), false);
		st_enable_state = (u8)stNotDefitnite;
	}
	return ret;
}

void	CPhysicsShellHolder::PHHit(f32 P, fVector3& dir, CObject* who, s16 element, fVector3 p_in_object_space, f32 impulse, ALife::EHitType hit_type /* ALife::eHitTypeWound*/)
{
	if (impulse > 0)
		if (m_pPhysicsShell) m_pPhysicsShell->applyHit(p_in_object_space, dir, impulse, element, hit_type);
}

void	CPhysicsShellHolder::Hit(SHit* pHDS)
{
	PHHit(pHDS->damage( ), pHDS->dir, pHDS->who, pHDS->boneID, pHDS->p_in_bone_space, pHDS->impulse, pHDS->hit_type);
}

void CPhysicsShellHolder::create_physic_shell( )
{
	VERIFY(!m_pPhysicsShell);
	IPhysicShellCreator* shell_creator = smart_cast<IPhysicShellCreator*>(this);
	if (shell_creator)
		shell_creator->CreatePhysicsShell( );
}

void CPhysicsShellHolder::init( )
{
	m_pPhysicsShell = NULL;
	b_sheduled = false;
}
void CPhysicsShellHolder::correct_spawn_pos( )
{
	VERIFY(PPhysicsShell( ));

	fVector3								size;
	fVector3								c;
	get_box(PPhysicsShell( ), XFORM( ), size, c);

	CPHActivationShape					activation_shape;
	activation_shape.Create(c, size, this);
	activation_shape.set_rotation(XFORM( ));
	PPhysicsShell( )->DisableCollision( );
	activation_shape.Activate(size, 1, 1.f, M_PI / 8.f);
////	VERIFY								(valid_pos(activation_shape.Position(),phBoundaries));
//	if (!valid_pos(activation_shape.Position(),phBoundaries)) {
//		CPHActivationShape				activation_shape;
//		activation_shape.Create			(c,size,this);
//		activation_shape.set_rotation	(XFORM());
//		activation_shape.Activate		(size,1,1.f,M_PI/8.f);
////		VERIFY							(valid_pos(activation_shape.Position(),phBoundaries));
//	}

	PPhysicsShell( )->EnableCollision( );

	fVector3								ap = activation_shape.Position( );

#ifdef DEBUG
	if (!valid_pos(ap, phBoundaries))
	{
		Msg("not valid position	%f,%f,%f", ap.x, ap.y, ap.z);
		Msg("size	%f,%f,%f", size.x, size.y, size.z);
		Msg("Object: %s", Name( ));
		Msg("Visual: %s", *(cNameVisual( )));
		Msg("Object	pos	%f,%f,%f", Position( ).x, Position( ).y, Position( ).z);
	}
#endif // DEBUG

	VERIFY(valid_pos(activation_shape.Position( ), phBoundaries));

	fMatrix4x4								trans;
	trans.identity( );
	trans.c.sub(ap, c);
	PPhysicsShell( )->TransformPosition(trans);
	PPhysicsShell( )->GetGlobalTransformDynamic(&XFORM( ));
	activation_shape.Destroy( );
}

void CPhysicsShellHolder::activate_physic_shell( )
{
	VERIFY(!m_pPhysicsShell);
	create_physic_shell( );
	fVector3					l_fw;
	fVector3					l_up;
	l_fw.set(XFORM( ).k);
	l_up.set(XFORM( ).j);
	l_fw.mul(2.f);
	l_up.mul(2.f);

	fMatrix4x4					l_p1;
	fMatrix4x4					l_p2;
	l_p1.set(XFORM( ));
	l_p2.set(XFORM( ));
	l_fw.mul(2.f);
	l_p2.c.add(l_fw);

	m_pPhysicsShell->Activate(l_p1, 0, l_p2);
	if (H_Parent( ) && H_Parent( )->Visual( ))
	{
		smart_cast<CKinematics*>(H_Parent( )->Visual( ))->CalculateBones_Invalidate( );
		smart_cast<CKinematics*>(H_Parent( )->Visual( ))->CalculateBones( );
	}
	smart_cast<CKinematics*>(Visual( ))->CalculateBones_Invalidate( );
	smart_cast<CKinematics*>(Visual( ))->CalculateBones( );
//	XFORM().set					(l_p1);
	correct_spawn_pos( );

	m_pPhysicsShell->set_LinearVel(l_fw);
	m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM( ));
}

void CPhysicsShellHolder::setup_physic_shell( )
{
	VERIFY(!m_pPhysicsShell);
	create_physic_shell( );
	m_pPhysicsShell->Activate(XFORM( ), 0, XFORM( ));
	smart_cast<CKinematics*>(Visual( ))->CalculateBones_Invalidate( );
	smart_cast<CKinematics*>(Visual( ))->CalculateBones( );
	m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM( ));
}

void CPhysicsShellHolder::deactivate_physics_shell( )
{
	if (m_pPhysicsShell)
		m_pPhysicsShell->Deactivate( );
	xr_delete(m_pPhysicsShell);
}
void CPhysicsShellHolder::PHSetMaterial(u16 m)
{
	if (m_pPhysicsShell)
		m_pPhysicsShell->SetMaterial(m);
}

void CPhysicsShellHolder::PHSetMaterial(pcstr m)
{
	if (m_pPhysicsShell)
		m_pPhysicsShell->SetMaterial(m);
}

void CPhysicsShellHolder::PHGetLinearVell(fVector3& velocity)
{
	if (!m_pPhysicsShell)
	{
		velocity.set(0.0f, 0.0f, 0.0f);
		return;
	}

	m_pPhysicsShell->get_LinearVel(velocity);
}

void CPhysicsShellHolder::PHSetLinearVell(fVector3& velocity)
{
	if (!m_pPhysicsShell)
	{
		return;
	}

	m_pPhysicsShell->set_LinearVel(velocity);
}

f32 CPhysicsShellHolder::GetMass( )
{
	return m_pPhysicsShell ? m_pPhysicsShell->getMass( ) : 0.0f;
}

u16	CPhysicsShellHolder::PHGetSyncItemsNumber( )
{
	if (m_pPhysicsShell)	return m_pPhysicsShell->get_ElementsNumber( );
	else				return 0;
}

CPHSynchronize* CPhysicsShellHolder::PHGetSyncItem(u16 item)
{
	if (m_pPhysicsShell) return m_pPhysicsShell->get_ElementSync(item);
	else				return 0;
}
void	CPhysicsShellHolder::PHUnFreeze( )
{
	if (m_pPhysicsShell) m_pPhysicsShell->UnFreeze( );
}

void	CPhysicsShellHolder::PHFreeze( )
{
	if (m_pPhysicsShell) m_pPhysicsShell->Freeze( );
}

void CPhysicsShellHolder::OnChangeVisual( )
{
	inherited::OnChangeVisual( );
	if (0 == renderable.visual)
	{
		if (m_pPhysicsShell)m_pPhysicsShell->Deactivate( );
		xr_delete(m_pPhysicsShell);
		VERIFY(0 == m_pPhysicsShell);
	}
}

void CPhysicsShellHolder::UpdateCL( )
{
	inherited::UpdateCL( );
	//�������� �������������� ��������
	UpdateParticles( );
}
f32 CPhysicsShellHolder::EffectiveGravity( )
{
	return ph_world->Gravity( );
}

void		CPhysicsShellHolder::save(CNetPacket& output_packet)
{
	inherited::save(output_packet);
	u8 enable_state = (u8)stNotDefitnite;
	if (PPhysicsShell( ) && PPhysicsShell( )->isActive( ))
	{
		enable_state = u8(PPhysicsShell( )->isEnabled( ) ? stEnable : stDisable);
	}
	output_packet.w_u8(enable_state);
}

void		CPhysicsShellHolder::load(IReader& input_packet)
{
	inherited::load(input_packet);
	st_enable_state = input_packet.r_u8( );
}

void CPhysicsShellHolder::PHSaveState(CNetPacket& P)
{
	//CPhysicsShell* pPhysicsShell=PPhysicsShell();
	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
	//flags8 lflags;
	//if(pPhysicsShell&&pPhysicsShell->isActive())			lflags.set(CSE_PHSkeleton::flActive,pPhysicsShell->isEnabled());

//	P.w_u8 (lflags.get());
	if (K)
	{
		P.w_u64(K->LL_GetBonesVisible( ));
		P.w_u16(K->LL_GetBoneRoot( ));
	}
	else
	{
		P.w_u64(u64(-1));
		P.w_u16(0);
	}
	/////////////////////////////
	fVector3 min;
	fVector3 max;

	min.set(flt_max, flt_max, flt_max);
	max.set(-flt_max, -flt_max, -flt_max);
	/////////////////////////////////////

	u16 bones_number = PHGetSyncItemsNumber( );
	for (u16 i = 0; i < bones_number; i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
		fVector3& p = state.position;
		if (p.x < min.x)min.x = p.x;
		if (p.y < min.y)min.y = p.y;
		if (p.z < min.z)min.z = p.z;

		if (p.x > max.x)max.x = p.x;
		if (p.y > max.y)max.y = p.y;
		if (p.z > max.z)max.z = p.z;
	}

	min.sub(2.f * EPSILON_3);
	max.add(2.f * EPSILON_3);

	VERIFY(!min.similar(max));
	P.w_vec3(min);
	P.w_vec3(max);

	P.w_u16(bones_number);

	for (u16 i = 0; i < bones_number; i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
		state.net_Save(P, min, max);
	}
}
void
CPhysicsShellHolder::PHLoadState(IReader& P)
{

//	flags8 lflags;
	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
//	P.r_u8 (lflags.flags);
	if (K)
	{
		K->LL_SetBonesVisible(P.r_u64( ));
		K->LL_SetBoneRoot(P.r_u16( ));
	}

	fVector3 min = P.r_vec3( );
	fVector3 max = P.r_vec3( );

	VERIFY(!min.similar(max));

	u16 bones_number = P.r_u16( );
	for (u16 i = 0; i < bones_number; i++)
	{
		SPHNetState state;
		state.net_Load(P, min, max);
		PHGetSyncItem(i)->set_State(state);
	}
}

bool CPhysicsShellHolder::register_schedule( ) const
{
	return					(b_sheduled);
}

void CPhysicsShellHolder::on_physics_disable( )
{
	return;
}

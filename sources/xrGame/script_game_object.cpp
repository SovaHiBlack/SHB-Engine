////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "script_entity_action.h"
#include "AISpace.h"
#include "script_engine.h"
#include "script_entity.h"
#include "physicsshellholder.h"
#include "helicopter.h"
#include "HolderCustom.h"
#include "InventoryOwner.h"
#include "movement_manager.h"
//#include "EntityAlive.h"
#include "WeaponMagazined.h"
#include "xrmessages.h"
#include "Inventory.h"
#include "script_ini_file.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "HangingLamp.h"
#include "patrol_path_manager.h"
#include "ai_object_location.h"
#include "custommonster.h"
#include "EntityCondition.h"
#include "SpaceRestrictor.h"
#include "detail_path_manager.h"
#include "level_graph.h"
#include "Actor.h"
#include "ActorMemory.h"
#include "visual_memory_manager.h"

class CScriptBinderObject;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
fVector3 CScriptGameObject::Center( )
{
	fVector3 c;
	m_game_object->Center(c);
	return c;
}

BIND_FUNCTION10	(&object(),	CScriptGameObject::Position,			CGameObject,	Position, fVector3, fVector3());
BIND_FUNCTION10	(&object(),	CScriptGameObject::Direction,			CGameObject,	Direction, fVector3, fVector3());
BIND_FUNCTION10	(&object(),	CScriptGameObject::Mass,		CPhysicsShellHolder,	GetMass, f32, f32(-1));
BIND_FUNCTION10	(&object(),	CScriptGameObject::ID,					CGameObject,	ID,					u32,							u32(-1));
BIND_FUNCTION10	(&object(),	CScriptGameObject::getVisible,			CGameObject,	getVisible,			BOOL,							FALSE);
//BIND_FUNCTION01	(&object(),	CScriptGameObject::setVisible,			CGameObject,	setVisible,			BOOL,							BOOL);
BIND_FUNCTION10	(&object(),	CScriptGameObject::getEnabled,			CGameObject,	getEnabled,			BOOL,							FALSE);
//BIND_FUNCTION01	(&object(),	CScriptGameObject::setEnabled,			CGameObject,	setEnabled,			BOOL,							BOOL);
BIND_FUNCTION10	(&object(),	CScriptGameObject::story_id,			CGameObject,	story_id,			ALife::_STORY_ID,				ALife::_STORY_ID(-1));
BIND_FUNCTION10	(&object(),	CScriptGameObject::DeathTime,			CEntity,		GetLevelDeathTime,	u32,							0);
BIND_FUNCTION10	(&object(),	CScriptGameObject::MaxHealth,			CEntity,		GetMaxHealth, f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Accuracy,			CInventoryOwner,GetWeaponAccuracy, f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Team,				CEntity,		g_Team,				int,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Squad,				CEntity,		g_Squad,			int,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::Group,				CEntity,		g_Group,			int,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetFOV,				CEntityAlive,	ffGetFov, f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetRange,			CEntityAlive,	ffGetRange, f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetHealth,			CEntityAlive,	conditions().GetHealth, f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetPsyHealth,		CEntityAlive,	conditions().GetPsyHealth, f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetPower,			CEntityAlive,	conditions().GetPower, f32,							-1);
//BIND_FUNCTION10	(&object(),	CScriptGameObject::GetSatiety,			CEntityAlive,	conditions().GetSatiety,			f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetRadiation,		CEntityAlive,	conditions().GetRadiation, f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetBleeding,			CEntityAlive,	conditions().BleedingSpeed, f32,							-1);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetMorale,			CEntityAlive,	conditions().GetEntityMorale, f32,							-1);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetHealth,			CEntityAlive,	conditions().ChangeHealth, f32, f32);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetPsyHealth,		CEntityAlive,	conditions().ChangePsyHealth, f32, f32);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetPower,			CEntityAlive,	conditions().ChangePower, f32, f32);
//BIND_FUNCTION01	(&object(),	CScriptGameObject::SetSatiety,			CEntityAlive,	conditions().ChangeSatiety,		f32,							f32);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetRadiation,		CEntityAlive,	conditions().ChangeRadiation, f32, f32);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetCircumspection,	CEntityAlive,	conditions().ChangeCircumspection, f32, f32);
BIND_FUNCTION01	(&object(),	CScriptGameObject::SetMorale,			CEntityAlive,	conditions().ChangeEntityMorale, f32, f32);
BIND_FUNCTION02	(&object(),	CScriptGameObject::SetScriptControl,	CScriptEntity,	SetScriptControl,	bool, pcstr,					bool,					shared_str);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetScriptControl,	CScriptEntity,	GetScriptControl,	bool,								false);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetScriptControlName,CScriptEntity,GetScriptControlName, pcstr,					"");
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetEnemyStrength,	CScriptEntity,	get_enemy_strength,	int,					0);
BIND_FUNCTION10	(&object(),	CScriptGameObject::GetActionCount,		CScriptEntity,	GetActionCount,		u32,					0);
BIND_FUNCTION10	(&object(),	CScriptGameObject::can_script_capture,	CScriptEntity,	can_script_capture,	bool,					0);

u32 CScriptGameObject::level_vertex_id( ) const
{
	return object( ).ai_location( ).level_vertex_id( );
}

u32 CScriptGameObject::game_vertex_id( ) const
{
	return object( ).ai_location( ).game_vertex_id( );
}

f32 CScriptGameObject::level_vertex_light(const u32& level_vertex_id) const
{
	return						((f32)ai( ).level_graph( ).vertex(level_vertex_id)->light( ) / 15.0f);
}

CScriptIniFile* CScriptGameObject::spawn_ini( ) const
{
	return (CScriptIniFile*)object( ).spawn_ini( );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::ResetActionQueue()
{
	CScriptEntity		*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member ResetActionQueue!");
	else
		l_tpScriptMonster->ClearActionQueue();
}

CScriptEntityAction	*CScriptGameObject::GetCurrentAction	() const
{
	CScriptEntity		*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member GetCurrentAction!");
	else
		if (l_tpScriptMonster->GetCurrentAction())
			return		(xr_new<CScriptEntityAction>(l_tpScriptMonster->GetCurrentAction()));
	return				(0);
}

void CScriptGameObject::AddAction	(const CScriptEntityAction *tpEntityAction, bool bHighPriority)
{
	CScriptEntity		*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member AddAction!");
	else
		l_tpScriptMonster->AddAction(tpEntityAction, bHighPriority);
}

const CScriptEntityAction *CScriptGameObject::GetActionByIndex(u32 action_index)
{
	CScriptEntity	*l_tpScriptMonster = smart_cast<CScriptEntity*>(&object());
	if (!l_tpScriptMonster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptEntity : cannot access class member GetActionByIndex!");
		return			(0);
	}
	else
		return			(l_tpScriptMonster->GetActionByIndex(action_index));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CPhysicsShell* CScriptGameObject::get_physics_shell( ) const
{
	CPhysicsShellHolder* ph_shell_holder = smart_cast<CPhysicsShellHolder*>(&object( ));
	if (!ph_shell_holder)
	{
		return NULL;
	}

	return ph_shell_holder->PPhysicsShell( );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CHelicopter* CScriptGameObject::get_helicopter( )
{
	CHelicopter* helicopter = smart_cast<CHelicopter*>(&object( ));
	if (!helicopter)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : cannot access class member get_helicopter!");
		NODEFAULT;
	}

	return helicopter;
}

CHangingLamp* CScriptGameObject::get_hanging_lamp( )
{
	CHangingLamp* lamp = smart_cast<CHangingLamp*>(&object( ));
	if (!lamp)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : it is not a lamp!");
		NODEFAULT;
	}

	return lamp;
}

CHolderCustom* CScriptGameObject::get_custom_holder( )
{
	CHolderCustom* holder = smart_cast<CHolderCustom*>(&object( ));
	if (!holder)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CGameObject : it is not a holder!");
	}

	return holder;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

pcstr CScriptGameObject::WhoHitName( )
{
	CEntityAlive* entity_alive = smart_cast<CEntityAlive*>(&object( ));
	if (entity_alive)
	{
		return			entity_alive->conditions( ).GetWhoHitLastTime( ) ? (*entity_alive->conditions( ).GetWhoHitLastTime( )->cName( )) : NULL;
	}
	else
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CScriptGameObject : cannot access class member  WhoHitName()");
		return			NULL;
	}
}

pcstr CScriptGameObject::WhoHitSectionName()
{
	CEntityAlive *entity_alive = smart_cast<CEntityAlive*>(&object());
	if (entity_alive)
		return			entity_alive->conditions().GetWhoHitLastTime()?(*entity_alive->conditions().GetWhoHitLastTime()->cNameSect()):NULL;
	else 
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member  WhoHitName()");
		return			NULL;
	}
}

bool CScriptGameObject::CheckObjectVisibility(const CScriptGameObject *tpLuaGameObject)
{
	CEntityAlive		*entity_alive = smart_cast<CEntityAlive*>(&object());
	if (entity_alive && !entity_alive->g_Alive()) {
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot check visibility of dead object!");
		return			(false);
	}

	CScriptEntity		*script_entity = smart_cast<CScriptEntity*>(&object());
	if (!script_entity) {
		CActor			*actor = smart_cast<CActor*>(&object());
		if (!actor) {
			ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"CScriptGameObject : cannot access class member CheckObjectVisibility!");
			return		(false);
		}
		return			(actor->memory().visual().visible_now(&tpLuaGameObject->object()));
	}

	return				(script_entity->CheckObjectVisibility(&tpLuaGameObject->object()));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CScriptBinderObject	*CScriptGameObject::binded_object	()
{
	return									(object().object());
}

void CScriptGameObject::bind_object			(CScriptBinderObject *game_object)
{
	object().set_object	(game_object);
}

void CScriptGameObject::set_previous_point	(int point_index)
{
	CCustomMonster		*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member set_previous_point!");
	else
		monster->movement().patrol().set_previous_point(point_index);
}

void CScriptGameObject::set_start_point	(int point_index)
{
	CCustomMonster		*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member set_start_point!");
	else
		monster->movement().patrol().set_start_point(point_index);
}

u32 CScriptGameObject::get_current_patrol_point_index()
{
	CCustomMonster		*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot call [get_current_patrol_point_index()]!");
		return			(u32(-1));
	}
	return				(monster->movement().patrol().get_current_point_index());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

fVector3	CScriptGameObject::bone_position	(pcstr bone_name) const
{
	u16					bone_id;
	if (xr_strlen(bone_name))
		bone_id			= smart_cast<CKinematics*>(object().Visual())->LL_BoneID(bone_name);
	else
		bone_id			= smart_cast<CKinematics*>(object().Visual())->LL_GetBoneRoot();

	fMatrix4x4				matrix;
	matrix.mul_43		(object().XFORM(),smart_cast<CKinematics*>(object().Visual())->LL_GetBoneInstance(bone_id).mTransform);
	return				(matrix.c);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

u32 CScriptGameObject::GetAmmoElapsed( )
{
	const CWeapon* weapon = smart_cast<const CWeapon*>(&object( ));
	if (!weapon)
	{
		return 0;
	}

	return weapon->GetAmmoElapsed( );
}

void CScriptGameObject::SetAmmoElapsed(s32 ammo_elapsed)
{
	CWeapon* weapon = smart_cast<CWeapon*>(&object( ));
	if (!weapon)
	{
		return;
	}

	weapon->SetAmmoElapsed(ammo_elapsed);
}

u32 CScriptGameObject::GetAmmoCurrent( ) const
{
	const CWeapon* weapon = smart_cast<const CWeapon*>(&object( ));
	if (!weapon)
	{
		return 0;
	}
	return weapon->GetAmmoCurrent(true);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::SetQueueSize(u32 queue_size)
{
	CWeaponMagazined* weapon = smart_cast<CWeaponMagazined*>(&object( ));
	if (!weapon)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CWeaponMagazined : cannot access class member SetQueueSize!");
		return;
	}

	weapon->SetQueueSize(queue_size);
}

////////////////////////////////////////////////////////////////////////////
//// Inventory Owner
////////////////////////////////////////////////////////////////////////////

u32 CScriptGameObject::Cost( ) const
{
	CInventoryItem* inventory_item = smart_cast<CInventoryItem*>(&object( ));
	if (!inventory_item)
	{
		ai( ).script_engine( ).script_log(ScriptStorage::eLuaMessageTypeError, "CSciptEntity : cannot access class member Cost!");
		return			(false);
	}

	return				(inventory_item->Cost( ));
}

f32 CScriptGameObject::GetCondition	() const
{
	CInventoryItem		*inventory_item = smart_cast<CInventoryItem*>(&object());
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member GetCondition!");
		return			(false);
	}
	return				(inventory_item->GetCondition());
}

void CScriptGameObject::SetCondition	(f32 val)
{
	CInventoryItem		*inventory_item = smart_cast<CInventoryItem*>(&object());
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member SetCondition!");
		return;
	}
	val					-= inventory_item->GetCondition();
	inventory_item->ChangeCondition			(val);
}

void CScriptGameObject::eat				(CScriptGameObject *item)
{
	if(!item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member eat!");
		return;
	}

	CInventoryItem		*inventory_item = smart_cast<CInventoryItem*>(&item->object());
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member eat!");
		return;
	}

	CInventoryOwner		*inventory_owner = smart_cast<CInventoryOwner*>(&object());
	if (!inventory_owner) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptEntity : cannot access class member eat!");
		return;
	}
	
	inventory_owner->inventory().Eat(inventory_item);
}

bool CScriptGameObject::inside					(const fVector3& position, f32 epsilon) const
{
	CSpaceRestrictor		*space_restrictor = smart_cast<CSpaceRestrictor*>(&object());
	if (!space_restrictor) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSpaceRestrictor : cannot access class member inside!");
		return			(false);
	}
	fSphere				sphere;
	sphere.P			= position;
	sphere.R			= epsilon;
	return				(space_restrictor->inside(sphere));
}

bool CScriptGameObject::inside					(const fVector3& position) const
{
	return				(inside(position, EPSILON_3));
}

void CScriptGameObject::set_patrol_extrapolate_callback(const luabind::functor<bool> &functor)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member set_patrol_extrapolate_callback!");
		return;
	}
	monster->movement().patrol().extrapolate_callback().set(functor);
}

void CScriptGameObject::set_patrol_extrapolate_callback(const luabind::functor<bool> &functor, const luabind::object &object)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&this->object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member set_patrol_extrapolate_callback!");
		return;
	}
	monster->movement().patrol().extrapolate_callback().set(functor,object);
}

void CScriptGameObject::set_patrol_extrapolate_callback()
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&this->object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member set_patrol_extrapolate_callback!");
		return;
	}
	monster->movement().patrol().extrapolate_callback().clear();
}

void CScriptGameObject::extrapolate_length		(f32 extrapolate_length)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&this->object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member extrapolate_length!");
		return;
	}
	monster->movement().detail().extrapolate_length(extrapolate_length);
}

f32 CScriptGameObject::extrapolate_length		() const
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&this->object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member extrapolate_length!");
		return				(0.f);
	}
	return					(monster->movement().detail().extrapolate_length());
}

void CScriptGameObject::set_fov					(f32 new_fov)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&this->object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member set_fov!");
		return;
	}
	monster->set_fov		(new_fov);
}

void CScriptGameObject::set_range				(f32 new_range)
{
	CCustomMonster			*monster = smart_cast<CCustomMonster*>(&this->object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member set_range!");
		return;
	}
	monster->set_range		(new_range);
}

u32	CScriptGameObject::vertex_in_direction(u32 level_vertex_id, fVector3 direction, f32 max_distance) const
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member vertex_in_direction!");
		return		(u32(-1));
	}

	if (!monster->movement().restrictions().accessible(level_vertex_id)) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster::vertex_in_direction - start vertex id is not accessible!");
		return		(u32(-1));
	}

	direction.normalize_safe();
	direction.mul	(max_distance);
	fVector3			start_position = ai().level_graph().vertex_position(level_vertex_id);
	fVector3			finish_position = fVector3(start_position).add(direction);
	u32				result = u32(-1);
	monster->movement().restrictions().add_border(level_vertex_id,max_distance);
	ai().level_graph().farthest_vertex_in_direction(level_vertex_id,start_position,finish_position,result,0,true);
	monster->movement().restrictions().remove_border();
	return			(ai().level_graph().valid_vertex_id(result) ? result : level_vertex_id);
}

bool CScriptGameObject::invulnerable		() const
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member invulnerable!");
		return		(false);
	}

	return			(monster->invulnerable());
}

void CScriptGameObject::invulnerable		(bool invulnerable)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CCustomMonster : cannot access class member invulnerable!");
		return;
	}

	monster->invulnerable	(invulnerable);
}

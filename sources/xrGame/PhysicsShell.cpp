#include "stdafx.h"

#include "PhysicsShell.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "PHJoint.h"
#include "PHShell.h"
#include "PHJointDestroyInfo.h"
#include "PHSplitedShell.h"
#include "GameObject.h"
#include "physicsshellholder.h"
#include "..\XR_3DA\skeletoncustom.h"

extern CPHWorld* ph_world;
CPhysicsShell::~CPhysicsShell( )
{
	if (ph_world)ph_world->NetRelcase(this);
}

CPhysicsElement* P_create_Element( )
{
	CPHElement* element = xr_new<CPHElement>( );
	return element;
}

CPhysicsShell* P_create_Shell( )
{
	CPhysicsShell* shell = xr_new<CPHShell>( );
	return shell;
}

CPhysicsShell* P_create_splited_Shell( )
{
	CPhysicsShell* shell = xr_new<CPHSplitedShell>( );
	return shell;
}

CPhysicsJoint* P_create_Joint(CPhysicsJoint::enumType type, CPhysicsElement* first, CPhysicsElement* second)
{
	CPhysicsJoint* joint = xr_new<CPHJoint>(type, first, second);
	return joint;
}

CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, BONE_P_MAP* bone_map)
{
	CKinematics* pKinematics = smart_cast<CKinematics*>(obj->Visual( ));

	CPhysicsShell* pPhysicsShell = P_create_Shell( );
#ifdef DEBUG
	pPhysicsShell->dbg_obj = smart_cast<CPhysicsShellHolder*>(obj);
#endif
	pPhysicsShell->build_FromKinematics(pKinematics, bone_map);

	pPhysicsShell->set_PhysicsRefObject(smart_cast<CPhysicsShellHolder*>(obj));
	pPhysicsShell->mXFORM.set(obj->XFORM( ));
	pPhysicsShell->Activate(not_active_state);//,
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	pPhysicsShell->SetAirResistance( );//0.0014f,1.5f

	return pPhysicsShell;
}

void fix_bones(pcstr fixed_bones, CPhysicsShell* shell)
{
	VERIFY(fixed_bones);
	VERIFY(shell);
	CKinematics* pKinematics = shell->PKinematics( );
	VERIFY(pKinematics);
	s32 count = _GetItemCount(fixed_bones);
	for (s32 i = 0; i < count; ++i)
	{
		string64 fixed_bone;
		_GetItem(fixed_bones, i, fixed_bone);
		u16 fixed_bone_id = pKinematics->LL_BoneID(fixed_bone);
		R_ASSERT2(BI_NONE != fixed_bone_id, "wrong fixed bone");
		CPhysicsElement* E = shell->get_Element(fixed_bone_id);
		if (E)
		{
			E->Fix( );
		}
	}
}

CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, BONE_P_MAP* p_bone_map, pcstr	fixed_bones)
{
	CPhysicsShell* pPhysicsShell;
	CKinematics* pKinematics = smart_cast<CKinematics*>(obj->Visual( ));
	if (fixed_bones)
	{
		s32 count = _GetItemCount(fixed_bones);
		for (s32 i = 0; i < count; ++i)
		{
			string64 fixed_bone;
			_GetItem(fixed_bones, i, fixed_bone);
			u16 fixed_bone_id = pKinematics->LL_BoneID(fixed_bone);
			R_ASSERT2(BI_NONE != fixed_bone_id, "wrong fixed bone");
			p_bone_map->insert(mk_pair(fixed_bone_id, physicsBone( )));
		}

		pPhysicsShell = P_build_Shell(obj, not_active_state, p_bone_map);

		//m_pPhysicsShell->add_Joint(P_create_Joint(CPhysicsJoint::enumType::full_control,0,fixed_element));
	}
	else
	{
		pPhysicsShell = P_build_Shell(obj, not_active_state);
	}

	BONE_P_PAIR_IT i = p_bone_map->begin( ), e = p_bone_map->end( );
	if (i != e)
	{
		pPhysicsShell->SetPrefereExactIntegration( );
	}

	for (; i != e; i++)
	{
		CPhysicsElement* fixed_element = i->second.element;
		R_ASSERT2(fixed_element, "fixed bone has no physics");
		//if(!fixed_element) continue;
		fixed_element->Fix( );
	}

	return pPhysicsShell;
}

CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, pcstr	fixed_bones)
{
	U16Vec f_bones;
	if (fixed_bones)
	{
		CKinematics* K = smart_cast<CKinematics*>(obj->Visual( ));
		s32 count = _GetItemCount(fixed_bones);
		for (s32 i = 0; i < count; ++i)
		{
			string64 fixed_bone;
			_GetItem(fixed_bones, i, fixed_bone);
			f_bones.push_back(K->LL_BoneID(fixed_bone));
			R_ASSERT2(BI_NONE != f_bones.back( ), "wrong fixed bone");
		}
	}

	return P_build_Shell(obj, not_active_state, f_bones);
}

static BONE_P_MAP bone_map = BONE_P_MAP( );
CPhysicsShell* P_build_Shell(CGameObject* obj, bool not_active_state, U16Vec& fixed_bones)
{
	bone_map.clear( );
	CPhysicsShell* pPhysicsShell;
	if (!fixed_bones.empty( ))
	{
		for (U16It it = fixed_bones.begin( ); it != fixed_bones.end( ); it++)
		{
			bone_map.insert(mk_pair(*it, physicsBone( )));
		}
	}

	pPhysicsShell = P_build_Shell(obj, not_active_state, &bone_map);

	// fix bones
	BONE_P_PAIR_IT i = bone_map.begin( ), e = bone_map.end( );
	if (i != e)
	{
		pPhysicsShell->SetPrefereExactIntegration( );
	}

	for (; i != e; i++)
	{
		CPhysicsElement* fixed_element = i->second.element;
		//R_ASSERT2(fixed_element,"fixed bone has no physics");
		if (!fixed_element)
		{
			continue;
		}

		fixed_element->Fix( );
	}

	return pPhysicsShell;
}

CPhysicsShell* P_build_SimpleShell(CGameObject* obj, f32 mass, bool not_active_state)
{
	CPhysicsShell* pPhysicsShell = P_create_Shell( );

#ifdef DEBUG
	pPhysicsShell->dbg_obj = smart_cast<CPhysicsShellHolder*>(obj);
#endif

	fObb obb;
	obj->Visual( )->vis.box.get_CD(obb.m_translate, obb.m_halfsize);
	obb.m_rotate.identity( );
	CPhysicsElement* E = P_create_Element( ); R_ASSERT(E); E->add_Box(obb);
	pPhysicsShell->add_Element(E);
	pPhysicsShell->setMass(mass);
	pPhysicsShell->set_PhysicsRefObject(smart_cast<CPhysicsShellHolder*>(obj));
	if (!obj->H_Parent( ))
	{
		pPhysicsShell->Activate(obj->XFORM( ), 0, obj->XFORM( ), not_active_state);
	}

	return pPhysicsShell;
}

void ApplySpawnIniToPhysicShell(CIniFile* ini, CPhysicsShell* physics_shell, bool fixed)
{
	if (!ini)
	{
		return;
	}

	if (ini->section_exist("physics_common"))
	{
		fixed = fixed || (ini->line_exist("physics_common", "fixed_bones"));
#pragma todo("not ignore static if non realy fixed! ")
		fix_bones(ini->r_string("physics_common", "fixed_bones"), physics_shell);
	}

	if (ini->section_exist("collide"))
	{
		if (ini->line_exist("collide", "ignore_static") && fixed)
		{
			physics_shell->SetIgnoreStatic( );
		}

		if (ini->line_exist("collide", "small_object"))
		{
			physics_shell->SetSmall( );
		}

		if (ini->line_exist("collide", "ignore_small_objects"))
		{
			physics_shell->SetIgnoreSmall( );
		}

		if (ini->line_exist("collide", "ignore_ragdoll"))
		{
			physics_shell->SetIgnoreRagDoll( );
		}
	}
}

void get_box(CPhysicsShell* shell, const	fMatrix4x4& form, fVector3& sz, fVector3& c)
{
	c.set(0.0f, 0.0f, 0.0f);
	for (s32 i = 0; 3 > i; ++i)
	{
		f32 lo;
		f32 hi;
		const fVector3& ax = cast_fv(((const f32*)&form + i * 4));
		shell->get_Extensions(ax, 0, lo, hi);
		sz[i] = hi - lo;
		c.add(fVector3( ).mul(ax, (lo + hi) / 2));
	}
}

using namespace luabind;

fMatrix4x4	global_transform(CPhysicsElement* E)
{
	fMatrix4x4 m;
	E->GetGlobalTransformDynamic(&m);
	return m;
}

#pragma optimize("s",on)
void CPhysicsShell::script_register(lua_State* L)
{
	module(L)
		[
			class_<CPhysicsShell>("physics_shell")
				.def("apply_force", (void (CPhysicsShell::*)(f32, f32, f32))(&CPhysicsShell::applyForce))
				.def("get_element_by_bone_name", (CPhysicsElement * (CPhysicsShell::*)(pcstr))(&CPhysicsShell::get_Element))
				.def("get_element_by_bone_id", (CPhysicsElement * (CPhysicsShell::*)(u16))(&CPhysicsShell::get_Element))
				.def("get_element_by_order", &CPhysicsShell::get_ElementByStoreOrder)
				.def("get_elements_number", &CPhysicsShell::get_ElementsNumber)
				.def("get_joint_by_bone_name", (CPhysicsJoint * (CPhysicsShell::*)(pcstr))(&CPhysicsShell::get_Joint))
				.def("get_joint_by_bone_id", (CPhysicsJoint * (CPhysicsShell::*)(u16))(&CPhysicsShell::get_Joint))
				.def("get_joint_by_order", &CPhysicsShell::get_JointByStoreOrder)
				.def("get_joints_number", &CPhysicsShell::get_JointsNumber)
				.def("block_breaking", &CPhysicsShell::BlockBreaking)
				.def("unblock_breaking", &CPhysicsShell::UnblockBreaking)
				.def("is_breaking_blocked", &CPhysicsShell::IsBreakingBlocked)
				.def("is_breakable", &CPhysicsShell::isBreakable)
				.def("get_linear_vel", &CPhysicsShell::get_LinearVel)
				.def("get_angular_vel", &CPhysicsShell::get_AngularVel)
		];
}

void CPhysicsElement::script_register(lua_State* L)
{
	module(L)
		[
			class_<CPhysicsElement>("physics_element")
				.def("apply_force", (void (CPhysicsElement::*)(f32, f32, f32))(&CPhysicsElement::applyForce))
				.def("is_breakable", &CPhysicsElement::isBreakable)
				.def("get_linear_vel", &CPhysicsElement::get_LinearVel)
				.def("get_angular_vel", &CPhysicsElement::get_AngularVel)
				.def("get_mass", &CPhysicsElement::getMass)
				.def("get_density", &CPhysicsElement::getDensity)
				.def("get_volume", &CPhysicsElement::getVolume)
				.def("fix", &CPhysicsElement::Fix)
				.def("release_fixed", &CPhysicsElement::ReleaseFixed)
				.def("is_fixed", &CPhysicsElement::isFixed)
				.def("global_transform", &global_transform)
		];
}

void CPhysicsJoint::script_register(lua_State* L)
{
	module(L)
		[
			class_<CPhysicsJoint>("physics_joint")
				.def("get_bone_id", &CPhysicsJoint::BoneID)
				.def("get_first_element", &CPhysicsJoint::PFirst_element)
				.def("get_stcond_element", &CPhysicsJoint::PSecond_element)
				.def("set_anchor_global", (void(CPhysicsJoint::*)(const f32, const f32, const f32))(&CPhysicsJoint::SetAnchor))
				.def("set_anchor_vs_first_element", (void(CPhysicsJoint::*)(const f32, const f32, const f32))(&CPhysicsJoint::SetAnchorVsFirstElement))
				.def("set_anchor_vs_second_element", (void(CPhysicsJoint::*)(const f32, const f32, const f32))(&CPhysicsJoint::SetAnchorVsSecondElement))
				.def("get_axes_number", &CPhysicsJoint::GetAxesNumber)
				.def("set_axis_spring_dumping_factors", &CPhysicsJoint::SetAxisSDfactors)
				.def("set_joint_spring_dumping_factors", &CPhysicsJoint::SetJointSDfactors)
				.def("set_axis_dir_global", (void(CPhysicsJoint::*)(const f32, const f32, const f32, const int))(&CPhysicsJoint::SetAxisDir))
				.def("set_axis_dir_vs_first_element", (void(CPhysicsJoint::*)(const f32, const f32, const f32, const int))(&CPhysicsJoint::SetAxisDirVsFirstElement))
				.def("set_axis_dir_vs_second_element", (void(CPhysicsJoint::*)(const f32, const f32, const f32, const int))(&CPhysicsJoint::SetAxisDirVsSecondElement))
				.def("set_limits", &CPhysicsJoint::SetLimits)
				.def("set_max_force_and_velocity", &CPhysicsJoint::SetForceAndVelocity)
				.def("get_max_force_and_velocity", &CPhysicsJoint::GetMaxForceAndVelocity)
				.def("get_axis_angle", &CPhysicsJoint::GetAxisAngle)
				.def("get_limits", &CPhysicsJoint::GetLimits, out_value(_2) + out_value(_3))
				.def("get_axis_dir", &CPhysicsJoint::GetAxisDirDynamic)
				.def("get_anchor", &CPhysicsJoint::GetAnchorDynamic)
				.def("is_breakable", &CPhysicsJoint::isBreakable)
		];
}

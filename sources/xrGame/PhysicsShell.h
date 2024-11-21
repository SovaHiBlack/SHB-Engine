#pragma once

#include "PHDefs.h"
#include "PhysicsCommon.h"
#include "alife_space.h"
#include "script_export_space.h"

class CPhysicsJoint;
class CPhysicsElement;
class CPhysicsShell;
class CPHFracture;
class CPHJointDestroyInfo;
class CODEGeom;
class CPHSynchronize;
class CPhysicsShellHolder;
class CGameObject;
class NET_Packet;
struct SBoneShape;
class CPHShellSplitterHolder;
class CKinematics;

typedef u32	CLClassBits;
typedef u32	CLBits;
typedef u32	CGID;

struct physicsBone
{
	CPhysicsJoint* joint;
	CPhysicsElement* element;
	physicsBone()
	{
		joint=NULL;
		element=NULL;
	}
};

DEFINE_MAP	(u16,	physicsBone,	BONE_P_MAP,	BONE_P_PAIR_IT);
typedef const  BONE_P_MAP :: iterator			BONE_P_PAIR_CIT;

// ABSTRACT:
class	CPhysicsBase
{
public:
	fMatrix4x4					mXFORM																																							;					// In parent space

public:
	virtual		void			Activate								(const fMatrix4x4& m0, f32 dt01, const fMatrix4x4& m2,bool disable=false)													= 0;
	virtual		void			Activate								(const fMatrix4x4& transform,const fVector3& lin_vel,const fVector3& ang_vel,bool disable=false)								= 0;
	virtual		void			Activate								(bool disable=false)																									= 0;
	virtual		void			Activate								(const fMatrix4x4& form,bool disable=false)																				= 0;
	virtual		void			InterpolateGlobalTransform				(fMatrix4x4* m)																											= 0;
	virtual		void			GetGlobalTransformDynamic				(fMatrix4x4* m)																											= 0;
	virtual		void			InterpolateGlobalPosition				(fVector3* v)																											= 0;
	virtual		void			net_Import								(NET_Packet& P)																											= 0;
	virtual		void			net_Export								(NET_Packet& P)																											= 0;
	virtual		void			GetGlobalPositionDynamic				(fVector3* v)																											= 0;
	virtual		bool			isBreakable								()																														= 0;
	virtual		bool			isEnabled								()																														= 0;
	virtual		bool			isActive								()																														= 0;
	virtual		bool			isFullActive							()																														= 0;
	virtual		void			Deactivate								()																														= 0;
	virtual		void			Enable									()																														= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void			setMass									(f32 M)																												= 0;
	virtual		void			setDensity								(f32 M)																												= 0;
	virtual		f32			getMass									()																														= 0;
	virtual		f32			getDensity								()																														= 0;
	virtual		f32			getVolume								()																														= 0;
	virtual		void			get_Extensions							(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext)														= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void			applyForce								(const fVector3& dir, f32 val)																							= 0;
	virtual		void			applyForce								(f32 x, f32 y, f32 z)																								= 0;
	virtual		void			applyImpulse							(const fVector3& dir, f32 val)																							= 0;
	virtual		void			setTorque								(const fVector3& torque)																									= 0;
	virtual		void			setForce								(const fVector3& force)																									= 0;
	virtual		void			applyGravityAccel						(const fVector3& accel)																									= 0;
	virtual		void			SetAirResistance						(f32 linear=default_k_l, f32 angular=default_k_w)																	= 0;
	virtual		void			GetAirResistance						(f32& linear, f32& angular)																						= 0;
	virtual		void			set_DynamicLimits						(f32 l_limit=default_l_limit, f32 w_limit=default_w_limit)															= 0;
	virtual		void			set_DynamicScales						(f32 l_scale=default_l_scale, f32 w_scale=default_w_scale)															= 0;
	virtual		void			set_ContactCallback						(ContactCallbackFun* callback)																							= 0;
	virtual		void			set_ObjectContactCallback				(ObjectContactCallbackFun* callback)																					= 0;
	virtual		void			add_ObjectContactCallback				(ObjectContactCallbackFun* callback)																					= 0;
	virtual		void			remove_ObjectContactCallback			(ObjectContactCallbackFun* callback)																					= 0;
	virtual		void			set_CallbackData						(void * cd)																												= 0;
	virtual		void			*get_CallbackData						()																														= 0;
	virtual		void			set_PhysicsRefObject					(CPhysicsShellHolder* ref_object)																						= 0;
	virtual		void			get_LinearVel							(fVector3& velocity)																										= 0;
	virtual		void			get_AngularVel							(fVector3& velocity)																										= 0;
	virtual		void			set_LinearVel							(const fVector3& velocity)																								= 0;
	virtual		void			set_AngularVel							(const fVector3& velocity)																								= 0;
	virtual		void			TransformPosition						(const fMatrix4x4& form)																									= 0;
	virtual		void			set_ApplyByGravity						(bool flag)																												= 0;
	virtual		bool			get_ApplyByGravity						()																														= 0;

	virtual		void			SetMaterial								(u16 m)																													= 0;
	virtual		void			SetMaterial								(pcstr m)																												= 0;
	virtual		void			set_DisableParams						(const SAllDDOParams& params)																							= 0;
	virtual		void			SetTransform							(const fMatrix4x4& m0)																										= 0;
	virtual						~CPhysicsBase							()																																		{};
};

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
class	CPhysicsElement		: public CPhysicsBase
{

public:
				u16								m_SelfID																						;
	virtual		CPhysicsShell					*PhysicsShell							()																													= 0;		
	virtual		void							set_ContactCallback						(ContactCallbackFun	*callback)																						= 0;
	virtual		CPhysicsShellHolder				*PhysicsRefObject						()																													= 0;
	virtual		void							add_Sphere								(const fSphere&		V)																								= 0;
	virtual		void							add_Box									(const fObb&		V)																								= 0;
	virtual		void							add_Cylinder							(const fCylinder&	V)																								= 0;
	virtual		void							add_Shape								(const SBoneShape& shape)																							= 0;
	virtual		void							add_Shape								(const SBoneShape& shape,const fMatrix4x4& offset)																		= 0;
	virtual		CODEGeom						*last_geom								()																													= 0;
	virtual		bool							has_geoms								()																													= 0;
	virtual		void							add_Mass								(const SBoneShape& shape,const fMatrix4x4& offset,const fVector3& mass_center, f32 mass,CPHFracture* fracture=NULL)	= 0;
	virtual		void							set_ParentElement						(CPhysicsElement* p)																								= 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void							set_BoxMass								(const fObb& box, f32 mass)																						= 0;
	virtual		void							setInertia								(const dMass& M)																									= 0;																
	virtual		void							addInertia								(const dMass& M)																									= 0;
	virtual		void							setMassMC								(f32 M,const fVector3& mass_center)																				= 0;
	virtual		void							applyImpulseVsMC						(const fVector3& pos,const fVector3& dir, f32 val)																	= 0;
	virtual		void							applyImpulseVsGF						(const fVector3& pos,const fVector3& dir, f32 val)																	= 0;
	virtual		void							applyImpulseTrace						(const fVector3& pos, const fVector3& dir, f32 val,const u16 id)													= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void							setDensityMC							(f32 M,const fVector3& mass_center)																				= 0;
	virtual		u16								setGeomFracturable						(CPHFracture &fracture)																								= 0;
	virtual		CPHFracture						&Fracture								(u16 num)																											= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		u16								numberOfGeoms							()																													= 0;
	virtual				dBodyID					get_body								()																													= 0;
	virtual		const	fVector3&				mass_Center							()																													= 0;
	virtual		const	fVector3&				local_mass_Center						()																													= 0;
	virtual		f32							getRadius								()																													= 0;
	virtual		dMass							*getMassTensor							()																													= 0;
	virtual		void							get_MaxAreaDir							(fVector3& dir)																										= 0;
	virtual		ObjectContactCallbackFun		*get_ObjectContactCallback				()																													= 0;
	virtual		void							Fix										()																													= 0;
	virtual		void							ReleaseFixed							()																													= 0;
	virtual		bool							isFixed									()																													= 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual										~CPhysicsElement						()																													{};
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPhysicsElement)
#undef script_type_list
#define script_type_list save_type_list(CPhysicsElement)

//ABSTRACT:
// Joint between two elements 

class CPhysicsJoint	
{

public:
			bool			bActive																			;
	enum	eVs {								//coordinate system 
				vs_first	,			//in first local
				vs_second	,			//in second local 
				vs_global				//in global		
		};
	enum	enumType{							//joint type
		ball			,					// ball-socket
		hinge			,					// standart hinge 1 - axis
		hinge2			,					// for car wheels 2-axes 
		full_control	,					// 3 - axes control (eiler - angles)
		slider			
	};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				enumType				eType																										;          //type of the joint
public:
	virtual		~CPhysicsJoint										()																				{};
	virtual		u16					 	BoneID						()																				=0;
	virtual		void					SetBoneID					(u16 bone_id)																	=0;
	virtual		CPhysicsElement*		PFirst_element				()																				=0;
	virtual		CPhysicsElement*		PSecond_element				()																				=0;
	virtual		u16						GetAxesNumber				()																				=0;
	virtual		void 					Activate					()																				=0;
	virtual		void 					Create						()																				=0;
	virtual		void 					RunSimulation				()																				=0;
	virtual		void 					Deactivate					()																				=0;
	virtual		void 					SetBackRef					(CPhysicsJoint** j)																=0;
	virtual		void 					SetAnchor					(const fVector3& position)														=0;
	virtual		void 					SetAxisSDfactors			(f32 spring_factor, f32 damping_factor,int axis_num)							=0;
	virtual		void 					SetJointSDfactors			(f32 spring_factor, f32 damping_factor)										=0;
	virtual		void 					SetAnchorVsFirstElement		(const fVector3& position)														=0;
	virtual		void 					SetAnchorVsSecondElement	(const fVector3& position)														=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void					SetAxisDir					(const fVector3& orientation,const int axis_num)									=0;
	virtual		void 					SetAxisDirVsFirstElement	(const fVector3& orientation,const int axis_num)									=0;
	virtual		void 					SetAxisDirVsSecondElement	(const fVector3& orientation,const int axis_num)									=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void 					SetAnchor					(const f32 x,const f32 y,const f32 z)										=0;
	virtual		void 					SetAnchorVsFirstElement		(const f32 x,const f32 y,const f32 z)										=0;
	virtual		void 					SetAnchorVsSecondElement	(const f32 x,const f32 y,const f32 z)										=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void 					SetAxisDir					(const f32 x,const f32 y,const f32 z,const int axis_num)					=0;
	virtual		void 					SetAxisDirVsFirstElement	(const f32 x,const f32 y,const f32 z,const int axis_num)					=0;
	virtual		void 					SetAxisDirVsSecondElement	(const f32 x,const f32 y,const f32 z,const int axis_num)					=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void 					SetLimits					(const f32 low,const f32 high,const int axis_num)							=0;
	virtual		void 					SetLimitsVsFirstElement		(const f32 low,const f32 high,const int axis_num)							=0;
	virtual		void 					SetLimitsVsSecondElement	(const f32 low,const f32 high,const int axis_num)							=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void 					SetBreakable				(f32 force, f32 torque)										  				=0;
	virtual		CPHJointDestroyInfo		*JointDestroyInfo			()																  				=0;
	virtual		bool					isBreakable					()																  				=0;
	virtual		void					SetForceAndVelocity			(const f32 force,const f32 velocity=0.f,const int axis_num=-1)				=0;
	virtual		void					GetMaxForceAndVelocity		(f32& force, f32& velocity,int axis_num)						  				=0;
	virtual		f32					GetAxisAngle				(int axis_num)													  				=0;
	virtual		dJointID				GetDJoint					()																  				=0;
	virtual		void 					GetAxisSDfactors			(f32& spring_factor, f32& damping_factor,int axis_num)		  				=0;
	virtual		void 					GetJointSDfactors			(f32& spring_factor, f32& damping_factor)					  				=0;
	virtual		void 					GetLimits					(f32& lo_limit, f32& hi_limit,int axis_num)					  				=0;
	virtual		void 					GetAxisDir					(int num, fVector3& axis,eVs& vs)									  				=0;
	virtual		void 					GetAxisDirDynamic			(int num, fVector3& axis)											  				=0;
	virtual		void 					GetAnchorDynamic			(fVector3& anchor)												  				=0;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPhysicsJoint)
#undef script_type_list
#define script_type_list save_type_list(CPhysicsJoint)

// ABSTRACT: 
class CPHIsland;

class CPhysicsShell			: public CPhysicsBase
{
protected:
					CKinematics					*m_pKinematics																															;
public:
#ifdef DEBUG
					CPhysicsShellHolder			*dbg_obj																																;
#endif
public:
IC					CKinematics					*PKinematics								()																{return m_pKinematics		;}
					void						set_Kinematics								(CKinematics* p)														{m_pKinematics=p	;}
	virtual			void						set_JointResistance							(f32 force)																				= 0;
	virtual			void						add_Element									(CPhysicsElement* E)																		= 0;
	virtual			void						add_Joint									(CPhysicsJoint* E)																			= 0;
	virtual			CPHIsland					*PIsland									()																							= 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	///////////
	virtual	const	CGID						&GetCLGroup								()const																							= 0;
	virtual			void						RegisterToCLGroup							(CGID g)																					= 0;
	virtual			bool						IsGroupObject								()																							= 0;
	virtual			void						SetIgnoreStatic								()																							= 0;
	virtual			void						SetIgnoreDynamic							()																							= 0;
	virtual			void						SetRagDoll									()																							= 0;
	virtual			void						SetIgnoreRagDoll							()																							= 0;
	virtual			void						SetSmall									()																							= 0;
	virtual			void						SetIgnoreSmall								()																							= 0;
	virtual			bool						isFractured									()																							= 0;
	virtual			CPHShellSplitterHolder		*SplitterHolder								()																							= 0;
	virtual			void						SplitProcess								(PHSHELL_PAIR_VECTOR &out_shels)															= 0;
	virtual			void						BlockBreaking								()																							= 0;
	virtual			void						UnblockBreaking								()																							= 0;
	virtual			bool						IsBreakingBlocked							()																							= 0;
	virtual			void						applyImpulseTrace							(const fVector3& pos, const fVector3& dir, f32 val)											= 0;
	virtual			void						applyImpulseTrace							(const fVector3& pos, const fVector3& dir, f32 val,const u16 id)							= 0;
	virtual			void						applyHit									(const fVector3& pos, const fVector3& dir, f32 val,const u16 id,ALife::EHitType hit_type)	= 0;
	virtual			BoneCallbackFun*			GetBonesCallback							()																							= 0;
	virtual			BoneCallbackFun*			GetStaticObjectBonesCallback				()																							= 0;
	virtual			void						Update										()																							= 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual			void						setMass1									(f32 M)																					= 0;
	virtual			void						SmoothElementsInertia						(f32 k)																					= 0;
	virtual			void						setEquelInertiaForEls						(const dMass& M)																			= 0;
	virtual			void						addEquelInertiaToEls						(const dMass& M)																			= 0;
	virtual			ELEMENT_STORAGE				&Elements									()																							= 0;
	virtual			CPhysicsElement				*get_Element								(u16 bone_id)																				= 0;
	virtual			CPhysicsElement				*get_Element								(const shared_str & bone_name)																= 0;
	virtual			CPhysicsElement				*get_Element								(pcstr bone_name)																			= 0;
	virtual			CPhysicsElement				*get_ElementByStoreOrder					(u16 num)																					= 0;
	virtual			u16							get_ElementsNumber							()																							= 0;
	virtual			CPHSynchronize				*get_ElementSync							(u16 element)																				= 0;
	virtual			CPhysicsJoint				*get_Joint									(u16 bone_id)																				= 0;
	virtual			CPhysicsJoint				*get_Joint									(const shared_str & bone_name)																= 0;
	virtual			CPhysicsJoint				*get_Joint									(pcstr bone_name)																			= 0;
	virtual			CPhysicsJoint				*get_JointByStoreOrder						(u16 num)																					= 0;
	virtual			u16							get_JointsNumber							()																							= 0;
	virtual			CODEGeom					*get_GeomByID								(u16 bone_id)																				= 0;
	virtual			void						Freeze										()																							= 0;
	virtual			void						UnFreeze									()																							= 0;
	virtual			void						NetInterpolationModeON						()																							= 0;
	virtual			void						NetInterpolationModeOFF						()																							= 0;
	virtual			void						Disable										()																							= 0;
	virtual			void						DisableCollision							()																							= 0;
	virtual			void						EnableCollision								()																							= 0;
	virtual			void						SetRemoveCharacterCollLADisable				()																							= 0;
	virtual			void						DisableCharacterCollision					()																							= 0;
	virtual			void						PureStep									(f32 step = fixed_step)																	= 0;
	virtual			void						SetGlTransformDynamic						(const fMatrix4x4& form)																		= 0;
	virtual			void						CollideAll									()																							= 0;
	virtual			CPhysicsElement				*NearestToPoint								(const fVector3& point)																		= 0;
	virtual			void						build_FromKinematics						(CKinematics* K,BONE_P_MAP* p_geting_map=NULL)												= 0;
	virtual			void						preBuild_FromKinematics						(CKinematics* K,BONE_P_MAP* p_geting_map=NULL)												= 0;
	virtual			void						Build										(bool disable=false)																		= 0;
	virtual			void						SetMaxAABBRadius							(f32 size)																				 {};
	virtual			void						AddTracedGeom								(u16 element=0,u16 geom=0)																	= 0;
	virtual			void						SetAllGeomTraced							()																							= 0;
	virtual			void						RunSimulation								(bool place_current_forms=true)																= 0;
	virtual			void						UpdateRoot									()																							= 0;
	virtual			void            		    ZeroCallbacks								()																							= 0;
	virtual			void						ResetCallbacks								(u16 id, flags64& mask)																		= 0;
	virtual			void						SetCallbacks								(BoneCallbackFun* callback)																	= 0;
	virtual			void						EnabledCallbacks							(BOOL val)																					= 0;
	virtual			void						ToAnimBonesPositions						()																							= 0;
	virtual			bool						AnimToVelocityState							(f32 dt, f32 l_limit, f32 a_limit )													= 0;
	virtual 		void						SetBonesCallbacksOverwrite					(bool v)																					= 0;
	virtual			fMatrix4x4&					ObjectInRoot								()																							= 0;
	virtual			void						ObjectToRootForm							(const fMatrix4x4& form)							    										= 0;
	virtual			void						SetPrefereExactIntegration					()																							= 0;
	virtual										~CPhysicsShell								()																							;
	//build_FromKinematics		in returns elements  & joint pointers according bone IDs;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPhysicsShell)
#undef script_type_list
#define script_type_list save_type_list(CPhysicsShell)

void	get_box(CPhysicsShell*	shell,const	fMatrix4x4& form, fVector3&	sz, fVector3&	c);

// Implementation creator
CPhysicsJoint*				P_create_Joint				(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second)		;
CPhysicsElement*			P_create_Element			()																					;
CPhysicsShell*				P_create_Shell				()																					;
CPhysicsShell*				P_create_splited_Shell		()																					;
CPhysicsShell*				P_build_Shell				(CGameObject* obj,bool not_active_state, pcstr	fixed_bones)						;
CPhysicsShell*				P_build_Shell				(CGameObject* obj,bool not_active_state,U16Vec& fixed_bones)						;
CPhysicsShell*				P_build_Shell				(CGameObject* obj,bool not_active_state,BONE_P_MAP* bone_map, pcstr	fixed_bones)	;
CPhysicsShell*				P_build_Shell				(CGameObject* obj,bool not_active_state,BONE_P_MAP* bone_map=NULL)					;
CPhysicsShell*				P_build_SimpleShell			(CGameObject* obj, f32 mass,bool not_active_state)									;
		void				ApplySpawnIniToPhysicShell	(CIniFile* ini,CPhysicsShell* physics_shell,bool fixed)								;
		void				fix_bones					(pcstr	fixed_bones,CPhysicsShell* shell )											;

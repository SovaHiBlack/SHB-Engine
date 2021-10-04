#pragma once

#include "PHDefs.h"
#include "PhysicsCommon.h"
#include "ALife_space.h"
#include "script_export_space.h"

class CPhysicsJoint;
class CPhysicsElement;
class CPhysicsShell;
class CPHFracture;
class CPHJointDestroyInfo;
class CCodeGeom;
class CPHSynchronize;
class CPHShellHolder;
class CGameObject;
class CNetPacket;
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
		joint=nullptr;
		element=nullptr;
	}
};

//DEFINE_MAP	(U16,	physicsBone,	BONE_P_MAP,	BONE_P_PAIR_IT);
using BONE_P_MAP = xr_map<U16, physicsBone>;
using BONE_P_PAIR_IT = BONE_P_MAP::iterator;
typedef const  BONE_P_MAP :: iterator			BONE_P_PAIR_CIT;

// ABSTRACT:
class	CPhysicsBase
{
public:
	Fmatrix						mXFORM																																							;					// In parent space
public:
	virtual		void			Activate								(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false)													= 0;
	virtual		void			Activate								(const Fmatrix &transform,const Fvector3& lin_vel,const Fvector3& ang_vel,bool disable=false)								= 0;
	virtual		void			Activate								(bool disable=false)																									= 0;
	virtual		void			Activate								(const Fmatrix& form,bool disable=false)																				= 0;
	virtual		void			InterpolateGlobalTransform				(Fmatrix* m)																											= 0;
	virtual		void			GetGlobalTransformDynamic				(Fmatrix* m)																											= 0;
	virtual		void			InterpolateGlobalPosition				(Fvector3* v)																											= 0;
	virtual		void			net_Import								(CNetPacket& P)																											= 0;
	virtual		void			net_Export								(CNetPacket& P)																											= 0;
	virtual		void			GetGlobalPositionDynamic				(Fvector3* v)																											= 0;
	virtual		bool			isBreakable								()																														= 0;
	virtual		bool			isEnabled								()																														= 0;
	virtual		bool			isActive								()																														= 0;
	virtual		bool			isFullActive							()																														= 0;
	virtual		void			Deactivate								()																														= 0;
	virtual		void			Enable									()																														= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void			setMass									(float M)																												= 0;
	virtual		void			setDensity								(float M)																												= 0;
	virtual		float			getMass									()																														= 0;
	virtual		float			getDensity								()																														= 0;
	virtual		float			getVolume								()																														= 0;
	virtual		void			get_Extensions							(const Fvector3& axis,float center_prg,float& lo_ext, float& hi_ext)														= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void			applyForce								(const Fvector3& dir, float val)																							= 0;
	virtual		void			applyForce								(float x,float y,float z)																								= 0;
	virtual		void			applyImpulse							(const Fvector3& dir, float val)																							= 0;
	virtual		void			setTorque								(const Fvector3& torque)																									= 0;
	virtual		void			setForce								(const Fvector3& force)																									= 0;
	virtual		void			applyGravityAccel						(const Fvector3& accel)																									= 0;
	virtual		void			SetAirResistance						(float linear=default_k_l, float angular=default_k_w)																	= 0;
	virtual		void			GetAirResistance						(float	&linear, float &angular)																						= 0;
	virtual		void			set_DynamicLimits						(float l_limit=default_l_limit,float w_limit=default_w_limit)															= 0;
	virtual		void			set_DynamicScales						(float l_scale=default_l_scale,float w_scale=default_w_scale)															= 0;
	virtual		void			set_ContactCallback						(ContactCallbackFun* callback)																							= 0;
	virtual		void			set_ObjectContactCallback				(ObjectContactCallbackFun* callback)																					= 0;
	virtual		void			add_ObjectContactCallback				(ObjectContactCallbackFun* callback)																					= 0;
	virtual		void			remove_ObjectContactCallback			(ObjectContactCallbackFun* callback)																					= 0;
	virtual		void			set_CallbackData						(void * cd)																												= 0;
	virtual		void			*get_CallbackData						()																														= 0;
	virtual		void			set_PhysicsRefObject					(CPHShellHolder* ref_object)																						= 0;
	virtual		void			get_LinearVel							(Fvector3& velocity)																										= 0;
	virtual		void			get_AngularVel							(Fvector3& velocity)																										= 0;
	virtual		void			set_LinearVel							(const Fvector3& velocity)																								= 0;
	virtual		void			set_AngularVel							(const Fvector3& velocity)																								= 0;
	virtual		void			TransformPosition						(const Fmatrix &form)																									= 0;
	virtual		void			set_ApplyByGravity						(bool flag)																												= 0;
	virtual		bool			get_ApplyByGravity						()																														= 0;

	virtual		void			SetMaterial								(U16 m)																													= 0;
	virtual		void			SetMaterial								(const char* m)																												= 0;
	virtual		void			set_DisableParams						(const SAllDDOParams& params)																							= 0;
	virtual		void			SetTransform							(const Fmatrix& m0)																										= 0;
	virtual						~CPhysicsBase							()																																		{};
};

// ABSTRACT:
// Element is fully Rigid and consists of one or more forms, such as sphere, box, cylinder, etc.
class CPhysicsElement		: public CPhysicsBase
{
public:
				U16								m_SelfID																						;
	virtual		CPhysicsShell					*PhysicsShell							()																													= 0;		
	virtual		void							set_ContactCallback						(ContactCallbackFun	*callback)																						= 0;
	virtual		CPHShellHolder*PhysicsRefObject						()																													= 0;
	virtual		void							add_Sphere								(const Fsphere&		V)																								= 0;
	virtual		void							add_Box									(const Fobb&		V)																								= 0;
	virtual		void							add_Cylinder							(const Fcylinder&	V)																								= 0;
	virtual		void							add_Shape								(const SBoneShape& shape)																							= 0;
	virtual		void							add_Shape								(const SBoneShape& shape,const Fmatrix& offset)																		= 0;
	virtual		CCodeGeom*						last_geom								()																													= 0;
	virtual		bool							has_geoms								()																													= 0;
	virtual		void							add_Mass								(const SBoneShape& shape,const Fmatrix& offset,const Fvector3& mass_center,float mass,CPHFracture* fracture=NULL)	= 0;
	virtual		void							set_ParentElement						(CPhysicsElement* p)																								= 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void							set_BoxMass								(const Fobb& box, float mass)																						= 0;	
	virtual		void							setInertia								(const dMass& M)																									= 0;																
	virtual		void							addInertia								(const dMass& M)																									= 0;
	virtual		void							setMassMC								(float M,const Fvector3& mass_center)																				= 0;
	virtual		void							applyImpulseVsMC						(const Fvector3& pos,const Fvector3& dir, float val)																	= 0;
	virtual		void							applyImpulseVsGF						(const Fvector3& pos,const Fvector3& dir, float val)																	= 0;
	virtual		void							applyImpulseTrace						(const Fvector3& pos, const Fvector3& dir, float val,const U16 id)													= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void							setDensityMC							(float M,const Fvector3& mass_center)																				= 0;
	virtual		U16								setGeomFracturable						(CPHFracture &fracture)																								= 0;
	virtual		CPHFracture						&Fracture								(U16 num)																											= 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		U16								numberOfGeoms							()																													= 0;
	virtual				dBodyID					get_body								()																													= 0;
	virtual		const	Fvector3&				mass_Center							()																													= 0;
	virtual		const	Fvector3&				local_mass_Center						()																													= 0;
	virtual		float							getRadius								()																													= 0;
	virtual		dMass							*getMassTensor							()																													= 0;
	virtual		void							get_MaxAreaDir							(Fvector3& dir)																										= 0;
	virtual		ObjectContactCallbackFun		*get_ObjectContactCallback				()																													= 0;
	virtual		void							Fix										()																													= 0;
	virtual		void							ReleaseFixed							()																													= 0;
	virtual		bool							isFixed									()																													= 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual										~CPhysicsElement						()																													{};
public:
	static void script_register(lua_State*);
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
	virtual		U16					 	BoneID						()																				=0;
	virtual		void					SetBoneID					(U16 bone_id)																	=0;
	virtual		CPhysicsElement*		PFirst_element				()																				=0;
	virtual		CPhysicsElement*		PSecond_element				()																				=0;
	virtual		U16						GetAxesNumber				()																				=0;
	virtual		void 					Activate					()																				=0;
	virtual		void 					Create						()																				=0;
	virtual		void 					RunSimulation				()																				=0;
	virtual		void 					Deactivate					()																				=0;
	virtual		void 					SetBackRef					(CPhysicsJoint** j)																=0;
	virtual		void 					SetAnchor					(const Fvector3& position)														=0;
	virtual		void 					SetAxisSDfactors			(float spring_factor,float damping_factor,int axis_num)							=0;
	virtual		void 					SetJointSDfactors			(float spring_factor,float damping_factor)										=0;
	virtual		void 					SetAnchorVsFirstElement		(const Fvector3& position)														=0;
	virtual		void 					SetAnchorVsSecondElement	(const Fvector3& position)														=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void					SetAxisDir					(const Fvector3& orientation,const int axis_num)									=0;
	virtual		void 					SetAxisDirVsFirstElement	(const Fvector3& orientation,const int axis_num)									=0;
	virtual		void 					SetAxisDirVsSecondElement	(const Fvector3& orientation,const int axis_num)									=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void 					SetAnchor					(const float x,const float y,const float z)										=0;
	virtual		void 					SetAnchorVsFirstElement		(const float x,const float y,const float z)										=0;
	virtual		void 					SetAnchorVsSecondElement	(const float x,const float y,const float z)										=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void 					SetAxisDir					(const float x,const float y,const float z,const int axis_num)					=0;
	virtual		void 					SetAxisDirVsFirstElement	(const float x,const float y,const float z,const int axis_num)					=0;
	virtual		void 					SetAxisDirVsSecondElement	(const float x,const float y,const float z,const int axis_num)					=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void 					SetLimits					(const float low,const float high,const int axis_num)							=0;
	virtual		void 					SetLimitsVsFirstElement		(const float low,const float high,const int axis_num)							=0;
	virtual		void 					SetLimitsVsSecondElement	(const float low,const float high,const int axis_num)							=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual		void 					SetBreakable				(float force, float torque)										  				=0;
	virtual		CPHJointDestroyInfo		*JointDestroyInfo			()																  				=0;
	virtual		bool					isBreakable					()																  				=0;
	virtual		void					SetForceAndVelocity			(const float force,const float velocity=0.f,const int axis_num=-1)				=0;
	virtual		void					GetMaxForceAndVelocity		(float &force,float &velocity,int axis_num)						  				=0;
	virtual		float					GetAxisAngle				(int axis_num)													  				=0;
	virtual		dJointID				GetDJoint					()																  				=0;
	virtual		void 					GetAxisSDfactors			(float& spring_factor,float& damping_factor,int axis_num)		  				=0;
	virtual		void 					GetJointSDfactors			(float& spring_factor,float& damping_factor)					  				=0;
	virtual		void 					GetLimits					(float& lo_limit,float& hi_limit,int axis_num)					  				=0;
	virtual		void 					GetAxisDir					(int num, Fvector3& axis,eVs& vs)									  				=0;
	virtual		void 					GetAxisDirDynamic			(int num, Fvector3& axis)											  				=0;
	virtual		void 					GetAnchorDynamic			(Fvector3& anchor)												  				=0;
public:
	static void script_register(lua_State*);
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
	CPHShellHolder*dbg_obj																																;
#endif
public:
inline					CKinematics					*PKinematics								()																{return m_pKinematics		;}
					void						set_Kinematics								(CKinematics* p)														{m_pKinematics=p	;}
	virtual			void						set_JointResistance							(float force)																				= 0;
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
	virtual			void						applyImpulseTrace							(const Fvector3& pos, const Fvector3& dir, float val)											= 0;
	virtual			void						applyImpulseTrace							(const Fvector3& pos, const Fvector3& dir, float val,const U16 id)							= 0;
	virtual			void						applyHit									(const Fvector3& pos, const Fvector3& dir, float val,const U16 id,ALife::EHitType hit_type)	= 0;
	virtual			BoneCallbackFun*			GetBonesCallback							()																							= 0;
	virtual			BoneCallbackFun*			GetStaticObjectBonesCallback				()																							= 0;
	virtual			void						Update										()																							= 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual			void						setMass1									(float M)																					= 0;
	virtual			void						SmoothElementsInertia						(float k)																					= 0;
	virtual			void						setEquelInertiaForEls						(const dMass& M)																			= 0;
	virtual			void						addEquelInertiaToEls						(const dMass& M)																			= 0;
	virtual			ELEMENT_STORAGE				&Elements									()																							= 0;
	virtual			CPhysicsElement				*get_Element								(U16 bone_id)																				= 0;
	virtual			CPhysicsElement				*get_Element								(const CSharedString& bone_name)																= 0;
	virtual			CPhysicsElement				*get_Element								(const char* bone_name)																			= 0;
	virtual			CPhysicsElement				*get_ElementByStoreOrder					(U16 num)																					= 0;
	virtual			U16							get_ElementsNumber							()																							= 0;
	virtual			CPHSynchronize				*get_ElementSync							(U16 element)																				= 0;
	virtual			CPhysicsJoint				*get_Joint									(U16 bone_id)																				= 0;
	virtual			CPhysicsJoint				*get_Joint									(const CSharedString& bone_name)																= 0;
	virtual			CPhysicsJoint				*get_Joint									(const char* bone_name)																			= 0;
	virtual			CPhysicsJoint				*get_JointByStoreOrder						(U16 num)																					= 0;
	virtual			U16							get_JointsNumber							()																							= 0;
	virtual			CCodeGeom*					get_GeomByID								(U16 bone_id)																				= 0;
	virtual			void						Freeze										()																							= 0;
	virtual			void						UnFreeze									()																							= 0;
	virtual			void						NetInterpolationModeON						()																							= 0;
	virtual			void						NetInterpolationModeOFF						()																							= 0;
	virtual			void						Disable										()																							= 0;
	virtual			void						DisableCollision							()																							= 0;
	virtual			void						EnableCollision								()																							= 0;
	virtual			void						SetRemoveCharacterCollLADisable				()																							= 0;
	virtual			void						DisableCharacterCollision					()																							= 0;
	virtual			void						PureStep									(float step = fixed_step)																	= 0;
	virtual			void						SetGlTransformDynamic						(const Fmatrix &form)																		= 0;
	virtual			void						CollideAll									()																							= 0;
	virtual			CPhysicsElement				*NearestToPoint								(const Fvector3& point)																		= 0;
	virtual			void						build_FromKinematics						(CKinematics* K,BONE_P_MAP* p_geting_map=NULL)												= 0;
	virtual			void						preBuild_FromKinematics						(CKinematics* K,BONE_P_MAP* p_geting_map=NULL)												= 0;
	virtual			void						Build										(bool disable=false)																		= 0;
	virtual			void						SetMaxAABBRadius							(float size)																				 {};
	virtual			void						AddTracedGeom								(U16 element=0, U16 geom=0)																	= 0;
	virtual			void						SetAllGeomTraced							()																							= 0;
	virtual			void						RunSimulation								(bool place_current_forms=true)																= 0;
	virtual			void						UpdateRoot									()																							= 0;
	virtual			void            		    ZeroCallbacks								()																							= 0;
	virtual			void						ResetCallbacks								(U16 id,Flags64 &mask)																		= 0;
	virtual			void						SetCallbacks								(BoneCallbackFun* callback)																	= 0;
	virtual			void						EnabledCallbacks							(BOOL val)																					= 0;
	virtual			void						ToAnimBonesPositions						()																							= 0;
	virtual			bool						AnimToVelocityState							(float dt, float l_limit, float a_limit )													= 0;
	virtual 		void						SetBonesCallbacksOverwrite					(bool v)																					= 0;
	virtual			Fmatrix						&ObjectInRoot								()																							= 0;
	virtual			void						ObjectToRootForm							(const Fmatrix& form)							    										= 0;
	virtual			void						SetPrefereExactIntegration					()																							= 0;
	virtual										~CPhysicsShell								()																							;
	//build_FromKinematics		in returns elements  & joint pointers according bone IDs;
public:
	static void script_register(lua_State*);
};

add_to_type_list(CPhysicsShell)
#undef script_type_list
#define script_type_list save_type_list(CPhysicsShell)

void	get_box(CPhysicsShell*	shell,const	Fmatrix& form, Fvector3&	sz, Fvector3&	c);

// Implementation creator
CPhysicsJoint*				P_create_Joint				(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second)		;
CPhysicsElement*			P_create_Element			()																					;
CPhysicsShell*				P_create_Shell				()																					;
CPhysicsShell*				P_create_splited_Shell		()																					;
CPhysicsShell*				P_build_Shell				(CGameObject* obj,bool not_active_state, const char* fixed_bones)						;
CPhysicsShell*				P_build_Shell				(CGameObject* obj,bool not_active_state,U16Vec& fixed_bones)						;
CPhysicsShell*				P_build_Shell				(CGameObject* obj,bool not_active_state,BONE_P_MAP* bone_map, const char* fixed_bones)	;
CPhysicsShell*				P_build_Shell				(CGameObject* obj,bool not_active_state,BONE_P_MAP* bone_map=NULL)					;
CPhysicsShell*				P_build_SimpleShell			(CGameObject* obj,float mass,bool not_active_state)									;
		void				ApplySpawnIniToPhysicShell	(CIniFile* ini,CPhysicsShell* physics_shell,bool fixed)								;
		void				fix_bones					(const char* fixed_bones,CPhysicsShell* shell )											;

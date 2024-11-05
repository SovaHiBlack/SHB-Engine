/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////Implemetation//for//CPhysicsElement//////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Geometry.h"
#include "phdefs.h"
#include "PhysicsCommon.h"
#include "PHSynchronize.h"
#include "PHDisabling.h"
#include "PHGeometryOwner.h"
#include "PHInterpolation.h"

class CPHElement;
class CPHShell;
class CPHFracture;
struct SPHImpact;
class CPHFracturesHolder;

class CPHElement	:  
	public	CPhysicsElement ,
	public	CPHSynchronize,
	public	CPHDisablingFull,
	public	CPHGeometryOwner
{
	friend class CPHFracturesHolder;

	//f32						m_start_time;				//uu ->to shell ??	//aux
	dMass						m_mass;						//e ??				//bl
	dBodyID						m_body;						//e					//st
	dReal						m_l_scale;					// ->to shell ??	//bl
	dReal						m_w_scale;					// ->to shell ??	//bl
	CPHElement					*m_parent_element;			//bool !			//bl
	CPHShell					*m_shell;					//e					//bl
	CPHInterpolation			m_body_interpolation;		//e					//bl
	CPHFracturesHolder			*m_fratures_holder;			//e					//bl

	dReal						m_w_limit ;					//->to shell ??		//bl
	dReal						m_l_limit ;					//->to shell ??		//bl
//	dVector3					m_safe_position;			//e					//st
//	dQuaternion					m_safe_quaternion;
//	dVector3					m_safe_velocity;			//e					//st
//	fMatrix4x4						m_inverse_local_transform;	//e				//bt
	dReal						k_w;						//->to shell ??		//st
	dReal						k_l;						//->to shell ??		//st
	//ObjectContactCallbackFun*	temp_for_push_out;			//->to shell ??		//aux
	//u32							push_untill;				//->to shell ??		//st
	flags8						m_flags;					//
	enum				
	{
		flActive				=	1<<0,
		flActivating			=	1<<1,
		flUpdate				=	1<<2,
		flWasEnabledBeforeFreeze=	1<<3,
		flEnabledOnStep			=	1<<4,
		flFixed					=	1<<5
	};

private:
////////////////////////////////////////////Interpolation/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void					FillInterpolation				()																				//interpolation called anywhere visual influent
	{
		m_body_interpolation.ResetPositions();
		m_body_interpolation.ResetRotations();
		//bUpdate=true;
		m_flags.set(flUpdate,TRUE);
	}
IC	void					UpdateInterpolation				()																				//interpolation called from ph update visual influent
	{
		///VERIFY(dBodyStateValide(m_body));
		m_body_interpolation.UpdatePositions();
		m_body_interpolation.UpdateRotations();
		//bUpdate=true;
		m_flags.set(flUpdate,TRUE);
	}
public:
////////////////////////////////////////////////Geometry/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual	void						add_Sphere								(const fSphere&		V);															//aux
	virtual	void						add_Box									(const fObb&		V);															//aux
	virtual	void						add_Cylinder							(const fCylinder&	V);															//aux
	virtual void						add_Shape								(const SBoneShape& shape);														//aux
	virtual void						add_Shape								(const SBoneShape& shape,const fMatrix4x4& offset);								//aux
	virtual CODEGeom*					last_geom								(){return CPHGeometryOwner::last_geom();}										//aux
	virtual bool						has_geoms								(){return CPHGeometryOwner::has_geoms();}
	virtual void						set_ContactCallback						(ContactCallbackFun* callback);													//aux (may not be)
	virtual void						set_ObjectContactCallback				(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
	virtual void						add_ObjectContactCallback				(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
	virtual void						remove_ObjectContactCallback			(ObjectContactCallbackFun* callback);
	virtual void						set_CallbackData						(void * cd);
	virtual	void						*get_CallbackData						();
	virtual	ObjectContactCallbackFun	*get_ObjectContactCallback				();
	virtual void						set_PhysicsRefObject					(CPhysicsShellHolder* ref_object);												//aux
	virtual CPhysicsShellHolder*		PhysicsRefObject						(){return m_phys_ref_object;}													//aux
	virtual void						SetMaterial								(u16 m);																		//aux
	virtual void						SetMaterial								(pcstr m){CPHGeometryOwner::SetMaterial(m);}									//aux
	virtual u16							numberOfGeoms							();																				//aux
	virtual const fVector3&				local_mass_Center						()		{return CPHGeometryOwner::local_mass_Center();}							//aux
	virtual f32						getVolume								()		{return CPHGeometryOwner::get_volume();}								//aux
	virtual void						get_Extensions							(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext);			//aux
	virtual	void						get_MaxAreaDir							(fVector3& dir){CPHGeometryOwner::get_MaxAreaDir(dir);}
	virtual f32						getRadius								();
////////////////////////////////////////////////////Mass/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	void								calculate_it_data						(const fVector3& mc, f32 mass);													//aux
	void								calculate_it_data_use_density			(const fVector3& mc, f32 density);												//aux
	void								calc_it_fract_data_use_density  		(const fVector3& mc, f32 density);//sets element mass and fractures parts mass	//aux
	dMass								recursive_mass_summ						(u16 start_geom,FRACTURE_I cur_fracture);										//aux
public:																																				//
	virtual const fVector3&				mass_Center								()						;														//aux
	virtual void						setDensity								(f32 M);																		//aux
	virtual f32						getDensity								(){return m_mass.mass/m_volume;}												//aux
	virtual void						setMassMC								(f32 M,const fVector3& mass_center);											//aux
	virtual void						setDensityMC							(f32 M,const fVector3& mass_center);											//aux
	virtual void						setInertia								(const dMass& M);																//aux
	virtual void						addInertia								(const dMass& M);
	virtual void						add_Mass								(const SBoneShape& shape,const fMatrix4x4& offset,const fVector3& mass_center, f32 mass,CPHFracture* fracture=NULL);//aux
	virtual	void						set_BoxMass								(const fObb& box, f32 mass);													//aux
	virtual void						setMass									(f32 M);																		//aux
	virtual f32						getMass									(){return m_mass.mass;}															//aux
	virtual	dMass*						getMassTensor							();	//aux
			void						ReAdjustMassPositions					(const fMatrix4x4& shift_pivot, f32 density);										//aux
			void						ResetMass								(f32 density);																//aux
			void						CutVelocity								(f32 l_limit, f32 a_limit);
///////////////////////////////////////////////////PushOut///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

public:																																				//
//////////////////////////////////////////////Disable/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void						Disable									()	;																			//
	virtual	void						ReEnable								()	;																			//
			void						Enable									()	;																			//aux
	virtual bool						isEnabled								()	{return isActive()&&dBodyIsEnabled(m_body);}
	virtual	bool						isFullActive							()	{return isActive()&&!m_flags.test(flActivating);}
	virtual	bool						isActive								()	{return !!m_flags.test(flActive);}
	virtual void						Freeze									()	;																			//
	virtual void						UnFreeze								()	;																			//
	virtual bool						EnabledStateOnStep						()  {return dBodyIsEnabled(m_body)||m_flags.test(flEnabledOnStep);}							//
////////////////////////////////////////////////Updates///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			bool						AnimToVel								(f32 dt, f32 l_limit, f32 a_limit );
			void						BoneGlPos								(fMatrix4x4& m,CBoneInstance* B);
			void						ToBonePos								(CBoneInstance* B);

			void						SetBoneCallbackOverwrite				(bool v);
			void						BonesCallBack							(CBoneInstance* B);																//called from updateCL visual influent
			void						StataticRootBonesCallBack				(CBoneInstance* B);
			void						PhDataUpdate							(dReal step);																	//ph update
			void						PhTune									(dReal step);																	//ph update
	virtual void						Update									();																				//called update CL visual influence
//////////////////////////////////////////////////Dynamics////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void						SetAirResistance						(dReal linear=default_k_l, dReal angular=default_k_w)							//aux (may not be)
	{																																				//
		k_w= angular;																																//
		k_l=linear;																																	//
	}																																				//
	virtual	void						GetAirResistance				(f32& linear, f32& angular)															//
	{																																				//
		linear= k_l;																																//
		angular=k_w;																																//
	}	
	virtual void						applyImpact						(const SPHImpact& impact);																																	//
	virtual void						applyImpulseTrace				(const fVector3& pos, const fVector3& dir, f32 val,const u16 id)	;					//called anywhere ph state influent
	virtual	void						set_DisableParams				(const SAllDDOParams& params)										;					//
	virtual void						set_DynamicLimits				(f32 l_limit=default_l_limit, f32 w_limit=default_w_limit);							//aux (may not be)
	virtual void						set_DynamicScales				(f32 l_scale=default_l_scale, f32 w_scale=default_w_scale);							//aux (may not be)
	virtual	void						Fix								();
	virtual	void						ReleaseFixed					();
	virtual bool						isFixed							(){return !!(m_flags.test(flFixed));}
	virtual void						applyForce						(const fVector3& dir, f32 val);															//aux
	virtual void						applyForce						(f32 x, f32 y, f32 z);																//called anywhere ph state influent
	virtual void						applyImpulse					(const fVector3& dir, f32 val);//aux
	virtual void						applyImpulseVsMC				(const fVector3& pos,const fVector3& dir, f32 val);										//
	virtual void						applyImpulseVsGF				(const fVector3& pos,const fVector3& dir, f32 val);										//
	virtual void						applyGravityAccel				(const fVector3& accel);
	virtual void						getForce						(fVector3& force);
	virtual void						getTorque						(fVector3& torque);
	virtual void						get_LinearVel					(fVector3& velocity);															//aux
	virtual void						get_AngularVel					(fVector3& velocity);															//aux
	virtual void						set_LinearVel					(const fVector3& velocity);														//called anywhere ph state influent
	virtual void						set_AngularVel					(const fVector3& velocity);														//called anywhere ph state influent
	virtual void						setForce						(const fVector3& force);															//
	virtual void						setTorque						(const fVector3& torque);														//
	virtual void						set_ApplyByGravity				(bool flag)			;															//
	virtual bool						get_ApplyByGravity				()					;															//
///////////////////////////////////////////////////Net////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void						get_State						(		SPHNetState&	state);													//
	virtual void						set_State						(const	SPHNetState&	state);													//
	virtual	void						net_Import						(NET_Packet& P)				  ;
	virtual	void						net_Export						(NET_Packet& P)				  ;
///////////////////////////////////////////////////Position///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void						SetTransform					(const fMatrix4x4& m0);															//
	virtual void						TransformPosition				(const fMatrix4x4& form);
	virtual void						getQuaternion					(fQuaternion& quaternion);														//
	virtual void						setQuaternion					(const fQuaternion& quaternion);												//
	virtual void						SetGlobalPositionDynamic		(const fVector3& position);														//
	virtual void						GetGlobalPositionDynamic		(fVector3* v);																	//
	virtual void						cv2obj_Xfrom					(const fQuaternion& q,const fVector3& pos, fMatrix4x4& xform);						//
	virtual void						cv2bone_Xfrom					(const fQuaternion& q,const fVector3& pos, fMatrix4x4& xform);						//
	virtual void						InterpolateGlobalTransform		(fMatrix4x4* m);																	//called UpdateCL vis influent
	virtual void						InterpolateGlobalPosition		(fVector3* v);																	//aux
	virtual void						GetGlobalTransformDynamic		(fMatrix4x4* m);																	//aux
IC			void						InverceLocalForm				(fMatrix4x4&)	;
IC			void						MulB43InverceLocalForm			(fMatrix4x4&)	;

////////////////////////////////////////////////////Structure/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual CPhysicsShell*				PhysicsShell					();																				//aux
			CPHShell*					PHShell							();
	virtual void						set_ParentElement				(CPhysicsElement* p){m_parent_element=(CPHElement*)p;}							//aux
	void								SetShell						(CPHShell* p);																	//aux
	virtual	dBodyID						get_body				()		{return m_body;};																//aux
	virtual	const dBodyID				get_bodyConst			()const	{return m_body;};																//aux
//////////////////////////////////////////////////////Breakable//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	IC CPHFracturesHolder*				FracturesHolder							(){return m_fratures_holder;}											//aux
	IC const CPHFracturesHolder*		constFracturesHolder				()const{return m_fratures_holder;}										//aux
	void								DeleteFracturesHolder					();																		//
	virtual bool						isBreakable								();																		//aux
	virtual u16							setGeomFracturable						(CPHFracture& fracture);												//aux
	virtual CPHFracture&				Fracture								(u16 num);																//aux
			void						SplitProcess							(ELEMENT_PAIR_VECTOR &new_elements);									//aux
			void						PassEndGeoms							(u16 from,u16 to,CPHElement* dest);										//aux
////////////////////////////////////////////////////Build/Activate////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void						Activate				(const fMatrix4x4& m0, f32 dt01, const fMatrix4x4& m2,bool disable=false);					//some isues not to be aux
	virtual void						Activate				(const fMatrix4x4& transform,const fVector3& lin_vel,const fVector3& ang_vel,bool disable=false);//some isues not to be aux
	virtual void						Activate				(bool disable=false);									//some isues not to be aux
	virtual void						Activate				(const fMatrix4x4& start_from, bool disable=false);										//some isues not to be aux
	virtual void						Deactivate				();																						//aux																																			//aux
			void						CreateSimulBase			();//create body & cpace																//aux
			void						ReInitDynamics			(const fMatrix4x4& shift_pivot, f32 density);												//set body & geom positions					
			void						PresetActive			();																						//
			void						build									();																		//aux
			void						build									(bool disable);															//aux
			void						destroy									();																		//called anywhere ph state influent
			void						Start									();																		//aux
			void						RunSimulation							();																		//called anywhere ph state influent
			void						RunSimulation							(const fMatrix4x4& start_from);											//
			void						ClearDestroyInfo						();
			void						GetAnimBonePos							(fMatrix4x4& bp);
		CPHElement										();																						//aux
	virtual ~CPHElement								();																						//aux
};

IC CPHElement* cast_PHElement(CPhysicsElement* e){return static_cast<CPHElement*>(static_cast<CPhysicsElement*>(e));}
IC CPHElement* cast_PHElement(void* e){return static_cast<CPHElement*>(static_cast<CPhysicsElement*>(e));}
IC CPhysicsElement* cast_PhysicsElement(CPHElement* e){return static_cast<CPhysicsElement*>(static_cast<CPHElement*>(e));}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////Implemetation//for//CPhysicsElement//////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Geometry.h"
#include "PHDefs.h"
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

	//float						m_start_time;				//uu ->to shell ??	//aux
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
//	Fmatrix						m_inverse_local_transform;	//e				//bt
	dReal						k_w;						//->to shell ??		//st
	dReal						k_l;						//->to shell ??		//st
	//ObjectContactCallbackFun*	temp_for_push_out;			//->to shell ??		//aux
	//u32							push_untill;				//->to shell ??		//st
	Flags8						m_flags;					//
	enum				
	{
		flActive				=	1<<0,
		flActivating			=	1<<1,
		flUpdate				=	1<<2,
		flWasEnabledBeforeFreeze=	1<<3,
		flEnabledOnStep			=	1<<4,
		flFixed					=	1<<5
	};
//	bool						was_enabled_before_freeze;
//	bool						bUpdate;					//->to shell ??		//st
//	bool						b_enabled_onstep;
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
inline	void					UpdateInterpolation				()																				//interpolation called from ph update visual influent
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
	virtual	void						add_Sphere								(const Fsphere&		V);															//aux
	virtual	void						add_Box									(const Fobb&		V);															//aux
	virtual	void						add_Cylinder							(const Fcylinder&	V);															//aux
	virtual void						add_Shape								(const SBoneShape& shape);														//aux
	virtual void						add_Shape								(const SBoneShape& shape,const Fmatrix& offset);								//aux
	virtual CCodeGeom*					last_geom								(){return CPHGeometryOwner::last_geom();}										//aux
	virtual bool						has_geoms								(){return CPHGeometryOwner::has_geoms();}
	virtual void						set_ContactCallback						(ContactCallbackFun* callback);													//aux (may not be)
	virtual void						set_ObjectContactCallback				(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
	virtual void						add_ObjectContactCallback				(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
	virtual void						remove_ObjectContactCallback			(ObjectContactCallbackFun* callback);
	virtual void						set_CallbackData						(void * cd);
	virtual	void						*get_CallbackData						();
	virtual	ObjectContactCallbackFun	*get_ObjectContactCallback				();
	virtual void						set_PhysicsRefObject					(CPHShellHolder* ref_object);												//aux
	virtual CPHShellHolder*		PhysicsRefObject						(){return m_phys_ref_object;}													//aux
	virtual void						SetMaterial								(U16 m);																		//aux
	virtual void						SetMaterial								(const char* m){CPHGeometryOwner::SetMaterial(m);}									//aux
	virtual U16							numberOfGeoms							();																				//aux
	virtual const Fvector3&				local_mass_Center						()		{return CPHGeometryOwner::local_mass_Center();}							//aux
	virtual float						getVolume								()		{return CPHGeometryOwner::get_volume();}								//aux
	virtual void						get_Extensions							(const Fvector3& axis,float center_prg,float& lo_ext, float& hi_ext);			//aux
	virtual	void						get_MaxAreaDir							(Fvector3& dir){CPHGeometryOwner::get_MaxAreaDir(dir);}
	virtual float						getRadius								();
////////////////////////////////////////////////////Mass/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	void								calculate_it_data						(const Fvector3& mc,float mass);													//aux
	void								calculate_it_data_use_density			(const Fvector3& mc,float density);												//aux
	void								calc_it_fract_data_use_density  		(const Fvector3& mc,float density);//sets element mass and fractures parts mass	//aux
	dMass								recursive_mass_summ						(U16 start_geom,FRACTURE_I cur_fracture);										//aux
public:																																				//
	virtual const Fvector3&				mass_Center								()						;														//aux
	virtual void						setDensity								(float M);																		//aux
	virtual float						getDensity								(){return m_mass.mass/m_volume;}												//aux
	virtual void						setMassMC								(float M,const Fvector3& mass_center);											//aux
	virtual void						setDensityMC							(float M,const Fvector3& mass_center);											//aux
	virtual void						setInertia								(const dMass& M);																//aux
	virtual void						addInertia								(const dMass& M);
	virtual void						add_Mass								(const SBoneShape& shape,const Fmatrix& offset,const Fvector3& mass_center,float mass,CPHFracture* fracture=NULL);//aux
	virtual	void						set_BoxMass								(const Fobb& box, float mass);													//aux
	virtual void						setMass									(float M);																		//aux
	virtual float						getMass									(){return m_mass.mass;}															//aux
	virtual	dMass*						getMassTensor							();	//aux
			void						ReAdjustMassPositions					(const Fmatrix &shift_pivot,float density);										//aux
			void						ResetMass								(float density);																//aux
			void						CutVelocity								(float l_limit,float a_limit);
///////////////////////////////////////////////////PushOut///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
private:																																			//
		
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
			bool						AnimToVel								( float dt, float l_limit,float a_limit );
			void						BoneGlPos								(Fmatrix &m,CBoneInstance* B);
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
	virtual	void						GetAirResistance				(float &linear, float &angular)															//
	{																																				//
		linear= k_l;																																//
		angular=k_w;																																//
	}	
	virtual void						applyImpact						(const SPHImpact& impact);																																	//
	virtual void						applyImpulseTrace				(const Fvector3& pos, const Fvector3& dir, float val,const U16 id)	;					//called anywhere ph state influent
	virtual	void						set_DisableParams				(const SAllDDOParams& params)										;					//
	virtual void						set_DynamicLimits				(float l_limit=default_l_limit,float w_limit=default_w_limit);							//aux (may not be)
	virtual void						set_DynamicScales				(float l_scale=default_l_scale,float w_scale=default_w_scale);							//aux (may not be)
	virtual	void						Fix								();
	virtual	void						ReleaseFixed					();
	virtual bool						isFixed							(){return !!(m_flags.test(flFixed));}
	virtual void						applyForce						(const Fvector3& dir, float val);															//aux
	virtual void						applyForce						(float x,float y,float z);																//called anywhere ph state influent
	virtual void						applyImpulse					(const Fvector3& dir, float val);//aux
	virtual void						applyImpulseVsMC				(const Fvector3& pos,const Fvector3& dir, float val);										//
	virtual void						applyImpulseVsGF				(const Fvector3& pos,const Fvector3& dir, float val);										//
	virtual void						applyGravityAccel				(const Fvector3& accel);
	virtual void						getForce						(Fvector3& force);
	virtual void						getTorque						(Fvector3& torque);
	virtual void						get_LinearVel					(Fvector3& velocity);															//aux
	virtual void						get_AngularVel					(Fvector3& velocity);															//aux
	virtual void						set_LinearVel					(const Fvector3& velocity);														//called anywhere ph state influent
	virtual void						set_AngularVel					(const Fvector3& velocity);														//called anywhere ph state influent
	virtual void						setForce						(const Fvector3& force);															//
	virtual void						setTorque						(const Fvector3& torque);														//
	virtual void						set_ApplyByGravity				(bool flag)			;															//
	virtual bool						get_ApplyByGravity				()					;															//
///////////////////////////////////////////////////Net////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void						get_State						(		SPHNetState&	state);													//
	virtual void						set_State						(const	SPHNetState&	state);													//
	virtual	void						net_Import						(CNetPacket& P)				  ;
	virtual	void						net_Export						(CNetPacket& P)				  ;
///////////////////////////////////////////////////Position///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void						SetTransform					(const Fmatrix& m0);															//
	virtual void						TransformPosition				(const Fmatrix &form);
	virtual void						getQuaternion					(Fquaternion& quaternion);														//
	virtual void						setQuaternion					(const Fquaternion& quaternion);												//
	virtual void						SetGlobalPositionDynamic		(const Fvector3& position);														//
	virtual void						GetGlobalPositionDynamic		(Fvector3* v);																	//
	virtual void						cv2obj_Xfrom					(const Fquaternion& q,const Fvector3& pos, Fmatrix& xform);						//
	virtual void						cv2bone_Xfrom					(const Fquaternion& q,const Fvector3& pos, Fmatrix& xform);						//
	virtual void						InterpolateGlobalTransform		(Fmatrix* m);																	//called UpdateCL vis influent
	virtual void						InterpolateGlobalPosition		(Fvector3* v);																	//aux
	virtual void						GetGlobalTransformDynamic		(Fmatrix* m);																	//aux
inline			void						InverceLocalForm				(Fmatrix&)	;
inline			void						MulB43InverceLocalForm			(Fmatrix&)	;

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
	inline CPHFracturesHolder*				FracturesHolder							(){return m_fratures_holder;}											//aux
	inline const CPHFracturesHolder*		constFracturesHolder				()const{return m_fratures_holder;}										//aux
	void								DeleteFracturesHolder					();																		//
	virtual bool						isBreakable								();																		//aux
	virtual U16							setGeomFracturable						(CPHFracture& fracture);												//aux
	virtual CPHFracture&				Fracture								(U16 num);																//aux
			void						SplitProcess							(ELEMENT_PAIR_VECTOR &new_elements);									//aux
			void						PassEndGeoms							(U16 from, U16 to,CPHElement* dest);										//aux
////////////////////////////////////////////////////Build/Activate////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void						Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);					//some isues not to be aux
	virtual void						Activate				(const Fmatrix &transform,const Fvector3& lin_vel,const Fvector3& ang_vel,bool disable=false);//some isues not to be aux
	virtual void						Activate				(bool disable=false);									//some isues not to be aux
	virtual void						Activate				(const Fmatrix& start_from, bool disable=false);										//some isues not to be aux
	virtual void						Deactivate				();																						//aux																																			//aux
			void						CreateSimulBase			();//create body & cpace																//aux
			void						ReInitDynamics			(const Fmatrix &shift_pivot,float density);												//set body & geom positions					
			void						PresetActive			();																						//
			void						build									();																		//aux
			void						build									(bool disable);															//aux
			void						destroy									();																		//called anywhere ph state influent
			void						Start									();																		//aux
			void						RunSimulation							();																		//called anywhere ph state influent
			void						RunSimulation							(const Fmatrix& start_from);											//
			void						ClearDestroyInfo						();
			void						GetAnimBonePos							(Fmatrix &bp);
	//		bool						CheckBreakConsistent					()
	CPHElement										();																						//aux
	virtual ~CPHElement								();																						//aux
};

inline CPHElement* cast_PHElement(CPhysicsElement* e){return static_cast<CPHElement*>(static_cast<CPhysicsElement*>(e));}
inline CPHElement* cast_PHElement(void* e){return static_cast<CPHElement*>(static_cast<CPhysicsElement*>(e));}
inline CPhysicsElement* cast_PhysicsElement(CPHElement* e){return static_cast<CPhysicsElement*>(static_cast<CPHElement*>(e));}

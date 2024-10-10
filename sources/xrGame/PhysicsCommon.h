#pragma once

#include "DisablingParams.h"
#include "ode_include.h"

extern const dReal	default_l_limit									;
extern const dReal	default_w_limit									;
extern const dReal	default_k_l										;
extern const dReal	default_k_w										;
extern const dReal	default_l_scale									;
extern const dReal	default_w_scale									;

extern const dReal 		base_fixed_step								;
extern const dReal 		base_erp									;
extern const dReal 		base_cfm									;

extern dReal	fixed_step										;
extern dReal	world_cfm										;
extern dReal	world_erp										;
extern dReal	world_spring									;
extern dReal	world_damping									;

extern const dReal	mass_limit										;
extern const u16	max_joint_allowed_for_exeact_integration		;
extern const dReal	default_world_gravity							;
extern		 f32	phTimefactor									;
extern	int			phIterations									;
extern	f32		phBreakCommonFactor								;
extern	f32		phRigidBreakWeaponFactor						;
extern	f32		ph_tri_query_ex_aabb_rate						;
extern	int			ph_tri_clear_disable_count						;

struct SGameMtl;
#define ERP_S(k_p,k_d,s)		((s*(k_p)) / (((s)*(k_p)) + (k_d)))
#define CFM_S(k_p,k_d,s)		(1.f / (((s)*(k_p)) + (k_d)))
#define SPRING_S(cfm,erp,s)		((erp)/(cfm)/s)

//////////////////////////////////////////////////////////////////////////////////
#define DAMPING(cfm,erp)	((1.f-(erp))/(cfm))
#define ERP(k_p,k_d)		ERP_S(k_p,k_d,fixed_step)
#define CFM(k_p,k_d)		CFM_S(k_p,k_d,fixed_step)
#define SPRING(cfm,erp)		SPRING_S(cfm,erp,fixed_step)




IC f32 Erp(f32 k_p, f32 k_d, f32	s=fixed_step)		{return ((s*(k_p)) / (((s)*(k_p)) + (k_d)));}
IC f32 Cfm(f32 k_p, f32 k_d, f32	s=fixed_step)		{return (1.f / (((s)*(k_p)) + (k_d)));}
IC f32 Spring(f32 cfm, f32 erp, f32	s=fixed_step)	{return ((erp)/(cfm)/s);}
IC f32 Damping(f32 cfm, f32 erp)						{return ((1.f-(erp))/(cfm));}
IC void	 MulSprDmp(f32&cfm, f32&erp, f32 mul_spring, f32 mul_damping)
{
	f32 factor=1.f/(mul_spring*erp+mul_damping*(1-erp));
	cfm*=factor;
	erp*=(factor*mul_spring);
}
typedef void 	ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	void	ObjectContactCallbackFun(bool& do_colide,bool bo1,dContact& c,SGameMtl* material_1,SGameMtl* material_2);


typedef void	BoneCallbackFun(CBoneInstance* B);

extern ContactCallbackFun *ContactShotMark;
extern ContactCallbackFun *CharacterContactShotMark;

typedef	void	PhysicsStepTimeCallback		(u32	step_start,u32	step_end);
extern			PhysicsStepTimeCallback		*physics_step_time_callback;

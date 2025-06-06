#include "stdafx.h"
#include "ElevatorState.h"
#include "ClimableObject.h"
#include "PHCharacter.h"
#include "MathUtils.h"
#include "PHWorld.h"
#ifdef DEBUG
#include "..\XR_3DA\Statgraph.h"
#include "PHDebug.h"
#endif
static const f32 getting_on_dist		=0.3f;
static const f32 getting_out_dist		=0.4f;
static const f32 start_climbing_dist	=0.f;
static const f32 stop_climbing_dist	=0.1f;
static const f32 out_dist				=1.5f;

static const f32 look_angle_cosine	=0.9238795f;//22.5
static const f32 lookup_angle_sine	=0.34202014f;//20
extern	class CPHWorld	*ph_world;
CElevatorState::CElevatorState()
{
	m_state=clbNoLadder;
	m_ladder=NULL;
	m_character=NULL;
}

f32 CElevatorState::ClimbDirection()
{
	VERIFY(m_ladder&&m_character);
	fVector3 d;
	m_ladder->DToPlain(m_character,d);
	f32 dir=m_character->ControlAccel().dotproduct(d);
	if(dir> EPSILON_3)dir*=(m_character->CamDir().y+lookup_angle_sine);
	return dir;
}

void CElevatorState::PhTune(f32 step)
{	
	VERIFY(m_character&&m_character->b_exist&&m_character->is_active());
	if(!m_ladder)			return;
	switch(m_state)
	{
	case	clbNone			:UpdateStNone()			;		break;			
	case 	clbNearUp		:UpdateStNearUp()		;		break;						
	case 	clbNearDown		:UpdateStNearDown()		;		break;					
	case 	clbClimbingUp	:UpdateStClimbingUp()	;		break;					
	case 	clbClimbingDown	:UpdateStClimbingDown()	;		break;	
	case	clbDepart		:UpdateDepart()			;		break;
	case	clbNoLadder		:m_ladder = NULL		;		break;		
	}
}

void CElevatorState::PhDataUpdate(f32 step)
{
}

void CElevatorState::InitContact(dContact* c,bool &do_collide,u16 ,u16 )
{
}

void CElevatorState::SetElevator(CClimableObject* climable)
{
	fVector3 d;
	f32 dist=climable->DDToAxis(m_character,d);
	if(m_ladder==climable||dist>out_dist) return;
	if(m_ladder && m_ladder->DDToAxis(m_character,d)<dist) return;
	SwitchState(clbNone);
	m_ladder=climable;	
}
void CElevatorState::SetCharacter(CPHCharacter *character)
{
	m_character=character;
	SwitchState(clbNoLadder);
}
void CElevatorState::EvaluateState()
{
	VERIFY(m_ladder&&m_character);
}

#ifdef DEBUG
pcstr	dbg_state[] =	{
		"clbNone"			,				
		"clbNearUp"			,			
		"clbNearDown"		,		
		"clbClimbingUp"		,		
		"clbClimbingDown"	,	
		"clbDepart"			,
		"clbNoLadder"		
};
#endif
void CElevatorState::SwitchState(Estate new_state)
{
	if(!StateSwitchInertion(new_state))return;
#ifdef DEBUG
if(ph_dbg_draw_mask.test(phDbgLadder))
				Msg("%s",dbg_state[new_state]);
#endif
	VERIFY(m_character);
	if((m_state!=clbClimbingUp&&m_state!=clbClimbingDown) &&
	   (new_state==clbClimbingUp||new_state==clbClimbingDown)
 	   )dBodySetGravityMode(m_character->get_body(),0);

	if((new_state!=clbClimbingUp&&new_state!=clbClimbingDown) &&
		(m_state==clbClimbingUp||m_state==clbClimbingDown)
		)dBodySetGravityMode(m_character->get_body(),1);

	//if(new_state==clbDepart) InitDepart();
	NewState();
	m_state=new_state;
}
void CElevatorState::UpdateStNone()
{
	VERIFY(m_ladder&&m_character);
	fVector3 d;
	m_ladder->DToPlain(m_character,d);
	if(m_ladder->BeforeLadder(m_character)&&m_ladder->InTouch(m_character)&&dXZDotNormalized(d,m_character->CamDir())>look_angle_cosine)
	{
		if(ClimbDirection()>0.0f)
		{
			SwitchState(clbClimbingUp);
		}
		else
		{
			SwitchState(clbClimbingDown);
		}
	}
	else
	{
		fVector3 temp;
		f32 d_to_lower=m_ladder->DDLowerP(m_character,temp),d_to_upper=m_ladder->DDUpperP(m_character,temp);
		if(d_to_lower<d_to_upper)
		{
			if (getting_on_dist + m_character->FootRadius( ) > d_to_lower)
			{
				SwitchState(clbNearDown);
			}
		}
		else
		{
			if (getting_on_dist + m_character->FootRadius( ) > d_to_upper)
			{
				SwitchState(clbNearUp);
			}
		}
	}
}

void CElevatorState::UpdateStNearUp()
{
	VERIFY(m_ladder&&m_character);
	fVector3 d;
	
	if(	m_ladder->InTouch(m_character)								&&
		m_character->CamDir().y<-M_PI/20.f							&&
		//d.dotproduct(m_character->ControlAccel())<0.f&&
		//ClimbDirection()<0.f&&
		m_ladder->DDToPlain(m_character,d)>m_character->FootRadius()/3.f&&
		m_ladder->BeforeLadder(m_character,0.1f)
		)
		SwitchState(clbClimbingDown);
	f32 dist=m_ladder->DDUpperP(m_character,d);
	if(dist-m_character->FootRadius()>out_dist)SwitchState((clbNoLadder));
}

void CElevatorState::UpdateStNearDown()
{
	VERIFY(m_ladder&&m_character);
	fVector3 d;
	f32 dist=m_ladder->DDLowerP(m_character,d);
	if(	m_ladder->InTouch(m_character)&&
		dXZDotNormalized(d,m_character->CamDir())>look_angle_cosine&&
		d.dotproduct(m_character->ControlAccel())>0.f&&
		ClimbDirection()>0.f&&
		m_ladder->BeforeLadder(m_character)
		)SwitchState(clbClimbingUp);
	if(dist-m_character->FootRadius()>out_dist)SwitchState((clbNoLadder));
}

void CElevatorState::UpdateStClimbingDown()
{
	VERIFY(m_ladder&&m_character);
	fVector3 d;
	
	if(ClimbDirection()>0.f&&m_ladder->BeforeLadder(m_character))
		SwitchState(clbClimbingUp);
	f32 to_ax=m_ladder->DDToAxis(m_character,d);
	fVector3 ca;
	ca.set(m_character->ControlAccel());
	f32  control_a=to_mag_and_dir(ca);
	if (!fis_zero(to_ax) && !fis_zero(control_a) && abs(-ca.dotproduct(fVector3(m_ladder->Norm( )).normalize( ))) < M_SQRT1_2)
	{
		SwitchState(clbDepart);
	}

	if (m_ladder->AxDistToLowerP(m_character) - m_character->FootRadius( ) < stop_climbing_dist)
	{
		SwitchState(clbNearDown);
	}

	UpdateClimbingCommon(d,to_ax,ca,control_a);

	if (m_ladder->AxDistToUpperP(m_character) < -m_character->FootRadius( ))
	{
		SwitchState(clbNoLadder);
	}

	fVector3 vel;
	m_character->GetVelocity(vel);
	if(vel.y> EPSILON_7)
	{
		m_character->ApplyForce(0.0f,-m_character->Mass()*ph_world->Gravity(),0.0f);
	}
	//if(to_ax-m_character->FootRadius()>out_dist)
	//														SwitchState((clbNone));
	//if(fis_zero(control_a)) 
	//	m_character->ApplyForce(d,m_character->Mass());
}

void CElevatorState::UpdateStClimbingUp()
{
	VERIFY(m_ladder&&m_character);
	fVector3 d;

	if (ClimbDirection( ) < 0.f && m_ladder->BeforeLadder(m_character))
	{
		SwitchState(clbClimbingDown);
	}

	f32 to_ax=m_ladder->DDToAxis(m_character,d);
	fVector3 ca;
	ca.set(m_character->ControlAccel());
	f32 control_a=to_mag_and_dir(ca);
	if(!fis_zero(to_ax)&&!fis_zero(control_a)&&abs(-ca.dotproduct(fVector3(m_ladder->Norm()).normalize()))<M_SQRT1_2)SwitchState(clbDepart);
	if (m_ladder->AxDistToUpperP(m_character) + m_character->FootRadius( ) < stop_climbing_dist)
	{
		SwitchState(clbNearUp);
	}

	UpdateClimbingCommon(d,to_ax,ca,control_a);
	//if(to_ax-m_character->FootRadius()>out_dist)
	//										SwitchState((clbNone));
	//if(fis_zero(control_a)) 
	//	m_character->ApplyForce(d,m_character->Mass());
}
void CElevatorState::UpdateClimbingCommon(const fVector3& d_to_ax, f32 to_ax, const fVector3& control_accel, f32 ca)
{
	VERIFY(m_ladder&&m_character);
	if(to_ax-m_character->FootRadius()>out_dist)
										SwitchState((clbNoLadder));
	if(fis_zero(ca)&&d_to_ax.dotproduct(m_ladder->Norm())<0.f)
	{

#ifdef DEBUG
		if(ph_dbg_draw_mask.test(phDbgLadder))
		{
//.			Msg("force applied");
		}
#endif

		m_character->ApplyForce(d_to_ax,m_character->Mass()*ph_world->Gravity());//
	}
}
bool CElevatorState::GetControlDir(fVector3& dir)
{
	bool ret=true;
	VERIFY(m_ladder&&m_character);
	fVector3 d;
	f32 dist;
	switch(m_state)
	{
	case	clbDepart		: 
	case	clbNoLadder		:
	case	clbNone			: 		break;			
	case 	clbNearUp		:		dist= m_ladder->DDUpperP(m_character,d);
									if(	dXZDotNormalized(d,m_character->CamDir())>look_angle_cosine&&
										!fis_zero(dist, EPSILON_3)&&m_character->ControlAccel().dotproduct(d)>0.f) dir.set(d);
									break;						
	case 	clbNearDown		:		
									dist=m_ladder->DDLowerP(m_character,d);
									if(dXZDotNormalized(d,m_character->CamDir())>look_angle_cosine&&
									   !fis_zero(dist, EPSILON_3)&&m_character->ControlAccel().dotproduct(d)>0.f) dir.set(d);
									break;					
	case 	clbClimbingUp	:		m_ladder->DDAxis(dir);
									m_ladder->DDToAxis(m_character,d);
									dir.add(d);dir.normalize();
									break;					
	case 	clbClimbingDown	:		m_ladder->DDToAxis(m_character,d);
									if(m_ladder->BeforeLadder(m_character)||d.dotproduct(dir)>0.f)
									{
										m_ladder->DDAxis(dir);
										dir.invert();
										dir.add(d);dir.normalize();
									}
									else 
									{
#ifdef DEBUG
										if(ph_dbg_draw_mask.test(phDbgLadder))
										{
											Msg("no c dir");
										}
#endif
										ret=false;
									}
									break;				
	}
	return ret;
}
static const f32 depart_dist=2.0f;
static const u32   depart_time=3000;
void CElevatorState::UpdateDepart()
{
	VERIFY(m_ladder&&m_character);
	fVector3 temp;
	f32 d_to_lower = m_ladder->DDLowerP(m_character, temp);
	f32 d_to_upper = m_ladder->DDUpperP(m_character, temp);
	if(d_to_lower<d_to_upper)
	{
		if (getting_on_dist + m_character->FootRadius( ) > d_to_lower)
		{
			SwitchState(clbNearDown);
		}
	}
	else
	{
		if (getting_on_dist + m_character->FootRadius( ) > d_to_upper)
		{
			SwitchState(clbNearUp);
		}
	}

	//fVector3 p;m_character->GetFootCenter(p);
	//p.sub(m_start_position);
	//if(	p.magnitude()>depart_dist || 
	//	Device.dwTimeGlobal-m_start_time>depart_time)
		SwitchState(clbNoLadder);
}

void CElevatorState::NewState()
{
	VERIFY(m_character);
	m_start_time=Device.dwTimeGlobal;
	m_character->GetFootCenter(m_start_position);
}

void CElevatorState::Depart()
{
	VERIFY(m_character);
	if(m_ladder && ClimbingState())SwitchState(clbDepart);
}
void CElevatorState::GetLeaderNormal(fVector3& dir)
{
	if(!m_ladder)	return;
	VERIFY(m_ladder&&m_character);
	m_ladder->DDNorm(dir);
	//fVector3 d;
	//m_ladder->DToAxis(m_character,d);
	//if(dir.dotproduct(d)>0.f) dir.invert();
}

void CElevatorState::GetJumpDir(const fVector3& accel, fVector3& dir)
{
	VERIFY(m_ladder&&m_character);
	fVector3 norm;
	fVector3 side;
	m_ladder->DDNorm(norm);
	m_ladder->DDSide(side);
	fVector3 ac;
	ac.set(accel).normalize_safe();
	f32 side_component=ac.dotproduct(side);
	dir.set(norm);
	if(_abs(side_component)>M_SQRT1_2)
	{
		if(side_component<0.f)side.invert();
		dir.add(side);
		dir.normalize_safe();
	}
}

void CElevatorState::Deactivate()
{
	SwitchState(clbNoLadder);
	m_state=clbNoLadder;
	m_ladder=NULL;
	m_character=NULL;
}



CElevatorState::SEnertionState CElevatorState:: m_etable[CElevatorState::clbNoState][CElevatorState::clbNoState]=
{
//						clbNone			clbNearUp		clbNearDown		clbClimbingUp	clbClimbingDown	clbDepart	clbNoLadder
/*clbNone			*/	{{0,0},			{0,0},			{0,0},			{0,0},			{0,0},			{0,0},		{0,0}},							//clbNone			
/*clbNearUp			*/	{{0,0},			{0,0},			{0,0},			{0,0},			{0,0},			{0,0},		{0,0}},							//clbNearUp		
/*clbNearDown		*/	{{0,0},			{0.0f,0},		{0,0},			{0,0},			{0,0},			{0,0},		{0,0}},							//clbNearDown		
/*clbClimbingUp		*/	{{0,0},			{0,0},			{0,0},			{0,0},			{0,0},			{0,0},		{0,0}},							//clbClimbingUp	
/*clbClimbingDown	*/	{{0,0},			{0,0},			{0,0},			{0,0},			{0,0},			{0,0},		{0,0}},							//clbClimbingDown	
/*clbDepart			*/	{{0,0},			{0,0},			{0,0},			{0,0},			{0,0},			{0,0},		{depart_dist,depart_time}},		//clbDepart		
/*clbNoLadder		*/	{{0,0},			{0,0},			{0,0},			{0,0},			{0,0},			{0,0},		{0,0}} 							//clbNoLadder		
};

bool CElevatorState::StateSwitchInertion(Estate new_state)
{
	fVector3 p;
	m_character->GetFootCenter(p);
	p.sub(m_start_position);
	if(m_etable[m_state][new_state].dist<p.magnitude()||m_etable[m_state][new_state].time<Device.dwTimeGlobal-m_start_time) return true;
	else return false;
}
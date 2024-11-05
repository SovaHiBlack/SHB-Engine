#pragma once

class CPHCharacter;
struct dContact;
struct SGameMtl;
class CClimableObject;

class CElevatorState
{
public:	
	enum Estate
	{
		clbNone			=0	,
		clbNearUp			,
		clbNearDown			,
		clbClimbingUp		,
		clbClimbingDown		,
		clbDepart			,
		clbNoLadder			,
		clbNoState
	};
private:
	Estate m_state;

	struct  SEnertionState {
		f32 dist;
		u32	  time;
	};

static SEnertionState m_etable[CElevatorState::clbNoState][CElevatorState::clbNoState];

CClimableObject	*m_ladder;	
CPHCharacter	*m_character;
fVector3			m_start_position;//for depart state
u32				m_start_time;

public: 
						CElevatorState					();
			void		PhTune							(f32 step);
			void		SetCharacter					(CPHCharacter *character);
			void		SetElevator						(CClimableObject* climable);
			void		EvaluateState					();
			bool		GetControlDir					(fVector3& dir);
			void		GetJumpDir						(const fVector3& accel, fVector3& dir);
			void		GetLeaderNormal					(fVector3& dir);
			bool		Active							(){return m_ladder && m_state!=clbNone;}
			bool		NearDown						(){return m_state == clbNearDown;}
			bool		NearState						(){return m_state==clbNearUp || m_state == clbNearDown;}
			bool		ClimbingState					(){return m_state==clbClimbingUp || m_state == clbClimbingDown;}
			void		Depart							();
			f32		ClimbDirection					();
			void		Deactivate						();
IC			Estate		State							(){return m_state;}
private:
			void		NewState						();

			void		PhDataUpdate					(f32 step);
			void		InitContact						(dContact* c,bool &do_collide,u16 /*material_idx_1*/,u16 /*material_2*/);
			void		SwitchState						(Estate new_state);
			bool		StateSwitchInertion				(Estate new_state);
			void		UpdateStNone					();
			void		UpdateStNearUp					();
			void		UpdateStNearDown				();
			void		UpdateStClimbingUp				();
			void		UpdateStClimbingDown			();
			void		UpdateClimbingCommon(const fVector3& d_to_ax, f32 to_ax, const fVector3& control_accel, f32 ca);
			void		UpdateDepart					();
};

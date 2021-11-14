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
		clbNone = 0,
		clbNearUp,
		clbNearDown,
		clbClimbingUp,
		clbClimbingDown,
		clbDepart,
		clbNoLadder,
		clbNoState
	};
private:
	Estate m_state;

	struct  SEnertionState
	{
		float dist;
		u32	  time;
	};

	static SEnertionState m_etable[CElevatorState::clbNoState][CElevatorState::clbNoState];

	CClimableObject* m_ladder;
	CPHCharacter* m_character;
	Fvector3			m_start_position;//for depart state
	u32				m_start_time;
public:
	CElevatorState( );
	void		PhTune(float step);
	void		SetCharacter(CPHCharacter* character);
	void		SetElevator(CClimableObject* climable);
	void		EvaluateState( );
	bool		GetControlDir(Fvector3& dir);
	void		GetJumpDir(const Fvector3& accel, Fvector3& dir);
	void		GetLeaderNormal(Fvector3& dir);
	bool		Active( )
	{
		return m_ladder && m_state != clbNone;
	}
	bool		NearDown( )
	{
		return m_state == clbNearDown;
	}
	bool		NearState( )
	{
		return m_state == clbNearUp || m_state == clbNearDown;
	}
	bool		ClimbingState( )
	{
		return m_state == clbClimbingUp || m_state == clbClimbingDown;
	}
	void		Depart( );
	float		ClimbDirection( );
	void		Deactivate( );
	inline			Estate		State( )
	{
		return m_state;
	}
private:
	void		NewState( );

	void		PhDataUpdate(float step);
	void		InitContact(dContact* c, bool& do_collide, unsigned short /*material_idx_1*/, unsigned short /*material_2*/);
	void		SwitchState(Estate new_state);
	bool		StateSwitchInertion(Estate new_state);
	void		UpdateStNone( );
	void		UpdateStNearUp( );
	void		UpdateStNearDown( );
	void		UpdateStClimbingUp( );
	void		UpdateStClimbingDown( );
	void		UpdateClimbingCommon(const Fvector3& d_to_ax, float to_ax, const Fvector3& control_accel, float ca);
	void		UpdateDepart( );
};

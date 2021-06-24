#pragma once

class CUIMotionIcon : public CUIStatic
{
	using inherited										= CUIWindow;

public:
	enum EState
	{
		stNormal,
		stCrouch,
		stCreep,
		stClimb,
		stRun,
		stSprint,
		stLast
	};

private:
	EState												m_curren_state;
	CUIStatic											m_states[stLast];
	CUIProgressBar										m_power_progress;
	CUIProgressBar										m_luminosity_progress;
	CUIProgressBar										m_noise_progress;

	struct _npc_visibility
	{
		U16												id;
		float											value;
		bool operator ==								(const U16& _id)
		{
			return id == _id;
		}
		bool operator <									(const _npc_visibility& m) const
		{
			return (value < m.value);
		}
	};

	xr_vector<_npc_visibility>							m_npc_visibility;
	bool												m_bchanged;
	float												m_luminosity;

public:
	virtual						~CUIMotionIcon			( );
								CUIMotionIcon			( );

	virtual void				Update					( );
	void						Init					( );
	void						ShowState				(EState state);
	void						SetPower				(float Pos);
	void						SetNoise				(float Pos);
	void						SetLuminosity			(float Pos);
	void						SetActorVisibility		(U16 who_id, float value);
	void						ResetVisibility			( );
};

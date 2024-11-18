#pragma once

class CActor;
class CPostprocessAnimatorLerp;

class CZoneEffector
{
	f32						r_min_perc;
	f32						r_max_perc;
	f32						radius;
	f32						m_factor;
	CPostprocessAnimatorLerp* m_pp_effector;
	shared_str					m_pp_fname;

public:
	CZoneEffector( );
	~CZoneEffector( );

	CActor* m_pActor;

	void	Load(pcstr section);
	void	SetRadius(f32 r);
	void	Update(f32 dist);
	void	Stop( );
	f32 xr_stdcall GetFactor( );

private:
	void	Activate( );
};

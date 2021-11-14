#pragma once

class CCar;

//DEFINE_VECTOR(unsigned short,BIDS,BIDS_I);
using BIDS = xr_vector<unsigned short>;
using BIDS_I = BIDS::iterator;

struct CCarDamageParticles
{
	BIDS bones1;
	BIDS bones2;
	CSharedString							m_wheels_damage_particles1;
	CSharedString							m_wheels_damage_particles2;
	CSharedString							m_car_damage_particles1;
	CSharedString							m_car_damage_particles2;

public:
	void			Init(CCar* car);
	void			Clear( );
	void			Play1(CCar* car);
	void			Play2(CCar* car);
	void			PlayWheel1(CCar* car, unsigned short bone_id);
	void			PlayWheel2(CCar* car, unsigned short bone_id);
};
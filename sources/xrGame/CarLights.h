#pragma once

#include "..\XR_3DA\Render.h"

class CCarLights;
class CCar;

struct SCarLight
{
	ref_light				light_render;
	ref_glow				glow_render;
	u16						bone_id;
	CCarLights*				m_holder;
			SCarLight		();
			~SCarLight		();
	void	Switch			();
	void	TurnOn			();
	void	TurnOff			();
	bool	isOn			();
	void	Init			(CCarLights* holder);
	void	Update			();
	void	ParseDefinitions(pcstr section);
};

DEFINE_VECTOR(SCarLight*,LIGHTS_STORAGE,LIGHTS_I)
class CCarLights
{
public:
	void	ParseDefinitions	()								;
	void	Init				(CCar* pcar)					;
	void	Update				()								;
	CCar*	PCar				()								{return m_pcar;}
	void	SwitchHeadLights	()								;
	void	TurnOnHeadLights	()								;
	void	TurnOffHeadLights	()								;
	bool	IsLight				(u16 bone_id)					;
	bool	findLight			(u16 bone_id,SCarLight* &light);
			CCarLights			()								;
			~CCarLights			()								;
protected:
	struct SFindLightPredicate {
		const SCarLight *m_light;

		SFindLightPredicate	(const SCarLight *light) :
		m_light(light)
		{
		}

		bool		operator()					(const SCarLight *light) const
		{
			return light->bone_id==m_light->bone_id;
		}
	};
	LIGHTS_STORAGE	m_lights								;
	CCar*			m_pcar									;
/*
	iVector2		m_head_near_lights						;
	iVector2		m_head_far_lights						;
	iVector2		m_left_turns							;
	iVector2		m_stops									;
	iVector2		m_gabarites								;
	iVector2		m_door_gabarites						;
*/

};

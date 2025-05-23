#include "stdafx.h"
#include "CarLights.h"
#ifdef DEBUG
#include "ode_include.h"
#include "..\XR_3DA\StatGraph.h"
#include "PHDebug.h"
#endif
#include "alife_space.h"
#include "Hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "PHWorld.h"
extern CPHWorld*	ph_world;

SCarLight::SCarLight()
{
	light_render	=NULL;
	glow_render		=NULL;
	bone_id			=BI_NONE;
	m_holder		=NULL;
}

SCarLight::~SCarLight()
{

	light_render.destroy	()	;
	glow_render.destroy		()	;
	bone_id			=	BI_NONE	;
}

void SCarLight::Init(CCarLights* holder)
{
	m_holder=holder;
}

void SCarLight::ParseDefinitions(pcstr section)
{

	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
	glow_render				= ::Render->glow_create();
	//	lanim					= 0;
	//	time2hide				= 0;

	// set bone id
	CKinematics*			pKinematics=smart_cast<CKinematics*>(m_holder->PCar()->Visual());
	CIniFile* ini		=	pKinematics->LL_UserData();
	
	fColor					clr;
	clr.set					(ini->r_fcolor(section,"color"));
	//clr.mul_rgb				(torch->spot_brightness);
	//fBrightness				= torch->spot_brightness;
	light_render->set_range	(ini->r_float(section,"range"));
	light_render->set_color	(clr);
	light_render->set_cone	(deg2rad(ini->r_float(section,"cone_angle")));
	light_render->set_texture(ini->r_string(section,"spot_texture"));

	glow_render->set_texture(ini->r_string(section,"glow_texture"));
	glow_render->set_color	(clr);
	glow_render->set_radius	(ini->r_float(section,"glow_radius"));
	
	bone_id	= pKinematics->LL_BoneID(ini->r_string(section,"bone"));
	glow_render ->set_active(false);
	light_render->set_active(false);
	pKinematics->LL_SetBoneVisible(bone_id,FALSE,TRUE);

	//lanim					= LALib.FindItem(ini->r_string(section,"animator"));
	
}

void SCarLight::Switch()
{
	VERIFY(!ph_world->Processing());
	if(isOn())TurnOff();
	else	  TurnOn();
}
void SCarLight::TurnOn()
{
	VERIFY(!ph_world->Processing());
	if(isOn()) return;
	CKinematics* K=smart_cast<CKinematics*>(m_holder->PCar()->Visual());
	K->LL_SetBoneVisible(bone_id,TRUE,TRUE);
	K->CalculateBones_Invalidate	();
	K->CalculateBones();	
	glow_render ->set_active(true);
	light_render->set_active(true);
	Update();

}
void SCarLight::TurnOff()
{
	VERIFY(!ph_world->Processing());
	if(!isOn()) return;
 	glow_render ->set_active(false);
	light_render->set_active(false);
	smart_cast<CKinematics*>(m_holder->PCar()->Visual())->LL_SetBoneVisible(bone_id,FALSE,TRUE);
}

bool SCarLight::isOn()
{
	VERIFY(!ph_world->Processing());
	VERIFY(light_render->get_active()==glow_render->get_active());
	return light_render->get_active();
}

void SCarLight::Update()
{
	VERIFY(!ph_world->Processing());
	if(!isOn()) return;
	CCar* pcar=m_holder->PCar();
	CBoneInstance& BI = smart_cast<CKinematics*>(pcar->Visual())->LL_GetBoneInstance(bone_id);
	fMatrix4x4 M;
	M.mul(pcar->XFORM(),BI.mTransform);
	light_render->set_rotation	(M.k,M.i);
	glow_render->set_direction(M.k);
	glow_render->set_position	(M.c);
	light_render->set_position	(M.c);
}

CCarLights::CCarLights()
{
	m_pcar=NULL;
}

void CCarLights::Init(CCar* pcar)
{
	m_pcar=pcar;
	m_lights.clear();
}

void CCarLights::ParseDefinitions()
{
	CIniFile* ini= smart_cast<CKinematics*>(m_pcar->Visual())->LL_UserData();
	if(!ini->section_exist("lights")) return;
	pcstr S=  ini->r_string("lights","headlights");
	string64					S1;
	s32 count =					_GetItemCount(S);
	for (s32 i=0 ;i<count; ++i)
	{
		_GetItem					(S,i,S1);
		m_lights.push_back(xr_new<SCarLight>());
		m_lights.back()->Init(this);
		m_lights.back()->ParseDefinitions(S1);
	}
}

void CCarLights::Update()
{
	VERIFY(!ph_world->Processing());
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) (*i)->Update();
}

void CCarLights::SwitchHeadLights()
{
	VERIFY(!ph_world->Processing());
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) (*i)->Switch();
}

void CCarLights::TurnOnHeadLights()
{
	VERIFY(!ph_world->Processing());
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) (*i)->TurnOn();
}

void CCarLights::TurnOffHeadLights()
{
	VERIFY(!ph_world->Processing());
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) (*i)->TurnOff();
}

bool CCarLights::IsLight(u16 bone_id)
{
	SCarLight* light=NULL;
	return findLight(bone_id,light);
}

bool CCarLights::findLight(u16 bone_id,SCarLight* &light)
{
	LIGHTS_I i,e=m_lights.end();
	SCarLight find_light;
	find_light.bone_id=bone_id;
	i=std::find_if(m_lights.begin(),e,SFindLightPredicate(&find_light));
	light=*i;
	return i!=e;
}

CCarLights::~CCarLights()
{
	LIGHTS_I i =m_lights.begin(),e=m_lights.end();
	for(;i!=e;++i) xr_delete(*i);
	m_lights.clear();
}

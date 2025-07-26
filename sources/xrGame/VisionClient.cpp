////////////////////////////////////////////////////////////////////////////
//	Module 		: VisionClient.cpp
//	Description : vision client
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VisionClient.h"
#include "Entity.h"
#include "visual_memory_manager.h"

IC	const CEntity & CVisionClient::object		() const
{
	VERIFY						(m_object);
	return						(*m_object);
}

CVisionClient::CVisionClient(CEntity* object, const u32& update_interval) :
	m_object					(object)
{
	VERIFY						(m_object);

	m_visual					= xr_new<CVisualMemoryManager>(this);

	m_state						= 0;

	shedule.t_min				= update_interval;
	shedule.t_max				= shedule.t_min;
	shedule_register			();
}

CVisionClient::~CVisionClient()
{
	shedule_unregister			();
	xr_delete					(m_visual);
}

void CVisionClient::eye_pp_s01					()
{
	Device.Statistic->AI_Vis_Query.Begin		();
	
	fVector3					c;
	fVector3					k;
	fVector3					j;
	f32							field_of_view;
	f32							aspect_ratio;
	f32							near_plane;
	f32							far_plane;
	camera						(c, k, j, field_of_view, aspect_ratio, near_plane, far_plane);

	fMatrix4x4					mProject;
	fMatrix4x4					mFull;
	fMatrix4x4					mView;
	mView.build_camera_dir		(c, k, j);
	m_position					= c;
	mProject.build_projection	(field_of_view, aspect_ratio, near_plane, far_plane);
	mFull.mul					(mProject, mView);
	
	feel_vision_query			(mFull, c);

	Device.Statistic->AI_Vis_Query.End		();
}

void CVisionClient::eye_pp_s2					()
{
	Device.Statistic->AI_Vis_RayTests.Begin	();

	u32							dwTime = Device.dwTimeGlobal;
	u32							dwDT = dwTime - m_time_stamp;
	m_time_stamp				= dwTime;
	feel_vision_update			(m_object,m_position, f32(dwDT)/1000.f,visual().transparency_threshold());

	Device.Statistic->AI_Vis_RayTests.End	();
}

f32 CVisionClient::shedule_Scale				()
{
	return						(0.f);
}

void CVisionClient::shedule_Update				(u32 dt)
{
	inherited::shedule_Update	(dt);

	if (!object().g_Alive())
		return;

	switch (m_state) {
		case 0 : {
			m_state				= 1;
			eye_pp_s01			();
			break;
		}
		case 1 : {
			m_state				= 0;
			eye_pp_s2			();
			break;
		}
		default					: NODEFAULT;
	}

	visual().update				(f32(dt)/1000.f);
}

shared_str CVisionClient::shedule_Name			() const
{
	string256					temp;
	sprintf_s						(temp,"CVisionClient[%s]",*object().cName());
	return						(temp);
}

bool CVisionClient::shedule_Needed				()
{
	return						(true);
}

f32 CVisionClient::feel_vision_mtl_transp		(CObject* O, u32 element)
{
	return						(visual().feel_vision_mtl_transp(O,element));
}

void CVisionClient::reinit						()
{
	visual().reinit				();
}

void CVisionClient::reload						(pcstr section)
{
	visual().reload				(section);
}

void CVisionClient::remove_links				(CObject *object)
{
	visual().remove_links		(object);
}

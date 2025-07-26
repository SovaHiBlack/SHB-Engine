////////////////////////////////////////////////////////////////////////////
//	Module 		: VisionClient.h
//	Description : vision client
////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef VISION_CLIENT_H
#define VISION_CLIENT_H

#include "..\XR_3DA\feel_vision.h"

class CObject;
class CEntity;
class CVisualMemoryManager;

class CVisionClient :
	public ISheduled,
	public Feel::Vision
{
private:
	typedef ISheduled				inherited;

private:
	CEntity							*m_object;
	CVisualMemoryManager			*m_visual;

private:
	u32								m_state;
	u32								m_time_stamp;
	fVector3							m_position;

private:
	IC		const CEntity			&object					() const;

private:
			void					eye_pp_s01				();
			void					eye_pp_s2				();

public:
	CVisionClient(CEntity *object,const u32 &update_interval);
	virtual							~CVisionClient();

public:
	virtual f32					shedule_Scale			();
	virtual void					shedule_Update			(u32 dt);
	virtual	shared_str				shedule_Name			() const;
	virtual bool					shedule_Needed			();

public:
	virtual	f32					feel_vision_mtl_transp	(CObject *object, u32 element);

public:
	virtual	BOOL					feel_vision_isRelevant	(CObject *object) = 0;
	virtual	void					camera					(
		fVector3& position,
		fVector3& direction,
		fVector3& normal,
		f32& field_of_view,
		f32& aspect_ratio,
		f32& near_plane,
		f32& far_plane
									) = 0;


public:
	virtual	void					reinit					();
	virtual	void					reload					(pcstr section);
			void					remove_links			(CObject *object);

public:
	IC		CVisualMemoryManager	&visual					() const;
};

#include "VisionClient_inline.h"

#endif // VISION_CLIENT_H
#pragma once

#include "motion.h"

// refs
class ENGINE_API CObjectAnimator
{
private:
	DEFINE_VECTOR		(COMotion*,MotionVec,MotionIt);
protected:
	bool				bLoop;

	shared_str			m_Name;
	
	Fmatrix				m_XFORM;
	SAnimParams			m_MParam;
	MotionVec			m_Motions;
	f32				m_Speed;

	COMotion*			m_Current;
	void				LoadMotions		(pcstr fname);
	void				SetActiveMotion	(COMotion* mot);
	COMotion*			FindMotionByName(pcstr name);

public:
						CObjectAnimator	();
	virtual				~CObjectAnimator();

	void				Clear			();
	void				Load			(pcstr name);
	IC pcstr			Name			(){return *m_Name;}
	f32&				Speed			(){return m_Speed;}

	COMotion*			Play			(bool bLoop, pcstr name=0);
	void				Pause			(bool val){return m_MParam.Pause(val);}
	void				Stop			();
	IC BOOL				IsPlaying		(){return m_MParam.bPlay;}

	IC const Fmatrix&	XFORM			(){return m_XFORM;}
	f32				GetLength		();
	// Update
	void				Update			(f32 dt);
	void				DrawPath		();
};

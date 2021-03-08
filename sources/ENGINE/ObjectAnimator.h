#pragma once

#include "motion.h"

class ENGINE_API CObjectAnimator
{
private:
	using MotionVec							= xr_vector<COMotion*>;
	using MotionIt							= MotionVec::iterator;

protected:
	bool									bLoop;

	shared_str								m_Name;

	Fmatrix									m_XFORM;
	SAnimParams								m_MParam;
	MotionVec								m_Motions;
	float									m_Speed;

	COMotion*								m_Current;
	void				LoadMotions			(const char* fname);
	void				SetActiveMotion		(COMotion* mot);

public:
						CObjectAnimator		( );
	virtual				~CObjectAnimator	( );

	void				Clear				( );
	void				Load				(const char* name);
	inline const char*		Name				( )
	{
		return *m_Name;
	}
	float&				Speed				( )
	{
		return m_Speed;
	}

	COMotion*			Play				(bool bLoop, const char* name = 0);
	void				Pause				(bool val)
	{
		return m_MParam.Pause(val);
	}
	void				Stop				( );
	inline BOOL				IsPlaying			( )
	{
		return m_MParam.bPlay;
	}

	inline const Fmatrix&	XFORM				( )
	{
		return m_XFORM;
	}
	float				GetLength			( );
	//Update
	void				Update				(float dt);
};

#include "stdafx.h"

#include "ai_space.h"
#include "script_engine.h"
#include "ActorEffector.h"
#include "..\ENGINE\ObjectAnimator.h"

void CAnimatorCamEffectorScriptCB::ProcessIfInvalid(Fvector3& p, Fvector3& d, Fvector3& n, float& fFov, float& fFar, float& fAspect)
{
	if (m_bAbsolutePositioning)
	{
		const Fmatrix& m = m_objectAnimator->XFORM( );
		d = m.k;
		n = m.j;
		p = m.c;
	}
}

BOOL CAnimatorCamEffectorScriptCB::Valid( )
{
	BOOL res = inherited::Valid( );
	if (!res && cb_name.size( ))
	{
		luabind::functor<const char*> fl;
		R_ASSERT(ai( ).script_engine( ).functor<const char*>(*cb_name, fl));
		fl( );
		cb_name = "";
	}

	return res;
}

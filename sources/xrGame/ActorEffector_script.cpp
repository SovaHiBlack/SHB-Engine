#include "stdafx.h"

#include "ai_space.h"
#include "script_engine.h"
#include "ActorEffector.h"
#include "..\XR_3DA\ObjectAnimator.h"

void CAnimatorCamEffectorScriptCB::ProcessIfInvalid(Fvector &p, Fvector &d, Fvector &n, F32& fFov, F32& fFar, F32& fAspect)
{
	if(m_bAbsolutePositioning)
	{
		const Fmatrix& m			= m_objectAnimator->XFORM();
		d							= m.k;
		n							= m.j;
		p							= m.c;
	}
}

BOOL CAnimatorCamEffectorScriptCB::Valid()
{
	BOOL res = inherited::Valid();
	if(!res && cb_name.size() )
	{
		luabind::functor<pcstr>			fl;
		R_ASSERT							(ai().script_engine().functor<pcstr>(*cb_name,fl));
		fl									();
		cb_name								= "";
	}
	return res;
}
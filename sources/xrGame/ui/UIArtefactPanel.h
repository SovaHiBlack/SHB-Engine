#pragma once

#include "UIWindow.h"//
#include "UIStaticItem.h"//

class CUIXml;
class CArtefact;

class CUIArtefactPanel : public CUIWindow
{
	using ITr									= xr_vector<Frect>::const_iterator;
	using ITsi									= xr_vector<CUIStaticItem*>::const_iterator;

public:
						CUIArtefactPanel		( );
						~CUIArtefactPanel		( );

	virtual void		InitIcons				(const xr_vector<const CArtefact*>& artefacts);
	virtual void		Draw					( );
	void				InitFromXML				(CUIXml& xml, const char* path, int index);

protected:
	float										m_fScale;
	Fvector2									m_cell_size;
	xr_vector<Frect>							m_vRects;
	CUIStaticItem								m_si;
};

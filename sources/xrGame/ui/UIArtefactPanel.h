#pragma once
#include "UIWindow.h"
#include "UIStaticItem.h"

class CUIXml;
class CArtefact;

class CUIArtefactPanel : public CUIWindow
{
	typedef xr_vector<fRect>::const_iterator ITr;
	typedef xr_vector<CUIStaticItem*>::const_iterator ITsi;

public:
						CUIArtefactPanel		( );
						~CUIArtefactPanel		( );

	virtual void		InitIcons				(const xr_vector<const CArtefact*>& artefacts);
	virtual void		Draw					( );
	void				InitFromXML				(CUIXml& xml, pcstr path, s32 index);

protected:
	f32											m_fScale;
	fVector2									m_cell_size;
	xr_vector<fRect>							m_vRects;
	CUIStaticItem								m_si;
};

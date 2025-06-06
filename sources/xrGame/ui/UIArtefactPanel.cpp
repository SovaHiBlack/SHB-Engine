#include "stdafx.h"
#include "UIArtefactPanel.h"//
#include "UIInventoryUtilities.h"//
#include "UIXmlInit.h"
#include "../Artefact.h"

using namespace InventoryUtilities;

CUIArtefactPanel::CUIArtefactPanel( )
{ }

CUIArtefactPanel::~CUIArtefactPanel( )
{ }

void CUIArtefactPanel::InitFromXML(CUIXml& xml, pcstr path, s32 index)
{
	CUIXmlInit::InitWindow(xml, path, index, this);
	m_cell_size.x = xml.ReadAttribFlt(path, index, "cell_width");
	m_cell_size.y = xml.ReadAttribFlt(path, index, "cell_height");
	m_fScale = xml.ReadAttribFlt(path, index, "scale");
}

void CUIArtefactPanel::InitIcons(const xr_vector<const CArtefact*>& artefacts)
{
	m_si.SetShader(GetEquipmentIconsShader( ));
	m_vRects.clear( );

	for (xr_vector<const CArtefact*>::const_iterator it = artefacts.begin( ); it != artefacts.end( ); it++)
	{
		const CArtefact* artefact = *it;
		fRect rect;
		rect.left = f32(artefact->GetXPos( ) * INV_GRID_WIDTH);
		rect.top = f32(artefact->GetYPos( ) * INV_GRID_HEIGHT);
		rect.right = rect.left + artefact->GetGridWidth( ) * INV_GRID_WIDTH;
		rect.bottom = rect.top + artefact->GetGridHeight( ) * INV_GRID_HEIGHT;
		m_vRects.push_back(rect);
	}
}

void CUIArtefactPanel::Draw( )
{
	const f32 iIndent = 1.0f;
	f32 x = 0.0f;
	f32 y = 0.0f;
	f32 iHeight;
	f32 iWidth;

	fRect rect;
	GetAbsoluteRect(rect);
	x = rect.left;
	y = rect.top;

	f32 _s = m_cell_size.x / m_cell_size.y;

	for (ITr it = m_vRects.begin( ); it != m_vRects.end( ); ++it)
	{
		const fRect& r = *it;

		iHeight = m_fScale * (r.bottom - r.top);
		iWidth = _s * m_fScale * (r.right - r.left);

		m_si.SetOriginalRect(r.left, r.top, r.width( ), r.height( ));
		m_si.SetRect(0, 0, iWidth, iHeight);

		m_si.SetPos(x, y);
		x = x + iIndent + iWidth;

		m_si.Render( );
	}

	CUIWindow::Draw( );
}

#pragma once
#include "uiwindow.h"

class CInventoryItem;
class CUIStatic;
class CUIScrollView;
class CUIProgressBar;
class CUIWpnParams;
class CUIArtefactParams;

extern pcstr const 		fieldsCaptionColor;

class CUIItemInfo : public CUIWindow
{
private:
	typedef CUIWindow inherited;
	struct _desc_info
	{
		CGameFont* pDescFont;
		u32					uDescClr;
		bool				bShowDescrText;
	};
	_desc_info				m_desc_info;
	CInventoryItem* m_pInvItem;

public:
	CUIItemInfo( );
	virtual				~CUIItemInfo( );

	void				Init(f32 x, f32 y, f32 width, f32 height, pcstr xml_name);
	void				Init(pcstr xml_name);
	void				InitItem(CInventoryItem* pInvItem);
	void				TryAddWpnInfo(const shared_str& wpn_section);
	void				TryAddArtefactInfo(const shared_str& af_section);

	virtual void		Draw( );
	bool				m_b_force_drawing;
	CUIStatic* UIName;
	CUIStatic* UIWeight;
	CUIStatic* UICost;
	CUIStatic* UICondition;
	CUIScrollView* UIDesc;
	CUIProgressBar* UICondProgresBar;
	CUIWpnParams* UIWpnParams;
	CUIArtefactParams* UIArtefactParams;

	Fvector2			UIItemImageSize;
	CUIStatic* UIItemImage;
};

//////////////////////////////////////////////////////////////////////
// UIPdaMsgListItem.h: ������� ���� ������ � �������� 
// ������ ��� ��������� PDA
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIStatic.h"
#include "..\InventoryOwner.h"

class CUIPdaMsgListItem : public CUIStatic
{
public:
	virtual void		Init(f32 x, f32 y, f32 width, f32 height);
	virtual void		InitCharacter(CInventoryOwner* pInvOwner);
	virtual void		SetTextColor(u32 color);
	virtual void		SetFont(CGameFont* pFont);
	virtual void		SetColor(u32 color);

	// ���������� � ���������
	CUIStatic			UIIcon;
	CUIStatic			UIName;
	CUIStatic			UIMsgText;
};

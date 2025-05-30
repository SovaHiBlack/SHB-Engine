//////////////////////////////////////////////////////////////////////
// UIPdaListItem.h: ������� ���� ������ � PDA
// ��� ����������� ���������� � �������� PDA
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIWindow.h"

class CUIFrameWindow;
class CUICharacterInfo;
class CInventoryOwner;

class CUIPdaListItem : public CUIWindow
{
private:
	typedef CUIWindow inherited;

public:
	CUIPdaListItem( );
	virtual			~CUIPdaListItem( );
	virtual void	Init(f32 x, f32 y, f32 width, f32 height);
	virtual void	InitCharacter(CInventoryOwner* pInvOwner);

	pvoid m_data;

protected:
	//���������� � ���������
	CUIFrameWindow* UIMask;
	CUICharacterInfo* UIInfo;
};

// CUIEditBox.h: ���� ������ � ����������
// 
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UILabel.h"
#include "../script_export_space.h"
#include "UIOptionsItem.h"//
#include "UIColorAnimatorWrapper.h"//
#include "UICustomEdit.h"

//////////////////////////////////////////////////////////////////////////

class game_cl_GameState;

//////////////////////////////////////////////////////////////////////////

class CUIEditBox : public CUIMultiTextureOwner, public CUIOptionsItem, public CUICustomEdit
{
public:
	CUIEditBox( );
	virtual			~CUIEditBox( );

	virtual void	Init(f32 x, f32 y, f32 width, f32 heigt);

	// CUIOptionsItem
	virtual void	SetCurrentValue( );
	virtual void	SaveValue( );
	virtual bool	IsChanged( );

	// CUIMultiTextureOwner
	virtual void	InitTexture(pcstr texture);

protected:
	CUIFrameLineWnd	m_frameLine;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIEditBox)
#undef script_type_list
#define script_type_list save_type_list(CUIEditBox)

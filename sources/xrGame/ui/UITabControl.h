#pragma once

#include "UIWindow.h"
#include "../script_export_space.h"
#include "UIOptionsItem.h"//

class CUITabButton;
class CUIButton;

DEF_VECTOR(TABS_VECTOR, CUITabButton*)

class CUITabControl : public CUIWindow, public CUIOptionsItem
{
	typedef				CUIWindow inherited;

public:
	CUITabControl( );
	virtual				~CUITabControl( );

	// options item
	virtual void		SetCurrentValue( );
	virtual void		SaveValue( );
	virtual bool		IsChanged( );

	virtual bool		OnKeyboard(s32 dik, EUIMessages keyboard_action);
	virtual void		OnTabChange(s32 iCur, s32 iPrev);
	virtual void		OnStaticFocusReceive(CUIWindow* pWnd);
	virtual void		OnStaticFocusLost(CUIWindow* pWnd);

	// Добавление кнопки-закладки в список закладок контрола
	bool				AddItem(pcstr pItemName, pcstr pTexName, f32 x, f32 y, f32 width, f32 height);
	bool				AddItem(CUITabButton* pButton);

	void				RemoveItem(const u32 Index);
	void				RemoveAll( );

	virtual void		SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData);

	s32			GetActiveIndex( )
	{
		return m_iPushedIndex;
	}
	s32			GetPrevActiveIndex( )
	{
		return m_iPrevPushedIndex;
	}
	void		SetNewActiveTab(const s32 iNewTab);
	const	s32			GetTabsCount( ) const
	{
		return m_TabsArr.size( );
	}

	// Режим клавилатурных акселераторов (вкл/выкл)
	IC bool				GetAcceleratorsMode( ) const
	{
		return m_bAcceleratorsEnable;
	}
	void				SetAcceleratorsMode(bool bEnable)
	{
		m_bAcceleratorsEnable = bEnable;
	}

	TABS_VECTOR* GetButtonsVector( )
	{
		return &m_TabsArr;
	}
	CUIButton* GetButtonByIndex(s32 i);
	const shared_str	GetCommandName(s32 i);
	CUIButton* GetButtonByCommand(const shared_str& n);
	void		ResetTab( );

protected:
	// Список кнопок - переключателей закладок
	TABS_VECTOR			m_TabsArr;

	// Текущая нажатая кнопка. -1 - ни одна, 0 - первая, 1 - вторая, и т.д.
	s32					m_iPushedIndex;
	s32					m_iPrevPushedIndex;

	// Цвет неактивных элементов
	u32					m_cGlobalTextColor;
	u32					m_cGlobalButtonColor;

	// Цвет надписи на активном элементе
	u32					m_cActiveTextColor;
	u32					m_cActiveButtonColor;

	// Разрешаем/запрещаем клавиатурные акселераторы
	bool				m_bAcceleratorsEnable;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUITabControl)
#undef script_type_list
#define script_type_list save_type_list(CUITabControl)

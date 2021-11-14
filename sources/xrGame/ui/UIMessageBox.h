#pragma once

#include "UIStatic.h"//
#include "../script_export_space.h"

class CUI3tButton;
class CUIEditBox;

class CUIMessageBox : public CUIStatic
{
private:
	using inherited = CUIStatic;

public:
	CUIMessageBox( );
	virtual		~CUIMessageBox( );

	//разновидности MessageBox
	typedef enum
	{
		MESSAGEBOX_OK,
		MESSAGEBOX_INFO,
		MESSAGEBOX_YES_NO,
		MESSAGEBOX_YES_NO_CANCEL,
		MESSAGEBOX_QUIT_WINDOWS,
		MESSAGEBOX_QUIT_GAME
	} E_MESSAGEBOX_STYLE;

	void Init(const char* box_template);
	void Clear( );
	virtual void SetText(const char* str);
	virtual const char* GetText( );

	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void SendMessage(CUIWindow* pWnd, signed short msg, void* pData);

protected:
	xr_string	m_ret_val;
	CUI3tButton* m_UIButtonYesOk;
	CUI3tButton* m_UIButtonNo;
	CUI3tButton* m_UIButtonCancel;

	CUIStatic* m_UIStaticPicture;
	CUIStatic* m_UIStaticText;

	E_MESSAGEBOX_STYLE m_eMessageBoxStyle;

public:
	static void script_register(lua_State*);
};

add_to_type_list(CUIMessageBox)
#undef script_type_list
#define script_type_list save_type_list(CUIMessageBox)

// File:        UI3tButton.h
// Description: Button with 3 texutres (for <enabled>, <disabled> and <touched> states)

#pragma once

#include "UIButton.h"//
#include "UIInteractiveBackground_Static.h"//

class CUI3tButton : public CUIButton
{
	friend class CUIXmlInit;
	using CUIButton::SetTextColor;

public:
	CUI3tButton( );
	virtual			~CUI3tButton		( );
	// appearance
	using CUIButton::Init;

	virtual void 	Init				(float x, float y, float width, float height);
	virtual void 	InitTexture			(const char* tex_name);
	virtual void 	InitTexture			(const char* tex_enabled, const char* tex_disabled, const char* tex_touched, const char* tex_highlighted);

	void 			SetTextColor		(u32 color);
	void 			SetTextColorH		(u32 color);
	void 			SetTextColorD		(u32 color);
	void 			SetTextColorT		(u32 color);
	virtual void 	SetTextureOffset	(float x, float y);
	virtual void 	SetWidth			(float width);
	virtual void 	SetHeight			(float height);
	void 			InitSoundH			(const char* sound_file);
	void 			InitSoundT			(const char* sound_file);

	virtual void 	OnClick				( );
	virtual void 	OnFocusReceive		( );
	virtual void	OnFocusLost			( );

	// check button
	bool			GetCheck			( )
	{
		return m_eButtonState == BUTTON_PUSHED;
	}
	void			SetCheck			(bool ch)
	{
		m_eButtonState = ch ? BUTTON_PUSHED : BUTTON_NORMAL;
	}

	// behavior
	virtual void	DrawTexture			( );
	virtual void	Update				( );

	virtual bool 	OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual bool 	OnMouseDown			(int mouse_btn);
	void 			SetCheckMode		(bool mode)
	{
		m_bCheckMode = mode;
	}

	CUIStatic		m_hint;
	CUIInteractiveBackground_Static	m_background;

protected:
	bool			m_bCheckMode;

private:
	void			PlaySoundH			( );
	void			PlaySoundT			( );

	ref_sound		m_sound_h;
	ref_sound		m_sound_t;
};

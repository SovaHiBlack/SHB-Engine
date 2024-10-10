#pragma once

#include "UILabel.h"
#include "UIOptionsItem.h"//

struct _action;
struct _keyboard;
class CUIColorAnimatorWrapper;

class CUIEditKeyBind : public CUILabel, public CUIOptionsItem 
{
	bool			m_bPrimary;
	_action*		m_action;
	_keyboard*		m_keyboard;
public:
					CUIEditKeyBind			(bool bPrim);
	virtual			~CUIEditKeyBind			();
	// options item
	virtual void	Register				(pcstr entry, pcstr group);
	virtual void	SetCurrentValue			();
	virtual void	SaveValue				();
	virtual	void	OnMessage				(pcstr message);
	virtual bool	IsChanged				();

	// CUIWindow methods
	virtual void	Init					(f32 x, f32 y, f32 width, f32 height);
	virtual void	Update					();
	virtual bool	OnMouseDown				(int mouse_btn);
	virtual void	OnFocusLost				();
	virtual bool	OnKeyboard				(int dik, EUIMessages keyboard_action);
	// IUITextControl
	virtual void	SetText					(pcstr text);

protected:
	void			BindAction2Key			();
	virtual void	InitTexture				(pcstr texture, bool horizontal = true);

	bool		m_bEditMode;
	bool		m_bChanged;

	CUIColorAnimatorWrapper*				m_pAnimation;
};

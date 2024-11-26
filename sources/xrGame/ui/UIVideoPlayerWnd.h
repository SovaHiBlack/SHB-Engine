#pragma once
#include "UIDialogWnd.h"
#include "UIWndCallback.h"

class CUITabControl;
class CUIStatic;
class CUIXml;

class CUIVideoPlayerWnd : public CUIDialogWnd, public CUIWndCallback
{
	typedef CUIDialogWnd inherited;
	CUITabControl* m_tabControl;
	CUIStatic* m_surface;

protected:
	ref_sound		m_sound;
	ref_texture		m_texture;
	shared_str		m_fn;
	flags8			m_flags;
	enum
	{
		eStoping = (1 << 0),
		ePlaying = (1 << 1),
		eLooped = (1 << 2)
	};

private:
	void __stdcall	OnTabChanged(CUIWindow* pWnd, pvoid pData);

public:
	void			Init(CUIXml* doc, pcstr start_from);
	void			Init(pcstr name);
	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData);
	void			SetFile(pcstr fn);

	virtual void	Draw( );
	virtual void	Update( );
	void	Play( );
	void	Stop( );
	bool	IsPlaying( );
};

class CUIActorSleepVideoPlayer : public CUIVideoPlayerWnd
{
	typedef CUIVideoPlayerWnd inherited;

public:
	void	Activate( );
	void	DeActivate( );
	virtual bool	NeedCursor( ) const
	{
		return false;
	}
	virtual bool OnKeyboard(s32 dik, EUIMessages keyboard_action);
};

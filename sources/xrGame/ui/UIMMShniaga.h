#pragma once

#include "UIWindow.h"

class CUIAnimatedStatic;
class CUIStatic;
class CUIXml;
class CUIScrollView;
class CMMSound;

class CUIMMShniaga : public CUIWindow, public CDeviceResetNotifier
{
public:
	using CUIWindow::Init;

						CUIMMShniaga			( );
	virtual				~CUIMMShniaga			( );

	void				Init					(CUIXml& xml_doc, pcstr path);
	virtual void		Update					( );
	virtual void		Draw					( );

	virtual bool		OnMouse					(f32 x, f32 y, EUIMessages mouse_action);
	virtual bool		OnKeyboard				(s32 dik, EUIMessages keyboard_action);
	virtual void		SendMessage				(CUIWindow* pWnd, s16 msg, pvoid pData = 0);
	void				SetVisibleMagnifier		(bool f);
	virtual void		OnDeviceReset			( );

protected:
	typedef enum
	{
		E_Begin									= 0,
		E_Update,
		E_Finilize,
		E_Stop
	} EVENT;

	void				SelectBtn				(s32 btn);
	void				SelectBtn				(CUIWindow* btn);
	s32					BtnCount				( );
	void				OnBtnClick				( );

	void				ProcessEvent			(EVENT ev);

	bool				IsButton				(CUIWindow* st);
	void				CreateList				(xr_vector<CUIStatic*>& lst, CUIXml& xml_doc, pcstr path);
	void				ShowMain				( );
	void				ShowNewGame				( );
	f32					pos						(f32 x1, f32 x2, u32 t);

	CUIStatic*									m_shniaga;
	CUIStatic*									m_magnifier;
	CUIStatic*									m_anims[2];
	CUIStatic*									m_gratings[2];
	CUIScrollView*								m_view;

	u32											m_start_time;
	u32											m_run_time;
	f32											m_origin;
	f32											m_destination;
	f32											m_mag_pos;
	f32											m_offset;

	xr_vector<CUIStatic*>						m_buttons;
	xr_vector<CUIStatic*>						m_buttons_new;
	s32											m_selected_btn;
	s32											m_page;
	CUIWindow*									m_selected;
	CMMSound*									m_sound;
	fVector2									m_wheel_size[2];
	enum
	{
		fl_SoundFinalized						= 1,
		fl_MovingStoped							= 2
	};

	flags32										m_flags;
};

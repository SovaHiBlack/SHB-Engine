#pragma once

class CUIWindow;
class CUIDialogWnd;
class CUICursor;
class CUIMessageBoxEx;

#include "..\XR_3DA\IInputReceiver.h"
#include "..\XR_3DA\IGame_Persistent.h"
#include "ui/UIDialogHolder.h"
#include "ui/UIWndCallback.h"
#include "ui/ui_base.h"

class CMainMenu : public IMainMenu, public IInputReceiver, public pureRender, public CUIDialogHolder, public CUIWndCallback, public CDeviceResetNotifier
{
	CUIDialogWnd* m_startDialog;

	enum
	{
		flRestoreConsole = (1 << 0),
		flRestorePause = (1 << 1),
		flRestorePauseStr = (1 << 2),
		flActive = (1 << 3),
		flNeedChangeCapture = (1 << 4),
		flRestoreCursor = (1 << 5),
		flGameSaveScreenshot = (1 << 6),
		flNeedVidRestart = (1 << 7)
	};
	flags16			m_Flags;
	string_path		m_screenshot_name;
	u32				m_screenshotFrame;
	void						ReadTextureInfo( );

	xr_vector<CUIWindow*>		m_pp_draw_wnds;

public:
	enum	EErrorDlg
	{
		SessionTerminate,
		ErrMax,
		ErrNoError = ErrMax
	};

protected:
	EErrorDlg		m_NeedErrDialog;
	u32				m_start_time;

	xr_vector<CUIMessageBoxEx*>	m_pMB_ErrDlgs;

public:
	u32				m_deactivated_frame;
	virtual void	DestroyInternal(bool bForce);
	CMainMenu( );
	virtual			~CMainMenu( );

	virtual void	Activate(bool bActive);
	virtual bool	IsActive( );

	virtual void	IR_OnMousePress(s32 btn);
	virtual void	IR_OnMouseRelease(s32 btn);
	virtual void	IR_OnMouseHold(s32 btn);
	virtual void	IR_OnMouseMove(s32 x, s32 y);
	virtual void	IR_OnMouseStop(s32 x, s32 y);

	virtual void	IR_OnKeyboardPress(s32 dik);
	virtual void	IR_OnKeyboardRelease(s32 dik);
	virtual void	IR_OnKeyboardHold(s32 dik);

	virtual void	IR_OnMouseWheel(s32 direction);

	bool			OnRenderPPUI_query( );
	void			OnRenderPPUI_main( );
	void			OnRenderPPUI_PP( );

	virtual void	OnRender( );
	virtual void	OnFrame( );
	virtual void	StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators);
	virtual bool	UseIndicators( )
	{
		return false;
	}

	void			OnDeviceCreate( );

	void			Screenshot(IRender_interface::ScreenshotMode mode = IRender_interface::SM_NORMAL, pcstr name = 0);
	void			RegisterPPDraw(CUIWindow* w);
	void			UnregisterPPDraw(CUIWindow* w);

	void			SetErrorDialog(EErrorDlg ErrDlg);
	EErrorDlg		GetErrorDialogType( ) const
	{
		return m_NeedErrDialog;
	}
	void			CheckForErrorDlg( );
	void			OnSessionTerminate(pcstr reason);
	void			SetNeedVidRestart( );
	virtual void	OnDeviceReset( );
	pcstr			GetGSVer( );
};

extern CMainMenu* MainMenu( );

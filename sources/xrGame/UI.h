#pragma once

#include "UICursor.h"
#include "UIDialogHolder.h"

// refs
class CHUDManager;
class CWeapon;
class CUIGameCustom;
class CUIMainIngameWnd;
class CUIMessagesWindow;
struct SDrawStaticStruct;

class CUI : public CUIDialogHolder
{
	CUIGameCustom* pUIGame;
	bool					m_bShowGameIndicators;

public:
	CHUDManager* m_Parent;
	CUIMainIngameWnd* UIMainIngameWnd;
	CUIMessagesWindow* m_pMessagesWnd;

public:
	CUI(CHUDManager* p);
	virtual					~CUI( );

	bool					Render( );
	void					UIOnFrame( );

	void					Load(CUIGameCustom* pGameUI);
	void					UnLoad( );

	bool					IR_OnKeyboardHold(s32 dik);
	bool					IR_OnKeyboardPress(s32 dik);
	bool					IR_OnKeyboardRelease(s32 dik);
	bool					IR_OnMouseMove(s32, s32);
	bool					IR_OnMouseWheel(s32 direction);

	CUIGameCustom* UIGame( )
	{
		return pUIGame;
	}

	void					ShowGameIndicators( );
	void					HideGameIndicators( );
	bool					GameIndicatorsShown( )
	{
		return m_bShowGameIndicators;
	}

	void					ShowCrosshair( );
	void					HideCrosshair( );
	bool					CrosshairShown( );

	SDrawStaticStruct* AddInfoMessage(pcstr message);
	void					OnConnected( );
};

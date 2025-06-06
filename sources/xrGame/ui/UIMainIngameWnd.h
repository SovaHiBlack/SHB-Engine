// UIMainIngameWnd.h:  ������-���������� � ����
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIProgressBar.h"
#include "UIGameLog.h"

#include "../alife_space.h"

#include "UICarPanel.h"//
#include "UIMotionIcon.h"//
#include "../HudSound.h"

// ��� ������ ��������� HUD
extern s32				g_bHudAdjustMode;
extern f32			g_fHudAdjustValue;

class					CUIPdaMsgListItem;
class CLightAnimItem;
class					CUIZoneMap;
class					CUIArtefactPanel;
class					CUIScrollView;
struct SGameNewsData;
class					CActor;
class					CWeapon;
class					CMissile;
class					CInventoryItem;

class CUIMainIngameWnd : public CUIWindow
{
public:
	CUIMainIngameWnd( );
	virtual ~CUIMainIngameWnd( );

	virtual void Init( );
	virtual void Draw( );
	virtual void Update( );

	bool OnKeyboardPress(s32 dik);

protected:
	CUIStatic			UIStaticDiskIO;
	CUIStatic			UIStaticHealth;
	CUIStatic			UIStaticArmor;
	CUIStatic			UIStaticQuickHelp;
	CUIProgressBar		UIHealthBar;
	CUIProgressBar		UIArmorBar;
	CUICarPanel			UICarPanel;
	CUIMotionIcon		UIMotionIcon;
	CUIZoneMap* UIZoneMap;

	// ������, ������������ ���������� �������� PDA
	CUIStatic			UIPdaOnline;

	// ����������� ������
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponIcon;
	fRect				UIWeaponIcon_rect;

public:
	CUIStatic* GetPDAOnline( )
	{
		return &UIPdaOnline;
	}

protected:
	// 5 �������� ��� ����������� ������:
	// - ���������� ������
	// - ��������
	// - �������
	// - ������
	// - ���������
	CUIStatic			UIWeaponJammedIcon;
	CUIStatic			UIRadiaitionIcon;
	CUIStatic			UIWoundIcon;
	CUIStatic			UIStarvationIcon;
	CUIStatic			UIPsyHealthIcon;
	CUIStatic			UIInvincibleIcon;
	CUIStatic			UIArtefactIcon;

	CUIScrollView* m_UIIcons;
	CUIWindow* m_pMPChatWnd;
	CUIWindow* m_pMPLogWnd;

public:
	CUIArtefactPanel* m_artefactPanel;

public:
	// ����� �������������� ��������������� �������
	enum EWarningIcons
	{
		ewiAll = 0,
		ewiWeaponJammed,
		ewiRadiation,
		ewiWound,
		ewiStarvation,
		ewiPsyHealth,
		ewiInvincible,
		ewiArtefact
	};

	void				SetMPChatLog(CUIWindow* pChat, CUIWindow* pLog);

	// ������ ���� ��������������� ������
	void				SetWarningIconColor(EWarningIcons icon, const u32 cl);
	void				TurnOffWarningIcon(EWarningIcons icon);

	// ������ ��������� ����� �����������, ����������� �� system.ltx
	typedef				xr_map<EWarningIcons, xr_vector<f32>>		Thresholds;
	typedef				Thresholds::iterator						Thresholds_it;
	Thresholds			m_Thresholds;

	// ���� ������������ ��������� �������� ������
	enum EFlashingIcons
	{
		efiPdaTask = 0,
		efiMail
	};

	void				SetFlashIconState_(EFlashingIcons type, bool enable);

	void				AnimateContacts(bool b_snd);
	SHudSound			m_contactSnd;

	void				ReceiveNews(SGameNewsData* news);

protected:
	void				SetWarningIconColor(CUIStatic* s, const u32 cl);
	void				InitFlashingIcons(CUIXml* node);
	void				DestroyFlashingIcons( );
	void				UpdateFlashingIcons( );
	void				UpdateActiveItemInfo( );

	void				SetAmmoIcon(const shared_str& se�t_name);

	// first - ������, second - ��������
	DEF_MAP(FlashingIcons, EFlashingIcons, CUIStatic*);
	FlashingIcons		m_FlashingIcons;

	// ��� �������� ��������� ������ � ������
	CActor* m_pActor;
	CWeapon* m_pWeapon;
	CMissile* m_pGrenade;
	CInventoryItem* m_pItem;

	// ����������� ��������� ��� ��������� ������� �� ������
	void				RenderQuickInfos( );

public:
	CUICarPanel& CarPanel( )
	{
		return UICarPanel;
	}
	CUIMotionIcon& MotionIcon( )
	{
		return UIMotionIcon;
	}
	void				OnConnected( );
	void				reset_ui( );

protected:
	CInventoryItem* m_pPickUpItem;
	CUIStatic			UIPickUpItemIcon;

	f32				m_iPickUpItemIconX;
	f32				m_iPickUpItemIconY;
	f32				m_iPickUpItemIconWidth;
	f32				m_iPickUpItemIconHeight;

	void				UpdatePickUpItem( );

public:
	void				SetPickUpItem(CInventoryItem* PickUpItem);

#ifdef DEBUG
	void				draw_adjust_mode( );
#endif // def DEBUG

};

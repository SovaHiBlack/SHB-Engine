// UIMainIngameWnd.h:  окошки-информация в игре
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIProgressBar.h"//
#include "UIGameLog.h"//

//#include "..\ALife_space.h"

#include "UICarPanel.h"//
#include "UIMotionIcon.h"//
#include "../HudSound.h"

//для режима настройки HUD
extern int											g_bHudAdjustMode;
extern float										g_fHudAdjustValue;

class CUIPdaMsgListItem;
class CLAItem;
class CUIZoneMap;
class CUIArtefactPanel;
class CUIScrollView;
struct SGameNewsData;
class CActor;
class CWeapon;
class CMissile;
class CInventoryItem;

class CUIMainIngameWnd : public CUIWindow
{
public:
						CUIMainIngameWnd			( );
	virtual				~CUIMainIngameWnd			( );

	virtual void		Init						( );
	virtual void		Draw						( );
	virtual void		Update						( );

	bool				OnKeyboardPress				(int dik);

protected:
	CUIStatic										UIStaticDiskIO;
	CUIStatic										UIStaticHealth;
	CUIStatic										UIStaticArmor;
	CUIStatic										UIStaticQuickHelp;
	CUIProgressBar									UIHealthBar;
	CUIProgressBar									UIArmorBar;
	CUICarPanel										UICarPanel;
	CUIMotionIcon									UIMotionIcon;
	CUIZoneMap*										UIZoneMap;

	//иконка, показывающая количество активных PDA
	CUIStatic										UIPdaOnline;

	//изображение оружия
	CUIStatic										UIWeaponBack;
	CUIStatic										UIWeaponSignAmmo;
	CUIStatic										UIWeaponIcon;
	Frect											UIWeaponIcon_rect;

public:
	CUIStatic*			GetPDAOnline				( )
	{
		return &UIPdaOnline;
	};

protected:
	// 5 статиков для отображения иконок:
	// - сломанного оружия
	// - радиации
	// - ранения
	// - голода
	// - усталости
	CUIStatic										UIWeaponJammedIcon;
	CUIStatic										UIRadiaitionIcon;
	CUIStatic										UIWoundIcon;
	CUIStatic										UIStarvationIcon;
	CUIStatic										UIPsyHealthIcon;
	CUIStatic										UIInvincibleIcon;
	CUIStatic										UIArtefactIcon;

	CUIScrollView*									m_UIIcons;

public:
	CUIArtefactPanel*								m_artefactPanel;

public:
	// Енумы соответсвующие предупреждающим иконкам
	enum EWarningIcons
	{
		ewiAll										= 0,
		ewiWeaponJammed,
		ewiRadiation,
		ewiWound,
		ewiStarvation,
		ewiPsyHealth,
		ewiInvincible,
		ewiArtefact
	};

	// Задаем цвет соответствующей иконке
	void				SetWarningIconColor			(EWarningIcons icon, const u32 cl);
	void				TurnOffWarningIcon			(EWarningIcons icon);

	// Пороги изменения цвета индикаторов, загружаемые из system.ltx
	using Thresholds								= xr_map<EWarningIcons, xr_vector<float>>;
	using Thresholds_it								= Thresholds::iterator;
	Thresholds										m_Thresholds;

	// Енум перечисления возможных мигающих иконок
	enum EFlashingIcons
	{
		efiPdaTask									= 0,
		efiMail
	};

	void				SetFlashIconState_			(EFlashingIcons type, bool enable);

	void				AnimateContacts				(bool b_snd);
	HUD_SOUND										m_contactSnd;

	void				ReceiveNews					(SGameNewsData* news);

protected:
	void				SetWarningIconColor			(CUIStatic* s, const u32 cl);
	void				InitFlashingIcons			(CUIXml* node);
	void				DestroyFlashingIcons		( );
	void				UpdateFlashingIcons			( );
	void				UpdateActiveItemInfo		( );

	void				SetAmmoIcon					(const CSharedString& seсt_name);

	// first - иконка, second - анимация
	using FlashingIcons								= xr_map<EFlashingIcons, CUIStatic*>;
	using FlashingIcons_it							= FlashingIcons::iterator;
	FlashingIcons									m_FlashingIcons;

	//для текущего активного актера и оружия
	CActor*											m_pActor;
	CWeapon*										m_pWeapon;
	CMissile*										m_pGrenade;
	CInventoryItem*									m_pItem;

	// Отображение подсказок при наведении прицела на объект
	void				RenderQuickInfos			( );

public:
	CUICarPanel&		CarPanel					( )
	{
		return UICarPanel;
	};
	CUIMotionIcon&		MotionIcon					( )
	{
		return UIMotionIcon;
	}
	void				OnConnected					( );
	void				reset_ui					( );

protected:
	CInventoryItem*									m_pPickUpItem;
	CUIStatic										UIPickUpItemIcon;

	float											m_iPickUpItemIconX;
	float											m_iPickUpItemIconY;
	float											m_iPickUpItemIconWidth;
	float											m_iPickUpItemIconHeight;

	void				UpdatePickUpItem			( );

public:
	void				SetPickUpItem				(CInventoryItem* PickUpItem);

#ifdef DEBUG
	void				draw_adjust_mode			( );
#endif // def DEBUG

};

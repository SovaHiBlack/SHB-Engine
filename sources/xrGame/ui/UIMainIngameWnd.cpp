#include "stdafx.h"

#include <functional>// ������ �� ����

#include "UIZoneMap.h"//
#include "UIMainIngameWnd.h"//
#include "UIMessagesWindow.h"//

#include "../Actor.h"
#include "../alife_object_registry.h"
#include "../alife_simulator.h"
#include "../CharacterInfo.h"
#include "../game_cl_base.h"
#include "../Grenade.h"
#include "../HUDManager.h"
#include "../Inventory.h"
#include "../level.h"
#include "../Missile.h"
#include "../PDA.h"
#include "../seniority_hierarchy_holder.h"
#include "UIGame.h"
#include "../WeaponHUD.h"
#include "../WeaponMagazined.h"
#include "../xrServer_objects_ALife.h"
#include <dinput.h>

#include "../..\XR_3DA\LightAnimLibrary.h"
#include "../date_time.h"
#include "../xrServer_Objects_ALife_Monsters.h"

#include "UIInventoryUtilities.h"//

#include "../actorcondition.h"
#include "../alife_registry_wrappers.h"
#include "UIPdaMsgListItem.h"//
#include "UIXmlInit.h"

#include "../clsid_game.h"
#include "../StringTable.h"
#include "UIArtefactPanel.h"//

#ifdef DEBUG
#	include "../AttachableItem.h"
#	include "../..\XR_3DA\Input.h"
#endif

#include "UIScrollView.h"
#include "UIMapHint.h"//
#include "UIColorAnimatorWrapper.h"//
#include "../game_news.h"

#ifdef DEBUG
#	include "../DebugRenderer.h"

void test_draw( );
void test_key(s32 dik);
void test_update( );
#endif

#include "UIProgressShape.h"

using namespace InventoryUtilities;

//	hud adjust mode
s32			g_bHudAdjustMode = 0;
f32			g_fHudAdjustValue = 0.0f;

const u32	g_clWhite = 0xffffffff;

#define		DEFAULT_MAP_SCALE			1.0f

#define		C_SIZE						0.025f
#define		NEAR_LIM					0.5f

#define		SHOW_INFO_SPEED				0.5f
#define		HIDE_INFO_SPEED				10.0f
#define		C_ON_ENEMY					D3DCOLOR_XRGB(0xff,0,0)
#define		C_DEFAULT					D3DCOLOR_XRGB(0xff,0xff,0xff)

#define				MAININGAME_XML				"maingame.xml"

CUIMainIngameWnd::CUIMainIngameWnd( )
{
	m_pActor = NULL;
	m_pWeapon = NULL;
	m_pGrenade = NULL;
	m_pItem = NULL;
	UIZoneMap = xr_new<CUIZoneMap>( );
	m_pPickUpItem = NULL;
	m_artefactPanel = xr_new<CUIArtefactPanel>( );
	m_pMPChatWnd = NULL;
	m_pMPLogWnd = NULL;
}

extern CUIProgressShape* g_MissileForceShape;

CUIMainIngameWnd::~CUIMainIngameWnd( )
{
	DestroyFlashingIcons( );
	xr_delete(UIZoneMap);
	xr_delete(m_artefactPanel);
	SHudSound::DestroySound(m_contactSnd);
	xr_delete(g_MissileForceShape);
}

void CUIMainIngameWnd::Init( )
{
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, MAININGAME_XML);

	CUIXmlInit xml_init;
	CUIWindow::Init(0.0f, 0.0f, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	Enable(false);

	AttachChild(&UIStaticHealth);
	xml_init.InitStatic(uiXml, "static_health", 0, &UIStaticHealth);

	AttachChild(&UIStaticArmor);
	xml_init.InitStatic(uiXml, "static_armor", 0, &UIStaticArmor);

	AttachChild(&UIWeaponBack);
	xml_init.InitStatic(uiXml, "static_weapon", 0, &UIWeaponBack);

	UIWeaponBack.AttachChild(&UIWeaponSignAmmo);
	xml_init.InitStatic(uiXml, "static_ammo", 0, &UIWeaponSignAmmo);
	UIWeaponSignAmmo.SetElipsis(CUIStatic::eepEnd, 2);

	UIWeaponBack.AttachChild(&UIWeaponIcon);
	xml_init.InitStatic(uiXml, "static_wpn_icon", 0, &UIWeaponIcon);
	UIWeaponIcon.SetShader(GetEquipmentIconsShader( ));
	UIWeaponIcon_rect = UIWeaponIcon.GetWndRect( );
	//---------------------------------------------------------
	AttachChild(&UIPickUpItemIcon);
	xml_init.InitStatic(uiXml, "pick_up_item", 0, &UIPickUpItemIcon);
	UIPickUpItemIcon.SetShader(GetEquipmentIconsShader( ));
	UIPickUpItemIcon.ClipperOn( );

	m_iPickUpItemIconWidth = UIPickUpItemIcon.GetWidth( );
	m_iPickUpItemIconHeight = UIPickUpItemIcon.GetHeight( );
	m_iPickUpItemIconX = UIPickUpItemIcon.GetWndRect( ).left;
	m_iPickUpItemIconY = UIPickUpItemIcon.GetWndRect( ).top;
	//---------------------------------------------------------

	UIWeaponIcon.Enable(false);

	// ����������
	UIZoneMap->Init( );
	UIZoneMap->SetScale(DEFAULT_MAP_SCALE);

	xml_init.InitStatic(uiXml, "static_pda_online", 0, &UIPdaOnline);
	UIZoneMap->Background( ).AttachChild(&UIPdaOnline);

	// ������ ��������� ��������
	UIStaticHealth.AttachChild(&UIHealthBar);
	xml_init.InitProgressBar(uiXml, "progress_bar_health", 0, &UIHealthBar);

	// ������ ��������� ������
	UIStaticArmor.AttachChild(&UIArmorBar);
	xml_init.InitProgressBar(uiXml, "progress_bar_armor", 0, &UIArmorBar);

	// ���������, ������� ��������� ��� ��������� ������� �� ������
	AttachChild(&UIStaticQuickHelp);
	xml_init.InitStatic(uiXml, "quick_info", 0, &UIStaticQuickHelp);

	uiXml.SetLocalRoot(uiXml.GetRoot( ));

	m_UIIcons = xr_new<CUIScrollView>( );
	m_UIIcons->SetAutoDelete(true);
	xml_init.InitScrollView(uiXml, "icons_scroll_view", 0, m_UIIcons);
	AttachChild(m_UIIcons);

	// ��������� ������
	xml_init.InitStatic(uiXml, "starvation_static", 0, &UIStarvationIcon);
	UIStarvationIcon.Show(false);

	xml_init.InitStatic(uiXml, "psy_health_static", 0, &UIPsyHealthIcon);
	UIPsyHealthIcon.Show(false);

	xml_init.InitStatic(uiXml, "weapon_jammed_static", 0, &UIWeaponJammedIcon);
	UIWeaponJammedIcon.Show(false);

	xml_init.InitStatic(uiXml, "radiation_static", 0, &UIRadiaitionIcon);
	UIRadiaitionIcon.Show(false);

	xml_init.InitStatic(uiXml, "wound_static", 0, &UIWoundIcon);
	UIWoundIcon.Show(false);

	xml_init.InitStatic(uiXml, "invincible_static", 0, &UIInvincibleIcon);
	UIInvincibleIcon.Show(false);

	shared_str warningStrings[6] =
	{
		"jammed",
		"radiation",
		"wounds",
		"starvation",
		"fatigue",
		"invincible"
	};

	// ��������� ��������� �������� ��� �����������
	EWarningIcons j = ewiWeaponJammed;
	while (j < ewiInvincible)
	{
		// ������ ������ ������� ��� ������� ����������
		shared_str cfgRecord = pSettings->r_string("main_ingame_indicators_thresholds", *warningStrings[static_cast<int>(j) - 1]);
		u32 count = _GetItemCount(*cfgRecord);

		string8 singleThreshold;
		f32	f = 0.0f;
		for (u32 k = 0; k < count; ++k)
		{
			_GetItem(*cfgRecord, k, singleThreshold);
			sscanf(singleThreshold, "%f", &f);

			m_Thresholds[j].push_back(f);
		}

		j = static_cast<EWarningIcons>(j + 1);
	}

	// Flashing icons initialize
	uiXml.SetLocalRoot(uiXml.NavigateToNode("flashing_icons"));
	InitFlashingIcons(&uiXml);

	uiXml.SetLocalRoot(uiXml.GetRoot( ));

	AttachChild(&UICarPanel);
	xml_init.InitWindow(uiXml, "car_panel", 0, &UICarPanel);

	AttachChild(&UIMotionIcon);
	UIMotionIcon.Init( );

	m_artefactPanel->InitFromXML(uiXml, "artefact_panel", 0);
	this->AttachChild(m_artefactPanel);

	AttachChild(&UIStaticDiskIO);
	UIStaticDiskIO.SetWndRect(1000.0f, 750.0f, 16.0f, 16.0f);
	UIStaticDiskIO.GetUIStaticItem( ).SetRect(0.0f, 0.0f, 16.0f, 16.0f);
	UIStaticDiskIO.InitTexture("ui\\ui_disk_io");
	UIStaticDiskIO.SetOriginalRect(0.0f, 0.0f, 32.0f, 32.0f);
	UIStaticDiskIO.SetStretchTexture(TRUE);

	SHudSound::LoadSound("maingame_ui", "snd_new_contact", m_contactSnd, eST_IDLE);
}

f32 UIStaticDiskIO_start_time = 0.0f;
void CUIMainIngameWnd::Draw( )
{

#ifdef DEBUG
	test_draw( );
#endif // def DEBUG

	// show IO icon
	bool IOActive = (FS.dwOpenCounter > 0);
	if (IOActive)
	{
		UIStaticDiskIO_start_time = Device.fTimeGlobal;
	}

	if ((UIStaticDiskIO_start_time + 1.0f) < Device.fTimeGlobal)
	{
		UIStaticDiskIO.Show(false);
	}
	else
	{
		u32 alpha = clampr(iFloor(255.0f * (1.0f - (Device.fTimeGlobal - UIStaticDiskIO_start_time) / 1.0f)), 0, 255);
		UIStaticDiskIO.Show(true);
		UIStaticDiskIO.SetColor(color_rgba(255, 255, 255, alpha));
	}

	FS.dwOpenCounter = 0;

	if (!m_pActor)
	{
		return;
	}

	UIMotionIcon.SetNoise((s16)(0xffff & iFloor(m_pActor->m_snd_noise * 100.0f)));
	CUIWindow::Draw( );
	UIZoneMap->Render( );

	RenderQuickInfos( );

#ifdef DEBUG
	draw_adjust_mode( );
#endif // def DEBUG

}

void CUIMainIngameWnd::SetMPChatLog(CUIWindow* pChat, CUIWindow* pLog)
{
	m_pMPChatWnd = pChat;
	m_pMPLogWnd = pLog;
}

void CUIMainIngameWnd::SetAmmoIcon(const shared_str& sect_name)
{
	if (!sect_name.size( ))
	{
		UIWeaponIcon.Show(false);
		return;
	}

	UIWeaponIcon.Show(true);
	//properties used by inventory menu
	f32 iGridWidth = pSettings->r_float(sect_name, "inv_grid_width");
	f32 iGridHeight = pSettings->r_float(sect_name, "inv_grid_height");

	f32 iXPos = pSettings->r_float(sect_name, "inv_grid_x");
	f32 iYPos = pSettings->r_float(sect_name, "inv_grid_y");

	UIWeaponIcon.GetUIStaticItem( ).SetOriginalRect((iXPos * INV_GRID_WIDTH), (iYPos * INV_GRID_HEIGHT), (iGridWidth * INV_GRID_WIDTH), (iGridHeight * INV_GRID_HEIGHT));
	UIWeaponIcon.SetStretchTexture(true);

	// now perform only width scale for ammo, which (W)size >2
	// all others ammo (1x1, 1x2) will be not scaled (original picture)
	f32 w = ((iGridWidth > 2) ? 1.6f : iGridWidth) * INV_GRID_WIDTH * 0.9f;
	f32 h = INV_GRID_HEIGHT * 0.9f;//1 cell

	f32 x = UIWeaponIcon_rect.x1;
	if (iGridWidth < 2)
	{
		x += (UIWeaponIcon_rect.width( ) - w) / 2.0f;
	}

	UIWeaponIcon.SetWndPos(x, UIWeaponIcon_rect.y1);

	UIWeaponIcon.SetWidth(w);
	UIWeaponIcon.SetHeight(h);
}

void CUIMainIngameWnd::Update( )
{

#ifdef DEBUG
	test_update( );
#endif // def DEBUG

	if (m_pMPChatWnd)
	{
		m_pMPChatWnd->Update( );
	}

	if (m_pMPLogWnd)
	{
		m_pMPLogWnd->Update( );
	}

	m_pActor = smart_cast<CActor*>(Level( ).CurrentViewEntity( ));
	if (!m_pActor)
	{
		m_pItem = NULL;
		m_pWeapon = NULL;
		m_pGrenade = NULL;
		CUIWindow::Update( );
		return;
	}

	if (!(Device.dwFrame % 30))
	{
		string256 text_str;
		CPda* _pda = m_pActor->GetPDA( );
		u32 _cn = 0;
		if (_pda && 0 != (_cn = _pda->ActiveContactsNum( )))
		{
			sprintf_s(text_str, "%d", _cn);
			UIPdaOnline.SetText(text_str);
		}
		else
		{
			UIPdaOnline.SetText("");
		}
	}

	if (!(Device.dwFrame % 5))
	{

		if (!(Device.dwFrame % 30))
		{
			bool b_God = (GodMode( ) || (!Game( ).local_player)) ? true : Game( ).local_player->testFlag(GAME_PLAYER_FLAG_INVINCIBLE);
			if (b_God)
			{
				SetWarningIconColor(ewiInvincible, 0xffffffff);
			}
			else
			{
				SetWarningIconColor(ewiInvincible, 0x00ffffff);
			}
		}

		// Armor indicator stuff
		PIItem	pItem = m_pActor->inventory( ).ItemFromSlot(OUTFIT_SLOT);
		if (pItem)
		{
			UIArmorBar.Show(true);
			UIStaticArmor.Show(true);
			UIArmorBar.SetProgressPos(pItem->GetCondition( ) * 100);
		}
		else
		{
			UIArmorBar.Show(false);
			UIStaticArmor.Show(false);
		}

		UpdateActiveItemInfo( );

		EWarningIcons i = ewiWeaponJammed;

		while (i < ewiInvincible)
		{
			f32 value = 0.0f;
			switch (i)
			{
				//radiation
				case ewiRadiation:
				{
					value = m_pActor->conditions( ).GetRadiation( );
				}
				break;
				case ewiWound:
				{
					value = m_pActor->conditions( ).BleedingSpeed( );
				}
				break;
				case ewiWeaponJammed:
				{
					if (m_pWeapon)
					{
						value = 1 - m_pWeapon->GetConditionToShow( );
					}
				}
				break;
				case ewiStarvation:
				{
					value = 1 - m_pActor->conditions( ).GetSatiety( );
				}
				break;
				case ewiPsyHealth:
				{
					value = 1 - m_pActor->conditions( ).GetPsyHealth( );
				}
				break;
				default:
				{
					R_ASSERT(!"Unknown type of warning icon");
				}
			}

			xr_vector<f32>::reverse_iterator	rit;

			// ������� ��������� �� ������ �����������
			rit = std::find(m_Thresholds[i].rbegin( ), m_Thresholds[i].rend( ), value);

			// ���� ��� ���, �� ����� ��������� ������� �������� ()
			if (rit == m_Thresholds[i].rend( ))
			{
				rit = std::find_if(m_Thresholds[i].rbegin( ), m_Thresholds[i].rend( ), std::bind2nd(std::less<f32>( ), value));
			}

			// ����������� � ������������ �������� �������
			f32 min = m_Thresholds[i].front( );
			f32 max = m_Thresholds[i].back( );

			if (rit != m_Thresholds[i].rend( ))
			{
				f32 v = *rit;
				SetWarningIconColor(i, color_argb(0xFF, clampr<u32>(static_cast<u32>(255 * ((v - min) / (max - min) * 2)), 0, 255),
												  clampr<u32>(static_cast<u32>(255 * (2.0f - (v - min) / (max - min) * 2)), 0, 255),
												  0));
			}
			else
			{
				TurnOffWarningIcon(i);
			}

			i = (EWarningIcons)(i + 1);
		}
	}

	// health&armor
	UIHealthBar.SetProgressPos(m_pActor->GetfHealth( ) * 100.0f);
	UIMotionIcon.SetPower(m_pActor->conditions( ).GetPower( ) * 100.0f);

	UIZoneMap->UpdateRadar(Device.vCameraPosition);
	f32 h;
	f32 p;
	Device.vCameraDirection.getHP(h, p);
	UIZoneMap->SetHeading(-h);

	UpdatePickUpItem( );
	CUIWindow::Update( );
}

bool CUIMainIngameWnd::OnKeyboardPress(s32 dik)
{

#if 0//def DEBUG
	test_key(dik);
#endif

	// ��������� ������ adjust hud mode
	bool flag = false;
	if (g_bHudAdjustMode)
	{
		CWeaponHUD* pWpnHud = NULL;
		if (m_pWeapon)
		{
			pWpnHud = m_pWeapon->GetHUD( );
		}
		else
		{
			return false;
		}

		fVector3 tmpV;

		if (1 == g_bHudAdjustMode) //zoom offset
		{
			if (!pWpnHud)
			{
				return false;
			}

			tmpV = pWpnHud->ZoomOffset( );

			switch (dik)
			{
				// Rotate +y
				case DIK_K:
				{
					pWpnHud->SetZoomRotateX(pWpnHud->ZoomRotateX( ) + g_fHudAdjustValue);
					flag = true;
				}
				break;
				// Rotate -y
				case DIK_I:
				{
					pWpnHud->SetZoomRotateX(pWpnHud->ZoomRotateX( ) - g_fHudAdjustValue);
					flag = true;
				}
				break;
				// Rotate +x
				case DIK_L:
				{
					pWpnHud->SetZoomRotateY(pWpnHud->ZoomRotateY( ) + g_fHudAdjustValue);
					flag = true;
				}
				break;
				// Rotate -x
				case DIK_J:
				{
					pWpnHud->SetZoomRotateY(pWpnHud->ZoomRotateY( ) - g_fHudAdjustValue);
					flag = true;
				}
				break;
				// Shift +x
				case DIK_W:
				{
					tmpV.y += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -y
				case DIK_S:
				{
					tmpV.y -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift +x
				case DIK_D:
				{
					tmpV.x += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -x
				case DIK_A:
				{
					tmpV.x -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift +z
				case DIK_Q:
				{
					tmpV.z += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -z
				case DIK_E:
				{
					tmpV.z -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// output coordinate info to the console
				case DIK_P:
				{
					string256 tmpStr;
					sprintf_s(tmpStr, "%s", *m_pWeapon->cNameSect( ));
					Log(tmpStr);

					sprintf_s(tmpStr, "zoom_offset\t\t\t= %f,%f,%f", pWpnHud->ZoomOffset( ).x, pWpnHud->ZoomOffset( ).y, pWpnHud->ZoomOffset( ).z);
					Log(tmpStr);
					sprintf_s(tmpStr, "zoom_rotate_x\t\t= %f", pWpnHud->ZoomRotateX( ));
					Log(tmpStr);
					sprintf_s(tmpStr, "zoom_rotate_y\t\t= %f", pWpnHud->ZoomRotateY( ));
					Log(tmpStr);
					flag = true;
				}
				break;
			}

			if (tmpV.x || tmpV.y || tmpV.z)
			{
				pWpnHud->SetZoomOffset(tmpV);
			}
		}
		else if (2 == g_bHudAdjustMode || 5 == g_bHudAdjustMode) //firePoints
		{
			if (TRUE == m_pWeapon->GetHUDmode( ))
			{
				tmpV = (2 == g_bHudAdjustMode) ? pWpnHud->FirePoint( ) : pWpnHud->FirePoint2( );
			}
			else
			{
				tmpV = (2 == g_bHudAdjustMode) ? m_pWeapon->vLoadedFirePoint : m_pWeapon->vLoadedFirePoint2;
			}

			switch (dik)
			{
				// Shift +x
				case DIK_A:
				{
					tmpV.y += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -x
				case DIK_D:
				{
					tmpV.y -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift +z
				case DIK_Q:
				{
					tmpV.x += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -z
				case DIK_E:
				{
					tmpV.x -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift +y
				case DIK_S:
				{
					tmpV.z += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -y
				case DIK_W:
				{
					tmpV.z -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// output coordinate info to the console
				case DIK_P:
				{
					string256 tmpStr;
					if (m_pWeapon)
					{
						sprintf_s(tmpStr, "%s", *m_pWeapon->cNameSect( ));
						Log(tmpStr);
					}

					if (TRUE == m_pWeapon->GetHUDmode( ))
					{
						Msg("weapon hud section:");
					}
					else
					{
						Msg("weapon section:");
					}

					sprintf_s(tmpStr, "fire_point\t\t\t= %f,%f,%f", tmpV.x, tmpV.y, tmpV.z);
					Log(tmpStr);
					flag = true;
				}
				break;
			}

#ifdef	DEBUG
			if (TRUE == m_pWeapon->GetHUDmode( ))
			{
				if (2 == g_bHudAdjustMode)
				{
					pWpnHud->dbg_SetFirePoint(tmpV);
				}
				else
				{
					pWpnHud->dbg_SetFirePoint2(tmpV);
				}
			}
			else
			{
				if (2 == g_bHudAdjustMode)
				{
					m_pWeapon->vLoadedFirePoint = tmpV;
				}
				else
				{
					m_pWeapon->vLoadedFirePoint2 = tmpV;
				}
			}
#endif

		}
		else if (4 == g_bHudAdjustMode) //ShellPoint
		{
			if (TRUE == m_pWeapon->GetHUDmode( ))
			{
				tmpV = pWpnHud->ShellPoint( );
			}
			else
			{
				tmpV = m_pWeapon->vLoadedShellPoint;
			}

			switch (dik)
			{
				// Shift +x
				case DIK_A:
				{
					tmpV.y += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -x
				case DIK_D:
				{
					tmpV.y -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift +z
				case DIK_Q:
				{
					tmpV.x += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -z
				case DIK_E:
				{
					tmpV.x -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift +y
				case DIK_S:
				{
					tmpV.z += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -y
				case DIK_W:
				{
					tmpV.z -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// output coordinate info to the console
				case DIK_P:
				{
					string256 tmpStr;
					if (m_pWeapon)
					{
						sprintf_s(tmpStr, "%s", *m_pWeapon->cNameSect( ));
						Log(tmpStr);
					}

					if (TRUE == m_pWeapon->GetHUDmode( ))
					{
						Msg("weapon hud section:");
					}
					else
					{
						Msg("weapon section:");
					}

					sprintf_s(tmpStr, "shell_point\t\t\t= %f,%f,%f", tmpV.x, tmpV.y, tmpV.z);
					Log(tmpStr);
					flag = true;
				}
				break;
			}

#ifdef DEBUG
			if (TRUE == m_pWeapon->GetHUDmode( ))
			{
				pWpnHud->dbg_SetShellPoint(tmpV);
			}
			else
			{
				m_pWeapon->vLoadedShellPoint = tmpV;
			}
#endif

		}
		else if (3 == g_bHudAdjustMode) //MissileOffset
		{
			CActor* pActor = smart_cast<CActor*>(Level( ).CurrentEntity( ));

			R_ASSERT(pActor);

			tmpV = pActor->GetMissileOffset( );

			if (!pActor)
			{
				return false;
			}

			switch (dik)
			{
				// Shift +x
				case DIK_E:
				{
					tmpV.y += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -x
				case DIK_Q:
				{
					tmpV.y -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift +z
				case DIK_D:
				{
					tmpV.x += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -z
				case DIK_A:
				{
					tmpV.x -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift +y
				case DIK_W:
				{
					tmpV.z += g_fHudAdjustValue;
					flag = true;
				}
				break;
				// Shift -y
				case DIK_S:
				{
					tmpV.z -= g_fHudAdjustValue;
					flag = true;
				}
				break;
				// output coordinate info to the console
				case DIK_P:
				{
					string256 tmpStr;
					if (m_pWeapon)
					{
						sprintf_s(tmpStr, "%s", *m_pWeapon->cNameSect( ));
						Log(tmpStr);
					}

					sprintf_s(tmpStr, "missile_throw_offset\t\t\t= %f,%f,%f", pActor->GetMissileOffset( ).x, pActor->GetMissileOffset( ).y, pActor->GetMissileOffset( ).z);

					Log(tmpStr);
					flag = true;
				}
				break;
			}

			pActor->SetMissileOffset(tmpV);
		}

		if (flag)
		{
			return true;
		}
	}

#ifdef DEBUG
	if (CAttachableItem::m_dbgItem)
	{
		static f32 rot_d = deg2rad(0.5f);
		static f32 mov_d = 0.01f;
		bool shift = !!pInput->iGetAsyncKeyState(DIK_LSHIFT);
		flag = true;
		switch (dik)
		{
			// Shift +x
			case DIK_A:
			{
				if (shift)
				{
					CAttachableItem::rot_dx(rot_d);
				}
				else
				{
					CAttachableItem::mov_dx(rot_d);
				}
			}
			break;
			// Shift -x
			case DIK_D:
			{
				if (shift)
				{
					CAttachableItem::rot_dx(-rot_d);
				}
				else
				{
					CAttachableItem::mov_dx(-rot_d);
				}
			}
			break;
			// Shift +z
			case DIK_Q:
			{
				if (shift)
				{
					CAttachableItem::rot_dy(rot_d);
				}
				else
				{
					CAttachableItem::mov_dy(rot_d);
				}
			}
			break;
			// Shift -z
			case DIK_E:
			{
				if (shift)
				{
					CAttachableItem::rot_dy(-rot_d);
				}
				else
				{
					CAttachableItem::mov_dy(-rot_d);
				}
			}
			break;
			// Shift +y
			case DIK_S:
			{
				if (shift)
				{
					CAttachableItem::rot_dz(rot_d);
				}
				else
				{
					CAttachableItem::mov_dz(rot_d);
				}
			}
			break;
			// Shift -y
			case DIK_W:
			{
				if (shift)
				{
					CAttachableItem::rot_dz(-rot_d);
				}
				else
				{
					CAttachableItem::mov_dz(-rot_d);
				}
			}
			break;
			case DIK_SUBTRACT:
			{
				if (shift)
				{
					rot_d -= deg2rad(0.01f);
				}
				else
				{
					mov_d -= 0.001f;
				}

				Msg("rotation delta=[%f]; moving delta=[%f]", rot_d, mov_d);
			}
			break;
			case DIK_ADD:
			{
				if (shift)
				{
					rot_d += deg2rad(0.01f);
				}
				else
				{
					mov_d += 0.001f;
				}

				Msg("rotation delta=[%f]; moving delta=[%f]", rot_d, mov_d);
			}
			break;
			case DIK_P:
			{
				Msg("LTX section [%s]", *CAttachableItem::m_dbgItem->item( ).object( ).cNameSect( ));
				Msg("attach_angle_offset [%f,%f,%f]", VPUSH(CAttachableItem::get_angle_offset( )));
				Msg("attach_position_offset [%f,%f,%f]", VPUSH(CAttachableItem::get_pos_offset( )));
			}
			break;
			default:
			{
				flag = false;
			}
			break;
		}

		if (flag)
		{
			return true;
		}
	}
#endif

	if (Level( ).IR_GetKeyState(DIK_LSHIFT) || Level( ).IR_GetKeyState(DIK_RSHIFT))
	{
		switch (dik)
		{
			case DIK_NUMPADMINUS:
			{
				UIZoneMap->ZoomOut( );
				return true;
			}
			break;
			case DIK_NUMPADPLUS:
			{
				UIZoneMap->ZoomIn( );
				return true;
			}
			break;
		}
	}
	else
	{
		switch (dik)
		{
			case DIK_NUMPADMINUS:
			{
				HUD( ).GetUI( )->HideGameIndicators( );
				return true;
			}
			break;
			case DIK_NUMPADPLUS:
			{
				HUD( ).GetUI( )->ShowGameIndicators( );
				return true;
			}
			break;
		}
	}

	return false;
}

void CUIMainIngameWnd::RenderQuickInfos( )
{
	if (!m_pActor)
	{
		return;
	}

	static CGameObject* pObject = NULL;
	pcstr actor_action = m_pActor->GetDefaultActionForObject( );
	UIStaticQuickHelp.Show(NULL != actor_action);

	if (NULL != actor_action)
	{
		if (stricmp(actor_action, UIStaticQuickHelp.GetText( )))
		{
			UIStaticQuickHelp.SetTextST(actor_action);
		}
	}

	if (pObject != m_pActor->ObjectWeLookingAt( ))
	{
		UIStaticQuickHelp.SetTextST(actor_action);
		UIStaticQuickHelp.ResetClrAnimation( );
		pObject = m_pActor->ObjectWeLookingAt( );
	}
}

void CUIMainIngameWnd::ReceiveNews(SGameNewsData* news)
{
	VERIFY(news->texture_name.size( ));
	HUD( ).GetUI( )->m_pMessagesWnd->AddIconedPdaMessage(*(news->texture_name), news->tex_rect, news->SingleLineText( ), news->show_time);
}

void CUIMainIngameWnd::SetWarningIconColor(CUIStatic* s, const u32 cl)
{
	s32 bOn = (cl >> 24);
	bool bIsShown = s->IsShown( );

	if (bOn)
	{
		s->SetColor(cl);
	}

	if (bOn && !bIsShown)
	{
		m_UIIcons->AddWindow(s, false);
		s->Show(true);
	}

	if (!bOn && bIsShown)
	{
		m_UIIcons->RemoveWindow(s);
		s->Show(false);
	}
}

void CUIMainIngameWnd::SetWarningIconColor(EWarningIcons icon, const u32 cl)
{
	bool bMagicFlag = true;

	// ������ ���� ��������� ������
	switch (icon)
	{
		case ewiAll:
		{
			bMagicFlag = false;
		}
		case ewiWeaponJammed:
		{
			SetWarningIconColor(&UIWeaponJammedIcon, cl);
			if (bMagicFlag)
			{
				break;
			}
		}
		case ewiRadiation:
		{
			SetWarningIconColor(&UIRadiaitionIcon, cl);
			if (bMagicFlag)
			{
				break;
			}
		}
		case ewiWound:
		{
			SetWarningIconColor(&UIWoundIcon, cl);
			if (bMagicFlag)
			{
				break;
			}
		}
		case ewiStarvation:
		{
			SetWarningIconColor(&UIStarvationIcon, cl);
			if (bMagicFlag)
			{
				break;
			}
		}
		case ewiPsyHealth:
		{
			SetWarningIconColor(&UIPsyHealthIcon, cl);
			if (bMagicFlag)
			{
				break;
			}
		}
		case ewiInvincible:
		{
			SetWarningIconColor(&UIInvincibleIcon, cl);
			if (bMagicFlag)
			{
				break;
			}
		}
		break;
		case ewiArtefact:
		{
			SetWarningIconColor(&UIArtefactIcon, cl);
		}
		break;
		default:
		{
			R_ASSERT(!"Unknown warning icon type");
		}
	}
}

void CUIMainIngameWnd::TurnOffWarningIcon(EWarningIcons icon)
{
	SetWarningIconColor(icon, 0x00ffffff);
}

void CUIMainIngameWnd::SetFlashIconState_(EFlashingIcons type, bool enable)
{
	// �������� �������� ��������� ������
	FlashingIcons_it icon = m_FlashingIcons.find(type);
	R_ASSERT2(icon != m_FlashingIcons.end( ), "Flashing icon with this type not existed");
	icon->second->Show(enable);
}

void CUIMainIngameWnd::InitFlashingIcons(CUIXml* node)
{
	pcstr const flashingIconNodeName = "flashing_icon";
	s32 staticsCount = node->GetNodesNum("", 0, flashingIconNodeName);

	CUIXmlInit xml_init;
	CUIStatic* pIcon = NULL;
	// ����������� �� ���� ����� � �������������� �� ��� �������
	for (s32 i = 0; i < staticsCount; ++i)
	{
		pIcon = xr_new<CUIStatic>( );
		xml_init.InitStatic(*node, flashingIconNodeName, i, pIcon);
		shared_str iconType = node->ReadAttrib(flashingIconNodeName, i, "type", "none");

		// ������ ���������� ������ � �� ���
		EFlashingIcons type = efiPdaTask;

		if (iconType == "pda")
		{
			type = efiPdaTask;
		}
		else if (iconType == "mail")
		{
			type = efiMail;
		}
		else
		{
			R_ASSERT(!"Unknown type of mainingame flashing icon");
		}

		R_ASSERT2(m_FlashingIcons.find(type) == m_FlashingIcons.end( ), "Flashing icon with this type already exists");

		CUIStatic*& val = m_FlashingIcons[type];
		val = pIcon;

		AttachChild(pIcon);
		pIcon->Show(false);
	}
}

void CUIMainIngameWnd::DestroyFlashingIcons( )
{
	for (FlashingIcons_it it = m_FlashingIcons.begin( ); it != m_FlashingIcons.end( ); ++it)
	{
		DetachChild(it->second);
		xr_delete(it->second);
	}

	m_FlashingIcons.clear( );
}

void CUIMainIngameWnd::UpdateFlashingIcons( )
{
	for (FlashingIcons_it it = m_FlashingIcons.begin( ); it != m_FlashingIcons.end( ); ++it)
	{
		it->second->Update( );
	}
}

void CUIMainIngameWnd::AnimateContacts(bool b_snd)
{
	UIPdaOnline.ResetClrAnimation( );

	if (b_snd)
	{
		SHudSound::PlaySound(m_contactSnd, fVector3( ).set(0.0f, 0.0f, 0.0f), 0, true);
	}
}

void CUIMainIngameWnd::SetPickUpItem(CInventoryItem* PickUpItem)
{
	m_pPickUpItem = PickUpItem;
}

void CUIMainIngameWnd::UpdatePickUpItem( )
{
	if (!m_pPickUpItem || !Level( ).CurrentViewEntity( ) || Level( ).CurrentViewEntity( )->CLS_ID != CLSID_OBJECT_ACTOR)
	{
		UIPickUpItemIcon.Show(false);
		return;
	}

	shared_str sect_name = m_pPickUpItem->object( ).cNameSect( );

	//properties used by inventory menu
	s32 m_iGridWidth = pSettings->r_u32(sect_name, "inv_grid_width");
	s32 m_iGridHeight = pSettings->r_u32(sect_name, "inv_grid_height");

	s32 m_iXPos = pSettings->r_u32(sect_name, "inv_grid_x");
	s32 m_iYPos = pSettings->r_u32(sect_name, "inv_grid_y");

	f32 scale_x = m_iPickUpItemIconWidth / f32(m_iGridWidth * INV_GRID_WIDTH);
	f32 scale_y = m_iPickUpItemIconHeight / f32(m_iGridHeight * INV_GRID_HEIGHT);

	scale_x = (scale_x > 1) ? 1.0f : scale_x;
	scale_y = (scale_y > 1) ? 1.0f : scale_y;

	f32 scale = scale_x < scale_y ? scale_x : scale_y;

	UIPickUpItemIcon.GetUIStaticItem( ).SetOriginalRect(
		f32(m_iXPos * INV_GRID_WIDTH),
		f32(m_iYPos * INV_GRID_HEIGHT),
		f32(m_iGridWidth * INV_GRID_WIDTH),
		f32(m_iGridHeight * INV_GRID_HEIGHT));

	UIPickUpItemIcon.SetStretchTexture(true);

	UIPickUpItemIcon.SetWidth(m_iGridWidth * INV_GRID_WIDTH * scale);
	UIPickUpItemIcon.SetHeight(m_iGridHeight * INV_GRID_HEIGHT * scale);

	UIPickUpItemIcon.SetWndPos(m_iPickUpItemIconX +
							   (m_iPickUpItemIconWidth - UIPickUpItemIcon.GetWidth( )) / 2,
							   m_iPickUpItemIconY +
							   (m_iPickUpItemIconHeight - UIPickUpItemIcon.GetHeight( )) / 2);

	UIPickUpItemIcon.SetColor(color_rgba(255, 255, 255, 192));
	UIPickUpItemIcon.Show(true);
}

void CUIMainIngameWnd::UpdateActiveItemInfo( )
{
	PIItem item = m_pActor->inventory( ).ActiveItem( );
	if (item)
	{
		xr_string str_name;
		xr_string icon_sect_name;
		xr_string str_count;
		item->GetBriefInfo(str_name, icon_sect_name, str_count);

		UIWeaponSignAmmo.Show(true);
		UIWeaponBack.SetText(str_name.c_str( ));
		UIWeaponSignAmmo.SetText(str_count.c_str( ));
		SetAmmoIcon(icon_sect_name.c_str( ));

		//-------------------
		m_pWeapon = smart_cast<CWeapon*> (item);
	}
	else
	{
		UIWeaponIcon.Show(false);
		UIWeaponSignAmmo.Show(false);
		UIWeaponBack.SetText("");
		m_pWeapon = NULL;
	}
}

void CUIMainIngameWnd::OnConnected( )
{
	UIZoneMap->SetupCurrentMap( );
}

void CUIMainIngameWnd::reset_ui( )
{
	m_pActor = NULL;
	m_pWeapon = NULL;
	m_pGrenade = NULL;
	m_pItem = NULL;
	m_pPickUpItem = NULL;
	UIMotionIcon.ResetVisibility( );
}

#ifdef DEBUG
#include "UIFrameWindow.h"
CUIFrameWindow* pUIFrame = NULL;

void test_update( )
{
	if (pUIFrame)
	{
		pUIFrame->Update( );
	}
}

void test_key(s32 dik)
{
	if (dik == DIK_K)
	{
		if (!pUIFrame)
		{
			CUIXml uiXml;
			uiXml.Init(CONFIG_PATH, UI_PATH, "talk.xml");

			pUIFrame = xr_new<CUIFrameWindow>( );
			CUIXmlInit::InitFrameWindow(uiXml, "frame_window", 0, pUIFrame);
		}
		else
		{
			xr_delete(pUIFrame);
		}
	}
}

void test_draw( )
{
	if (pUIFrame)
	{
		pUIFrame->Draw( );
	}
}

void CUIMainIngameWnd::draw_adjust_mode( )
{
	if (g_bHudAdjustMode && m_pWeapon) //draw firePoint,ShellPoint etc
	{
		CActor* pActor = smart_cast<CActor*>(Level( ).CurrentEntity( ));
		if (!pActor)
		{
			return;
		}

		bool bCamFirstEye = !!m_pWeapon->GetHUDmode( );
		string32 hud_view = "HUD view";
		string32 _3rd_person_view = "3-rd person view";
		CGameFont* F = UI( )->Font( )->pFontDI;
		F->SetAligment(CGameFont::alCenter);
		//.		F->SetSizeI			(0.02f);
		F->OutSetI(0.0f, -0.8f);
		F->SetColor(0xffffffff);
		F->OutNext("Hud_adjust_mode=%d", g_bHudAdjustMode);
		if (g_bHudAdjustMode == 1)
		{
			F->OutNext("adjusting zoom offset");
		}
		else if (g_bHudAdjustMode == 2)
		{
			F->OutNext("adjusting fire point for %s", bCamFirstEye ? hud_view : _3rd_person_view);
		}
		else if (g_bHudAdjustMode == 3)
		{
			F->OutNext("adjusting missile offset");
		}
		else if (g_bHudAdjustMode == 4)
		{
			F->OutNext("adjusting shell point for %s", bCamFirstEye ? hud_view : _3rd_person_view);
		}
		else if (g_bHudAdjustMode == 5)
		{
			F->OutNext("adjusting fire point 2 for %s", bCamFirstEye ? hud_view : _3rd_person_view);
		}

		if (bCamFirstEye)
		{
			CWeaponHUD* pWpnHud = NULL;
			pWpnHud = m_pWeapon->GetHUD( );

			fVector3 FP;
			fVector3 SP;
			fVector3 FP2;

			CKinematics* V = smart_cast<CKinematics*>(pWpnHud->Visual( ));
			VERIFY(V);
			V->CalculateBones( );

			// fire point&direction
			fMatrix4x4& fire_mat = V->LL_GetTransform(u16(pWpnHud->FireBone( )));
			fMatrix4x4& parent = pWpnHud->Transform( );

			const fVector3& fp = pWpnHud->FirePoint( );
			const fVector3& fp2 = pWpnHud->FirePoint2( );
			const fVector3& sp = pWpnHud->ShellPoint( );

			fire_mat.transform_tiny(FP, fp);
			parent.transform_tiny(FP);

			fire_mat.transform_tiny(FP2, fp2);
			parent.transform_tiny(FP2);

			fire_mat.transform_tiny(SP, sp);
			parent.transform_tiny(SP);

			RCache.dbg_DrawAABB(FP, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(255, 0, 0));
			RCache.dbg_DrawAABB(FP2, 0.02f, 0.02f, 0.02f, D3DCOLOR_XRGB(0, 0, 255));
			RCache.dbg_DrawAABB(SP, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(0, 255, 0));
		}
		else
		{
			fVector3 FP = m_pWeapon->get_CurrentFirePoint( );
			fVector3 FP2 = m_pWeapon->get_CurrentFirePoint2( );
			fVector3 SP = m_pWeapon->get_LastSP( );
			RCache.dbg_DrawAABB(FP, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(255, 0, 0));
			RCache.dbg_DrawAABB(FP2, 0.02f, 0.02f, 0.02f, D3DCOLOR_XRGB(0, 0, 255));
			RCache.dbg_DrawAABB(SP, 0.02f, 0.02f, 0.02f, D3DCOLOR_XRGB(0, 255, 0));
		}
	}
}
#endif

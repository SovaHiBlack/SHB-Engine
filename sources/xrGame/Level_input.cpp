#include "stdafx.h"
#include <dinput.h>
#include "HUDmanager.h"
#include "..\XR_3DA\Console.h"
#include "Level.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "Inventory.h"
#include "xrServer.h"

#include "Actor.h"
#include "HudItem.h"
#include "ui/UIDialogWnd.h"
#include "clsid_game.h"
#include "..\XR_3DA\Input.h"
#include "saved_game_wrapper.h"

#ifdef _DEBUG
#	include "ai/monsters/BaseMonster/base_monster.h"
#endif

#ifdef DEBUG
extern void try_change_current_entity( );
extern void restore_actor( );
#endif

bool g_bDisableAllInput = false;
extern	f32	g_fTimeFactor;

#define CURRENT_ENTITY( ) (game ? CurrentEntity() : nullptr)

void CLevel::IR_OnMouseWheel(s32 direction)
{
	if (g_bDisableAllInput)
	{
		return;
	}

	if (HUD( ).GetUI( )->IR_OnMouseWheel(direction))
	{
		return;
	}

	if (Device.Paused( ))
	{
		return;
	}

	if (game && Game( ).IR_OnMouseWheel(direction))
	{
		return;
	}

	if (HUD( ).GetUI( )->MainInputReceiver( ))
	{
		return;
	}

	if (CURRENT_ENTITY( ))
	{
		IInputReceiver* IR = smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CURRENT_ENTITY( )));
		if (IR)
		{
			IR->IR_OnMouseWheel(direction);
		}
	}
}

static s32 mouse_button_2_key[ ] = {MOUSE_1,MOUSE_2,MOUSE_3};

void CLevel::IR_OnMousePress(s32 btn)
{
	IR_OnKeyboardPress(mouse_button_2_key[btn]);
}

void CLevel::IR_OnMouseRelease(s32 btn)
{
	IR_OnKeyboardRelease(mouse_button_2_key[btn]);
}

void CLevel::IR_OnMouseHold(s32 btn)
{
	IR_OnKeyboardHold(mouse_button_2_key[btn]);
}

void CLevel::IR_OnMouseMove(s32 dx, s32 dy)
{
	if (g_bDisableAllInput)
	{
		return;
	}

	if (pHUD->GetUI( )->IR_OnMouseMove(dx, dy))
	{
		return;
	}

	if (Device.Paused( ))
	{
		return;
	}

	if (CURRENT_ENTITY( ))
	{
		IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY( )));
		if (IR)
		{
			IR->IR_OnMouseMove(dx, dy);
		}
	}
}

// Обработка нажатия клавиш
extern bool g_block_pause;

void CLevel::IR_OnKeyboardPress(s32 key)
{
	bool b_ui_exist = (pHUD && pHUD->GetUI( ));

	EGameActions _curr = get_binded_action(key);
	switch (_curr)
	{
		case kSCREENSHOT:
		{
			Render->Screenshot( );
			return;
		}
		break;
		case kCONSOLE:
		{
			Console->Show( );
			return;
		}
		break;
		case kQUIT:
		{
			if (b_ui_exist && HUD( ).GetUI( )->MainInputReceiver( ))
			{
				if (HUD( ).GetUI( )->MainInputReceiver( )->IR_OnKeyboardPress(key))
				{
					return;
				}

				HUD( ).GetUI( )->StartStopMenu(HUD( ).GetUI( )->MainInputReceiver( ), true);
			}
			else
			{
				Console->Execute("main_menu");
			}

			return;
		}
		break;
		case kPAUSE:
		{
			if (!g_block_pause)
			{
				Device.Pause(!Device.Paused( ), TRUE, TRUE, "li_pause_key");
			}

			return;
		}
		break;
	}

	if (g_bDisableAllInput)
	{
		return;
	}

	if (!b_ui_exist)
	{
		return;
	}

	if (b_ui_exist && pHUD->GetUI( )->IR_OnKeyboardPress(key))
	{
		return;
	}

	if (Device.Paused( ))
	{
		return;
	}

	if (game && Game( ).IR_OnKeyboardPress(key))
	{
		return;
	}

	if (_curr == kQUICK_SAVE)
	{
		Console->Execute("save");
		return;
	}

	if (_curr == kQUICK_LOAD)
	{

#ifdef DEBUG
		FS.get_path("$game_config$")->m_Flags.set(FS_Path::flNeedRescan, TRUE);
		FS.get_path("$game_scripts$")->m_Flags.set(FS_Path::flNeedRescan, TRUE);
		FS.rescan_pathes( );
#endif // def DEBUG

		string_path saved_game;
		string_path command;
		strconcat(sizeof(saved_game), saved_game, Core.UserName, "_", "quicksave");
		if (!CSavedGameWrapper::valid_saved_game(saved_game))
		{
			return;
		}

		strconcat(sizeof(command), command, "load ", saved_game);
		Console->Execute(command);
		return;
	}

#ifndef MASTER_GOLD
	switch (key)
	{
		case DIK_NUMPAD5:
		{
			Console->Hide( );
			Console->Execute("demo_record 1");
		}
		break;
#endif // MASTER_GOLD

#ifdef DEBUG
		case DIK_RETURN:
		{
			bDebug = !bDebug;
			return;
		}
		case DIK_BACK:
		{
			HW.Caps.SceneMode = (HW.Caps.SceneMode + 1) % 3;
			return;
		}
		case DIK_F4:
		{
			if (pInput->iGetAsyncKeyState(DIK_LALT))
			{
				break;
			}

			if (pInput->iGetAsyncKeyState(DIK_RALT))
			{
				break;
			}

			bool bOk = false;
			u32 i = 0;
			u32 j;
			u32 n = Objects.o_count( );
			if (pCurrentEntity)
			{
				for (; i < n; ++i)
				{
					if (Objects.o_get_by_iterator(i) == pCurrentEntity)
					{
						break;
					}
				}
			}

			if (i < n)
			{
				j = i;
				bOk = false;
				for (++i; i < n; ++i)
				{
					CEntityAlive* tpEntityAlive = smart_cast<CEntityAlive*>(Objects.o_get_by_iterator(i));
					if (tpEntityAlive)
					{
						bOk = true;
						break;
					}
				}

				if (!bOk)
				{
					for (i = 0; i < j; ++i)
					{
						CEntityAlive* tpEntityAlive = smart_cast<CEntityAlive*>(Objects.o_get_by_iterator(i));
						if (tpEntityAlive)
						{
							bOk = true;
							break;
						}
					}
				}

				if (bOk)
				{
					CObject* tpObject = CurrentEntity( );
					CObject* __I = Objects.o_get_by_iterator(i);
					CObject** I = &__I;

					SetEntity(*I);
					if (tpObject != *I)
					{
						CActor* pActor = smart_cast<CActor*> (tpObject);
						if (pActor)
						{
							pActor->inventory( ).Items_SetCurrentEntityHud(false);
						}
					}

					if (tpObject)
					{
						Engine.Sheduler.Unregister(tpObject);
						Engine.Sheduler.Register(tpObject, TRUE);
					}

					Engine.Sheduler.Unregister(*I);
					Engine.Sheduler.Register(*I, TRUE);

					CActor* pActor = smart_cast<CActor*> (*I);
					if (pActor)
					{
						pActor->inventory( ).Items_SetCurrentEntityHud(true);
						CHudItem* pHudItem = smart_cast<CHudItem*>(pActor->inventory( ).ActiveItem( ));
						if (pHudItem)
						{
							pHudItem->OnStateSwitch(pHudItem->GetState( ));
						}
					}
				}
			}

			return;
		}
		case MOUSE_1:
		{
			if (pInput->iGetAsyncKeyState(DIK_LALT))
			{
				if (CurrentEntity( )->CLS_ID == CLSID_OBJECT_ACTOR)
				{
					try_change_current_entity( );
				}
				else
				{
					restore_actor( );
				}

				return;
			}
		}
		break;
		case DIK_DIVIDE:
		{
			if (OnServer( ))
			{
				Server->game->SetGameTimeFactor(g_fTimeFactor);
			}
		}
		break;
		case DIK_MULTIPLY:
		{
			if (OnServer( ))
			{
				f32 NewTimeFactor = 1000.0f;
				Server->game->SetGameTimeFactor(NewTimeFactor);
			}
		}
		break;
#endif // def DEBUG

#ifdef DEBUG
		case DIK_F9:
		{
			//		if (!ai().get_alife())
			//			break;
			//		const_cast<CALifeSimulatorHeader&>(ai().alife().header()).set_state(ALife::eZoneStateSurge);
			break;
		}
		return;
		//	case DIK_F10:{
		//		ai().level_graph().set_dest_point();
		//		ai().level_graph().build_detail_path();
		//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
		//			return;
		//		if (!m_bSynchronization) {
		//			m_bSynchronization	= true;
		//			ai().level_graph().set_start_point();
		//			m_bSynchronization	= false;
		//		}
		//		luabind::functor<void>	functor;
		//		ai().script_engine().functor("alife_test.set_switch_online",functor);
		//		functor(0,false);
		//	}
		//		return;
		//	case DIK_F11:
		//		ai().level_graph().build_detail_path();
		//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
		//			return;
		//		if (!m_bSynchronization) {
		//			m_bSynchronization	= true;
		//			ai().level_graph().set_dest_point();
		//			ai().level_graph().select_cover_point();
		//			m_bSynchronization	= false;
		//		}
		//		return;
#endif // def DEBUG

#ifndef MASTER_GOLD
	}
#endif // ndef MASTER_GOLD

	if (bindConsoleCmds.execute(key))
	{
		return;
	}

	if (b_ui_exist && HUD( ).GetUI( )->MainInputReceiver( ))
	{
		return;
	}

	if (CURRENT_ENTITY( ))
	{
		IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY( )));
		if (IR)
		{
			IR->IR_OnKeyboardPress(get_binded_action(key));
		}
	}

#ifdef _DEBUG
	CObject* obj = Level( ).Objects.FindObjectByName("monster");
	if (obj)
	{
		CBaseMonster* monster = smart_cast<CBaseMonster*>(obj);
		if (monster)
		{
			monster->debug_on_key(key);
		}
	}
#endif

}

void CLevel::IR_OnKeyboardRelease(s32 key)
{
	if (g_bDisableAllInput)
	{
		return;
	}

	bool b_ui_exist = (pHUD && pHUD->GetUI( ));
	if (b_ui_exist && pHUD->GetUI( )->IR_OnKeyboardRelease(key))
	{
		return;
	}

	if (b_ui_exist && HUD( ).GetUI( )->MainInputReceiver( ))
	{
		return;
	}

	if (Device.Paused( ))
	{
		return;
	}

	if (game && Game( ).OnKeyboardRelease(get_binded_action(key)))
	{
		return;
	}

	if (CURRENT_ENTITY( ))
	{
		IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY( )));
		if (IR)
		{
			IR->IR_OnKeyboardRelease(get_binded_action(key));
		}
	}
}

void CLevel::IR_OnKeyboardHold(s32 key)
{
	if (g_bDisableAllInput)
	{
		return;
	}

	bool b_ui_exist = (pHUD && pHUD->GetUI( ));
	if (b_ui_exist && pHUD->GetUI( )->IR_OnKeyboardHold(key))
	{
		return;
	}

	if (b_ui_exist && HUD( ).GetUI( )->MainInputReceiver( ))
	{
		return;
	}

	if (Device.Paused( ))
	{
		return;
	}

	if (CURRENT_ENTITY( ))
	{
		IInputReceiver* IR = smart_cast<IInputReceiver*>(smart_cast<CGameObject*>(CURRENT_ENTITY( )));
		if (IR)
		{
			IR->IR_OnKeyboardHold(get_binded_action(key));
		}
	}
}

void CLevel::IR_OnMouseStop(s32 /*axis*/, s32 /*value*/)
{ }

void CLevel::IR_OnActivate( )
{
	if (!pInput)
	{
		return;
	}

	for (s32 i = 0; i < CInput::COUNT_KB_BUTTONS; i++)
	{
		if (IR_GetKeyState(i))
		{
			EGameActions action = get_binded_action(i);
			switch (action)
			{
				case kFWD:
				case kBACK:
				case kL_STRAFE:
				case kR_STRAFE:
				case kLEFT:
				case kRIGHT:
				case kUP:
				case kDOWN:
				case kCROUCH:
				case kACCEL:
				case kL_LOOKOUT:
				case kR_LOOKOUT:
				case kWPN_FIRE:
				{
					IR_OnKeyboardPress(i);
				}
				break;
			}
		}
	}
}

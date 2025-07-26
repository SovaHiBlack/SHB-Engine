////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.сpp :	функции для торговли и торговцев
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "ai/trader/Trader.h"
#include "ai/trader/TraderAnimation.h"

void CScriptGameObject::set_trader_global_anim(pcstr anim)
{
	CTrader* trader = smart_cast<CTrader*>(&object( ));
	if (!trader) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot cast sctipt game object to trader!");
		return;
	}
	trader->animation().set_animation(anim);
}

void CScriptGameObject::set_trader_head_anim(pcstr anim)
{
	CTrader* trader = smart_cast<CTrader*>(&object( ));
	if (!trader) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot cast sctipt game object to trader!");
		return;
	}
	trader->animation().set_head_animation(anim);
}

void CScriptGameObject::set_trader_sound(pcstr sound, pcstr anim)
{
	CTrader* trader = smart_cast<CTrader*>(&object( ));
	if (!trader) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot cast sctipt game object to trader!");
		return;
	}
	trader->animation().set_sound(sound, anim);
}

void CScriptGameObject::external_sound_start(pcstr sound)
{
	CTrader* trader = smart_cast<CTrader*>(&object( ));
	if (!trader) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot cast sctipt game object to trader!");
		return;
	}
	trader->animation().external_sound_start(sound);
}

void CScriptGameObject::external_sound_stop()
{
	CTrader* trader = smart_cast<CTrader*>(&object( ));
	if (!trader) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot cast sctipt game object to trader!");
		return;
	}
	trader->animation().external_sound_stop();
}

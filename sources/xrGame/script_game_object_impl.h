////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_impl.h
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "AISpace.h"
#include "script_engine.h"

IC	CGameObject &CScriptGameObject::object	() const
{
	if (m_game_object && m_game_object->lua_game_object() == this)
		return	(*m_game_object);

#ifdef DEBUG
	ai().script_engine().script_log(eLuaMessageTypeError,"you are trying to use a destroyed object [%x]",m_game_object);
	THROW2	(m_game_object && m_game_object->lua_game_object() == this,"Probably, you are trying to use a destroyed object!");
#endif
	return	(*m_game_object);
}

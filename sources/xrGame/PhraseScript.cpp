#include "stdafx.h"

#include "PhraseScript.h"
#include "script_engine.h"
#include "AISpace.h"
#include "GameObject.h"
#include "script_game_object.h"
#include "InfoPortion.h"
#include "InventoryOwner.h"
#include "ai_debug.h"
#include "ui/UIXml.h"
#include "Actor.h"

CPhraseScript::CPhraseScript( )
{ }

CPhraseScript::~CPhraseScript( )
{ }

//�������� �� XML �����
void CPhraseScript::Load(CUIXml* uiXml, XML_NODE* phrase_node)
{
	LoadSequence(uiXml, phrase_node, "precondition", m_Preconditions);
	LoadSequence(uiXml, phrase_node, "action", m_ScriptActions);

	LoadSequence(uiXml, phrase_node, "has_info", m_HasInfo);
	LoadSequence(uiXml, phrase_node, "dont_has_info", m_DontHasInfo);

	LoadSequence(uiXml, phrase_node, "give_info", m_GiveInfo);
	LoadSequence(uiXml, phrase_node, "disable_info", m_DisableInfo);
}

template<class T>
void CPhraseScript::LoadSequence(CUIXml* uiXml, XML_NODE* phrase_node, pcstr tag, T& str_vector)
{
	s32 tag_num = uiXml->GetNodesNum(phrase_node, tag);
	str_vector.clear( );
	for (s32 i = 0; i < tag_num; i++)
	{
		pcstr tag_text = uiXml->Read(phrase_node, tag, i, NULL);
		str_vector.push_back(tag_text);
	}
}

bool CPhraseScript::CheckInfo(const CInventoryOwner* pOwner) const
{
	THROW(pOwner);

	for (u32 i = 0; i < m_HasInfo.size( ); i++)
	{
		if (!Actor( )->HasInfo(m_HasInfo[i]))
		{

#ifdef DEBUG
			if (psAI_Flags.test(aiDialogs))
			{
				Msg("----rejected: [%s] has info %s", pOwner->Name( ), *m_HasInfo[i]);
			}
#endif

			return false;
		}
	}

	for (i = 0; i < m_DontHasInfo.size( ); i++)
	{
		if (Actor( )->HasInfo(m_DontHasInfo[i]))
		{

#ifdef DEBUG
			if (psAI_Flags.test(aiDialogs))
			{
				Msg("----rejected: [%s] dont has info %s", pOwner->Name( ), *m_DontHasInfo[i]);
			}
#endif

			return false;
		}
	}

	return true;
}

void CPhraseScript::TransferInfo(const CInventoryOwner* pOwner) const
{
	THROW(pOwner);

	for (u32 i = 0; i < m_GiveInfo.size( ); i++)
	{
		Actor( )->TransferInfo(m_GiveInfo[i], true);
	}

	for (i = 0; i < m_DisableInfo.size( ); i++)
	{
		Actor( )->TransferInfo(m_DisableInfo[i], false);
	}
}

bool CPhraseScript::Precondition(const CGameObject* pSpeakerGO, pcstr dialog_id, pcstr phrase_id) const
{
	bool predicate_result = true;

	if (!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO)))
	{

#ifdef DEBUG
		if (psAI_Flags.test(aiDialogs))
		{
			Msg("dialog [%s] phrase[%s] rejected by CheckInfo", dialog_id, phrase_id);
		}
#endif

		return false;
	}

	for (u32 i = 0; i < Preconditions( ).size( ); ++i)
	{
		luabind::functor<bool>	lua_function;
		THROW(*Preconditions( )[i]);
		bool functor_exists = ai( ).script_engine( ).functor(*Preconditions( )[i], lua_function);
		THROW3(functor_exists, "Cannot find precondition", *Preconditions( )[i]);
		predicate_result = lua_function(pSpeakerGO->lua_game_object( ));
		if (!predicate_result)
		{

#ifdef DEBUG
			if (psAI_Flags.test(aiDialogs))
			{
				Msg("dialog [%s] phrase[%s] rejected by script predicate", dialog_id, phrase_id);
			}
#endif

			break;
		}
	}

	return predicate_result;
}

void CPhraseScript::Action(const CGameObject* pSpeakerGO, pcstr dialog_id, pcstr phrase_id) const
{
	for (u32 i = 0; i < Actions( ).size( ); ++i)
	{
		luabind::functor<void>	lua_function;
		THROW(*Actions( )[i]);
		bool functor_exists = ai( ).script_engine( ).functor(*Actions( )[i], lua_function);
		THROW3(functor_exists, "Cannot find phrase dialog script function", *Actions( )[i]);
		lua_function(pSpeakerGO->lua_game_object( ), dialog_id);
	}

	TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO));
}

bool CPhraseScript::Precondition(const CGameObject* pSpeakerGO1,
								 const CGameObject* pSpeakerGO2,
								 pcstr dialog_id,
								 pcstr phrase_id,
								 pcstr next_phrase_id) const
{
	bool predicate_result = true;

	if (!CheckInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1)))
	{

#ifdef DEBUG
		if (psAI_Flags.test(aiDialogs))
		{
			Msg("dialog [%s] phrase[%s] rejected by CheckInfo", dialog_id, phrase_id);
		}
#endif

		return false;
	}

	for (u32 i = 0; i < Preconditions( ).size( ); ++i)
	{
		luabind::functor<bool>	lua_function;
		THROW(*Preconditions( )[i]);
		bool functor_exists = ai( ).script_engine( ).functor(*Preconditions( )[i], lua_function);
		THROW3(functor_exists, "Cannot find phrase precondition", *Preconditions( )[i]);
		predicate_result = lua_function(pSpeakerGO1->lua_game_object( ), pSpeakerGO2->lua_game_object( ), dialog_id, phrase_id, next_phrase_id);
		if (!predicate_result)
		{

#ifdef DEBUG
			if (psAI_Flags.test(aiDialogs))
			{
				Msg("dialog [%s] phrase[%s] rejected by script predicate", dialog_id, phrase_id);
			}
#endif

			break;
		}
	}

	return predicate_result;
}

void CPhraseScript::Action(const CGameObject* pSpeakerGO1, const CGameObject* pSpeakerGO2, pcstr dialog_id, pcstr phrase_id) const
{
	TransferInfo(smart_cast<const CInventoryOwner*>(pSpeakerGO1));

	for (u32 i = 0; i < Actions( ).size( ); ++i)
	{
		luabind::functor<void>	lua_function;
		THROW(*Actions( )[i]);
		bool functor_exists = ai( ).script_engine( ).functor(*Actions( )[i], lua_function);
		THROW3(functor_exists, "Cannot find phrase dialog script function", *Actions( )[i]);
		try
		{
			lua_function(pSpeakerGO1->lua_game_object( ), pSpeakerGO2->lua_game_object( ), dialog_id, phrase_id);
		}
		catch (...)
		{
		}
	}
}

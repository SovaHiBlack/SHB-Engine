// PhraseScript.h классы дл€ св€зи диалогов со скриптами

#pragma once

#include "InfoPortion_defs.h"

class CGameObject;
class CInventoryOwner;
class TiXmlNode;
class CUIXml;

using XML_NODE = TiXmlNode;

class CPhraseScript
{
public:
											CPhraseScript		( );
	virtual									~CPhraseScript		( );

	//загрузка из XML файла
	virtual void							Load				(CUIXml* ui_xml, XML_NODE* phrase_node);

	//вызов с одним параметром (info_portion)
	virtual bool							Precondition		(const CGameObject* pSpeaker, const char* dialog_id, const char* phrase_id) const;
	virtual void							Action				(const CGameObject* pSpeaker, const char* dialog_id, const char* phrase_id) const;
	//вызов с двум€ параметрами (dialog, phrase)
	virtual bool							Precondition		(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2, const char* dialog_id, const char* phrase_id, const char* next_phrase_id) const;
	virtual void							Action				(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2, const char* dialog_id, const char* phrase_id) const;

	using PRECONDITION_VECTOR									= xr_vector<CSharedString>;

	virtual const PRECONDITION_VECTOR&		Preconditions		( ) const
	{
		return m_Preconditions;
	}

	using ACTION_NAME_VECTOR									= xr_vector<CSharedString>;

	virtual const ACTION_NAME_VECTOR&		Actions				( ) const
	{
		return m_ScriptActions;
	}

	void									AddPrecondition		(const char* str);
	void									AddAction			(const char* str);
	void									AddHasInfo			(const char* str);
	void									AddDontHasInfo		(const char* str);
	void									AddGiveInfo			(const char* str);
	void									AddDisableInfo		(const char* str);

protected:
	//загрузка содержани€ последовательности тагов в контейнер строк
	template<class T>
	void									LoadSequence		(CUIXml* ui_xml, XML_NODE* phrase_node, const char* tag, T& str_vector);

	//манипул€ции с информацией во врем€ вызовов Precondition и Action
	virtual bool							CheckInfo			(const CInventoryOwner* pOwner) const;
	virtual void							TransferInfo		(const CInventoryOwner* pOwner) const;

	//скриптовые действи€, которые активируетс€ после того как говоритс€ фраза
	using ACTION_NAME_VECTOR									= xr_vector<CSharedString>;
	ACTION_NAME_VECTOR											m_ScriptActions;

	using INFO_VECTOR											= xr_vector<CSharedString>;
	INFO_VECTOR													m_GiveInfo;
	INFO_VECTOR													m_DisableInfo;

	//список скриптовых предикатов, выполнение, которых необходимо дл€ того чтоб фраза стала доступной
	using PRECONDITION_VECTOR									= xr_vector<CSharedString>;
	PRECONDITION_VECTOR											m_Preconditions;

	//проверка наличи€/отсутстви€ информации
	INFO_VECTOR													m_HasInfo;
	INFO_VECTOR													m_DontHasInfo;
};

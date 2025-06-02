#pragma once

#include "shared_data.h"
#include "PhraseScript.h"

#include "xml_str_id_loader.h"
#include "EncyclopediaArticle_defs.h"

#include "GameTask_defs.h"
#include "PhraseDialog_defs.h"

struct SInfoPortionData : CSharedResource
{
	SInfoPortionData( );
	virtual				~SInfoPortionData( );

	//массив с именами диалогов, которые могут быть инициированы из этой InfoPortion.
	DIALOG_ID_VECTOR	m_DialogNames;

	//список статей в энциклопедии, которые становятся известными.
	ARTICLE_ID_VECTOR	m_Articles;
	//список статей в энциклопедии, которые становятся неизвестными (на тот случай если нужно заменить одну статью другой).
	ARTICLE_ID_VECTOR	m_ArticlesDisable;

	//присоединенные задания.
	TASK_ID_VECTOR		m_GameTasks;

	//скриптовые действия, которые активируется после того как информацию получает персонаж.
	CPhraseScript		m_PhraseScript;

	//массив с индексами тех порций информации, которые исчезнут, после получения этой InfoPortion
	DEFINE_VECTOR(shared_str, INFO_ID_VECTOR, INFO_ID_VECTOR_IT);
	INFO_ID_VECTOR		m_DisableInfo;
};

class CInfoPortion;

//квант - порция информации
class CInfoPortion : public CSharedClass<SInfoPortionData, shared_str, false>, public CXML_IdToIndex<CInfoPortion>
{
private:
	typedef CSharedClass<SInfoPortionData, shared_str, false>	inherited_shared;
	typedef CXML_IdToIndex<CInfoPortion>						id_to_index;

	friend id_to_index;

public:
	CInfoPortion( );
	virtual		~CInfoPortion( );

	//инициализация InfoPortion данными.
	//если InfoPortion с таким id раньше не использовалась она будет загружена из файла
	virtual void Load(shared_str info_str_id);

	const ARTICLE_ID_VECTOR& Articles( ) const
	{
		return info_data( )->m_Articles;
	}
	const ARTICLE_ID_VECTOR& ArticlesDisable( ) const
	{
		return info_data( )->m_ArticlesDisable;
	}
	const TASK_ID_VECTOR& GameTasks( ) const
	{
		return info_data( )->m_GameTasks;
	}
	const DIALOG_ID_VECTOR& DialogNames( ) const
	{
		return info_data( )->m_DialogNames;
	}
	const SInfoPortionData::INFO_ID_VECTOR& DisableInfos( ) const
	{
		return info_data( )->m_DisableInfo;
	}

	void RunScriptActions(const CGameObject* pOwner)
	{
		info_data( )->m_PhraseScript.Action(pOwner, NULL, NULL);
	}

protected:
	shared_str		m_InfoId;

	void			load_shared(pcstr);
	SInfoPortionData* info_data( )
	{
		VERIFY(inherited_shared::get_sd( ));
		return inherited_shared::get_sd( );
	}
	const SInfoPortionData* info_data( ) const
	{
		VERIFY(inherited_shared::get_sd( ));
		return inherited_shared::get_sd( );
	}

	static void InitXmlIdToIndex( );
};

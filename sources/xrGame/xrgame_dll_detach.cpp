#include "stdafx.h"

#include "AISpace.h"
#include "object_factory.h"
#include "ai/monsters/ai_monster_squad_manager.h"
#include "StringTable.h"

#include "EntityAlive.h"
#include "ui/UIInventoryUtilities.h"//
#include "ui/UIXmlInit.h"

#include "InfoPortion.h"
#include "PhraseDialog.h"
#include "GameTask.h"
#include "EncyclopediaArticle.h"

#include "CharacterInfo.h"
#include "SpecificCharacter.h"
#include "CharacterCommunity.h"
#include "MonsterCommunity.h"
#include "CharacterRank.h"
#include "CharacterReputation.h"

#include "Profiler.h"

#include "sound_collection_storage.h"
#include "RelationRegistry.h"

typedef xr_vector<std::pair<shared_str,int> >	STORY_PAIRS;
extern STORY_PAIRS								story_ids;
extern STORY_PAIRS								spawn_story_ids;

extern void show_smart_cast_stats					();
extern void clear_smart_cast_stats					();
extern void release_smart_cast_stats				();
extern void dump_list_wnd							();
extern void dump_list_lines							();
extern void dump_list_sublines						();
extern void clean_wnd_rects							();
extern void dump_list_xmls							();
extern void CreateUIGeom							();
extern void DestroyUIGeom							();

#include "..\XR_3DA\IGame_Persistent.h"
void init_game_globals()
{
	CreateUIGeom									();

	CInfoPortion::InitInternal						();
	CEncyclopediaArticle::InitInternal				();
	CPhraseDialog::InitInternal						();
	InventoryUtilities::CreateShaders				();

	CCharacterInfo::InitInternal					();
	CSpecificCharacter::InitInternal				();
	CCharacterCommunity::InitInternal				();
	CCharacterRank::InitInternal					();
	CCharacterReputation::InitInternal				();
	CMonsterCommunity::InitInternal					();
}

extern CUIXml*	g_gameTaskXml;
extern CUIXml*	g_uiSpotXml;

extern void destroy_lua_wpn_params	();

void clean_game_globals()
{
	destroy_lua_wpn_params							();
	// destroy ai space
	xr_delete										(g_ai_space);
	// destroy object factory
	xr_delete										(g_object_factory);
	// destroy monster squad global var
	xr_delete										(g_monster_squad);

	story_ids.clear									();
	spawn_story_ids.clear							();

	CInfoPortion::DeleteSharedData					();
	CInfoPortion::DeleteIdToIndexData				();

	CEncyclopediaArticle::DeleteSharedData			();
	CEncyclopediaArticle::DeleteIdToIndexData		();

	CPhraseDialog::DeleteSharedData					();
	CPhraseDialog::DeleteIdToIndexData				();

	InventoryUtilities::DestroyShaders				();

	CCharacterInfo::DeleteSharedData				();
	CCharacterInfo::DeleteIdToIndexData				();
	
	CSpecificCharacter::DeleteSharedData			();
	CSpecificCharacter::DeleteIdToIndexData			();

	CCharacterCommunity::DeleteIdToIndexData		();
	CCharacterRank::DeleteIdToIndexData				();
	CCharacterReputation::DeleteIdToIndexData		();
	CMonsterCommunity::DeleteIdToIndexData			();

	//static shader for blood
	CEntityAlive::UnloadBloodyWallmarks				();
	CEntityAlive::UnloadFireParticles				();
	//очищение памяти таблицы строк
	CStringTable::Destroy							();
	// Очищение таблицы цветов
	CUIXmlInit::DeleteColorDefs						();
	// Очищение таблицы идентификаторов рангов и отношений сталкеров
	InventoryUtilities::ClearCharacterInfoStrings	();

	xr_delete										(g_sound_collection_storage);
	
#ifdef DEBUG
	xr_delete										(g_profiler);
	release_smart_cast_stats						();
#endif

	SRelationRegistry::clear_relation_registry		();

	dump_list_wnd									();
	dump_list_lines									();
	dump_list_sublines								();
	clean_wnd_rects									();
	xr_delete										(g_gameTaskXml);
	xr_delete										(g_uiSpotXml);
	dump_list_xmls									();
	DestroyUIGeom									();
}

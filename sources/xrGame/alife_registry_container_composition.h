//	Module 		: alife_registry_container_composition.h
//	Description : ALife registry container class, composition of the registries

#pragma once

#include "InfoPortion_defs.h"
#include "PdaMsg.h"
#include "EncyclopediaArticle_defs.h"

#include "CharacterInfo_defs.h"
#include "RelationRegistry_defs.h"

#include "GameTask_defs.h"
#include "game_news.h"
#include "MapLocation_defs.h"

#include "ActorStatistic_defs.h"

#pragma warning(push)
#pragma warning(disable:4005)

//#include "alife_abstract_registry.h"

template <typename _index_type, typename _data_type>
class CALifeAbstractRegistry;

//для всех персонажей, те порции информации, которые они помнят
typedef CALifeAbstractRegistry<unsigned short, KNOWN_INFO_VECTOR> CInfoPortionRegistry;
add_to_registry_type_list(CInfoPortionRegistry)
#define info_portions define_constant(CInfoPortionRegistry)
#define registry_type_list save_registry_type_list(CInfoPortionRegistry)

//для всех персонажей, отношения с другими персонажами
typedef CALifeAbstractRegistry<unsigned short, SRelationData> CRelationRegistry;
add_to_registry_type_list(CRelationRegistry)
#define character_relations define_constant(CRelationRegistry)
#define registry_type_list save_registry_type_list(CRelationRegistry)

//для актеров, список персонажей с которыми были разговоры
typedef CALifeAbstractRegistry<unsigned short, TALK_CONTACT_VECTOR> CKnownContactsRegistry;
add_to_registry_type_list(CKnownContactsRegistry)
#define known_contacts define_constant(CKnownContactsRegistry)
#define registry_type_list save_registry_type_list(CKnownContactsRegistry)

// список статей энциклопедии, которые знает актер
typedef CALifeAbstractRegistry<unsigned short, ARTICLE_VECTOR> CEncyclopediaRegistry;
add_to_registry_type_list(CEncyclopediaRegistry)
#define encyclopedia_articles define_constant(CEncyclopediaRegistry) 
#define registry_type_list save_registry_type_list(CEncyclopediaRegistry)

// список новостей полученных актером, состоит из новостей симуляции и сюжетных (скриптованых) новостей
typedef CALifeAbstractRegistry<unsigned short, GameNewsVec> CGameNewsRegistry;
add_to_registry_type_list(CGameNewsRegistry)
#define game_news define_constant(CGameNewsRegistry)
#define registry_type_list save_registry_type_list(CGameNewsRegistry)

// список описаний персонажей, которые уже задействованы в игре
typedef CALifeAbstractRegistry<CSharedString, int> CSpecificCharacterRegistry;
add_to_registry_type_list(CSpecificCharacterRegistry)
#define specific_characters define_constant(CSpecificCharacterRegistry)
#define registry_type_list save_registry_type_list(CSpecificCharacterRegistry)

// map locations for actor
add_to_registry_type_list(CMapLocationRegistry)
#define map_locations define_constant(CMapLocationRegistry)
#define registry_type_list save_registry_type_list(CMapLocationRegistry)

// game tasks for actor
add_to_registry_type_list(CGameTaskRegistry)
#define map_locations define_constant(CGameTaskRegistry)
#define registry_type_list save_registry_type_list(CGameTaskRegistry)

// ActorStatistics
add_to_registry_type_list(CActorStatisticRegistry)
#define map_locations define_constant(CActorStatisticRegistry)
#define registry_type_list save_registry_type_list(CActorStatisticRegistry)

#pragma warning(pop)

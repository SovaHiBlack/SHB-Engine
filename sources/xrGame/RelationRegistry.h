// RelationRegistry.h: реестр для хранения данных об отношении персонажа к другим персонажам

#pragma once

#include "CharacterInfo_defs.h"

class CRelationRegistryWrapper;

class CInventoryOwner;
class CEntityAlive;

#define GAME_RELATIONS_SECT "game_relations"
#define ACTIONS_POINTS_SECT "action_points"

struct SRelationRegistry
{
public:
	SRelationRegistry( );
	virtual ~SRelationRegistry( );

	template<typename T>
	ALife::ERelationType GetRelationBetween(T char1, T char2) const;

	template<typename T>
	ALife::ERelationType GetRelationType(T from, T to) const;
	template<typename T>
	void				 SetRelationType(T from, T to, ALife::ERelationType new_relation);

	//общее отношение  одного персонажа к другому, вычисленное по формуле
	//с учетом всех факторов - величина от 
	//-100< (крайне враждебное) до >100 (очень дрюжелюбное)

	template<typename T>
	CharacterGoodwill	 GetAttitude(T from, T to)	const;

	//личное отношение (благосклонность) одного персонажа к другому - 
	//величина от -100< (крайне враждебное) до >100 (очень дрюжелюбное)
	CharacterGoodwill	 GetGoodwill(U16 from, U16 to) const;
	void				 SetGoodwill(U16 from, U16 to, CharacterGoodwill goodwill);
	void				 ChangeGoodwill(U16 from, U16 to, CharacterGoodwill delta_goodwill);

	//отношения группировки к персонажу (именно так, а не наоборот)
	//т.е. персонаж сам помнит, как к нему какая группировка отностися
	CharacterGoodwill	 GetCommunityGoodwill(CharacterCommunityIndex from_community, U16 to_character) const;
	void				 SetCommunityGoodwill(CharacterCommunityIndex from_community, U16 to_character, CharacterGoodwill goodwill);
	void				 ChangeCommunityGoodwill(CharacterCommunityIndex from_community, U16 to_character, CharacterGoodwill delta_goodwill);

	void				 ClearRelations(U16 person_id);

private:
	CharacterGoodwill	 GetCommunityRelation(CharacterCommunityIndex, CharacterCommunityIndex) const;
	CharacterGoodwill	 GetRankRelation(CharacterRankValue, CharacterRankValue) const;
	CharacterGoodwill	 GetReputationRelation(CharacterReputationValue, CharacterReputationValue) const;

	//реакцией на действия персонажей и соответствующее изменение отношения
public:
	//список действий актера, за которые начисляются
	//очки рейтинга, репутации или меняется отношения персонажа
	//к группировке
	enum ERelationAction
	{
		KILL = 0x00,		//убийство персонажа
		ATTACK = 0x01,		//атака персонажа
		FIGHT_HELP_HUMAN = 0x02,		//помощь в драке персонажу с другим персонажем
		FIGHT_HELP_MONSTER = 0x04,		//помощь в драке персонажу c монстром
		SOS_HELP = 0x08		//приход на помощь по сигналу SOS
	};
	void Action(CEntityAlive* from, CEntityAlive* to, ERelationAction action);

	struct SFightData
	{
		SFightData( );
		U16					attacker;
		U16					defender;
		float				total_hit;
		u32					time;
		u32					time_old;

		u32						attack_time;			//время фиксирования события "атака"
		ALife::ERelationType	defender_to_attacker;	//как относился атакованый к нападавшему во время начальной атаки
	};

	struct SRelationMapSpots
	{
		SRelationMapSpots( );
		CSharedString			spot_names[ALife::eRelationTypeLast + 1];
		const CSharedString& GetSpotName(ALife::ERelationType& type)
		{
			if (type < ALife::eRelationTypeLast)return spot_names[type];
			else return spot_names[ALife::eRelationTypeLast];
		}
	};

	// зарегистрировать драку (реакция на Hit в EntityAlive)
	void FightRegister(U16 attacker, U16 defender, ALife::ERelationType defender_to_attacker, float hit_amount);
	void UpdateFightRegister( );

private:
	using FightDataVec = xr_vector<SFightData>;
	using FightDataVec_it = FightDataVec::iterator;

	static FightDataVec* m_fight_registry;
	static FightDataVec& fight_registry( );

	SFightData* FindFight(U16 object_id, bool by_attacker/* = true*/);
	static SRelationMapSpots* m_spot_names;

public:
	const CSharedString& GetSpotName(ALife::ERelationType& type);
	static CRelationRegistryWrapper& relation_registry( );
	static void									clear_relation_registry( );

private:
	static CRelationRegistryWrapper* m_relation_registry;
};

#include "RelationRegistry_inline.h"

#pragma once

#include "alife_space.h"

// личное отношение (благосклонность) одного персонажа к другому - величина от -100< (крайне враждебное) до >100 (очень дрюжелюбное)
using CharacterGoodwill			= int;
#define NO_GOODWILL				-type_max(CharacterGoodwill)
#define NEUTRAL_GOODWILL		CharacterGoodwill(0)

using CharacterClass			= CSharedString;
#define NO_CHARACTER_CLASS		NULL

// репутация персонажа - величина от -100 (очень плохой, беспредельщик) до 100 (очень хороший, благородный)
using CharacterReputationValue	= int;
#define NO_REPUTATION			-type_max(CharacterReputationValue)
#define NEUTAL_REPUTATION		0

// ранг персонажа - величина от 0 (совсем неопытный) до >100 (очень опытный)
using CharacterRankValue		= int;
#define NO_RANK					-type_max(CharacterRankValue)

using CharacterCommunityId		= CSharedString;
#define NO_COMMUNITY_ID			CharacterCommunityId(NULL)

using CharacterCommunityIndex	= int;
#define NO_COMMUNITY_INDEX		CharacterCommunityIndex(-1)

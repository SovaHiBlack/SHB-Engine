#pragma once

#include "../BaseMonster/base_monster.h"
#include "../../../script_export_space.h"

class CPseudoDog : public CBaseMonster
{
	typedef		CBaseMonster	inherited;

public:
	f32			m_anger_hunger_threshold;
	f32			m_anger_loud_threshold;

	TTime			m_time_became_angry;

	TTime			time_growling;			// время нахождения в состоянии пугания

	enum
	{
		eAdditionalSounds = MonsterSound::eMonsterSoundCustom,
		ePsyAttack = eAdditionalSounds | 0
	};

public:
	CPseudoDog( );
	virtual			~CPseudoDog( );

	virtual DLL_Pure* _construct( );

	virtual void	Load(pcstr section);

	virtual void	reinit( );
	virtual void	reload(pcstr section);

	virtual bool	ability_can_drag( )
	{
		return true;
	}
	virtual bool	ability_psi_attack( )
	{
		return true;
	}

	virtual void	CheckSpecParams(u32 spec_params);

	virtual void	HitEntityInJump(const CEntity* pEntity);

	virtual IStateManagerBase* create_state_manager( );

private:

#ifdef _DEBUG
	virtual void	debug_on_key(s32 key);
#endif

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPseudoDog)
#undef script_type_list
#define script_type_list save_type_list(CPseudoDog)

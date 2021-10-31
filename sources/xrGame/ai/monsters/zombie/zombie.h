#pragma once

#include "../BaseMonster/BaseMonster.h"
#include "..\ControlledEntity.h"
#include "../ai_monster_bones.h"
#include "../anim_triple.h"
#include "../../../script_export_space.h"

#define FAKE_DEATH_TYPES_COUNT	4

class CZombie :	public CBaseMonster,
				public CControlledEntity<CZombie> {
	
	typedef		CBaseMonster				inherited;
	typedef		CControlledEntity<CZombie>	CControlled;

	bonesManipulation	Bones;

public:
					CZombie		();
	virtual			~CZombie	();	

	virtual void	Load				(const char* section);
	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	reinit				();
	virtual	void	reload				(const char* section);
	
	virtual	void	Hit					(SHit* pHDS);

	virtual bool	ability_pitch_correction () {return false;}

	virtual void	shedule_Update		(u32 dt);
	
	static	void 	BoneCallback		(CBoneInstance *B);
			void	vfAssignBones		();

	virtual bool	use_center_to_aim				() const {return true;}

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	SAnimationTripleData	anim_triple_death[FAKE_DEATH_TYPES_COUNT];
	unsigned char				active_triple_idx;
	
	u32				time_dead_start;
	u32				last_hit_frame;
	u32				time_resurrect;

	unsigned char				fake_death_count;
	float			health_death_threshold;
	unsigned char				fake_death_left;

	bool			fake_death_fall_down	(); //return true if everything is ok
	void			fake_death_stand_up		();

#ifdef _DEBUG
	virtual void	debug_on_key			(int key);
#endif

public:
	static void script_register(lua_State*);
};

add_to_type_list(CZombie)
#undef script_type_list
#define script_type_list save_type_list(CZombie)

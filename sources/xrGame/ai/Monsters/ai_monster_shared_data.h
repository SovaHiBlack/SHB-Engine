#pragma once 
#include "ai_monster_defs.h"

struct SMonsterSettings {
	
	// float speed factors

	f32					m_fDistToCorpse;
	f32					m_fDamagedThreshold;		// порог здоровья, ниже которого устанавливается флаг m_bDamaged

	// -------------------------------------------------------

	u32						m_dwIdleSndDelay;
	u32						m_dwEatSndDelay;
	u32						m_dwAttackSndDelay;

	u32						m_dwDistantIdleSndDelay;
	f32					m_fDistantIdleSndRange;

	// -------------------------------------------------------

	u32						m_dwDayTimeBegin;
	u32						m_dwDayTimeEnd;
	f32					satiety_threshold;
	
	// ----------------------------------------------------------- 

	f32					m_fSoundThreshold;

	f32					m_fEatFreq;
	f32					m_fEatSlice;
	f32					m_fEatSliceWeight;

	u8						m_legs_number;
	SAttackEffector			m_attack_effector;

	f32					m_max_hear_dist;

	f32					m_run_attack_path_dist;
	f32					m_run_attack_start_dist;
};

#pragma once 
#include "ai_monster_defs.h"

struct SMonsterSettings {
	
	// float speed factors

	f32					m_fDistToCorpse;
	F32					m_fDamagedThreshold;		// порог здоровья, ниже которого устанавливается флаг m_bDamaged

	// -------------------------------------------------------

	u32						m_dwIdleSndDelay;
	u32						m_dwEatSndDelay;
	u32						m_dwAttackSndDelay;

	u32						m_dwDistantIdleSndDelay;
	F32					m_fDistantIdleSndRange;

	// -------------------------------------------------------

	u32						m_dwDayTimeBegin;
	u32						m_dwDayTimeEnd;
	F32					satiety_threshold;
	
	// ----------------------------------------------------------- 

	F32					m_fSoundThreshold;

	F32					m_fEatFreq;
	F32					m_fEatSlice;
	F32					m_fEatSliceWeight;

	u8						m_legs_number;
	SAttackEffector			m_attack_effector;

	F32					m_max_hear_dist;

	F32					m_run_attack_path_dist;
	F32					m_run_attack_start_dist;
};

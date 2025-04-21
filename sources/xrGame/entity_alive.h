#pragma once

#include "entity.h"

DEFINE_VECTOR(ref_shader, SHADER_VECTOR, SHADER_VECTOR_IT);
DEFINE_VECTOR(shared_str, STR_VECTOR, STR_VECTOR_IT);

class MONSTER_COMMUNITY;
class CEntityCondition;
class CWound;
class CCharacterPhysicsSupport;
class CMaterialManager;
class CVisualMemoryManager;
class CBlend;

class CEntityAlive : public CEntity
{
private:
	typedef	CEntity			inherited;

public:
	virtual CEntityAlive* cast_entity_alive( )
	{
		return this;
	}

public:
	bool					m_bMobility;
	f32					m_fAccuracy;
	f32					m_fIntelligence;
	//m_PhysicMovementControl
	//CPHMovementControl		*m_PhysicMovementControl;

public:
	// General
	CEntityAlive( );
	virtual					~CEntityAlive( );

	// Core events
	virtual DLL_Pure* _construct( );
	virtual void			Load(pcstr section);
	virtual void			reinit( );
	virtual void			reload(pcstr section);

	//object serialization
	virtual void			save(CNetPacket& output_packet);
	virtual void			load(IReader& input_packet);

	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );
	virtual	BOOL			net_SaveRelevant( );

	virtual void			shedule_Update(u32 dt);
	virtual	void			create_anim_mov_ctrl(CBlend* b);
	virtual	void			destroy_anim_mov_ctrl( );

	virtual void			HitImpulse(f32 amount, fVector3& vWorldDir, fVector3& vLocalDir);
	virtual	void			Hit(SHit* pHDS);
	virtual void			Die(CObject* who);
	virtual void			g_WeaponBones(s32& L, s32& R1, s32& R2) = 0;

	virtual f32			g_Radiation( )	const;
	virtual	f32			SetfRadiation(f32 value);

	virtual f32			CalcCondition(f32 hit);

	// Visibility related
	virtual	f32			ffGetFov( )	const = 0;
	virtual	f32			ffGetRange( )	const = 0;

	virtual bool			human_being( ) const
	{
		return	(false);
	}

public:
	//IC	CPHMovementControl*					PMovement					()						{return m_PhysicMovementControl;}

	virtual u16								PHGetSyncItemsNumber( );
	virtual CPHSynchronize* PHGetSyncItem(u16 item);
	virtual void							PHUnFreeze( );
	virtual void							PHFreeze( );

	virtual void							PHGetLinearVell(fVector3& velocity);
	virtual CPHSoundPlayer* ph_sound_player( );
	virtual	CIKLimbsController* character_ik_controller( );
	virtual SCollisionHitCallback* get_collision_hit_callback( );
	virtual bool							set_collision_hit_callback(SCollisionHitCallback* cc);

protected:
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR				m_ParticleWounds;

	virtual void				StartFireParticles(CWound* pWound);
	virtual void				UpdateFireParticles( );
	virtual void				LoadFireParticles(pcstr section);

public:
	static  void				UnloadFireParticles( );

protected:
	static STR_VECTOR* m_pFireParticlesVector;
	static u32					m_dwMinBurnTime;
	static f32				m_fStartBurnWoundSize;
	static f32				m_fStopBurnWoundSize;

	virtual void				BloodyWallmarks(f32 P, const fVector3& dir, s16 element, const fVector3& position_in_object_space);
	static  void				LoadBloodyWallmarks(pcstr section);

public:
	static  void				UnloadBloodyWallmarks( );

	void						ClearBloodWounds( )
	{
		m_BloodWounds.clear( );
	};

protected:
	virtual void				PlaceBloodWallmark(const fVector3& dir, const fVector3& start_pos,
												   f32 trace_dist, f32 wallmark_size,
												   SHADER_VECTOR& wallmarks_vector);

//информация о кровавых отметках на стенах, общая для всех CEntityAlive
	static SHADER_VECTOR* m_pBloodMarksVector;
	static f32				m_fBloodMarkSizeMax;
	static f32				m_fBloodMarkSizeMin;
	static f32				m_fBloodMarkDistance;
	static f32				m_fNominalHit;

	//текстурки капель крови
	static SHADER_VECTOR* m_pBloodDropsVector;
	//список ран с которых капает кровь

	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR				m_BloodWounds;
	//размер раны, чтоб начала капать кровь
	static f32				m_fStartBloodWoundSize;
	//размер раны, чтоб остановить кровь
	static f32				m_fStopBloodWoundSize;
	static f32				m_fBloodDropSize;

	//обновление ран, и рисование отметок от капающей крови
	virtual void				StartBloodDrops(CWound* pWound);
	virtual void				UpdateBloodDrops( );

	//отношения между существами и персонажами в зоне
public:
	virtual	ALife::ERelationType tfGetRelationType(const CEntityAlive* tpEntityAlive) const;
	virtual	bool				 is_relation_enemy(const CEntityAlive* tpEntityAlive) const;
public:
	MONSTER_COMMUNITY* monster_community;

private:
	CEntityCondition* m_entity_condition;
	CMaterialManager* m_material_manager;

protected:
	virtual	CEntityConditionSimple* create_entity_condition(CEntityConditionSimple* ec);

public:
	IC		CEntityCondition& conditions( ) const;
	IC		CMaterialManager& material( ) const
	{
		VERIFY(m_material_manager); return(*m_material_manager);
	}


protected:
	u32							m_ef_creature_type;
	u32							m_ef_weapon_type;
	u32							m_ef_detector_type;

public:
	virtual u32					ef_creature_type( ) const;
	virtual u32					ef_weapon_type( ) const;
	virtual u32					ef_detector_type( ) const;

public:

	virtual	CVisualMemoryManager* visual_memory( ) const
	{
		return(0);
	}
	virtual	void				net_Relcase(CObject* O);
};

#include "entity_alive_inline.h"
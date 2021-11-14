// Explosive.h: интерфейс для взврывающихся объектов

#pragma once

#include "..\ENGINE\Render.h"
#include "..\ENGINE\feel_touch.h"
#include "InventoryItem.h"
#include "ai_sounds.h"
#include "script_export_space.h"
#include "IDamageSource.h"
#include "WalmarkManager.h"
#include "ParticlesObject.h"

class IRender_Light;

//DEFINE_VECTOR(CPHShellHolder*, BLASTED_OBJECTS_V, BLASTED_OBJECTS_I);
using BLASTED_OBJECTS_V										= xr_vector<CPHShellHolder*>;
using BLASTED_OBJECTS_I										= BLASTED_OBJECTS_V::iterator;

class CExplosive : public I_DamageSource
{
private:
	collide::rq_results										rq_storage;

public:
								CExplosive					( );
	virtual						~CExplosive					( );

	virtual void				Load						(const char* section);
	virtual void				Load						(CConfigurationFile* ini, const char* section);

	virtual void				net_Destroy					( );
	virtual void				net_Relcase					(CObject* O);
	virtual void				UpdateCL					( );

private:
	virtual void				Explode						( );

public:
	virtual void				ExplodeParams				(const Fvector3& pos, const Fvector3& dir);

	static float				ExplosionEffect				(collide::rq_results& storage, CExplosive* exp_obj, CPHShellHolder* blasted_obj, const Fvector3& expl_centre, const float expl_radius);

	virtual void				OnEvent						(CNetPacket& P, unsigned short type);
	virtual void				OnAfterExplosion			( );
	virtual void				OnBeforeExplosion			( );
	virtual void				SetCurrentParentID			(unsigned short parent_id)
	{
		m_iCurrentParentID = parent_id;
	}
	inline unsigned short					CurrentParentID				( ) const
	{
		return m_iCurrentParentID;
	}

	virtual void				SetInitiator				(unsigned short id)
	{
		SetCurrentParentID(id);
	}
	virtual unsigned short					Initiator					( );

	virtual void				UpdateExplosionPos			( )
	{ }
	virtual void				GetExplVelocity				(Fvector3& v);
	virtual void				GetExplPosition				(Fvector3& p);
	virtual void				GetExplDirection			(Fvector3& d);
	virtual void				GenExplodeEvent				(const Fvector3& pos, const Fvector3& normal);
	virtual void				FindNormal					(Fvector3& normal);
	virtual CGameObject*		cast_game_object			( ) = 0;
	virtual CExplosive*			cast_explosive				( )
	{
		return this;
	}
	virtual I_DamageSource*		cast_IDamageSource			( )
	{
		return this;
	}
	virtual void				GetRayExplosionSourcePos	(Fvector3& pos);
	virtual void				GetExplosionBox				(Fvector3& size);
	virtual void				ActivateExplosionBox		(const Fvector3& size, Fvector3& in_out_pos);
	void						SetExplosionSize			(const Fvector3& new_size);
	virtual bool				Useful						( ) const;

protected:
	bool						IsSoundPlaying				( )
	{
		return !!sndExplode._feedback( );
	}
	bool						IsExploded					( )
	{
		return !!m_explosion_flags.test(flExploded);
	}

private:
	void						PositionUpdate				( );
	static void					GetRaySourcePos				(CExplosive* exp_obj, const Fvector3& expl_centre, Fvector3& p);

	void						ExplodeWaveProcessObject	(collide::rq_results& storage, CPHShellHolder* sh);
	void						ExplodeWaveProcess			( );
	static float				TestPassEffect				(const Fvector3& source_p, const Fvector3& dir, float range, float ef_radius, collide::rq_results& storage, CObject* blasted_obj);
	void						LightCreate					( );
	void						LightDestroy				( );

protected:
	CWalmarkManager											m_wallmark_manager;
	//ID персонажа который иницировал действие
	unsigned short														m_iCurrentParentID;

	Fvector3												m_vExplodePos;
	Fvector3												m_vExplodeSize;
	Fvector3												m_vExplodeDir;

	//параметры взрыва
	float													m_fBlastHit;
	float													m_fBlastHitImpulse;
	float													m_fBlastRadius;

	//параметры и количество осколков
	float													m_fFragsRadius;
	float													m_fFragHit;
	float													m_fFragHitImpulse;
	int														m_iFragsNum;

	//типы наносимых хитов
	ALife::EHitType											m_eHitTypeBlast;
	ALife::EHitType											m_eHitTypeFrag;

	//фактор подпроса предмета вверх взрывной волной
	float													m_fUpThrowFactor;

	//список пораженных объектов
	BLASTED_OBJECTS_V										m_blasted_objects;

	//текущая продолжительность взрыва
	float													m_fExplodeDuration;
	//общее время взрыва
	float													m_fExplodeDurationMax;
	//Время, через которое надо сделать взрывчатку невиимой, если она не становится невидимой во время взрыва
	float													m_fExplodeHideDurationMax;
	//флаг состояния взрыва
	enum
	{
		flExploding											= 1 << 0,
		flExplodEventSent									= 1 << 1,
		flReadyToExplode									= 1 << 2,
		flExploded											= 1 << 3
	};
	Flags8													m_explosion_flags;

	//Должен ли объект быть скрыт после взрыва: true - для всех кроме дымовой гранаты
	BOOL													m_bHideInExplosion;
	bool													m_bAlreadyHidden;
	virtual void				HideExplosive				( );

	//для разлета осколков
	float													m_fFragmentSpeed;

	//звуки
	ref_sound												sndExplode;
	ESoundTypes												m_eSoundExplode;

	//размер отметки на стенах
	float													fWallmarkSize;

	//эффекты и подсветка
	CSharedString												m_sExplodeParticles;

	//подсветка взрыва
	ref_light												m_pLight;
	Fcolor													m_LightColor;
	float													m_fLightRange;
	float													m_fLightTime;

	virtual void				StartLight					( );
	virtual void				StopLight					( );

	BOOL													m_bDynamicParticles;
	CParticlesObject*										m_pExpParticle;
	virtual void				UpdateExplosionParticles	( );

	//эффектор
	struct
	{
		CSharedString											effect_sect_name;
	} effector;

public:
	static void					script_register				(lua_State*);
};

add_to_type_list(CExplosive)
#undef script_type_list
#define script_type_list save_type_list(CExplosive)

inline void random_point_in_object_box(Fvector3& out_pos, CObject* obj)
{
	const Fbox3& l_b1 = obj->BoundingBox( );
	Fvector3 l_c;
	Fvector3 l_d;
	l_b1.get_CD(l_c, l_d);
	out_pos.random_point(l_d);
	obj->XFORM( ).transform_tiny(out_pos);
	out_pos.add(l_c);
}

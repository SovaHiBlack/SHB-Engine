// Explosive.h: ��������� ��� ������������� ��������
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\XR_3DA\Render.h"
#include "..\XR_3DA\feel_touch.h"
#include "inventory_item.h"
#include "ai_sounds.h"
#include "script_export_space.h"
#include "DamageSource.h"
#include "wallmark_manager.h"
#include "ParticlesObject.h"

class IRender_Light;
DEFINE_VECTOR(CPhysicsShellHolder*, BLASTED_OBJECTS_V, BLASTED_OBJECTS_I);

class CExplosive :
	public IDamageSource
{
private:
	collide::rq_results			rq_storage;

public:
	CExplosive(void);
	virtual						~CExplosive(void);

	virtual void 				Load(pcstr section);
	virtual void				Load(CInifile* ini, pcstr section);

	virtual void 				net_Destroy( );
	virtual void				net_Relcase(CObject* O);
	virtual void 				UpdateCL( );

private:
	virtual void 				Explode( );
public:
	virtual void 				ExplodeParams(const fVector3& pos, const fVector3& dir);

	static f32 				ExplosionEffect(collide::rq_results& storage, CExplosive* exp_obj, CPhysicsShellHolder* blasted_obj, const fVector3& expl_centre, const f32 expl_radius);


	virtual void 				OnEvent(NET_Packet& P, u16 type);//{inherited::OnEvent( P, type);}
	virtual void				OnAfterExplosion( );
	virtual void				OnBeforeExplosion( );
	virtual void 				SetCurrentParentID(u16 parent_id)
	{
		m_iCurrentParentID = parent_id;
	}
	IC		u16 				CurrentParentID( ) const
	{
		return m_iCurrentParentID;
	}

	virtual	void				SetInitiator(u16 id)
	{
		SetCurrentParentID(id);
	}
	virtual	u16					Initiator( );

	virtual void				UpdateExplosionPos( )
	{ }
	virtual void				GetExplVelocity(fVector3& v);
	virtual void				GetExplPosition(fVector3& p);
	virtual void				GetExplDirection(fVector3& d);
	virtual void 				GenExplodeEvent(const fVector3& pos, const fVector3& normal);
	virtual void 				FindNormal(fVector3& normal);
	virtual CGameObject* cast_game_object( ) = 0;
	virtual CExplosive* cast_explosive( )
	{
		return this;
	}
	virtual IDamageSource* cast_IDamageSource( )
	{
		return this;
	}
	virtual void				GetRayExplosionSourcePos(fVector3& pos);
	virtual	void				GetExplosionBox(fVector3& size);
	virtual void				ActivateExplosionBox(const fVector3& size, fVector3& in_out_pos);
	void				SetExplosionSize(const fVector3& new_size);
	virtual bool				Useful( ) const;

protected:
	bool				IsSoundPlaying( )
	{
		return !!sndExplode._feedback( );
	}
	bool				IsExploded( )
	{
		return !!m_explosion_flags.test(flExploded);
	}

private:
	void				PositionUpdate( );
	static		void				GetRaySourcePos(CExplosive* exp_obj, const fVector3& expl_centre, fVector3& p);

	void				ExplodeWaveProcessObject(collide::rq_results& storage, CPhysicsShellHolder* sh);
	void				ExplodeWaveProcess( );
	static		f32				TestPassEffect(const	fVector3& source_p, const	fVector3& dir, f32 range, f32 ef_radius, collide::rq_results& storage, CObject* blasted_obj);
	void				LightCreate( );
	void				LightDestroy( );
protected:

	CWalmarkManager				m_wallmark_manager;
	//ID ��������� ������� ���������� ��������
	u16							m_iCurrentParentID;

	//bool						m_bReadyToExplode;
	fVector3						m_vExplodePos;
	fVector3 					m_vExplodeSize;
	fVector3 					m_vExplodeDir;

	//��������� ������
	f32 						m_fBlastHit;
	f32 						m_fBlastHitImpulse;
	f32 						m_fBlastRadius;

	//��������� � ���������� ��������
	f32 						m_fFragsRadius;
	f32 						m_fFragHit;
	f32 						m_fFragHitImpulse;
	int	  						m_iFragsNum;

	//���� ��������� �����
	ALife::EHitType 			m_eHitTypeBlast;
	ALife::EHitType 			m_eHitTypeFrag;

	//������ �������� �������� ����� �������� ������ 
	f32						m_fUpThrowFactor;

	//������ ���������� ��������
	BLASTED_OBJECTS_V			m_blasted_objects;

	//������� ����������������� ������
	f32						m_fExplodeDuration;
	//����� ����� ������
	f32						m_fExplodeDurationMax;
	//�����, ����� ������� ���� ������� ���������� ��������, ���� ��� �� ���������� ��������� �� ����� ������
	f32						m_fExplodeHideDurationMax;
	//���� ��������� ������
	enum
	{
		flExploding = 1 << 0,
		flExplodEventSent = 1 << 1,
		flReadyToExplode = 1 << 2,
		flExploded = 1 << 3
	};
	flags8						m_explosion_flags;
	///////////////////////////////////////////////
	//������ �� ������ ���� ����� ����� ������: true - ��� ���� ����� ������� �������
	BOOL						m_bHideInExplosion;
	bool						m_bAlreadyHidden;
	virtual void				HideExplosive( );
	//bool						m_bExploding;
	//bool						m_bExplodeEventSent;

	//////////////////////////////////////////////
	//��� ������� ��������
	f32						m_fFragmentSpeed;

	//�����
	ref_sound					sndExplode;
	ESoundTypes					m_eSoundExplode;

	//������ ������� �� ������
	f32						fWallmarkSize;

	//������� � ���������
	shared_str					m_sExplodeParticles;

	//��������� ������
	ref_light					m_pLight;
	fColor						m_LightColor;
	f32						m_fLightRange;
	f32						m_fLightTime;

	virtual	void				StartLight( );
	virtual	void				StopLight( );

	BOOL						m_bDynamicParticles;
	CParticlesObject* m_pExpParticle;
	virtual void				UpdateExplosionParticles( );

	// ��������
	struct
	{
		/*		f32 					time;
				f32 					amplitude;
				f32 					period_number;
				shared_str				file_name;*/
		shared_str				effect_sect_name;
	} effector;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CExplosive)
#undef script_type_list
#define script_type_list save_type_list(CExplosive)

IC void random_point_in_object_box(fVector3& out_pos, CObject* obj)
{
	const fBox3& l_b1 = obj->BoundingBox( );
	fVector3 l_c;
	fVector3 l_d;
	l_b1.get_CD(l_c, l_d);
	out_pos.random_point(l_d);
	obj->XFORM( ).transform_tiny(out_pos);
	out_pos.add(l_c);
}

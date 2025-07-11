#pragma once

#include "..\XR_3DA\SkeletonAnimated.h"

class CHudItem;

struct weapon_hud_value : public shared_value
{
	CKinematicsAnimated* m_animations;

public:
	s32					m_fire_bone;
	fVector3				m_fp_offset;
	fVector3				m_fp2_offset;
	fVector3				m_sp_offset;

	fMatrix4x4				m_offset;

public:
	virtual				~weapon_hud_value( );
	BOOL				load(const shared_str& section, CHudItem* owner);
};

typedef shared_container<weapon_hud_value>		weapon_hud_container;
extern weapon_hud_container* g_pWeaponHUDContainer;

class shared_weapon_hud : public shared_item<weapon_hud_value>
{
protected:
	struct	on_new_pred
	{
		CHudItem* owner;
		on_new_pred(CHudItem* _owner) : owner(_owner)
		{ }
		BOOL			operator()				(const shared_str& key, weapon_hud_value* val) const
		{
			return val->load(key, owner);
		}
	};

public:
	void				create(shared_str key, CHudItem* owner)
	{
		shared_item<weapon_hud_value>::create(key, g_pWeaponHUDContainer, on_new_pred(owner));
	}
	CKinematicsAnimated* animations( )
	{
		return p_->m_animations;
	}
	u32					motion_length(MotionID M);
	MotionID			motion_id(pcstr name);
};
//---------------------------------------------------------------------------

class CWeaponHUD
{
	//������������ ������ HUD
	CHudItem* m_pParentWeapon;
	//����, ���� hud ������� �� ������������
	bool				m_bHidden;
	bool				m_bVisible;

	fMatrix4x4				m_Transform;

	//shared HUD data
	shared_weapon_hud	m_shared_data;

	//������� ��� ������������ ��������
	u32					m_dwAnimTime;
	u32					m_dwAnimEndTime;
	bool				m_bStopAtEndAnimIsRunning;
	u32					m_startedAnimState;
//	CInventoryItem*		m_pCallbackItem;
	CHudItem* m_pCallbackItem;

	//������������ ������� ������� ��������, � ����� callback
	void				StopCurrentAnim( );

	//������� � �������� ��� ������ �����������
	f32				m_fZoomRotateX;
	f32				m_fZoomRotateY;
	fVector3				m_fZoomOffset;

public:
	CWeaponHUD(CHudItem* pHudItem);
	~CWeaponHUD( );

	// misc
	void				Load(pcstr section);
	void				net_DestroyHud( );
	void				Init( );

	IC IRenderVisual* Visual( )
	{
		return m_shared_data.animations( );
	}
	IC fMatrix4x4& Transform( )
	{
		return m_Transform;
	}

	s32					FireBone( )
	{
		return m_shared_data.get_value( )->m_fire_bone;
	}
	const fVector3& FirePoint( )
	{
		return m_shared_data.get_value( )->m_fp_offset;
	}
	const fVector3& FirePoint2( )
	{
		return m_shared_data.get_value( )->m_fp2_offset;
	}
	const fVector3& ShellPoint( )
	{
		return m_shared_data.get_value( )->m_sp_offset;
	}

	const fVector3& ZoomOffset( ) const
	{
		return m_fZoomOffset;
	}
	f32				ZoomRotateX( ) const
	{
		return m_fZoomRotateX;
	}
	f32				ZoomRotateY( ) const
	{
		return m_fZoomRotateY;
	}
	void				SetZoomOffset(const fVector3& zoom_offset)
	{
		m_fZoomOffset = zoom_offset;
	}
	void				SetZoomRotateX(f32 zoom_rotate_x)
	{
		m_fZoomRotateX = zoom_rotate_x;
	}
	void				SetZoomRotateY(f32 zoom_rotate_y)
	{
		m_fZoomRotateY = zoom_rotate_y;
	}

	// Animations
	void				animPlay(MotionID M, BOOL bMixIn/*=TRUE*/, CHudItem* W /*=0*/, u32 state);
	void				animDisplay(MotionID M, BOOL bMixIn);
	MotionID			animGet(pcstr name);

	void				UpdatePosition(const fMatrix4x4& transform);

	bool				IsHidden( )
	{
		return m_bHidden;
	}
	void				Hide( )
	{
		m_bHidden = true;
	}
	void				Show( )
	{
		m_bHidden = false;
	}

	void				Visible(bool val)
	{
		m_bVisible = val;
	}

	//���������� HUD ������ ���������� �� ������ �����
	void				Update( );

	void				StopCurrentAnimWithoutCallback( );

public:
	static void			CreateSharedContainer( );
	static void			DestroySharedContainer( );
	static void			CleanSharedContainer( );

#ifdef DEBUG
public:
	void				dbg_SetFirePoint(const fVector3& fp)
	{
		((weapon_hud_value*)m_shared_data.get_value( ))->m_fp_offset.set(fp);
	}
	void				dbg_SetFirePoint2(const fVector3& fp)
	{
		((weapon_hud_value*)m_shared_data.get_value( ))->m_fp2_offset.set(fp);
	}
	void				dbg_SetShellPoint(const fVector3& sp)
	{
		((weapon_hud_value*)m_shared_data.get_value( ))->m_sp_offset.set(sp);
	}
#endif

};

#define		MAX_ANIM_COUNT							8
typedef		svector<MotionID, MAX_ANIM_COUNT>		MotionSVec;
MotionID	random_anim(MotionSVec& v);

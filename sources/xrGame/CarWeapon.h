#pragma once
#include "ShootingObject.h"
#include "HudSound.h"

class CPhysicsShellHolder;

class CCarWeapon :public CShootingObject
{
protected:
	typedef CShootingObject		inheritedShooting;

	void					SetBoneCallbacks( );
	void					ResetBoneCallbacks( );
	virtual	void			FireStart( );
	virtual	void			FireEnd( );
	virtual	void			UpdateFire( );
	virtual	void			OnShot( );
	void					UpdateBarrelDir( );
	virtual const fVector3& get_CurrentFirePoint( );
	virtual const fMatrix4x4& get_ParticlesXFORM( );

	CPhysicsShellHolder* m_object;
	bool					m_bActive;
	bool					m_bAutoFire;
	f32					m_weapon_h;

public:
	enum
	{
		eWpnDesiredDir = 1,
		eWpnDesiredPos,
		eWpnActivate,
		eWpnFire,
		eWpnAutoFire,
		eWpnToDefaultDir
	};
	CCarWeapon(CPhysicsShellHolder* obj);
	virtual		~CCarWeapon( );
	static void 			BoneCallbackX(CBoneInstance* B);
	static void				BoneCallbackY(CBoneInstance* B);
	void		Load(pcstr section);
	void		UpdateCL( );
	void			Action(s32 id, u32 flags);
	void			SetParam(s32 id, fVector2 val);
	void			SetParam(s32 id, fVector3 val);
	bool			AllowFire( );
	f32			FireDirDiff( );
	IC bool			IsActive( )
	{
		return m_bActive;
	}
	f32			_height( ) const
	{
		return m_weapon_h;
	};
	const fVector3& ViewCameraPos( );
	const fVector3& ViewCameraDir( );
	const fVector3& ViewCameraNorm( );

	void			Render_internal( );

private:
	u16						m_rotate_x_bone, m_rotate_y_bone, m_fire_bone, m_camera_bone;
	f32					m_tgt_x_rot;
	f32					m_tgt_y_rot;
	f32					m_cur_x_rot;
	f32					m_cur_y_rot;
	f32					m_bind_x_rot;
	f32					m_bind_y_rot;
	fVector3			m_bind_x;
	fVector3			m_bind_y;
	fVector3					m_fire_dir;
	fVector3			m_fire_pos;
	fVector3			m_fire_norm;

	fMatrix4x4					m_i_bind_x_xform, m_i_bind_y_xform, m_fire_bone_xform;
	fVector2				m_lim_x_rot, m_lim_y_rot; //in bone space
	f32						m_min_gun_speed;
	f32						m_max_gun_speed;
	CCartridge* m_Ammo;
	f32						m_barrel_speed;
	fVector3					m_destEnemyDir;
	bool					m_allow_fire;
	SHudSound				m_sndShot;
};

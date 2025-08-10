#pragma once

#include "HolderCustom.h"
#include "ShootingObject.h"
#include "physicsshellholder.h"
#include "HudSound.h"

class CCartridge;
class CCameraBase;

#define DESIRED_DIR 1

class CWeaponStatMgun : public CPhysicsShellHolder, public CHolderCustom, public CShootingObject
{
private:
	typedef CPhysicsShellHolder inheritedPH;
	typedef CHolderCustom		inheritedHolder;
	typedef CShootingObject		inheritedShooting;

private:
	CCameraBase* camera;
	// 
	static void				BoneCallbackX(CBoneInstance* B);
	static void				BoneCallbackY(CBoneInstance* B);
	void					SetBoneCallbacks( );
	void					ResetBoneCallbacks( );
//casts
public:
	virtual CHolderCustom* cast_holder_custom( )
	{
		return this;
	}

//general
public:
	CWeaponStatMgun( );
	virtual					~CWeaponStatMgun( );

	virtual void			Load(pcstr section);

	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );
	virtual void			net_Export(CNetPacket& P);	// export to server
	virtual void			net_Import(CNetPacket& P);	// import from server

	virtual void			UpdateCL( );

	virtual void			Hit(SHit* pHDS);

//shooting
private:
	u16						m_rotate_x_bone;
	u16						m_rotate_y_bone;
	u16						m_fire_bone;
	u16						m_camera_bone;
	f32						m_tgt_x_rot;
	f32						m_tgt_y_rot;
	f32						m_cur_x_rot;
	f32						m_cur_y_rot;
	f32						m_bind_x_rot;
	f32						m_bind_y_rot;
	fVector3				m_bind_x;
	fVector3				m_bind_y;
	fVector3				m_fire_dir;
	fVector3				m_fire_pos;

	fMatrix4x4				m_i_bind_x_xform;
	fMatrix4x4				m_i_bind_y_xform;
	fMatrix4x4				m_fire_bone_xform;
	fVector2				m_lim_x_rot; //in bone space
	fVector2				m_lim_y_rot; //in bone space
	CCartridge*				m_Ammo;
	f32						m_barrel_speed;
	fVector2				m_dAngle;
	fVector3				m_destEnemyDir;
	bool					m_allow_fire;
	SHudSound				sndShot;
	f32						camRelaxSpeed;
	f32						camMaxAngle;

protected:
	void					UpdateBarrelDir( );
	virtual const fVector3& get_CurrentFirePoint( );
	virtual const fMatrix4x4& get_ParticlesXFORM( );

	virtual void			FireStart( );
	virtual void			FireEnd( );
	virtual void			UpdateFire( );
	virtual void			OnShot( );
	void			AddShotEffector( );
	void			RemoveShotEffector( );
	void			SetDesiredDir(f32 h, f32 p);

	//HolderCustom
public:
	virtual bool			Use(const fVector3& pos, const fVector3& dir, const fVector3& foot_pos)
	{
		return !Owner( );
	}
	virtual void			OnMouseMove(s32 x, s32 y);
	virtual void			OnKeyboardPress(s32 dik);
	virtual void			OnKeyboardRelease(s32 dik);
	virtual void			OnKeyboardHold(s32 dik);
	virtual CInventory* GetInventory( )
	{
		return NULL;
	}
	virtual void			cam_Update(f32 dt, f32 fov = 90.0f);

	virtual void			renderable_Render( );

	virtual bool			attach_Actor(CGameObject* actor);
	virtual void			detach_Actor( );
	virtual bool			allowWeapon( ) const
	{
		return false;
	}
	virtual bool			HUDView( ) const
	{
		return true;
	}
	virtual fVector3			ExitPosition( )
	{
		return fVector3( ).set(0.0f, 0.0f, 0.0f);
	}

	virtual CCameraBase* Camera( )
	{
		return camera;
	}

	virtual void			Action(s32 id, u32 flags);
	virtual void			SetParam(s32 id, fVector2 val);
};

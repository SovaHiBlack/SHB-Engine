#pragma once

#include "HolderCustom.h"
#include "ShootingObject.h"
#include "PHShellHolder.h"
#include "HudSound.h"

class CCartridge;
class CCameraBase;

#define DESIRED_DIR 1

class CWeaponStatMgun : public CPHShellHolder, public CHolderCustom, public CShootingObject
{
private:
	using inheritedPH = CPHShellHolder;
	using inheritedHolder = CHolderCustom;
	using inheritedShooting = CShootingObject;

	CCameraBase* camera;

	static void				BoneCallbackX(CBoneInstance* B);
	static void				BoneCallbackY(CBoneInstance* B);
	void					SetBoneCallbacks( );
	void					ResetBoneCallbacks( );

public:
	// casts
	virtual CHolderCustom* cast_holder_custom( )
	{
		return this;
	}

	// general
	CWeaponStatMgun( );
	virtual					~CWeaponStatMgun( );

	virtual void			Load(const char* section);

	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );
	virtual void			net_Export(CNetPacket& P);	// export to server
	virtual void			net_Import(CNetPacket& P);	// import from server

	virtual void			UpdateCL( );

	virtual void			Hit(SHit* pHDS);

private:
	// shooting
	unsigned short			m_rotate_x_bone;
	unsigned short			m_rotate_y_bone;
	unsigned short			m_fire_bone;
	unsigned short			m_camera_bone;
	float					m_tgt_x_rot;
	float					m_tgt_y_rot;
	float					m_cur_x_rot;
	float					m_cur_y_rot;
	float					m_bind_x_rot;
	float					m_bind_y_rot;
	Fvector3				m_bind_x;
	Fvector3				m_bind_y;
	Fvector3				m_fire_dir;
	Fvector3				m_fire_pos;

	Fmatrix					m_i_bind_x_xform;
	Fmatrix					m_i_bind_y_xform;
	Fmatrix					m_fire_bone_xform;
	Fvector2				m_lim_x_rot;	// in bone space
	Fvector2				m_lim_y_rot;	// in bone space
	CCartridge*				m_Ammo;
	float					m_barrel_speed;
	Fvector2				m_dAngle;
	Fvector3				m_destEnemyDir;
	bool					m_allow_fire;
	HUD_SOUND				sndShot;
	float					camRelaxSpeed;	// скорость возврата в исходное положение
	float					camMaxAngle;	// максимальный угол отдачи

protected:
	void					UpdateBarrelDir( );
	virtual const Fvector3& get_CurrentFirePoint( );
	virtual const Fmatrix& get_ParticlesXFORM( );

	virtual void			FireStart( );
	virtual void			FireEnd( );
	virtual void			UpdateFire( );
	virtual void			OnShot( );
	void			AddShotEffector( );
	void			RemoveShotEffector( );
	void			SetDesiredDir(float h, float p);

public:
	// HolderCustom
	virtual bool			Use(const Fvector3& pos, const Fvector3& dir, const Fvector3& foot_pos)
	{
		return !Owner( );
	}
	virtual void			OnMouseMove(int x, int y);
	virtual void			OnKeyboardPress(int dik);
	virtual void			OnKeyboardRelease(int dik);
	virtual void			OnKeyboardHold(int dik);
	virtual CInventory* GetInventory( )
	{
		return nullptr;
	}
	virtual void			cam_Update(float dt, float fov = 90.0f);

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
	virtual Fvector3		ExitPosition( )
	{
		return Fvector3( ).set(0.0f, 0.0f, 0.0f);
	}

	virtual CCameraBase* Camera( )
	{
		return camera;
	}

	virtual void			Action(int id, u32 flags);
	virtual void			SetParam(int id, Fvector2 val);
};

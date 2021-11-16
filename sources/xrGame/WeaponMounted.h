#pragma once

#include "HolderCustom.h"
#include "ShootingObject.h"

#include "HudSound.h"
#include "WeaponAmmo.h"
#include "PHShellHolder.h"

class CWeaponMounted : public CPHShellHolder, public CHolderCustom, public CShootingObject
{
private:
	//////////////////////////////////////////////////////////////////////////
	//  General
	//////////////////////////////////////////////////////////////////////////
	using inherited = CPHShellHolder;

	CCameraBase* camera;
	unsigned short						fire_bone;
	unsigned short						actor_bone;
	unsigned short						rotate_x_bone;
	unsigned short						rotate_y_bone;
	unsigned short						camera_bone;

	Fvector3				fire_pos;
	Fvector3				fire_dir;
	Fmatrix					fire_bone_xform;
	Fvector2				m_dAngle;
	static void 			BoneCallbackX(CBoneInstance* B);
	static void				BoneCallbackY(CBoneInstance* B);

public:
	CWeaponMounted( );
	virtual					~CWeaponMounted( );

	// for shooting object
	virtual const Fvector3& get_CurrentFirePoint( )
	{
		return fire_pos;
	}
	virtual const Fmatrix& get_ParticlesXFORM( );

	//////////////////////////////////////////////////
	// непосредственно обработка стрельбы
	//////////////////////////////////////////////////
protected:
	virtual void			FireStart( );
	virtual void			FireEnd( );
	virtual void			UpdateFire( );
	virtual void			OnShot( );
	void			AddShotEffector( );
	void			RemoveShotEffector( );

	CSharedString					m_sAmmoType;
	CCartridge				m_CurrentAmmo;

	//звук стрельбы
	HUD_SOUND				sndShot;

	//для отдачи
	float					camRelaxSpeed;	//скорость возврата в исходное положение
	float					camMaxAngle;	//максимальный угол отдачи

	/////////////////////////////////////////////////
	// Generic
	/////////////////////////////////////////////////
public:
	virtual CHolderCustom* cast_holder_custom( )
	{
		return this;
	}
	virtual void			Load(const char* section);

	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );
	virtual void			net_Export(CNetPacket& P);	// export to server
	virtual void			net_Import(CNetPacket& P);	// import from server

	virtual void			UpdateCL( );
	virtual void			shedule_Update(u32 dt);

	virtual void			renderable_Render( );

	virtual BOOL			UsedAI_Locations( )
	{
		return FALSE;
	}

	// control functions
	virtual void			OnMouseMove(int x, int y);
	virtual void			OnKeyboardPress(int dik);
	virtual void			OnKeyboardRelease(int dik);
	virtual void			OnKeyboardHold(int dik);

	virtual CInventory* GetInventory( )
	{
		return nullptr;
	}

	virtual void			cam_Update(float dt, float fov = 90.0f);

	virtual bool			Use(const Fvector3& pos, const Fvector3& dir, const Fvector3& foot_pos);
	virtual bool			attach_Actor(CGameObject* actor);
	virtual void			detach_Actor( );
	virtual Fvector3			ExitPosition( );
	virtual bool			allowWeapon( ) const
	{
		return false;
	}
	virtual bool			HUDView( ) const
	{
		return true;
	}

	virtual CCameraBase* Camera( );
};

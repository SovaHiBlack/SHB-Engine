#ifndef WeaponMountedH
#define WeaponMountedH
#pragma once

#include "HolderCustom.h"
#include "shootingobject.h"

#include "hudsound.h"
#include "weaponammo.h"
#include "physicsshellholder.h"

class CWeaponMounted :	public CPhysicsShellHolder, 
						public CHolderCustom,
						public CShootingObject
{
private:
	//////////////////////////////////////////////////////////////////////////
	//  General
	//////////////////////////////////////////////////////////////////////////
	typedef CPhysicsShellHolder inherited;
	CCameraBase*			camera;
	u16						fire_bone;
	u16						actor_bone;
	u16						rotate_x_bone;
	u16						rotate_y_bone;
	u16						camera_bone;

	fVector3				fire_pos;
	fVector3				fire_dir;
	fMatrix4x4					fire_bone_xform;
	fVector2				m_dAngle;
	static void 			BoneCallbackX		(CBoneInstance *B);
	static void				BoneCallbackY		(CBoneInstance *B);

public:
							CWeaponMounted		();
	virtual					~CWeaponMounted		();

	// for shooting object
	virtual const fVector3&	get_CurrentFirePoint()	{return fire_pos;}
	virtual const fMatrix4x4&	get_ParticlesXFORM()	;

	//////////////////////////////////////////////////
	// непосредственно обработка стрельбы
	//////////////////////////////////////////////////
protected:
	virtual	void			FireStart	();
	virtual	void			FireEnd		();
	virtual	void			UpdateFire	();
	virtual	void			OnShot		();
			void			AddShotEffector		();
			void			RemoveShotEffector	();
protected:
	shared_str					m_sAmmoType;
	CCartridge				m_CurrentAmmo;

	//звук стрельбы
	HUD_SOUND				sndShot;

	//для отдачи
	f32					camRelaxSpeed;
	f32					camMaxAngle;

	/////////////////////////////////////////////////
	// Generic
	/////////////////////////////////////////////////
public:
	virtual CHolderCustom	*cast_holder_custom	()				{return this;}
	virtual void			Load				(pcstr section);

	virtual BOOL			net_Spawn			(CSE_Abstract* DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(CNetPacket& P);	// export to server
	virtual void			net_Import			(CNetPacket& P);	// import from server

	virtual void			UpdateCL			();
	virtual void			shedule_Update		(u32 dt);

	virtual void			renderable_Render	();

	virtual	BOOL			UsedAI_Locations	(){return FALSE;}

	// control functions
	virtual void			OnMouseMove			(s32 x, s32 y);
	virtual void			OnKeyboardPress		(s32 dik);
	virtual void			OnKeyboardRelease	(s32 dik);
	virtual void			OnKeyboardHold		(s32 dik);

	virtual CInventory*		GetInventory		(){return 0;}

	virtual void			cam_Update			(f32 dt, f32 fov=90.0f);

	virtual bool			Use					(const fVector3& pos,const fVector3& dir,const fVector3& foot_pos);
	virtual bool			attach_Actor		(CGameObject* actor);
	virtual void			detach_Actor		();
	virtual fVector3			ExitPosition		();
	virtual bool			allowWeapon			()	const		{return false;};
	virtual bool			HUDView				()  const		{return true;};

	virtual CCameraBase*	Camera				();
};
#endif // WeaponMountedH

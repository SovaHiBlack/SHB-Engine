#pragma once

#include "..\ENGINE\feel_touch.h"
#include "..\ENGINE\IInputReceiver.h"
#include "entity.h"
#include "actor_flags.h"

class CActor;

class CSpectator : public CGameObject, public IInputReceiver
{
private:
	using inherited									= CGameObject;

public:
	enum EActorCameras
	{
		eacFreeFly									= 0,
		eacFirstEye,
		eacLookAt,
		eacFreeLook,
		eacMaxCam
	};

private:
	// Cameras
	CCameraBase*									cameras[eacMaxCam];
	EActorCameras									cam_active;

	int												look_idx;

	void					cam_Set					(EActorCameras style);
	void					cam_Update				(CActor* A = 0);

	CActor*											m_pActorToLookAt;

	void					FirstEye_ToPlayer		(CObject* pObject);

public:
							CSpectator				( );
	virtual					~CSpectator				( );

	virtual void			IR_OnMouseMove			(int dx, int dy);
	virtual void			IR_OnKeyboardPress		(int dik);
	virtual void			IR_OnKeyboardRelease	(int dik);
	virtual void			IR_OnKeyboardHold		(int dik);
	virtual void			shedule_Update			(u32 T);
	virtual void			UpdateCL				( );
	virtual BOOL			net_Spawn				(CSE_Abstract* DC);
	virtual void			net_Destroy				( );

	virtual void			Center					(Fvector& C) const
	{
		C.set(Position( ));
	}
	virtual float			Radius					( ) const
	{
		return EPS;
	}

	virtual CGameObject*	cast_game_object		( )
	{
		return this;
	}
	virtual IInputReceiver*	cast_input_receiver		( )
	{
		return this;
	}

	virtual void			net_Relcase				(CObject* O);
};

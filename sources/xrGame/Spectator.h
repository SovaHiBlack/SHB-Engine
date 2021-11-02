//	Module 		: Spectator.h
//	Description : interface for the CSpectator class.

#pragma once

#include "..\ENGINE\feel_touch.h"
#include "..\ENGINE\IInputReceiver.h"
#include "Entity.h"
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
	void					cam_Update				(CActor* A = nullptr);

	CActor*											m_pActorToLookAt;

	void					FirstEye_ToPlayer		(CObject* pObject);

public:
							CSpectator				( );
	virtual					~CSpectator				( );

	virtual void			IR_OnMouseMove			(int dx, int dy);
	virtual void			IR_OnKeyboardPress		(int cmd);
	virtual void			IR_OnKeyboardRelease	(int cmd);
	virtual void			IR_OnKeyboardHold		(int cmd);
	virtual void			shedule_Update			(unsigned int T);
	virtual void			UpdateCL				( );
	virtual BOOL			net_Spawn				(CSE_Abstract* DC);
	virtual void			net_Destroy				( );

	virtual void			Center					(Fvector3& C) const
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

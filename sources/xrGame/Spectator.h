#pragma once

#include "..\XR_3DA\feel_touch.h"
#include "..\XR_3DA\IInputReceiver.h"

#include "entity.h"
#include "Actor_flags.h"

// refs
class CActor;

class CSpectator : public CGameObject, public IInputReceiver
{
private:
	typedef CGameObject		inherited;

public:
	enum EActorCameras
	{
		eacFreeFly = 0,
		eacFirstEye,
		eacLookAt,
		eacFreeLook,
		eacMaxCam
	};

private:
	// Cameras
	CCameraBase* cameras[eacMaxCam];
	EActorCameras			cam_active;

	s32						look_idx;

	//------------------------------
	void					cam_Set(EActorCameras style);
	void					cam_Update(CActor* A = 0);

	CActor* m_pActorToLookAt;

	void					FirstEye_ToPlayer(CObject* pObject);

public:
	CSpectator( );
	virtual					~CSpectator( );

	virtual void			IR_OnMouseMove(s32 x, s32 y);
	virtual void			IR_OnKeyboardPress(s32 dik);
	virtual void			IR_OnKeyboardRelease(s32 dik);
	virtual void			IR_OnKeyboardHold(s32 dik);
	virtual void			shedule_Update(u32 T);
	virtual void			UpdateCL( );
	virtual BOOL			net_Spawn(CSE_Abstract* DC);
	virtual void			net_Destroy( );

	virtual void			Center(fVector3& C)	const
	{
		C.set(Position( ));
	}
	virtual f32				Radius( ) const
	{
		return EPSILON_5;
	}
	virtual CGameObject* cast_game_object( )
	{
		return this;
	}
	virtual IInputReceiver* cast_input_receiver( )
	{
		return this;
	}

	virtual void			net_Relcase(CObject* O);
};

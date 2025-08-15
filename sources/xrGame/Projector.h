#pragma once

#include "script_object.h"

class CLightAnimItem;

class CProjector : public CScriptObject
{
	typedef	CScriptObject		inherited;

	friend void		BoneCallbackX(CBoneInstance* B);
	friend void		BoneCallbackY(CBoneInstance* B);

	f32			fBrightness;
	CLightAnimItem* lanim;
	fVector3			m_pos;
	ref_light		light_render;
	ref_glow		glow_render;

	u16				guid_bone;

	struct SBoneRot
	{
		f32	velocity;
		u16		id;
	} bone_x, bone_y;

	struct
	{
		f32	yaw;
		f32	pitch;
	} _start, _current, _target;

public:
	CProjector( );
	virtual			~CProjector( );

	virtual void	Load(pcstr section);
	virtual BOOL	net_Spawn(CSE_Abstract* DC);
	virtual void	shedule_Update(u32 dt);							// Called by sheduler
	virtual void	UpdateCL( );								// Called each frame, so no need for dt
	virtual void	renderable_Render( );

	virtual BOOL	UsedAI_Locations( );

	virtual bool	bfAssignWatch(CScriptEntityAction* tpEntityAction);
	virtual bool	bfAssignObject(CScriptEntityAction* tpEntityAction);

	fVector3 GetCurrentDirection( );

private:
	void	TurnOn( );
	void	TurnOff( );

	// Rotation routines
	static void		BoneCallbackX(CBoneInstance* B);
	static void		BoneCallbackY(CBoneInstance* B);

	void			SetTarget(const fVector3& target_pos);
};

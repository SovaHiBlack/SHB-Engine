#pragma once
#include "..\XR_3DA\SkeletonAnimated.h"

class CPhysicsShell;

class CInteractiveMotion
{
	MotionID motion;

protected:
	flags8	 flags;
	enum Flag
	{
		fl_use_death_motion = 1 << 4,
		fl_switch_dm_toragdoll = 1 << 5
	};

public:
	CInteractiveMotion( );
	void	init( );
	void	setup(pcstr m, CPhysicsShell* s);
	void	update(CPhysicsShell* s);
	IC bool	is_enabled( )
	{
		return !!flags.test(fl_use_death_motion);
	}

	void	play(CPhysicsShell* s);

private:
	virtual void	move_update(CPhysicsShell* s) = 0;
	virtual void	collide(CPhysicsShell* s) = 0;

protected:
	virtual void	state_end(CPhysicsShell* s);
	virtual void	state_start(CPhysicsShell* s);

private:
	void	switch_to_free(CPhysicsShell* s);
	static void	anim_callback(CBlend* B);
};

class CInteractiveMotionVelocity : public CInteractiveMotion
{
	typedef			CInteractiveMotion inherited;
	virtual void	move_update(CPhysicsShell* s);
	virtual void	collide(CPhysicsShell* s);
	virtual void	state_end(CPhysicsShell* s);
	virtual void	state_start(CPhysicsShell* s);
};

class CInteractiveMotionPosition : public CInteractiveMotion
{
	typedef			CInteractiveMotion inherited;
	virtual void	move_update(CPhysicsShell* s);
	virtual void	collide(CPhysicsShell* s);
	virtual void	state_end(CPhysicsShell* s);
	virtual void	state_start(CPhysicsShell* s);
};

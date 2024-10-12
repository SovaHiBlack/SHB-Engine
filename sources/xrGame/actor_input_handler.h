#pragma once

class CActor;

class CActorInputHandler
{
public:
	virtual void	reinit				();
	
	virtual void	install				();
	virtual void	install				(CActor *);
	virtual void	release				();

	virtual bool	authorized			(int cmd){return true;}
	virtual	f32		mouse_scale_factor	(){return 1.0f;}

protected:
	CActor			*m_actor;
};

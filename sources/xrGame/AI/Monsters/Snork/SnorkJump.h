#pragma once

//class CJumpingAbility;
class CSnork;

class CSnorkJump
{
	CSnork* m_object;
	//CJumpingAbility		*m_jumper;

	f32	m_cur_dist;
	bool	m_specific_jump;

	CObject* m_target_object;
	u32		m_velocity_mask;

public:
	//			CSnorkJump			(CSnork *monster);
	//			~CSnorkJump			();
	//	void	load				(pcstr section);
	//	void	update_frame		();
	//	void	try_to_jump			(u32 velocity_mask);
	//
	//private:	
	//	void	init_jump_normal	();
	//	void	init_jump_specific	();
	//	void	try_jump_normal		();
	//	void	try_jump_specific	();
	//
	//
	//	f32	trace_current		(f32 dist);
};

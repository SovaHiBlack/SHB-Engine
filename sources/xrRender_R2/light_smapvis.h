#pragma	once

class	smapvis		: public	R_feedback
{
public:
	enum			{
		state_counting	= 0,
		state_working	= 1,
		state_usingTC	= 3,
	}							state;
	xr_vector<IRenderVisual*>	invisible;

	u32							frame_sleep;
	u32							test_count;
	u32							test_current;
	IRenderVisual*				testQ_V;
	u32							testQ_id;
	u32							testQ_frame;
public:
	smapvis			();
	~smapvis		();

	void			invalidate	();
	void			begin		();			// should be called before 'marker++' and before graph-build
	void			end			();
	void			mark		();
	void			flushoccq	();			// should be called when no rendering of light is supposed

	void			resetoccq	();

	IC	bool		sleep		()			{ return Device.dwFrame > frame_sleep; }

	virtual		void	rfeedback_static	(IRenderVisual*	V);
};

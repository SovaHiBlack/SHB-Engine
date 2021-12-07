#pragma once

class ENGINE_API ISheduled
{
public:
	struct
	{
		unsigned int									t_min : 14;		// minimal bound of update time (sample: 20ms)
		unsigned int									t_max : 14;		// maximal bound of update time (sample: 200ms)
		unsigned int									b_RT : 1;
		unsigned int									b_locked : 1;
	}	shedule;

#ifdef DEBUG
	unsigned int										dbg_startframe;
	unsigned int										dbg_update_shedule;
#endif // def DEBUG

						ISheduled			( );
	virtual				~ISheduled			( );

	void				shedule_register	( );
	void				shedule_unregister	( );

	virtual float		shedule_Scale		( ) = 0;
	virtual void		shedule_Update		(unsigned int dt);
	virtual CSharedString	shedule_Name		( ) const
	{
		return CSharedString("unknown");
	}
	virtual bool		shedule_Needed		( ) = 0;
};

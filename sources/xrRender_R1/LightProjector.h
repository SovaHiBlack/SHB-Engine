// LightProjector.h: interface for the CLightProjector class.

#pragma once

#include "..\xrRender\r__dsgraph_types.h"

class CLightProjector : public pureAppActivate
{
private:
	static const int P_rt_size				= 512;
	static const int P_o_size				= 51;
	static const int P_o_line				= P_rt_size / P_o_size;
	static const int P_o_count				= P_o_line * P_o_line;

	//
	using NODE								= R_dsgraph::_MatrixItem;
	struct recv
	{
		IRenderable*						O;
		Fvector3							C;
		Fmatrix								UVgen;
		Fvector3							UVclamp_min;
		Fvector3							UVclamp_max;
		Fbox3								BB;
		unsigned long						dwFrame;
		unsigned long						dwTimeValid;
	};

	IRenderable*							current;
	xr_vector<recv>							cache;			// same as number of slots
	xr_vector<IRenderable*>					receivers;
	xr_vector<int>							taskid;

	ref_rt									RT;
	CSharedString							c_xform;
	CSharedString							c_clamp;
	CSharedString							c_factor;

public:
	void				set_object			(IRenderable* O);
	BOOL				shadowing			( )
	{
		return current != 0;
	}
	void				calculate			( );
	void				setup				(int slot);
	void				finalize			( )
	{
		receivers.clear( );
		taskid.clear( );
	}
	void				invalidate			( );

	virtual void		OnAppActivate		( );

#ifdef DEBUG
	void				render				( );
#endif // def DEBUG

						CLightProjector		( );
						~CLightProjector	( );
};

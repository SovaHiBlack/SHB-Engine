// LightTrack.h: interface for the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

const	f32				lt_inc			= 4.0f	;
const	f32				lt_dec			= 2.0f	;
const	int					lt_hemisamples	= 26	;

class	CROS_impl			: public IRender_ObjectSpecific
{
public:
	struct	Item			{
		u32					frame_touched		;	// to track creation & removal
		light*				source				;	// 
		collide::ray_cache	cache				;	//
		f32				test				;	// note range: (-1[no]..1[yes])
		f32				energy				;	//
	};
	struct	Light			{
		light*				source				;
		f32				energy				;
		fColor				color				;
	};
public:
	// general
	u32						MODE				;
	u32						dwFrame				;
	u32						dwFrameSmooth		;

	// 
	xr_vector<Item>			track				;	// everything what touches
	xr_vector<Light>		lights				;	// 

	bool					result				[lt_hemisamples];
	collide::ray_cache		cache				[lt_hemisamples];
	collide::ray_cache		cache_sun			;
	s32						result_count		;
	u32						result_iterator		;
	u32						result_frame		;
	s32						result_sun			;
public:
	u32						shadow_gen_frame	;
	u32						shadow_recv_frame	;
	int						shadow_recv_slot	;
private:
	f32					hemi_value			;
	f32					hemi_smooth			;
	f32					sun_value			;
	f32					sun_smooth			;

	Fvector					approximate			;
public:
	virtual	void			force_mode			(u32 mode)		{ MODE = mode;															};
	virtual f32			get_luminocity		()				{ Fvector one; return one.set(1,1,1).div(3.f).dotproduct(approximate);	};
	virtual f32			get_luminocity_hemi	()				{ return get_hemi();}

	void					add					(light*			L);
	void					update				(IRenderable*	O);
	void					update_smooth		(IRenderable*	O=0);
	
	ICF	f32				get_hemi			()	{
		if (dwFrameSmooth!=Device.dwFrame)		update_smooth();
		return									hemi_smooth;
	}
	ICF	f32				get_sun				()	{
		if (dwFrameSmooth!=Device.dwFrame)		update_smooth();
		return									sun_smooth;
	}
	ICF fVector3&			get_approximate		()	{
		if (dwFrameSmooth!=Device.dwFrame)		update_smooth();
		return									approximate;
	}

	CROS_impl				();
	virtual ~CROS_impl	()	{};
};

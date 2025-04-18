#pragma once

#include "..\XR_3DA\ispatial.h"
#if RENDER==R_R2
	#include "light_package.h"
	#include "light_smapvis.h"
	#include "light_GI.h"
#endif

class	light		:	public IRender_Light, public ISpatial
{
public:
	struct {
		u32			type	:	4;
		u32			bStatic	:	1;
		u32			bActive	:	1;
		u32			bShadow	:	1;
	}				flags;
	fVector3			position	;
	fVector3			direction	;
	fVector3			right		;
	f32			range		;
	f32			cone		;
	fColor			color		;

	vis_data		hom			;
	u32				frame_render;

#if RENDER==R_R2
	light*						omnipart	[6]	;
	xr_vector<light_indirect>	indirect		;
	u32							indirect_photons;

	smapvis			svis;		// used for 6-cubemap faces

	ref_shader		s_spot;
	ref_shader		s_point;

	u32				m_xform_frame;
	fMatrix4x4			m_xform;

	struct _vis		{
		u32			frame2test;		// frame the test is sheduled to
		u32			query_id;		// ID of occlusion query
		u32			query_order;	// order of occlusion query
		bool		visible;		// visible/invisible
		bool		pending;		// test is still pending
		u16			smap_ID;
	}				vis;

	union			_xform	{
		struct		_D		{
			fMatrix4x4						combine	;
			s32							minX,maxX	;
			s32							minY,maxY	;
			BOOL						transluent	;
		}	D;
		struct		_P		{
			fMatrix4x4						world		;
			fMatrix4x4						view		;
			fMatrix4x4						project		;
			fMatrix4x4						combine		;
		}	P;
		struct		_S		{
			fMatrix4x4						view		;
			fMatrix4x4						project		;
			fMatrix4x4						combine		;
			u32							size		;
			u32							posX		;
			u32							posY		;
			BOOL						transluent	;
		}	S;
	}	X;
#endif

public:
	virtual void	set_type				(LT type)						{ flags.type = type;		}
	virtual void	set_active				(bool b);
	virtual bool	get_active				()								{ return flags.bActive;		}
	virtual void	set_shadow				(bool b)						
	{ 
		flags.bShadow=b;			
	}
	virtual void	set_position			(const fVector3& P);
	virtual void	set_rotation			(const fVector3& D, const fVector3& R);
	virtual void	set_cone				(f32 angle);
	virtual void	set_range				(f32 R);
	virtual void	set_virtual_size		(f32 R)						{};
	virtual void	set_color				(const fColor& C)				{ color.set(C);				}
	virtual void	set_color				(f32 r, f32 g, f32 b)		{ color.set(r,g,b,1);		}
	virtual void	set_texture				(pcstr name);

	virtual	void	spatial_move			();
	virtual	fVector3	spatial_sector_point	();

	virtual IRender_Light*	dcast_Light		()	{ return this; }

	vis_data&		get_homdata				();

#if RENDER==R_R2
	void			gi_generate				();
	void			xform_calc				();
	void			vis_prepare				();
	void			vis_update				();
	void			export 					(light_Package& dest);
#endif

	f32			get_LOD					();

	light();
	virtual ~light();
};

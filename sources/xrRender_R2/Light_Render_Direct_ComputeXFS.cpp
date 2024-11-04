#include "stdafx.h"
#include "light_render_direct.h"

void CLight_Compute_XFORM_and_VIS::compute_xf_spot(light* L)
{
	// Build EYE-space xform
	fVector3						L_dir;
	fVector3 L_up;
	fVector3 L_right;
	fVector3 L_pos;
	L_dir.set(L->direction);
	L_dir.normalize( );

	if (L->right.square_magnitude( ) > EPSILON_5)
	{
		// use specified 'up' and 'right', just enshure ortho-normalization
		L_right.set(L->right);
		L_right.normalize( );
		L_up.crossproduct(L_dir, L_right);
		L_up.normalize( );
		L_right.crossproduct(L_up, L_dir);
		L_right.normalize( );
	}
	else
	{
		// auto find 'up' and 'right' vectors
		L_up.set(0.0f, 1.0f, 0.0f);
		if (_abs(L_up.dotproduct(L_dir)) > 0.99f)
		{
			L_up.set(0.0f, 0.0f, 1.0f);
		}

		L_right.crossproduct(L_up, L_dir);
		L_right.normalize( );
		L_up.crossproduct(L_dir, L_right);
		L_up.normalize( );
	}

	L_pos.set(L->position);

	// 
	int _cached_size = L->X.S.size;
	L->X.S.posX = L->X.S.posY = 0;
	L->X.S.size = SMAP_adapt_max;
	L->X.S.transluent = FALSE;

	// Compute approximate screen area (treating it as an point light) - R*R/dist_sq
	// Note: we clamp screen space area to ONE, although it is not correct at all
	f32	dist = Device.vCameraPosition.distance_to(L->spatial.sphere.P) - L->spatial.sphere.R;
	if (dist < 0)	dist = 0;
	f32	ssa = clampr(L->range * L->range / (1.f + dist * dist), 0.f, 1.f);

	// compute intensity
	f32	intensity0 = (L->color.r + L->color.g + L->color.b) / 3.f;
	f32	intensity1 = (L->color.r * 0.2125f + L->color.g * 0.7154f + L->color.b * 0.0721f);
	f32	intensity = (intensity0 + intensity1) / 2.f;		// intensity1 tends to underestimate...

	// compute how much duelling frusta occurs	[-1..1]-> 1 + [-0.5 .. +0.5]
	f32	duel_dot = 1.f - 0.5f * Device.vCameraDirection.dotproduct(L_dir);

	// compute how large the light is - give more texels to larger lights, assume 8m as being optimal radius
	f32	sizefactor = L->range / 8.f;				// 4m = .5, 8m=1.f, 16m=2.f, 32m=4.f

	// compute how wide the light frustum is - assume 90deg as being optimal
	f32	widefactor = L->cone / deg2rad(90.f);	// 

	// factors
	f32	factor0 = powf(ssa, 1.f / 2.f);		// ssa is quadratic
	f32	factor1 = powf(intensity, 1.f / 16.f);		// less perceptually important?
	f32	factor2 = powf(duel_dot, 1.f / 4.f);		// difficult to fast-change this -> visible
	f32	factor3 = powf(sizefactor, 1.f / 4.f);		// this shouldn't make much difference
	f32	factor4 = powf(widefactor, 1.f / 2.f);		// make it linear ???
	f32	factor = ps_r2_ls_squality * factor0 * factor1 * factor2 * factor3 * factor4;

	// final size calc
	u32 _size = iFloor(factor * SMAP_adapt_optimal);
	if (_size < SMAP_adapt_min)	_size = SMAP_adapt_min;
	if (_size > SMAP_adapt_max)	_size = SMAP_adapt_max;
	int _epsilon = iCeil(f32(_size) * 0.01f);
	int _diff = _abs(int(_size) - int(_cached_size));
	L->X.S.size = (_diff >= _epsilon) ? _size : _cached_size;

	// make N pixel border
	L->X.S.view.build_camera_dir(L_pos, L_dir, L_up);
	//f32	n			= 2.f						;
	//f32	x			= f32(L->X.S.size)		;
	//f32	alpha		= L->cone/2					;
	//f32	tan_beta	= (x+2*n)*tanf(alpha) / x	;
	//f32	g_alpha		= 2*rad2deg		(alpha);
	//f32	g_beta		= 2*rad2deg		(atanf(tan_beta));
	//Msg				("x(%f) : a(%f), b(%f)",x,g_alpha,g_beta);
	L->X.S.project.build_projection(L->cone + deg2rad(3.5f), 1.f, SMAP_near_plane, L->range + EPSILON_7);
	L->X.S.combine.mul(L->X.S.project, L->X.S.view);
}

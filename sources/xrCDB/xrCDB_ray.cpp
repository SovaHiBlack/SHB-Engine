#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <xmmintrin.h>
#pragma warning(pop)

#include "xrCDB.h"

using namespace		CDB;
using namespace		Opcode;

// can you say "barebone"?
#ifndef _MM_ALIGN16
#	define _MM_ALIGN16	__declspec(align(16))
#endif // _MM_ALIGN16

struct	_MM_ALIGN16		vec_t	: public fVector3
{
	f32		pad;
};
vec_t	vec_c	(f32 _x, f32 _y, f32 _z)	{ vec_t v; v.x=_x;v.y=_y;v.z=_z;v.pad=0; return v; }
struct _MM_ALIGN16		aabb_t	{ 
	vec_t		min;
	vec_t		max;
};
struct _MM_ALIGN16		ray_t	{
	vec_t		pos;
	vec_t		inv_dir;
	vec_t		fwd_dir;
};
struct ray_segment_t {
	f32		t_near,t_far;
};

ICF u32&	uf			(f32& x)	{ return (u32&)x; }
ICF BOOL	isect_fpu	(const fVector3& min, const fVector3& max, const ray_t &ray, fVector3& coord)
{
	fVector3				MaxT;
	MaxT.x=MaxT.y=MaxT.z=-1.0f;
	BOOL Inside			= TRUE;

	// Find candidate planes.
	if(ray.pos[0] < min[0]) {
		coord[0]	= min[0];
		Inside		= FALSE;
		if(uf(ray.inv_dir[0]))	MaxT[0] = (min[0] - ray.pos[0]) * ray.inv_dir[0]; // Calculate T distances to candidate planes
	} else if(ray.pos[0] > max[0]) {
		coord[0]	= max[0];
		Inside		= FALSE;
		if(uf(ray.inv_dir[0]))	MaxT[0] = (max[0] - ray.pos[0]) * ray.inv_dir[0]; // Calculate T distances to candidate planes
	}
	if(ray.pos[1] < min[1]) {
		coord[1]	= min[1];
		Inside		= FALSE;
		if(uf(ray.inv_dir[1]))	MaxT[1] = (min[1] - ray.pos[1]) * ray.inv_dir[1]; // Calculate T distances to candidate planes
	} else if(ray.pos[1] > max[1]) {
		coord[1]	= max[1];
		Inside		= FALSE;
		if(uf(ray.inv_dir[1]))	MaxT[1] = (max[1] - ray.pos[1]) * ray.inv_dir[1]; // Calculate T distances to candidate planes
	}
	if(ray.pos[2] < min[2]) {
		coord[2]	= min[2];
		Inside		= FALSE;
		if(uf(ray.inv_dir[2]))	MaxT[2] = (min[2] - ray.pos[2]) * ray.inv_dir[2]; // Calculate T distances to candidate planes
	} else if(ray.pos[2] > max[2]) {
		coord[2]	= max[2];
		Inside		= FALSE;
		if(uf(ray.inv_dir[2]))	MaxT[2] = (max[2] - ray.pos[2]) * ray.inv_dir[2]; // Calculate T distances to candidate planes
	}

	// Ray ray.pos inside bounding box
	if(Inside)		{
		coord		= ray.pos;
		return		true;
	}

	// Get largest of the maxT's for final choice of intersection
	u32 WhichPlane = 0;
	if	(MaxT[1] > MaxT[0])				WhichPlane = 1;
	if	(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;

	// Check final candidate actually inside box (if max < 0)
	if(uf(MaxT[WhichPlane])&0x80000000) return false;

	if  (0==WhichPlane)	{	// 1 & 2
		coord[1] = ray.pos[1] + MaxT[0] * ray.fwd_dir[1];
		if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
		coord[2] = ray.pos[2] + MaxT[0] * ray.fwd_dir[2];
		if((coord[2] < min[2]) || (coord[2] > max[2]))	return false;
		return true;
	}
	if (1==WhichPlane)	{	// 0 & 2
		coord[0] = ray.pos[0] + MaxT[1] * ray.fwd_dir[0];
		if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
		coord[2] = ray.pos[2] + MaxT[1] * ray.fwd_dir[2];
		if((coord[2] < min[2]) || (coord[2] > max[2]))	return false;
		return true;
	}
	if (2==WhichPlane)	{	// 0 & 1
		coord[0] = ray.pos[0] + MaxT[2] * ray.fwd_dir[0];
		if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
		coord[1] = ray.pos[1] + MaxT[2] * ray.fwd_dir[1];
		if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
		return true;
	}
	return false;
}

// turn those verbose intrinsics into something readable.
#define loadps(mem)			_mm_load_ps((const f32 * const)(mem))
#define storess(ss,mem)		_mm_store_ss((f32 * const)(mem),(ss))
#define minss				_mm_min_ss
#define maxss				_mm_max_ss
#define minps				_mm_min_ps
#define maxps				_mm_max_ps
#define mulps				_mm_mul_ps
#define subps				_mm_sub_ps
#define rotatelps(ps)		_mm_shuffle_ps((ps),(ps), 0x39)	// a,b,c,d -> b,c,d,a
#define muxhps(low,high)	_mm_movehl_ps((low),(high))		// low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}


static const f32 flt_plus_inf = -logf(0);	// let's keep C and C++ compilers happy.
static const f32 _MM_ALIGN16
	ps_cst_plus_inf	[4]	=	{  flt_plus_inf,  flt_plus_inf,  flt_plus_inf,  flt_plus_inf },
	ps_cst_minus_inf[4]	=	{ -flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf };

ICF BOOL isect_sse			(const aabb_t &box, const ray_t &ray, f32& dist)	{
	// you may already have those values hanging around somewhere
	const __m128
		plus_inf	= loadps(ps_cst_plus_inf),
		minus_inf	= loadps(ps_cst_minus_inf);

	// use whatever's apropriate to load.
	const __m128
		box_min		= loadps(&box.min),
		box_max		= loadps(&box.max),
		pos			= loadps(&ray.pos),
		inv_dir		= loadps(&ray.inv_dir);

	// use a div if inverted directions aren't available
	const __m128 l1 = mulps(subps(box_min, pos), inv_dir);
	const __m128 l2 = mulps(subps(box_max, pos), inv_dir);

	// the order we use for those min/max is vital to filter out
	// NaNs that happens when an inv_dir is +/- inf and
	// (box_min - pos) is 0. inf * 0 = NaN
	const __m128 filtered_l1a = minps(l1, plus_inf);
	const __m128 filtered_l2a = minps(l2, plus_inf);

	const __m128 filtered_l1b = maxps(l1, minus_inf);
	const __m128 filtered_l2b = maxps(l2, minus_inf);

	// now that we're back on our feet, test those slabs.
	__m128 lmax = maxps(filtered_l1a, filtered_l2a);
	__m128 lmin = minps(filtered_l1b, filtered_l2b);

	// unfold back. try to hide the latency of the shufps & co.
	const __m128 lmax0 = rotatelps(lmax);
	const __m128 lmin0 = rotatelps(lmin);
	lmax = minss(lmax, lmax0);
	lmin = maxss(lmin, lmin0);

	const __m128 lmax1 = muxhps(lmax,lmax);
	const __m128 lmin1 = muxhps(lmin,lmin);
	lmax = minss(lmax, lmax1);
	lmin = maxss(lmin, lmin1);

	const BOOL ret = _mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax,lmin);

	storess		(lmin, &dist);
	//storess	(lmax, &rs.t_far);

	return  ret;
}

template <bool bUseSSE, bool bCull, bool bFirst, bool bNearest>
class _MM_ALIGN16	ray_collider
{
public:
	COLLIDER*		dest;
	TRI*			tris;
	fVector3*		verts;
	
	ray_t			ray;
	f32				rRange;
	f32				rRange2;

	IC void			_init		(COLLIDER* CL, fVector3* V, TRI* T, const fVector3& C, const fVector3& D, f32 R)
	{
		dest			= CL;
		tris			= T;
		verts			= V;
		ray.pos.set		(C);
		ray.inv_dir.set	(1.f,1.f,1.f).div(D);
		ray.fwd_dir.set	(D);
		rRange			= R;
		rRange2			= R*R;
		if (!bUseSSE)	{
			// for FPU - zero out inf
			if (_abs(D.x)>flt_eps){}	else ray.inv_dir.x=0;
			if (_abs(D.y)>flt_eps){}	else ray.inv_dir.y=0;
			if (_abs(D.z)>flt_eps){}	else ray.inv_dir.z=0;
		}
	}

	// fpu
	ICF BOOL		_box_fpu	(const fVector3& bCenter, const fVector3& bExtents, fVector3& coord)
	{
		fBox3		BB;
		BB.min.sub	(bCenter,bExtents);
		BB.max.add	(bCenter,bExtents);
        return 		isect_fpu	(BB.min,BB.max,ray,coord);
	}
	// sse
	ICF BOOL		_box_sse	(const fVector3& bCenter, const fVector3& bExtents, f32&  dist )
	{
		aabb_t		box;
		box.min.sub (bCenter,bExtents);	box.min.pad = 0;
		box.max.add	(bCenter,bExtents); box.max.pad = 0;
        return 		isect_sse	(box,ray,dist);
	}
	
	IC bool			_tri		(u32* p, f32& u, f32& v, f32& range)
	{
		fVector3 edge1;
		fVector3 edge2;
		fVector3 tvec;
		fVector3 pvec;
		fVector3 qvec;
		f32	det;
		f32 inv_det;
		
		// find vectors for two edges sharing vert0
		fVector3&			p0	= verts[ p[0] ];
		fVector3&			p1	= verts[ p[1] ];
		fVector3&			p2	= verts[ p[2] ];
		edge1.sub			(p1, p0);
		edge2.sub			(p2, p0);
		// begin calculating determinant - also used to calculate U parameter
		// if determinant is near zero, ray lies in plane of triangle
		pvec.crossproduct	(ray.fwd_dir, edge2);
		det = edge1.dotproduct(pvec);
		if (bCull)
		{						
			if (det < EPSILON_5)  return false;
			tvec.sub(ray.pos, p0);						// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);					// calculate U parameter and test bounds
			if (u < 0.f || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = ray.fwd_dir.dotproduct(qvec);			// calculate V parameter and test bounds
			if (v < 0.f || u + v > det) return false;
			range = edge2.dotproduct(qvec);				// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range	*= inv_det;
			u		*= inv_det;
			v		*= inv_det;
		}
		else
		{			
			if (det > -EPSILON_5 && det < EPSILON_5) return false;
			inv_det = 1.0f / det;
			tvec.sub(ray.pos, p0);						// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;			// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = ray.fwd_dir.dotproduct(qvec)*inv_det;	// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;		// calculate t, ray intersects triangle
		}
		return true;
	}
	
	void			_prim		(DWORD prim)
	{
		f32 u;
		f32 v;
		f32 r;
		if (!_tri(tris[prim].verts, u, v, r))	return;
		if (r<=0 || r>rRange)					return;
		
		if (bNearest)	
		{
			if (dest->r_count())	
			{
				RESULT& R = *dest->r_begin();
				if (r<R.range)	{
					R.id		= prim;
					R.range		= r;
					R.u			= u;
					R.v			= v;
					R.verts	[0]	= verts[tris[prim].verts[0]];
					R.verts	[1]	= verts[tris[prim].verts[1]];
					R.verts	[2]	= verts[tris[prim].verts[2]];
					R.dummy		= tris[prim].dummy;
					rRange		= r;
					rRange2		= r*r;
				}
			} else {
				RESULT& R	= dest->r_add();
				R.id		= prim;
				R.range		= r;
				R.u			= u;
				R.v			= v;
				R.verts	[0]	= verts[tris[prim].verts[0]];
				R.verts	[1]	= verts[tris[prim].verts[1]];
				R.verts	[2]	= verts[tris[prim].verts[2]];
				R.dummy		= tris[prim].dummy;
				rRange		= r;
				rRange2		= r*r;
			}
		} else {
			RESULT& R	= dest->r_add();
			R.id		= prim;
			R.range		= r;
			R.u			= u;
			R.v			= v;
			R.verts	[0]	= verts[tris[prim].verts[0]];
			R.verts	[1]	= verts[tris[prim].verts[1]];
			R.verts	[2]	= verts[tris[prim].verts[2]];
			R.dummy		= tris[prim].dummy;
		}
	}
	void			_stab		(const AABBNoLeafNode* node)
	{
		// Actual ray/aabb test
		if (bUseSSE)			{
			// use SSE
			f32		d;
			if (!_box_sse((fVector3&)node->mAABB.mCenter,(fVector3&)node->mAABB.mExtents,d))	return;
			if (d>rRange)																	return;
		} else {
			// use FPU
			fVector3		P;
			if (!_box_fpu((fVector3&)node->mAABB.mCenter,(fVector3&)node->mAABB.mExtents,P))	return;
			if (P.distance_to_sqr(ray.pos)>rRange2)											return;
		}
		
		// 1st chield
		if (node->HasLeaf())	_prim	(node->GetPrimitive());
		else					_stab	(node->GetPos());
		
		// Early exit for "only first"
		if (bFirst && dest->r_count())														return;
		
		// 2nd chield
		if (node->HasLeaf2())	_prim	(node->GetPrimitive2());
		else					_stab	(node->GetNeg());
	}
};

void	COLLIDER::ray_query	(const MODEL *m_def, const fVector3& r_start,  const fVector3& r_dir, f32 r_range)
{
	m_def->syncronize		();

	// Get nodes
	const AABBNoLeafTree* T = (const AABBNoLeafTree*)m_def->tree->GetTree();
	const AABBNoLeafNode* N = T->GetNodes();
	r_clear					();
	
	if (CPU::ID.feature&_CPU_FEATURE_SSE)	{
		// SSE
		// Binary dispatcher
		if (ray_mode&OPT_CULL)		{
			if (ray_mode&OPT_ONLYFIRST)		{
				if (ray_mode&OPT_ONLYNEAREST)		{
					ray_collider<true,true,true,true>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				} else {
					ray_collider<true,true,true,false>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				}
			} else {
				if (ray_mode&OPT_ONLYNEAREST)		{
					ray_collider<true,true,false,true>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				} else {
					ray_collider<true,true,false,false>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				}
			}
		} else {
			if (ray_mode&OPT_ONLYFIRST)		{
				if (ray_mode&OPT_ONLYNEAREST)		{
					ray_collider<true,false,true,true>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				} else {
					ray_collider<true,false,true,false>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				}
			} else {
				if (ray_mode&OPT_ONLYNEAREST)		{
					ray_collider<true,false,false,true>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				} else {
					ray_collider<true,false,false,false>	RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				}
			}
		}
	} else {
		// FPU
		// Binary dispatcher
		if (ray_mode&OPT_CULL)		{
			if (ray_mode&OPT_ONLYFIRST)		{
				if (ray_mode&OPT_ONLYNEAREST)		{
					ray_collider<false,true,true,true>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				} else {
					ray_collider<false,true,true,false>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				}
			} else {
				if (ray_mode&OPT_ONLYNEAREST)		{
					ray_collider<false,true,false,true>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				} else {
					ray_collider<false,true,false,false>	RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				}
			}
		} else {
			if (ray_mode&OPT_ONLYFIRST)		{
				if (ray_mode&OPT_ONLYNEAREST)		{
					ray_collider<false,false,true,true>		RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				} else {
					ray_collider<false,false,true,false>	RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				}
			} else {
				if (ray_mode&OPT_ONLYNEAREST)		{
					ray_collider<false,false,false,true>	RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				} else {
					ray_collider<false,false,false,false>	RC;
					RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
					RC._stab(N);
				}
			}
		}
	}
}


#include "stdafx.h"
#include "..\XR_3DA\igame_persistent.h"
#include "..\XR_3DA\irenderable.h"

const	f32	tweak_COP_initial_offs = 1200.f;
const	f32	tweak_ortho_xform_initial_offs = 1000.f;	//. ?
const	f32	tweak_guaranteed_range = 20.f;	//. ?

f32			OLES_SUN_LIMIT_27_01_07 = 180.f;

//////////////////////////////////////////////////////////////////////////
// tables to calculate view-frustum bounds in world space
// note: D3D uses [0..1] range for Z
static fVector3		corners[8] = {
	{ -1, -1,  0 },		{ -1, -1, +1},
	{ -1, +1, +1 },		{ -1, +1,  0},
	{ +1, +1, +1 },		{ +1, +1,  0},
	{ +1, -1, +1},		{ +1, -1,  0}
};
static int			facetable[6][4] = {
	{ 0, 3, 5, 7 },		{ 1, 2, 3, 0 },
	{ 6, 7, 5, 4 },		{ 4, 2, 1, 6 },
	{ 3, 2, 4, 5 },		{ 1, 0, 7, 6 },
};
//////////////////////////////////////////////////////////////////////////
#define DW_AS_FLT(DW) (*(FLOAT*)&(DW))
#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define FLT_SIGN(F) ((FLT_AS_DW(F) & 0x80000000L))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)
#define IS_SPECIAL(F) ((FLT_AS_DW(F) & 0x7f800000L)==0x7f800000L)

//////////////////////////////////////////////////////////////////////////
struct Frustum
{
	Frustum( );
	Frustum(const D3DXMATRIX* matrix);

	D3DXPLANE	camPlanes[6];
	int			nVertexLUT[6];
	D3DXVECTOR3 pntList[8];
};
struct BoundingBox
{
	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;

	BoundingBox( ) : minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{ }
	BoundingBox(const BoundingBox& other) : minPt(other.minPt), maxPt(other.maxPt)
	{ }

	explicit BoundingBox(const D3DXVECTOR3* points, UINT n) : minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{
		for (unsigned int i = 0; i < n; i++)
			Merge(&points[i]);
	}

	explicit BoundingBox(const std::vector<D3DXVECTOR3>* points) : minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{
		for (unsigned int i = 0; i < points->size( ); i++)
			Merge(&(*points)[i]);
	}
	explicit BoundingBox(const std::vector<BoundingBox>* boxes) : minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{
		for (unsigned int i = 0; i < boxes->size( ); i++)
		{
			Merge(&(*boxes)[i].maxPt);
			Merge(&(*boxes)[i].minPt);
		}
	}
	void Centroid(D3DXVECTOR3* vec) const
	{
		*vec = 0.5f * (minPt + maxPt);
	}
	void Merge(const D3DXVECTOR3* vec)
	{
		minPt.x = _min(minPt.x, vec->x);
		minPt.y = _min(minPt.y, vec->y);
		minPt.z = _min(minPt.z, vec->z);
		maxPt.x = _max(maxPt.x, vec->x);
		maxPt.y = _max(maxPt.y, vec->y);
		maxPt.z = _max(maxPt.z, vec->z);
	}
	D3DXVECTOR3 Point(int i) const
	{
		return D3DXVECTOR3((i & 1) ? minPt.x : maxPt.x, (i & 2) ? minPt.y : maxPt.y, (i & 4) ? minPt.z : maxPt.z);
	}
};

///////////////////////////////////////////////////////////////////////////
BOOL LineIntersection2D(D3DXVECTOR2* result, const D3DXVECTOR2* lineA, const D3DXVECTOR2* lineB)
{
	//  if the lines are parallel, the lines will not intersect in a point
	//  NOTE: assumes the rays are already normalized!!!!
	VERIFY(_abs(D3DXVec2Dot(&lineA[1], &lineB[1])) < 1.f);

	f32 x[2] = {lineA[0].x, lineB[0].x};
	f32 y[2] = {lineA[0].y, lineB[0].y};
	f32 dx[2] = {lineA[1].x, lineB[1].x};
	f32 dy[2] = {lineA[1].y, lineB[1].y};

	f32 x_diff = x[0] - x[1];
	f32 y_diff = y[0] - y[1];

	f32 s = (x_diff - (dx[1] / dy[1]) * y_diff) / ((dx[1] * dy[0] / dy[1]) - dx[0]);
	// f32 t	= (x_diff + s*dx[0]) / dx[1];

	*result = lineA[0] + s * lineA[1];
	return		TRUE;
}
///////////////////////////////////////////////////////////////////////////
//  PlaneIntersection
//    computes the point where three planes intersect
//    returns whether or not the point exists.
static inline BOOL PlaneIntersection(D3DXVECTOR3* intersectPt, const D3DXPLANE* p0, const D3DXPLANE* p1, const D3DXPLANE* p2)
{
	D3DXVECTOR3 n0(p0->a, p0->b, p0->c);
	D3DXVECTOR3 n1(p1->a, p1->b, p1->c);
	D3DXVECTOR3 n2(p2->a, p2->b, p2->c);

	D3DXVECTOR3 n1_n2, n2_n0, n0_n1;

	D3DXVec3Cross(&n1_n2, &n1, &n2);
	D3DXVec3Cross(&n2_n0, &n2, &n0);
	D3DXVec3Cross(&n0_n1, &n0, &n1);

	f32 cosTheta = D3DXVec3Dot(&n0, &n1_n2);

	if (ALMOST_ZERO(cosTheta) || IS_SPECIAL(cosTheta))
		return FALSE;

	f32 secTheta = 1.f / cosTheta;

	n1_n2 = n1_n2 * p0->d;
	n2_n0 = n2_n0 * p1->d;
	n0_n1 = n0_n1 * p2->d;

	*intersectPt = -(n1_n2 + n2_n0 + n0_n1) * secTheta;
	return TRUE;
}

Frustum::Frustum( )
{
	for (int i = 0; i < 6; i++)
		camPlanes[i] = D3DXPLANE(0.f, 0.f, 0.f, 0.f);
}

//  build a frustum from a camera (projection, or viewProjection) matrix
Frustum::Frustum(const D3DXMATRIX* matrix)
{
	//  build a view frustum based on the current view & projection matrices...
	D3DXVECTOR4 column4(matrix->_14, matrix->_24, matrix->_34, matrix->_44);
	D3DXVECTOR4 column1(matrix->_11, matrix->_21, matrix->_31, matrix->_41);
	D3DXVECTOR4 column2(matrix->_12, matrix->_22, matrix->_32, matrix->_42);
	D3DXVECTOR4 column3(matrix->_13, matrix->_23, matrix->_33, matrix->_43);

	D3DXVECTOR4 planes[6];
	planes[0] = column4 - column1;  // left
	planes[1] = column4 + column1;  // right
	planes[2] = column4 - column2;  // bottom
	planes[3] = column4 + column2;  // top
	planes[4] = column4 - column3;  // near
	planes[5] = column4 + column3;  // far
	// ignore near & far plane

	int p;

	for (p = 0; p < 6; p++)  // normalize the planes
	{
		f32 dot = planes[p].x * planes[p].x + planes[p].y * planes[p].y + planes[p].z * planes[p].z;
		dot = 1.f / _sqrt(dot);
		planes[p] = planes[p] * dot;
	}

	for (p = 0; p < 6; p++)
		camPlanes[p] = D3DXPLANE(planes[p].x, planes[p].y, planes[p].z, planes[p].w);

	//  build a bit-field that will tell us the indices for the nearest and farthest vertices from each plane...
	for (int i = 0; i < 6; i++)
		nVertexLUT[i] = ((planes[i].x < 0.f) ? 1 : 0) | ((planes[i].y < 0.f) ? 2 : 0) | ((planes[i].z < 0.f) ? 4 : 0);

	for (int i = 0; i < 8; i++)  // compute extrema
	{
		const D3DXPLANE& p0 = (i & 1) ? camPlanes[4] : camPlanes[5];
		const D3DXPLANE& p1 = (i & 2) ? camPlanes[3] : camPlanes[2];
		const D3DXPLANE& p2 = (i & 4) ? camPlanes[0] : camPlanes[1];
		PlaneIntersection(&pntList[i], &p0, &p1, &p2);
	}
}

//////////////////////////////////////////////////////////////////////////
// OLES: naive builder of infinite volume expanded from base frustum towards 
//		 light source. really slow, but it works for our simple usage :)
// note: normals points to 'outside'
//////////////////////////////////////////////////////////////////////////
template <bool _debug>
class	DumbConvexVolume
{
public:
	struct	_poly
	{
		xr_vector<int>	points;
		fVector3		planeN;
		f32			planeD;
		f32			classify(fVector3& p)
		{
			return planeN.dotproduct(p) + planeD;
		}
	};
	struct	_edge
	{
		int				p0, p1;
		int				counter;
		_edge(int _p0, int _p1, int m) : p0(_p0), p1(_p1), counter(m)
		{
			if (p0 > p1)	swap(p0, p1);
		}
		bool			equal(_edge& E)
		{
			return p0 == E.p0 && p1 == E.p1;
		}
	};
public:
	xr_vector<fVector3>		points;
	xr_vector<_poly>		polys;
	xr_vector<_edge>		edges;
public:
	void				compute_planes( )
	{
		for (int it = 0; it<int(polys.size( )); it++)
		{
			_poly& P = polys[it];
			fVector3		t1;
			fVector3		t2;
			t1.sub(points[P.points[0]], points[P.points[1]]);
			t2.sub(points[P.points[0]], points[P.points[2]]);
			P.planeN.crossproduct(t1, t2).normalize( );
			P.planeD = -P.planeN.dotproduct(points[P.points[0]]);

			// verify
			if (_debug)
			{
				fVector3& p0 = points[P.points[0]];
				fVector3& p1 = points[P.points[1]];
				fVector3& p2 = points[P.points[2]];
				fVector3& p3 = points[P.points[3]];
				fPlane3	p012;
				p012.build(p0, p1, p2);
				fPlane3	p123;
				p123.build(p1, p2, p3);
				fPlane3	p230;
				p230.build(p2, p3, p0);
				fPlane3	p301;
				p301.build(p3, p0, p1);
				VERIFY(p012.n.similar(p123.n) && p012.n.similar(p230.n) && p012.n.similar(p301.n));
			}
		}
	}
	void				compute_caster_model(xr_vector<fPlane3>& dest, fVector3 direction)
	{
		CRenderTarget& T = *RImplementation.Target;

		// COG
		fVector3	cog = {0.0f, 0.0f, 0.0f};
		for (int it = 0; it<int(points.size( )); it++)	cog.add(points[it]);
		cog.div(f32(points.size( )));

		// planes
		compute_planes( );
		for (int it = 0; it<int(polys.size( )); it++)
		{
			_poly& base = polys[it];
			if (base.classify(cog) > 0)	std::reverse(base.points.begin( ), base.points.end( ));
		}

		// remove faceforward polys, build list of edges -> find open ones
		compute_planes( );
		for (int it = 0; it<int(polys.size( )); it++)
		{
			_poly& base = polys[it];
			VERIFY(base.classify(cog) < 0);								// debug

			int		marker = (base.planeN.dotproduct(direction) <= 0) ? -1 : 1;

			// register edges
			xr_vector<int>& plist = polys[it].points;
			for (int p = 0; p<int(plist.size( )); p++)
			{
				_edge	E(plist[p], plist[(p + 1) % plist.size( )], marker);
				bool	found = false;
				for (int e = 0; e<int(edges.size( )); e++)
					if (edges[e].equal(E))
					{
						edges[e].counter += marker; found = true; break;
					}
				if (!found)
				{
					edges.push_back(E);
					if (_debug)	T.dbg_addline(points[E.p0], points[E.p1], color_rgba(255, 0, 0, 255));
				}
			}

			// remove if unused
			if (marker < 0)
			{
				polys.erase(polys.begin( ) + it);
				it--;
			}
		}

		// Extend model to infinity, the volume is not capped, so this is indeed up to infinity
		for (int e = 0; e<int(edges.size( )); e++)
		{
			if (edges[e].counter != 0)	continue;
			_edge& E = edges[e];
			if (_debug)	T.dbg_addline(points[E.p0], points[E.p1], color_rgba(255, 255, 255, 255));
			fVector3	point;
			points.push_back(point.sub(points[E.p0], direction));
			points.push_back(point.sub(points[E.p1], direction));
			polys.push_back(_poly( ));
			_poly& P = polys.back( );
			int			pend = int(points.size( ));
			P.points.push_back(E.p0);
			P.points.push_back(E.p1);
			P.points.push_back(pend - 1);	//p1 mod
			P.points.push_back(pend - 2);	//p0 mod
			if (_debug)	T.dbg_addline(points[E.p0], point.mad(points[E.p0], direction, -1000), color_rgba(0, 255, 0, 255));
			if (_debug)	T.dbg_addline(points[E.p1], point.mad(points[E.p1], direction, -1000), color_rgba(0, 255, 0, 255));
		}

		// Reorient planes (try to write more inefficient code :)
		compute_planes( );
		for (int it = 0; it<int(polys.size( )); it++)
		{
			_poly& base = polys[it];
			if (base.classify(cog) > 0)	std::reverse(base.points.begin( ), base.points.end( ));
		}

		// Export
		compute_planes( );
		for (int it = 0; it<int(polys.size( )); it++)
		{
			_poly& P = polys[it];
			fPlane3			pp = {P.planeN,P.planeD};
			dest.push_back(pp);
		}
	}
};

//////////////////////////////////////////////////////////////////////////
fVector3		wform(fMatrix4x4& m, fVector3& v)
{
	fVector4	r;
	r.x = v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41;
	r.y = v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42;
	r.z = v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43;
	r.w = v.x * m._14 + v.y * m._24 + v.z * m._34 + m._44;
	// VERIFY		(r.w>0.f);
	fVector3	r3 = {r.x / r.w, r.y / r.w, r.z / r.w};
	return		r3;
}

//////////////////////////////////////////////////////////////////////////
// OLES: naive 3D clipper - roubustness around 0, but works for this sample
// note: normals points to 'outside'
//////////////////////////////////////////////////////////////////////////
const	f32	_eps = 0.000001f;
struct	DumbClipper
{
	CFrustum				frustum;
	xr_vector<D3DXPLANE>	planes;
	BOOL					clip(D3DXVECTOR3& p0, D3DXVECTOR3& p1)		// returns TRUE if result meaningfull
	{
		f32		denum;
		D3DXVECTOR3	D;
		for (int it = 0; it<int(planes.size( )); it++)
		{
			D3DXPLANE& P = planes[it];
			f32		cls0 = D3DXPlaneDotCoord(&P, &p0);
			f32		cls1 = D3DXPlaneDotCoord(&P, &p1);
			if (cls0 > 0 && cls1 > 0)	return			false;	// fully outside

			if (cls0 > 0)
			{
// clip p0
				D = p1 - p0;
				denum = D3DXPlaneDotNormal(&P, &D);
				if (denum != 0) p0 += -D * cls0 / denum;
			}
			if (cls1 > 0)
			{
// clip p1
				D = p0 - p1;
				denum = D3DXPlaneDotNormal(&P, &D);
				if (denum != 0) p1 += -D * cls1 / denum;
			}
		}
		return	true;
	}
	D3DXVECTOR3			point(fBox3& bb, int i) const
	{
		return D3DXVECTOR3((i & 1) ? bb.min.x : bb.max.x, (i & 2) ? bb.min.y : bb.max.y, (i & 4) ? bb.min.z : bb.max.z);
	}
	fBox3				clipped_AABB(xr_vector<fBox3, render_alloc<fBox3> >& src, fMatrix4x4& xf)
	{
		fBox3		result;
		result.invalidate( );
		for (int it = 0; it<int(src.size( )); it++)
		{
			fBox3& bb = src[it];
			u32				mask = frustum.getMask( );
			EFC_Visible		res = frustum.testAABB(&bb.min.x, mask);
			switch (res)
			{
				case fcvNone:	continue;
				case fcvFully:
					for (int c = 0; c < 8; c++)
					{
						D3DXVECTOR3		p0 = point(bb, c);
						fVector3			x0 = wform(xf, *((fVector3*)(&p0)));
						result.modify(x0);
					}
					break;
				case fcvPartial:
					for (int c0 = 0; c0 < 8; c0++)
					{
						for (int c1 = 0; c1 < 8; c1++)
						{
							if (c0 == c1)			continue;
							D3DXVECTOR3		p0 = point(bb, c0);
							D3DXVECTOR3		p1 = point(bb, c1);
							if (!clip(p0, p1))	continue;
							fVector3			x0 = wform(xf, *((fVector3*)(&p0)));
							fVector3			x1 = wform(xf, *((fVector3*)(&p1)));
							result.modify(x0);
							result.modify(x1);
						}
					}
					break;
			};
		}
		return			result;
	}
};

template <class _Tp>
inline const _Tp& min(const _Tp& __a, const _Tp& __b)
{
	return __b < __a ? __b : __a;
}
template <class _Tp>
inline const _Tp& max(const _Tp& __a, const _Tp& __b)
{
	return  __a < __b ? __b : __a;
}

xr_vector<fBox3, render_alloc<fBox3> >	s_casters;

D3DXVECTOR2 BuildTSMProjectionMatrix_caster_depth_bounds(D3DXMATRIX& lightSpaceBasis)
{
	f32		min_z = 1e32f;
	f32		max_z = -1e32f;
	D3DXMATRIX	minmax_xf;
	D3DXMatrixMultiply(&minmax_xf, (D3DXMATRIX*)&Device.mView, &lightSpaceBasis);
	fMatrix4x4& minmax_xform = *((fMatrix4x4*)&minmax_xf);
	for (u32 c = 0; c < s_casters.size( ); c++)
	{
		fVector3	pt;
		for (int e = 0; e < 8; e++)
		{
			s_casters[c].getpoint(e, pt);
			pt = wform(minmax_xform, pt);
			min_z = _min(min_z, pt.z);
			max_z = _max(max_z, pt.z);
		}
	}
	return D3DXVECTOR2(min_z, max_z);
}

void CRender::render_sun( )
{
	light* fuckingsun = (light*)Lights.sun_adapted._get( );
	D3DXMATRIX		m_LightViewProj;

	// calculate view-frustum bounds in world space
	fMatrix4x4	ex_project;
	fMatrix4x4	ex_full;
	fMatrix4x4	ex_full_inverse;
	{
		f32 _far_ = min(OLES_SUN_LIMIT_27_01_07, g_pGamePersistent->Environment( ).CurrentEnv.far_plane);
		ex_project.build_projection(deg2rad(Device.fFOV/* *Device.fASPECT*/), Device.fASPECT, ps_r2_sun_near, _far_);
		ex_full.mul(ex_project, Device.mView);
		D3DXMatrixInverse((D3DXMATRIX*)&ex_full_inverse, 0, (D3DXMATRIX*)&ex_full);
	}

	// Compute volume(s) - something like a frustum for infinite directional light
	// Also compute virtual light position and sector it is inside
	CFrustum					cull_frustum;
	xr_vector<fPlane3>			cull_planes;
	fVector3					cull_COP;
	CSector* cull_sector;
	fMatrix4x4						cull_xform;
	{
		FPU::m64r( );
		// Lets begin from base frustum
		fMatrix4x4		fullxform_inv = ex_full_inverse;
		DumbConvexVolume<false>		hull;
		{
			hull.points.reserve(8);
			for (int p = 0; p < 8; p++)
			{
				fVector3				xf = wform(fullxform_inv, corners[p]);
				hull.points.push_back(xf);
			}
			for (int plane = 0; plane < 6; plane++)
			{
				hull.polys.push_back(DumbConvexVolume<false>::_poly( ));
				for (int pt = 0; pt < 4; pt++)
					hull.polys.back( ).points.push_back(facetable[plane][pt]);
			}
		}
		hull.compute_caster_model(cull_planes, fuckingsun->direction);

		// Search for default sector - assume "default" or "outdoor" sector is the largest one
		//. hack: need to know real outdoor sector
		CSector* largest_sector = 0;
		f32		largest_sector_vol = 0;
		for (u32 s = 0; s < Sectors.size( ); s++)
		{
			CSector* S = (CSector*)Sectors[s];
			IRenderVisual* V = S->root( );
			f32				vol = V->vis.box.getvolume( );
			if (vol > largest_sector_vol)
			{
				largest_sector_vol = vol;
				largest_sector = S;
			}
		}
		cull_sector = largest_sector;

		// COP - 100 km away
		cull_COP.mad(Device.vCameraPosition, fuckingsun->direction, -tweak_COP_initial_offs);

		// Create frustum for query
		cull_frustum._clear( );
		for (u32 p = 0; p < cull_planes.size( ); p++)
			cull_frustum._add(cull_planes[p]);

		// Create approximate ortho-xform
		// view: auto find 'up' and 'right' vectors
		fMatrix4x4					mdir_View;
		fMatrix4x4					mdir_Project;
		fVector3					L_dir;
		fVector3					L_up;
		fVector3					L_right;
		fVector3					L_pos;
		L_pos.set(fuckingsun->position);
		L_dir.set(fuckingsun->direction).normalize( );
		L_up.set(0, 1, 0);					if (_abs(L_up.dotproduct(L_dir)) > .99f)	L_up.set(0, 0, 1);
		L_right.crossproduct(L_up, L_dir).normalize( );
		L_up.crossproduct(L_dir, L_right).normalize( );
		mdir_View.build_camera_dir(L_pos, L_dir, L_up);

		// projection: box
		fBox3	frustum_bb;
		frustum_bb.invalidate( );
		for (int it = 0; it < 8; it++)
		{
			fVector3	xf = wform(mdir_View, hull.points[it]);
			frustum_bb.modify(xf);
		}
		fBox3& bb = frustum_bb;
		bb.grow(EPSILON_5);
		D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)&mdir_Project, bb.min.x, bb.max.x, bb.min.y, bb.max.y, bb.min.z - tweak_ortho_xform_initial_offs, bb.max.z);

		// full-xform
		cull_xform.mul(mdir_Project, mdir_View);
		FPU::m24r( );
	}

	// Begin SMAP-render
	{
		HOM.Disable( );
		phase = PHASE_SMAP;
		if (RImplementation.o.Tshadows)	r_pmask(true, true);
		else							r_pmask(true, false);
//		fuckingsun->svis.begin					();
	}

	// Fill the database
	xr_vector<fBox3, render_alloc<fBox3> >& s_receivers = main_coarse_structure;
	s_casters.reserve(s_receivers.size( ));
	set_Recorder(&s_casters);
	r_dsgraph_render_subspace(cull_sector, &cull_frustum, cull_xform, cull_COP, TRUE);

	// IGNORE PORTALS
	if (ps_r2_ls_flags.test(R2FLAG_SUN_IGNORE_PORTALS))
	{
		for (u32 s = 0; s < Sectors.size( ); s++)
		{
			CSector* S = (CSector*)Sectors[s];
			IRenderVisual* root = S->root( );

			set_Frustum(&cull_frustum);
			add_Geometry(root);
		}
	}
	set_Recorder(NULL);

	//	Prepare to interact with D3DX code
	const D3DXMATRIX& m_View = *((D3DXMATRIX*)(&Device.mView));
	const D3DXMATRIX& m_Projection = *((D3DXMATRIX*)(&ex_project));
	const D3DXVECTOR3	m_lightDir = -D3DXVECTOR3(fuckingsun->direction.x, fuckingsun->direction.y, fuckingsun->direction.z);

	//  these are the limits specified by the physical camera
	//  gamma is the "tilt angle" between the light and the view direction.
	f32 m_fCosGamma = m_lightDir.x * m_View._13 +
		m_lightDir.y * m_View._23 +
		m_lightDir.z * m_View._33;
	f32 m_fTSM_Delta = ps_r2_sun_tsm_projection;

	// Compute REAL sheared xform based on receivers/casters information
	FPU::m64r( );
	if (_abs(m_fCosGamma) < 0.99f && ps_r2_ls_flags.test(R2FLAG_SUN_TSM))
	{
		//  get the near and the far plane (points) in eye space.
		D3DXVECTOR3 frustumPnts[8];

		Frustum eyeFrustum(&m_Projection);  // autocomputes all the extrema points

		for (int i = 0; i < 4; i++)
		{
			frustumPnts[i] = eyeFrustum.pntList[(i << 1)];       // far plane
			frustumPnts[i + 4] = eyeFrustum.pntList[(i << 1) | 0x1]; // near plane
		}

		//   we need to transform the eye into the light's post-projective space.
		//   however, the sun is a directional light, so we first need to find an appropriate
		//   rotate/translate matrix, before constructing an ortho projection.
		//   this matrix is a variant of "light space" from LSPSMs, with the Y and Z axes permuted

		D3DXVECTOR3			leftVector, upVector, viewVector;
		const D3DXVECTOR3	eyeVector(0.f, 0.f, -1.f);  //  eye is always -Z in eye space

		//  code copied straight from BuildLSPSMProjectionMatrix
		D3DXVec3TransformNormal(&upVector, &m_lightDir, &m_View);  // lightDir is defined in eye space, so xform it
		D3DXVec3Cross(&leftVector, &upVector, &eyeVector);
		D3DXVec3Normalize(&leftVector, &leftVector);
		D3DXVec3Cross(&viewVector, &upVector, &leftVector);

		D3DXMATRIX lightSpaceBasis;
		lightSpaceBasis._11 = leftVector.x; lightSpaceBasis._12 = viewVector.x; lightSpaceBasis._13 = -upVector.x; lightSpaceBasis._14 = 0.f;
		lightSpaceBasis._21 = leftVector.y; lightSpaceBasis._22 = viewVector.y; lightSpaceBasis._23 = -upVector.y; lightSpaceBasis._24 = 0.f;
		lightSpaceBasis._31 = leftVector.z; lightSpaceBasis._32 = viewVector.z; lightSpaceBasis._33 = -upVector.z; lightSpaceBasis._34 = 0.f;
		lightSpaceBasis._41 = 0.f;          lightSpaceBasis._42 = 0.f;          lightSpaceBasis._43 = 0.f;        lightSpaceBasis._44 = 1.f;

		//  rotate the view frustum into light space
		D3DXVec3TransformCoordArray(frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceBasis, sizeof(frustumPnts) / sizeof(D3DXVECTOR3));

		//  build an off-center ortho projection that translates and scales the eye frustum's 3D AABB to the unit cube
		BoundingBox frustumBox(frustumPnts, sizeof(frustumPnts) / sizeof(D3DXVECTOR3));

		//  also - transform the shadow caster bounding boxes into light projective space.  we want to translate along the Z axis so that
		//  all shadow casters are in front of the near plane.
		D3DXVECTOR2	depthbounds = BuildTSMProjectionMatrix_caster_depth_bounds(lightSpaceBasis);

		f32 min_z = min(depthbounds.x, frustumBox.minPt.z);
		f32 max_z = max(depthbounds.y, frustumBox.maxPt.z);

		if (min_z <= 1.f)	//?
		{
			D3DXMATRIX lightSpaceTranslate;
			D3DXMatrixTranslation(&lightSpaceTranslate, 0.f, 0.f, -min_z + 1.f);
			max_z = -min_z + max_z + 1.f;
			min_z = 1.f;
			D3DXMatrixMultiply(&lightSpaceBasis, &lightSpaceBasis, &lightSpaceTranslate);
			D3DXVec3TransformCoordArray(frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceTranslate, sizeof(frustumPnts) / sizeof(D3DXVECTOR3));
			frustumBox = BoundingBox(frustumPnts, sizeof(frustumPnts) / sizeof(D3DXVECTOR3));
		}

		D3DXMATRIX lightSpaceOrtho;
		D3DXMatrixOrthoOffCenterLH(&lightSpaceOrtho, frustumBox.minPt.x, frustumBox.maxPt.x, frustumBox.minPt.y, frustumBox.maxPt.y, min_z, max_z);

		//  transform the view frustum by the new matrix
		D3DXVec3TransformCoordArray(frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceOrtho, sizeof(frustumPnts) / sizeof(D3DXVECTOR3));

		D3DXVECTOR2 centerPts[2];
		//  near plane
		centerPts[0].x = 0.25f * (frustumPnts[4].x + frustumPnts[5].x + frustumPnts[6].x + frustumPnts[7].x);
		centerPts[0].y = 0.25f * (frustumPnts[4].y + frustumPnts[5].y + frustumPnts[6].y + frustumPnts[7].y);
		//  far plane
		centerPts[1].x = 0.25f * (frustumPnts[0].x + frustumPnts[1].x + frustumPnts[2].x + frustumPnts[3].x);
		centerPts[1].y = 0.25f * (frustumPnts[0].y + frustumPnts[1].y + frustumPnts[2].y + frustumPnts[3].y);

		D3DXVECTOR2 centerOrig = (centerPts[0] + centerPts[1]) * 0.5f;

		D3DXMATRIX trapezoid_space;

		D3DXMATRIX xlate_center(1.f, 0.f, 0.f, 0.f,
								0.f, 1.f, 0.f, 0.f,
								0.f, 0.f, 1.f, 0.f,
								-centerOrig.x, -centerOrig.y, 0.f, 1.f);

		D3DXVECTOR2	center_dirl = D3DXVECTOR2(centerPts[1] - centerOrig);
		f32 half_center_len = D3DXVec2Length(&center_dirl);
		f32 x_len = centerPts[1].x - centerOrig.x;
		f32 y_len = centerPts[1].y - centerOrig.y;

		f32 cos_theta = x_len / half_center_len;
		f32 sin_theta = y_len / half_center_len;

		D3DXMATRIX rot_center(cos_theta, -sin_theta, 0.f, 0.f,
							  sin_theta, cos_theta, 0.f, 0.f,
							  0.f, 0.f, 1.f, 0.f,
							  0.f, 0.f, 0.f, 1.f);

						  //  this matrix transforms the center line to y=0.
						  //  since Top and Base are orthogonal to Center, we can skip computing the convex hull, and instead
						  //  just find the view frustum X-axis extrema.  The most negative is Top, the most positive is Base
						  //  Point Q (trapezoid projection point) will be a point on the y=0 line.
		D3DXMatrixMultiply(&trapezoid_space, &xlate_center, &rot_center);
		D3DXVec3TransformCoordArray(frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &trapezoid_space, sizeof(frustumPnts) / sizeof(D3DXVECTOR3));

		BoundingBox frustumAABB2D(frustumPnts, sizeof(frustumPnts) / sizeof(D3DXVECTOR3));

		f32 x_scale = max(_abs(frustumAABB2D.maxPt.x), _abs(frustumAABB2D.minPt.x));
		f32 y_scale = max(_abs(frustumAABB2D.maxPt.y), _abs(frustumAABB2D.minPt.y));
		x_scale = 1.f / x_scale;
		y_scale = 1.f / y_scale;

		//  maximize the area occupied by the bounding box
		D3DXMATRIX scale_center(x_scale, 0.f, 0.f, 0.f,
								0.f, y_scale, 0.f, 0.f,
								0.f, 0.f, 1.f, 0.f,
								0.f, 0.f, 0.f, 1.f);

		D3DXMatrixMultiply(&trapezoid_space, &trapezoid_space, &scale_center);

		//  scale the frustum AABB up by these amounts (keep all values in the same space)
		frustumAABB2D.minPt.x *= x_scale;
		frustumAABB2D.maxPt.x *= x_scale;
		frustumAABB2D.minPt.y *= y_scale;
		frustumAABB2D.maxPt.y *= y_scale;

		//  compute eta.
		f32 lambda = frustumAABB2D.maxPt.x - frustumAABB2D.minPt.x;
		f32 delta_proj = m_fTSM_Delta * lambda;	//focusPt.x - frustumAABB2D.minPt.x;
		const f32 xi = -0.6f;					// - 0.6f;  // 80% line
		f32		eta = (lambda * delta_proj * (1.f + xi)) / (lambda * (1.f - xi) - 2.f * delta_proj);

		//  compute the projection point a distance eta from the top line.  this point is on the center line, y=0
		D3DXVECTOR2 projectionPtQ(frustumAABB2D.maxPt.x + eta, 0.f);

		//  find the maximum slope from the projection point to any point in the frustum.  this will be the
		//  projection field-of-view
		f32 max_slope = -1e32f;
		f32 min_slope = 1e32f;

		for (int i = 0; i < sizeof(frustumPnts) / sizeof(D3DXVECTOR3); i++)
		{
			D3DXVECTOR2 tmp(frustumPnts[i].x * x_scale, frustumPnts[i].y * y_scale);
			f32 x_dist = tmp.x - projectionPtQ.x;
			if (!(ALMOST_ZERO(tmp.y) || ALMOST_ZERO(x_dist)))
			{
				max_slope = max(max_slope, tmp.y / x_dist);
				min_slope = min(min_slope, tmp.y / x_dist);
			}
		}

		f32 xn = eta;
		f32 xf = lambda + eta;

		D3DXMATRIX ptQ_xlate(-1.f, 0.f, 0.f, 0.f,
							 0.f, 1.f, 0.f, 0.f,
							 0.f, 0.f, 1.f, 0.f,
							 projectionPtQ.x, 0.f, 0.f, 1.f);
		D3DXMatrixMultiply(&trapezoid_space, &trapezoid_space, &ptQ_xlate);

		//  this shear balances the "trapezoid" around the y=0 axis (no change to the projection pt position)
		//  since we are redistributing the trapezoid, this affects the projection field of view (shear_amt)
		f32 shear_amt = (max_slope + _abs(min_slope)) * 0.5f - max_slope;
		max_slope = max_slope + shear_amt;

		D3DXMATRIX trapezoid_shear(1.f, shear_amt, 0.f, 0.f,
								   0.f, 1.f, 0.f, 0.f,
								   0.f, 0.f, 1.f, 0.f,
								   0.f, 0.f, 0.f, 1.f);

		D3DXMatrixMultiply(&trapezoid_space, &trapezoid_space, &trapezoid_shear);


		f32 z_aspect = (frustumBox.maxPt.z - frustumBox.minPt.z) / (frustumAABB2D.maxPt.y - frustumAABB2D.minPt.y);

		//  perform a 2DH projection to 'unsqueeze' the top line.
		D3DXMATRIX trapezoid_projection(xf / (xf - xn), 0.f, 0.f, 1.f,
										0.f, 1.f / max_slope, 0.f, 0.f,
										0.f, 0.f, 1.f / (z_aspect * max_slope), 0.f,
										-xn * xf / (xf - xn), 0.f, 0.f, 0.f);

		D3DXMatrixMultiply(&trapezoid_space, &trapezoid_space, &trapezoid_projection);

		//  the x axis is compressed to [0..1] as a result of the projection, so expand it to [-1,1]
		D3DXMATRIX biasedScaleX(2.f, 0.f, 0.f, 0.f,
								0.f, 1.f, 0.f, 0.f,
								0.f, 0.f, 1.f, 0.f,
								-1.f, 0.f, 0.f, 1.f);
		D3DXMatrixMultiply(&trapezoid_space, &trapezoid_space, &biasedScaleX);

		D3DXMatrixMultiply(&m_LightViewProj, &m_View, &lightSpaceBasis);
		D3DXMatrixMultiply(&m_LightViewProj, &m_LightViewProj, &lightSpaceOrtho);
		D3DXMatrixMultiply(&m_LightViewProj, &m_LightViewProj, &trapezoid_space);
	}
	else
	{
		m_LightViewProj = *((D3DXMATRIX*)(&cull_xform));
	}
	FPU::m24r( );

	// perform "refit" or "focusing" on relevant
	if (ps_r2_ls_flags.test(R2FLAG_SUN_FOCUS))
	{
		FPU::m64r( );

		// create clipper
		DumbClipper	view_clipper;
		fMatrix4x4& xform = *((fMatrix4x4*)&m_LightViewProj);
		view_clipper.frustum.CreateFromMatrix(ex_full, FRUSTUM_P_ALL);
		for (int p = 0; p < view_clipper.frustum.p_count; p++)
		{
			fPlane3& P = view_clipper.frustum.planes[p];
			view_clipper.planes.push_back(D3DXPLANE(P.n.x, P.n.y, P.n.z, P.d));
		}

		// 
		fBox3		b_casters;
		fBox3		b_receivers;
		fVector3	pt;

		// casters
		b_casters.invalidate( );
		for (u32 c = 0; c < s_casters.size( ); c++)
		{
			for (int e = 0; e < 8; e++)
			{
				s_casters[c].getpoint(e, pt);
				pt = wform(xform, pt);
				b_casters.modify(pt);
			}
		}

		// receivers
		b_receivers.invalidate( );
		b_receivers = view_clipper.clipped_AABB(s_receivers, xform);
		fMatrix4x4	x_project;
		fMatrix4x4	x_full;
		fMatrix4x4	x_full_inverse;
		{
			x_project.build_projection(deg2rad(Device.fFOV/* *Device.fASPECT*/), Device.fASPECT, ps_r2_sun_near, ps_r2_sun_near + tweak_guaranteed_range);
			x_full.mul(x_project, Device.mView);
			D3DXMatrixInverse((D3DXMATRIX*)&x_full_inverse, 0, (D3DXMATRIX*)&x_full);
		}
		for (int e = 0; e < 8; e++)
		{
			pt = wform(x_full_inverse, corners[e]);	// world space
			pt = wform(xform, pt);						// trapezoid space
			b_receivers.modify(pt);
		}

		// some tweaking
		b_casters.grow(EPSILON_5);
		b_receivers.grow(EPSILON_5);

		// because caster points are from coarse representation only allow to "shrink" box, not grow
		// that is the same as if we first clip casters by frustum
		if (b_receivers.min.x < -1)	b_receivers.min.x = -1;
		if (b_receivers.min.y < -1)	b_receivers.min.y = -1;
		if (b_casters.min.z < 0)		b_casters.min.z = 0;
		if (b_receivers.max.x > +1)	b_receivers.max.x = +1;
		if (b_receivers.max.y > +1)	b_receivers.max.y = +1;
		if (b_casters.max.z > +1)		b_casters.max.z = +1;

		f32 boxWidth = b_receivers.max.x - b_receivers.min.x;
		f32 boxHeight = b_receivers.max.y - b_receivers.min.y;
		//  the divide by two's cancel out in the translation, but included for clarity
		f32 boxX = (b_receivers.max.x + b_receivers.min.x) / 2.f;
		f32 boxY = (b_receivers.max.y + b_receivers.min.y) / 2.f;
		D3DXMATRIX trapezoidUnitCube(2.f / boxWidth, 0.f, 0.f, 0.f,
									 0.f, 2.f / boxHeight, 0.f, 0.f,
									 0.f, 0.f, 1.f, 0.f,
									 -2.f * boxX / boxWidth, -2.f * boxY / boxHeight, 0.f, 1.f);
		D3DXMatrixMultiply(&m_LightViewProj, &m_LightViewProj, &trapezoidUnitCube);
		//D3DXMatrixMultiply( &trapezoid_space, &trapezoid_space, &trapezoidUnitCube );
		FPU::m24r( );
	}

	// Finalize & Cleanup
	fuckingsun->X.D.combine = *((fMatrix4x4*)&m_LightViewProj);
	s_receivers.clear( );
	s_casters.clear( );

	// Render shadow-map
	//. !!! We should clip based on shrinked frustum (again)
	{
		bool	bNormal = mapNormal[0].size( ) || mapMatrix[0].size( );
		bool	bSpecial = mapNormal[1].size( ) || mapMatrix[1].size( ) || mapSorted.size( );
		if (bNormal || bSpecial)
		{
			Target->phase_smap_direct(fuckingsun, SE_SUN_FAR);
			RCache.set_xform_world(Fidentity);
			RCache.set_xform_view(Fidentity);
			RCache.set_xform_project(fuckingsun->X.D.combine);
			r_dsgraph_render_graph(0);
			fuckingsun->X.D.transluent = FALSE;
			if (bSpecial)
			{
				fuckingsun->X.D.transluent = TRUE;
				Target->phase_smap_direct_tsh(fuckingsun, SE_SUN_FAR);
				r_dsgraph_render_graph(1);			// normal level, secondary priority
				r_dsgraph_render_sorted( );			// strict-sorted geoms
			}
		}
	}

	// End SMAP-render
	{
//		fuckingsun->svis.end					();
		r_pmask(true, false);
	}

	// Accumulate
	Target->phase_accumulator( );
	Target->accum_direct(SE_SUN_FAR);

	// Restore XForms
	RCache.set_xform_world(Fidentity);
	RCache.set_xform_view(Device.mView);
	RCache.set_xform_project(Device.mProject);
}

void CRender::render_sun_near( )
{
	light* fuckingsun = (light*)Lights.sun_adapted._get( );
	D3DXMATRIX		m_LightViewProj;

	// calculate view-frustum bounds in world space
	fMatrix4x4	ex_project;
	fMatrix4x4	ex_full;
	fMatrix4x4	ex_full_inverse;
	{
		ex_project.build_projection(deg2rad(Device.fFOV/* *Device.fASPECT*/), Device.fASPECT, VIEWPORT_NEAR, ps_r2_sun_near);
		ex_full.mul(ex_project, Device.mView);
		D3DXMatrixInverse((D3DXMATRIX*)&ex_full_inverse, 0, (D3DXMATRIX*)&ex_full);
	}

	// Compute volume(s) - something like a frustum for infinite directional light
	// Also compute virtual light position and sector it is inside
	CFrustum					cull_frustum;
	xr_vector<fPlane3>			cull_planes;
	fVector3					cull_COP;
	CSector* cull_sector;
	fMatrix4x4						cull_xform;
	{
		FPU::m64r( );
		// Lets begin from base frustum
		fMatrix4x4		fullxform_inv = ex_full_inverse;
#ifdef	_DEBUG
		typedef		DumbConvexVolume<true>	t_volume;
#else
		typedef		DumbConvexVolume<false>	t_volume;
#endif
		t_volume					hull;
		{
			hull.points.reserve(9);
			for (int p = 0; p < 8; p++)
			{
				fVector3				xf = wform(fullxform_inv, corners[p]);
				hull.points.push_back(xf);
			}
			for (int plane = 0; plane < 6; plane++)
			{
				hull.polys.push_back(t_volume::_poly( ));
				for (int pt = 0; pt < 4; pt++)
					hull.polys.back( ).points.push_back(facetable[plane][pt]);
			}
		}
		hull.compute_caster_model(cull_planes, fuckingsun->direction);
#ifdef	_DEBUG
		for (u32 it = 0; it < cull_planes.size( ); it++)
			RImplementation.Target->dbg_addplane(cull_planes[it], 0xffffffff);
#endif

		// Search for default sector - assume "default" or "outdoor" sector is the largest one
		//. hack: need to know real outdoor sector
		CSector* largest_sector = 0;
		f32		largest_sector_vol = 0;
		for (u32 s = 0; s < Sectors.size( ); s++)
		{
			CSector* S = (CSector*)Sectors[s];
			IRenderVisual* V = S->root( );
			f32				vol = V->vis.box.getvolume( );
			if (vol > largest_sector_vol)
			{
				largest_sector_vol = vol;
				largest_sector = S;
			}
		}
		cull_sector = largest_sector;

		// COP - 100 km away
		cull_COP.mad(Device.vCameraPosition, fuckingsun->direction, -tweak_COP_initial_offs);

		// Create frustum for query
		cull_frustum._clear( );
		for (u32 p = 0; p < cull_planes.size( ); p++)
			cull_frustum._add(cull_planes[p]);

		// Create approximate ortho-xform
		// view: auto find 'up' and 'right' vectors
		fMatrix4x4					mdir_View;
		fMatrix4x4					mdir_Project;
		fVector3					L_dir;
		fVector3					L_up;
		fVector3					L_right;
		fVector3					L_pos;
		L_pos.set(fuckingsun->position);
		L_dir.set(fuckingsun->direction).normalize( );
		L_right.set(1.0f, 0.0f, 0.0f);					if (_abs(L_right.dotproduct(L_dir)) > 0.99f)	L_right.set(0.0f, 0.0f, 1.0f);
		L_up.crossproduct(L_dir, L_right).normalize( );
		L_right.crossproduct(L_up, L_dir).normalize( );
		mdir_View.build_camera_dir(L_pos, L_dir, L_up);

		// projection: box
		f32	_D = ps_r2_sun_near;
		f32	a0 = deg2rad(Device.fFOV * Device.fASPECT) / 2.0f;
		f32	a1 = deg2rad(Device.fFOV) / 2.0f;
		f32	c0 = _D / _cos(a0);
		f32	c1 = _D / _cos(a1);
		f32	k0 = 2.0f * c0 * _sin(a0);
		f32	k1 = 2.0f * c1 * _sin(a1);
		f32	borderalpha = (Device.fFOV - 10.0f) / (90.0f - 10.0f);

		f32	nearborder = 1 * borderalpha + 1.136363636364f * (1 - borderalpha);
		f32	spherical_range = ps_r2_sun_near_border * nearborder * _max(_max(c0, c1), _max(k0, k1) * 1.414213562373f);
		fBox3	frustum_bb;
		frustum_bb.invalidate( );
		hull.points.push_back(Device.vCameraPosition);
		for (s32 it = 0; it < 9; it++)
		{
			fVector3	xf = wform(mdir_View, hull.points[it]);
			frustum_bb.modify(xf);
		}
		f32	size_x = frustum_bb.max.x - frustum_bb.min.x;
		f32	size_y = frustum_bb.max.y - frustum_bb.min.y;
		f32	diff_x = (spherical_range - size_x) / 2.0f;	//VERIFY(diff_x>=0);
		f32	diff_y = (spherical_range - size_y) / 2.f;	//VERIFY(diff_y>=0);
		frustum_bb.min.x -= diff_x; frustum_bb.max.x += diff_x;
		frustum_bb.min.y -= diff_y; frustum_bb.max.y += diff_y;
		fBox3& bb = frustum_bb;
		D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)&mdir_Project, bb.min.x, bb.max.x, bb.min.y, bb.max.y, bb.min.z - tweak_ortho_xform_initial_offs, bb.max.z);

		// build viewport xform
		f32	view_dim = f32(RImplementation.o.smapsize);
		fMatrix4x4	m_viewport = {
			view_dim / 2.0f,	0.0f,				0.0f,		0.0f,
			0.0f,			-view_dim / 2.0f,		0.0f,		0.0f,
			0.0f,			0.0f,				1.0f,		0.0f,
			view_dim / 2.0f,	view_dim / 2.0f,		0.0f,		1.0f
		};
		fMatrix4x4				m_viewport_inv;
		D3DXMatrixInverse((D3DXMATRIX*)&m_viewport_inv, 0, (D3DXMATRIX*)&m_viewport);

		// snap view-position to pixel
		cull_xform.mul(mdir_Project, mdir_View);
		fVector3 cam_proj = wform(cull_xform, Device.vCameraPosition);
		fVector3	cam_pixel = wform(m_viewport, cam_proj);
		cam_pixel.x = floorf(cam_pixel.x);
		cam_pixel.y = floorf(cam_pixel.y);
		fVector3 cam_snapped = wform(m_viewport_inv, cam_pixel);
		fVector3 diff;
		diff.sub(cam_snapped, cam_proj);
		fMatrix4x4 adjust;
		adjust.translate(diff);
		cull_xform.mulA_44(adjust);

		// calculate scissor
		fBox3		scissor;
		scissor.invalidate( );
		fMatrix4x4		scissor_xf;
		scissor_xf.mul(m_viewport, cull_xform);
		for (int it = 0; it < 9; it++)
		{
			fVector3	xf = wform(scissor_xf, hull.points[it]);
			scissor.modify(xf);
		}
		s32		limit = RImplementation.o.smapsize - 1;
		fuckingsun->X.D.minX = clampr(iFloor(scissor.min.x), 0, limit);
		fuckingsun->X.D.maxX = clampr(iCeil(scissor.max.x), 0, limit);
		fuckingsun->X.D.minY = clampr(iFloor(scissor.min.y), 0, limit);
		fuckingsun->X.D.maxY = clampr(iCeil(scissor.max.y), 0, limit);

		// full-xform
		FPU::m24r( );
	}

	// Begin SMAP-render
	{
		bool	bSpecialFull = mapNormal[1].size( ) || mapMatrix[1].size( ) || mapSorted.size( );
		VERIFY(!bSpecialFull);
		HOM.Disable( );
		phase = PHASE_SMAP;
		if (RImplementation.o.Tshadows)	r_pmask(true, true);
		else							r_pmask(true, false);
//		fuckingsun->svis.begin					();
	}

	// Fill the database
	r_dsgraph_render_subspace(cull_sector, &cull_frustum, cull_xform, cull_COP, TRUE);

	// Finalize & Cleanup
	fuckingsun->X.D.combine = cull_xform;	//*((fMatrix4x4*)&m_LightViewProj);

	// Render shadow-map
	//. !!! We should clip based on shrinked frustum (again)
	{
		bool	bNormal = mapNormal[0].size( ) || mapMatrix[0].size( );
		bool	bSpecial = mapNormal[1].size( ) || mapMatrix[1].size( ) || mapSorted.size( );
		if (bNormal || bSpecial)
		{
			Target->phase_smap_direct(fuckingsun, SE_SUN_NEAR);
			RCache.set_xform_world(Fidentity);
			RCache.set_xform_view(Fidentity);
			RCache.set_xform_project(fuckingsun->X.D.combine);
			r_dsgraph_render_graph(0);
			if (ps_r2_ls_flags.test(R2FLAG_SUN_DETAILS))
				Details->Render( );
			fuckingsun->X.D.transluent = FALSE;
			if (bSpecial)
			{
				fuckingsun->X.D.transluent = TRUE;
				Target->phase_smap_direct_tsh(fuckingsun, SE_SUN_NEAR);
				r_dsgraph_render_graph(1);			// normal level, secondary priority
				r_dsgraph_render_sorted( );			// strict-sorted geoms
			}
		}
	}

	// End SMAP-render
	{
//		fuckingsun->svis.end					();
		r_pmask(true, false);
	}

	// Accumulate
	Target->phase_accumulator( );
	Target->accum_direct(SE_SUN_NEAR);

	// Restore XForms
	RCache.set_xform_world(Fidentity);
	RCache.set_xform_view(Device.mView);
	RCache.set_xform_project(Device.mProject);
}

void CRender::render_sun_filtered( )
{
	if (!RImplementation.o.sunfilter)	return;
	Target->phase_accumulator( );
	Target->accum_direct(SE_SUN_LUMINANCE);
}

#include "stdafx.h"
#include "IKLimb.h"
#include "..\..\XR_3DA\SkeletonCustom.h"
#include "../ode_include.h"
#include "../MathUtils.h"
#include "../GameObject.h"
#include "../Level.h"
#include "../game_object_space.h"
#include "../ik_anim_state.h"

#ifdef DEBUG
#include "../PHDebug.h"
#endif
const Matrix	IKLocalJoint = { 0,0,1,0,  -1,0,0,0, 0,-1,0,0, 0,0,0,1 };//. in XGlobal
const fMatrix4x4	XLocalJoint = { 0,-1,0,0, -1,0,0,0, 0,0,1,0,  0,0,0,1 };

const fMatrix4x4	xm2im = { 0,0,1,0,	0,1,0,0, 1,0,0,0, 0,0,0,1 };

const fVector3	xgproj_axis = { 0.0f,1.0f,0.0f };
const fVector3	xgpos_axis = { 0.0f,0.0f,1.0f };

const fVector3	xlproj_axis = { 1.0f,0.0f,0.0f };
const fVector3	xlpos_axis = { 0.0f,0.0f,1.0f };
typedef f32 IVektor[3];

const IVektor	lproj_vector = { 0,0,1 };
const IVektor	lpos_vector = { -1,0,0 };

const IVektor	gproj_vector = { 0,0,1 };//. in XGlobal
const IVektor	gpos_vector = { 1,0,0 };

CIKLimb::CIKLimb( )
{
	Invalidate( );
}

void CIKLimb::Invalidate( )
{
	//	m_prev_frame			=u32(-1)											;
	//	m_prev_state_anim		=false												;
	m_id = u16(-1);
	m_toe_position.set(0, 0, 0);
	m_bones[0] = BI_NONE;
	m_bones[1] = BI_NONE;
	m_bones[2] = BI_NONE;
	m_bones[3] = BI_NONE;

}
void XM_IM(const fMatrix4x4& XM, fMatrix4x4& IM)
{
	IM.mul_43(xm2im, XM);
}
void XM_IM(const fMatrix4x4& XM, Matrix& IM)
{
	//((fMatrix4x4*)(&IM))->mul_43(xm2im,XM);
	XM_IM(XM, *((fMatrix4x4*)(&IM)));
}
void IM_XM(const Matrix& IM, fMatrix4x4& XM)
{
	XM.mul_43(xm2im, *((fMatrix4x4*)(&IM)));
}

void XM2IM(const fMatrix4x4& XM, fMatrix4x4& IM)
{
	//IM=xm2im*XM*xm2im^-1
	fMatrix4x4 tmp;
	tmp.mul_43(xm2im, XM);
	IM.mul_43(tmp, xm2im);
}
void XM2IM(const fMatrix4x4& XM, Matrix& IM)
{
	XM2IM(XM, *((fMatrix4x4*)(&IM)));
}
void IM2XM(const Matrix& IM, fMatrix4x4& XM)
{
	XM2IM(*((fMatrix4x4*)(&IM)), XM);
}
void XV2IV(const fVector3& XV, IVektor& IV)
{
	xm2im.transform_dir(cast_fv(IV), XV);
}

void IV2XV(const IVektor& IV, fVector3& XV)
{
	xm2im.transform_dir(XV), cast_fv(IV);
}

void CIKLimb::Calculate(SCalculateData& cd)
{
	//m_prev_state_anim=true;
	ApplyContext(cd);
	if (cd.apply)
		Solve(cd);
}

f32		CIKLimb::SwivelAngle(const fMatrix4x4& ihip, const SCalculateData& cd)
{
	fVector3 foot;
	foot.set(cd.m_K->LL_GetTransform(m_bones[2]).c);// use "0" channal only?
	ihip.transform_tiny(foot);
	xm2im.transform_tiny(foot);

	fVector3 knee;
	knee.set(cd.m_K->LL_GetTransform(m_bones[1]).c);

	fMatrix4x4 ih;
	CBoneData& BD = cd.m_K->LL_GetData(m_bones[0]);
	ih.mul_43(cd.m_K->LL_GetTransform(BD.GetParentID( )), BD.bind_transform);
	ih.invert( );

	ih.transform_tiny(knee);
	xm2im.transform_tiny(knee);

	return m_limb.KneeAngle(cast_fp(foot), cast_fp(knee));
}

IC fMatrix4x4& get_base(fMatrix4x4& base, const fVector3& p0, const fVector3& p1)
{
	base.c.set(0, 0, 0);
	base.i.sub(p1, p0);
	base.i.normalize( );//safe?
	fVector3::generate_orthonormal_basis(base.i, base.j, base.k);
	return base;
}

void	CIKLimb::GetKnee(fVector3& knee, const SCalculateData& cd) const
{
	const fVector3 hip = cd.m_K->LL_GetTransform(m_bones[0]).c;
	knee = cd.m_K->LL_GetTransform(m_bones[1]).c;
	const fVector3 foot = cd.m_K->LL_GetTransform(m_bones[2]).c;
	fVector3 p0;
	p0.sub(foot, hip);
	fVector3 p1;
	p1.sub(cd.goal.c, hip);
	f32 mp0 = p0.magnitude( );
	if (fis_zero(mp0))
		return;
	p0.mul(1.f / mp0);
	knee.sub(hip);
	f32 dot = p0.dotproduct(knee);
	fVector3 b1;
	b1.mul(p0, dot);
	fVector3 b2;
	b2.sub(knee, b1);
	fVector3 bb1;
	bb1.mul(p1, 1.0f / mp0 * dot);//mp1
	knee.add(bb1, b2);
	knee.add(hip);
}

#ifdef DEBUG
bool	repeat = false;
#endif
void	CIKLimb::Solve(SCalculateData& cd)
{
	Matrix gl;
#ifdef DEBUG 
	if (m_limb.SetGoal(Goal(gl, cd.goal, cd), ph_dbg_draw_mask.test(phDbgIKLimits)))
#else
	if (m_limb.SetGoal(Goal(gl, cd.goal, cd), FALSE))
#endif
	{
		f32 x[7];

		fVector3 pos;
		GetKnee(pos, cd);

#ifdef	DEBUG
		if (ph_dbg_draw_mask.test(phDbgDrawIKGoal))
		{
			fVector3 dbg_pos;
			cd.m_obj.transform_tiny(dbg_pos, pos);
			DBG_DrawPoint(dbg_pos, 0.02f, D3DCOLOR_XRGB(255, 255, 255));
		}
#endif
		fMatrix4x4 ihip;
		GetHipInvert(ihip, cd);
		ihip.transform_tiny(pos);
		xm2im.transform_tiny(pos);

		if (m_limb.SolveByPos(cast_fp(pos), x))
		{
			cd.m_angles = x;
			CalculateBones(cd);
			//m_prev_state_anim=false;
		}
#ifdef DEBUG
		else
		{
			Msg("ik not solved");
			if (repeat)
			{
				sv_state = sv_state_DBR;
			}
		}
#endif
	}
#ifdef DEBUG
	else
	{
		Msg("ik not solved");
		if (repeat)
		{
			sv_state = sv_state_DBR;
		}
	}

	if (ph_dbg_draw_mask.test(phDbgDrawIKGoal))
	{
		fVector3 dbg_pos;
		cd.m_K->LL_GetBoneInstance(m_bones[2]).mTransform.transform_tiny(dbg_pos, m_toe_position);
		cd.m_obj.transform_tiny(dbg_pos);
		DBG_DrawPoint(dbg_pos, 0.02f, D3DCOLOR_XRGB(255, 255, 0));
	}
#endif
}

IC void set_limits(f32& min, f32& max, SJointLimit& l)
{
	min = -l.limit.y; max = -l.limit.x;
	min += M_PI; max += M_PI;
	clamp(min, 0.f, 2 * M_PI); clamp(max, 0.f, 2 * M_PI);

	//min = 0/*-2*M_PI*/; max = 2*M_PI;
}

IC void free_limits(f32& min, f32& max)
{
	min = 0; max = 2 * M_PI;
}

void CIKLimb::Create(u16 id, CKinematics* K, const u16 bones[4], const fVector3& toe_pos, bool collide_)
{
	m_id = id;
	m_collide = collide_;
	m_bones[0] = bones[0]; m_bones[1] = bones[1]; m_bones[2] = bones[2]; m_bones[3] = bones[3];
	m_toe_position.set(toe_pos);
	//////////////////////////////////////////////////////////////////////
	xr_vector<fMatrix4x4> binds;
	K->LL_GetBindTransform(binds);
	fMatrix4x4 XT;
	fMatrix4x4 XS;
	XT.set(binds[bones[0]]); XT.invert( ); XT.mulB_43(binds[bones[1]]);
	XS.set(binds[bones[1]]); XS.invert( ); XS.mulB_43(binds[bones[2]]);
	Matrix T, S; XM2IM(XT, T); XM2IM(XS, S);
	/////////////////////////////////////////////////////////////////////
	f32 lmin[7], lmax[7];
	SJointLimit* limits = K->LL_GetData(bones[0]).IK_data.limits;
	set_limits(lmin[0], lmax[0], limits[0]);
	set_limits(lmin[1], lmax[1], limits[1]);
	set_limits(lmin[2], lmax[2], limits[1]);
	//free_limits( lmin[0], lmax[0] );
	//lmin[0] = M_PI * 3.f/4.f; 
	lmin[1] += 1.0f; lmax[1] -= 0.f;
	lmin[2] += 1.0f; lmax[2] -= 0.f;
	lmax[0] = 2 * M_PI - M_PI * 2.f / 3.f;

	//  lmin[2]=-1.f;lmax[2]=1.f;

	limits = K->LL_GetData(bones[1]).IK_data.limits;
	set_limits(lmin[3], lmax[3], limits[1]);
	free_limits(lmin[3], lmax[3]);

	limits = K->LL_GetData(bones[2]).IK_data.limits;
	set_limits(lmin[4], lmax[4], limits[0]);
	set_limits(lmin[5], lmax[5], limits[1]);
	set_limits(lmin[6], lmax[6], limits[2]);
	//free_limits( lmin[4], lmax[4] );
	//free_limits( lmin[5], lmax[5] );
	//free_limits( lmin[6], lmax[6] );
	//lmin[6]=-1.f;lmax[6]=1.f;
	lmin[4] -= 1.0f; lmax[4] += 1.f;
	lmin[5] -= 1.0f; lmax[5] += 1.f;
	lmin[6] -= 1.0f; lmax[6] += 1.f;
	//swap(lmin[4],lmax[4]);
	//swap(lmin[5],lmax[5]);
	//swap(lmin[6],lmax[6]);
	m_limb.init(T, S, ZXY, ZXY, gproj_vector, gpos_vector, lmin, lmax);
}

void CIKLimb::Destroy( )
{
}
IC bool state_valide(const calculate_state& prev_state)
{
	return  (Device.dwFrame - prev_state.frame) == 1; //prev_state.state !=calculate_state::not_definite &&
}

IC void	CIKLimb::GetPickDir(fVector3& v, const fMatrix4x4& gl_bone)
{
	v.set(0.0f, -1.0f, 0.0f);
	if (!state_valide(sv_state))
	{
		sv_state.pick = v;
		VERIFY(_valid(v));
#ifdef	DEBUG
		if (ph_dbg_draw_mask.test(phDbgIK))
			Msg("prev state not valide");
#endif
		return;
	}
	const fMatrix4x4& anim_global = gl_bone; //anim_global.mul_43( cd.m_obj, cd.goal );
	fMatrix4x4 sv_anim_global;
	sv_anim_global.mul_43(sv_state.obj_pos, sv_state.anim_pos);
	fVector3 p0;
	fVector3 p1;
	sv_anim_global.transform_tiny(p0, m_toe_position);
	anim_global.transform_tiny(p1, m_toe_position);
	fVector3 dir;
	dir.sub(p1, p0);
	if (dir.y > 0)
		dir.y = -dir.y;
	dir.mul(dir, 0.01f / Device.fTimeDelta);
	dir.add(fVector3( ).set(0.0f, -0.05f, 0.0f));
	dir.add(sv_state.pick);

	f32 m = dir.magnitude( );
	if (m < EPSILON_5)
	{
		return;
	}

	dir.mul(dir, 1 / m);
	v.set(dir);
	VERIFY(_valid(v));
	sv_state.pick = v;
}

f32 CIKLimb::CollideFoot(f32 angle, const fMatrix4x4& gl_anim, fPlane3& p, fVector3& ax)
{
	fVector3 nc_toe;
	gl_anim.transform_tiny(nc_toe, m_toe_position);				//non collided toe
	f32 dfoot_plain = m_toe_position.x;//xm.i.dotproduct( nc_toe ) - xm.i.dotproduct( xm.c );	//distanse from foot bone to foot plain
	f32 dfoot_tri = -p.d - p.n.dotproduct(gl_anim.c);// dist from foot bone pos to tri plain
	VERIFY(dfoot_plain > 0.f);
	if (dfoot_tri > dfoot_plain * gl_anim.i.dotproduct(p.n)) // foot under tri 
	{
		fVector3 axp;
		axp.sub(nc_toe, gl_anim.c);//normal from nc_toe to ax
		axp.sub(fVector3( ).mul(ax, axp.dotproduct(ax)));
		//fVector3( ).mul( ax, ax.dotproduct( nc_toe ) - ax.dotproduct( xm.c ) );
		f32 dtoe_ax = axp.magnitude( );
		axp.sub(fVector3( ).mul(gl_anim.i, axp.dotproduct(gl_anim.i)));
		f32 dfoot = axp.magnitude( );
		if (dtoe_ax > EPSILON_3 && dfoot_tri < dtoe_ax && dfoot > EPSILON_3 && dfoot < dtoe_ax)
		{
			angle += asinf(dfoot_tri / dtoe_ax);
			VERIFY(_valid(angle));
			angle -= acosf(dfoot / dtoe_ax);
			VERIFY(_valid(angle));
		}
	}
	clamp(angle, -M_PI / 3, 0.f);
	return angle;
}

IC void tri_plane(const CDB::TRI& tri, fPlane3& p)
{
	fVector3* pVerts = Level( ).ObjectSpace.GetStaticVerts( );
	p.n.mknormal(pVerts[tri.verts[0]], pVerts[tri.verts[1]], pVerts[tri.verts[2]]);
	VERIFY(!fis_zero(p.n.magnitude( )));
	p.n.invert( );
	p.d = -p.n.dotproduct(pVerts[tri.verts[0]]);
}

const f32 min_dot = 0.9f;// M_SQRT1_2;//M_SQRT1_2;
void CIKLimb::make_shift(fMatrix4x4& xm, const fPlane3& p, const fVector3& pick_dir)
{
	fVector3 shift = pick_dir;
	fVector3 toe;
	xm.transform_tiny(toe, m_toe_position);

	f32 dot = p.n.dotproduct(shift);
	if (_abs(dot) < min_dot)
	{
		shift.add(fVector3( ).mul(p.n, min_dot - _abs(dot)));
		dot = p.n.dotproduct(shift);
		//shift.set( p.n );
		//dot = 1.f;
	}

	VERIFY(!fis_zero(dot));
	f32 shift_m = (-p.d - p.n.dotproduct(toe)) / dot;
	shift.mul(shift_m);
	xm.c.add(shift);
}

void CIKLimb::GetFootStepMatrix(fMatrix4x4& m, const fMatrix4x4& gl_anim, const  SIKCollideData& cld, bool collide)
{
	if (!cld.collided || (collide && cld.clamp_down))
	{
		m.set(gl_anim);
		return;
	}

	fPlane3 p = cld.m_plane;//; tri_plane( *cld.m_tri, p );
	fMatrix4x4 xm;
	xm.set(gl_anim);
	fVector3 ax;
	ax.crossproduct(p.n, xm.i);
	f32 s = ax.magnitude( );
	clamp(s, 0.f, 1.f);
	f32 angle = asinf(-s);
	VERIFY(_valid(angle));
	if (!fis_zero(s))
	{
		ax.mul(1.f / s);
		if (collide)
		{
			angle = CollideFoot(angle, gl_anim, p, ax);
		}

		fVector3 c = xm.c;
		xm.mulA_43(fMatrix4x4( ).rotation(ax, angle));
		xm.c = c;
	}

	make_shift(xm, p, cld.m_pick_dir);
	VERIFY(_valid(xm));
	m.set(xm);
}

void CollideGoal(fMatrix4x4& g, const  SIKCollideData& cld)
{

	if (cld.collided && !cld.clamp_down)
	{

#ifdef DEBUG
		if (ph_dbg_draw_mask.test(phDbgDrawIKGoal))
		{
			DBG_DrawLine(cld.m_collide, cld.m_anime, D3DCOLOR_XRGB(0, 0, 255));
		}
#endif

		g.c.add(fVector3( ).sub(cld.m_collide, cld.m_anime));
	}
}

IC f32 clamp_rotation(Fquaternion& q, f32 v)
{
	f32 angl;
	fVector3 ax;
	q.get_axis_angle(ax, angl);
	f32 abs_angl = _abs(angl);
	if (abs_angl > v)
	{
		if (angl < 0.f) v = -v;
		q.rotation(ax, v);
		q.normalize( );
	}
	return abs_angl;
}

IC f32  clamp_rotation(fMatrix4x4& m, f32 v)
{
	Fquaternion q;
	q.set(m);
	f32 r = clamp_rotation(q, v);
	fVector3 c = m.c;
	m.rotation(q);
	m.c = c;
	return r;
}

IC void get_axix_angle(const fMatrix4x4& m, fVector3& ax, f32& angl)
{
	Fquaternion q;
	q.set(m);
	q.get_axis_angle(ax, angl);
}

IC bool clamp_change(fMatrix4x4& m, const fMatrix4x4& start, f32 ml, f32 ma, f32 tl, f32 ta)
{
	fMatrix4x4 diff;
	diff.mul_43(fMatrix4x4( ).invert(start), m);
	f32 linear_ch = diff.c.magnitude( );
	bool ret = linear_ch < tl;
	if (linear_ch > ml)
		diff.c.mul(ml / linear_ch);
	if (clamp_rotation(diff, ma) > ta)
		ret = false;
	if (!ret)
		m.mul_43(start, diff);
	return ret;
}

void get_diff_avalue(const fMatrix4x4& m0, const fMatrix4x4& m1, f32& l, f32& a)
{
	fMatrix4x4 diff;
	diff.mul_43(fMatrix4x4( ).invert(m1), m0);
	l = diff.c.magnitude( );
	fVector3 ax;
	get_axix_angle(diff, ax, a);
	a = _abs(a);
}
IC void get_blend_speed_limits(f32& l, f32& a, const SCalculateData& cd, const calculate_state& sv_state)
{
	fMatrix4x4 anim_global;
	anim_global.mul_43(cd.m_obj, cd.goal);
	fMatrix4x4 sv_anim_global;
	sv_anim_global.mul_43(sv_state.obj_pos, sv_state.anim_pos);
	get_diff_avalue(sv_anim_global, anim_global, l, a);
	l *= 1.5f;//a*=1.5;
}
void	CIKLimb::SetNewGoal(const SIKCollideData& cld, SCalculateData& cd)
{
	if (!cd.do_collide)
		return;
	const fMatrix4x4& obj = cd.m_obj;
	const fMatrix4x4 iobj = fMatrix4x4( ).invert(obj);
	const fMatrix4x4 anim_local = cd.goal;
	f32 l, a;
	get_blend_speed_limits(l, a, cd, sv_state);
	//CollideGoal( cd.goal, cld );
	GetFootStepMatrix(cd.goal, fMatrix4x4( ).mul_43(obj, cd.goal), cld, true);
	cd.goal.mulA_43(iobj);

	const fMatrix4x4 gl_goal = fMatrix4x4( ).mul_43(obj, cd.goal);
	fMatrix4x4 blend_to = gl_goal;
	fMatrix4x4 blend_from = sv_state.goal;
	bool	blending = state_valide(sv_state) && (sv_state.blending || sv_state.foot_step != cd.foot_step);
	if (!state_valide(sv_state))
	{
		if (cd.foot_step)
			GetFootStepMatrix(sv_state.collide_pos, gl_goal, cld, false);
	}
	else if (cd.foot_step) //the foot in animation on ground
	{
		if (!sv_state.foot_step)
		{
			fMatrix4x4 cl = gl_goal;
			if (sv_state.blending)
				cl = sv_state.goal;
			GetFootStepMatrix(sv_state.collide_pos, cl, cld, false); // find where we can place the foot
			sv_state.speed_blend_l = l / Device.fTimeDelta;
			sv_state.speed_blend_a = a / Device.fTimeDelta;
		}
		if (blending)
		{
			blend_to = sv_state.collide_pos;
			sv_state.speed_blend_l += 1.f * Device.fTimeDelta;
			sv_state.speed_blend_a += 5.f * Device.fTimeDelta;
			l = sv_state.speed_blend_l * Device.fTimeDelta;
			a = sv_state.speed_blend_a * Device.fTimeDelta;
		}
		else
			cd.goal.mul_43(iobj, sv_state.goal);
	}

#ifdef DEBUG
	if (ph_dbg_draw_mask.test(phDbgDrawIKGoal))
	{
		if (cd.foot_step && state_valide(sv_state))
		{
			DBG_DrawMatrix(sv_state.collide_pos, 1.0f, 100);
			DBG_DrawPoint(sv_state.collide_pos.c, 0.05, D3DCOLOR_XRGB(0, 255, 255));
		}
		if (cd.do_collide)
		{
			DBG_DrawPoint(cld.m_anime, 0.03f, D3DCOLOR_XRGB(255, 255, 255));
			if (cld.collided)
				DBG_DrawPoint(cld.m_collide, 0.05f, D3DCOLOR_XRGB(255, 0, 0));
		}
		if (blending)
		{
			if (cd.foot_step != sv_state.foot_step)
				sv_state.count = 50;
			int c = 55 + 200 / 50 * sv_state.count;
			if (sv_state.count > 0)
				DBG_OpenCashedDraw( );
			fVector3 a0;
			sv_state.goal.transform_tiny(a0, m_toe_position);
			fVector3 a1;
			blend_from.transform_tiny(a1, m_toe_position);
			DBG_DrawLine(a0, a1, D3DCOLOR_XRGB(c, 0, c));
			fVector3 a2;
			gl_goal.transform_tiny(a2, m_toe_position);
			DBG_DrawLine(a1, a2, D3DCOLOR_XRGB(0, c, 0));
			fVector3 a3;
			sv_state_DBR.goal.transform_tiny(a3, m_toe_position);
			DBG_DrawLine(a3, a0, D3DCOLOR_XRGB(c, c, 0));
			if (fVector3( ).sub(a3, a0).magnitude( ) > 0.1f)
			{
				DBG_DrawLine(a3, a0, D3DCOLOR_XRGB(c, 0, 0));
			}

			if (sv_state.count > -1)
			{
				DBG_ClosedCashedDraw(3000);
				--sv_state.count;
			}
		}
	}

	sv_state_DBR = sv_state;
#endif

	if (blending)
	{
		VERIFY(state_valide(sv_state));
		blending = !clamp_change(blend_to, blend_from, l, a, 0.0000001f, 0.00005f);//0.01f //0.005f
		cd.goal.mul_43(iobj, blend_to);
	}

	sv_state.foot_step = cd.foot_step;
	sv_state.anim_pos.set(anim_local);
	sv_state.goal.mul_43(obj, cd.goal);
	sv_state.obj_pos.set(obj);
	sv_state.frame = Device.dwFrame;
	sv_state.blending = blending;
}

void CIKLimb::ApplyContext(SCalculateData& cd)
{
	cd.do_collide = m_collide && cd.do_collide;
	SetAnimGoal(cd);
	cd.foot_step = anim_state.step( );//is_ground( cd );
	SIKCollideData cld;
	if (cd.do_collide)
	{
		cld = collide_data;
	}

	//Collide( cld, ( CGameObject* )cd.m_K->Update_Callback_Param, cd.goal, cd.foot_step );
	SetNewGoal(cld, cd);
}

void	CIKLimb::AnimGoal(fMatrix4x4& gl, CKinematicsAnimated& K)
{
	K.Bone_GetAnimPos(gl, m_bones[2], 1 << 0, false);
}

void	CIKLimb::SetAnimGoal(SCalculateData& cd)
{
	AnimGoal(cd.goal, *cd.m_K);
	cd.apply = true;
}

void	CIKLimb::Update(CGameObject* O, const	CBlend* b, u16 interval)
{
	if (!m_collide)
		return;
	fMatrix4x4 foot;
	CKinematicsAnimated* K = O->Visual( )->dcast_PKinematicsAnimated( );
	AnimGoal(foot, *K);
	anim_state.update(K, b, interval);
	Collide(collide_data, O, foot, anim_state.step( ));
}

const f32		pick_dist = 1.f;
void CIKLimb::Collide(SIKCollideData& cld, CGameObject* O, const fMatrix4x4& foot, bool foot_step)
{
	cld.collided = false;
	const fMatrix4x4& obj = O->XFORM( );
	fMatrix4x4 gl_bone;
	gl_bone.mul_43(obj, foot);
	fVector3 pos;
	gl_bone.transform_tiny(pos, m_toe_position);
	cld.m_anime.set(pos);
	GetPickDir(cld.m_pick_dir, gl_bone);
	const fVector3& pick_v = cld.m_pick_dir;

	pos.sub(fVector3( ).mul(pick_v, pick_dist));
	f32 l_pick_dist = pick_dist;
	if (foot_step)
	{
		l_pick_dist += 1.0f;
	}

	collide::rq_result	R;

	if (g_pGameLevel->ObjectSpace.RayPick(pos, pick_v, l_pick_dist, collide::rqtBoth, R, O))
	{
		if (!R.O)
		{
			cld.collided = true;
			CDB::TRI* tri = Level( ).ObjectSpace.GetStaticTris( ) + R.element;
			tri_plane(*tri, cld.m_plane);
			cld.m_collide.add(pos, fVector3( ).mul(pick_v, R.range));
			cld.clamp_down = R.range > pick_dist + EPSILON_3;
		}
		else
		{
			IRender_Visual* V = R.O->Visual( );
			if (V)
			{
				CKinematics* K = V->dcast_PKinematics( );
				if (K)
				{
					f32 dist = l_pick_dist;
					if (K->PickBone(R.O->XFORM( ), cld.m_plane.n, dist, pos, pick_v, (u16)R.element))
					{
						cld.collided = true;
						fVector3 point;
						point.add(pos, fVector3( ).mul(pick_v, dist));
						cld.m_plane.n.invert( );
						cld.m_plane.d = -cld.m_plane.n.dotproduct(point);
					}
				}
			}
		}
	}

#ifdef DEBUG
	if (ph_dbg_draw_mask.test(phDbgDrawIKGoal) && cld.collided && !R.O)
	{
		CDB::TRI* tri = Level( ).ObjectSpace.GetStaticTris( ) + R.element;
		fVector3 p = pos;
		p.add(fVector3( ).mul(pick_v, l_pick_dist));
		DBG_DrawLine(pos, p, D3DCOLOR_XRGB(255, 0, 0));
		if (tri)
		{
			fVector3 p = pos;
			p.add(fVector3( ).mul(pick_v, l_pick_dist));
			DBG_DrawTri(tri, Level( ).ObjectSpace.GetStaticVerts( ), D3DCOLOR_XRGB(255, 0, 0));
		}
	}
#endif

}

fMatrix4x4& CIKLimb::GetHipInvert(fMatrix4x4& ihip, const SCalculateData& cd)
{
	CKinematics* K = cd.m_K;
	fMatrix4x4 H;
	CBoneData& bd = K->LL_GetData(m_bones[0]);
	H.set(bd.bind_transform);
	H.mulA_43(K->LL_GetTransform(bd.GetParentID( )));
	H.c.set(K->LL_GetTransform(m_bones[0]).c);
	ihip.invert(H);
	return ihip;
}

Matrix& CIKLimb::Goal(Matrix& gl, const fMatrix4x4& xm, SCalculateData& cd)
{

#ifdef DEBUG
	const fMatrix4x4& obj = cd.m_obj;
	if (ph_dbg_draw_mask.test(phDbgDrawIKGoal))
	{
		fMatrix4x4 DBGG;
		DBGG.mul_43(obj, xm);
		DBG_DrawMatrix(DBGG, 0.2f);
		if (cd.do_collide)
		{
			DBG_DrawLine(sv_state.goal.c, DBGG.c, D3DCOLOR_XRGB(255, 0, 255));
			DBG_DrawPoint(sv_state.goal.c, 0.05, D3DCOLOR_XRGB(255, 255, 255));
			DBG_DrawPoint(DBGG.c, 0.04, D3DCOLOR_XRGB(0, 255, 0));
			fVector3 ch;
			ch.sub(DBGG.c, sv_state.goal.c);
			if (ch.magnitude( ) > 0.5f)
			{
				DBG_DrawMatrix(sv_state.goal, 3.5f);
			}
		}

		fMatrix4x4 DBH;
		GetHipInvert(DBH, cd);
		DBH.invert( );
		DBGG.mul_43(obj, DBH);
		DBG_DrawMatrix(DBGG, 0.2f);
	}
#endif

	fMatrix4x4 H;
	GetHipInvert(H, cd);
	fMatrix4x4 G;
	G.mul_43(H, xm);
	XM2IM(G, gl);
	return gl;
}

void CIKLimb::CalculateBones(SCalculateData& cd)
{
	VERIFY(cd.m_angles);
	CKinematics* K = cd.m_K;
	K->LL_GetBoneInstance(m_bones[0]).set_callback(bctCustom, BonesCallback0, &cd);
	K->LL_GetBoneInstance(m_bones[1]).set_callback(bctCustom, BonesCallback1, &cd);
	K->LL_GetBoneInstance(m_bones[2]).set_callback(bctCustom, BonesCallback2, &cd);
	K->LL_GetBoneInstance(m_bones[0]).Callback_overwrite = TRUE;
	K->LL_GetBoneInstance(m_bones[1]).Callback_overwrite = TRUE;
	K->LL_GetBoneInstance(m_bones[2]).Callback_overwrite = TRUE;
	CBoneData& BD = K->LL_GetData(m_bones[0]);
	K->Bone_Calculate(&BD, &K->LL_GetTransform(BD.GetParentID( )));

	K->LL_GetBoneInstance(m_bones[0]).set_callback(bctCustom, NULL, NULL);
	K->LL_GetBoneInstance(m_bones[1]).set_callback(bctCustom, NULL, NULL);
	K->LL_GetBoneInstance(m_bones[2]).set_callback(bctCustom, NULL, NULL);
	K->LL_GetBoneInstance(m_bones[0]).Callback_overwrite = FALSE;
	K->LL_GetBoneInstance(m_bones[1]).Callback_overwrite = FALSE;
	K->LL_GetBoneInstance(m_bones[2]).Callback_overwrite = FALSE;
}

void	DBG_DrawRotationLimitsY(const fMatrix4x4& start, f32 ang, f32 l, f32 h)
{
#ifdef DEBUG
	DBG_DrawRotationY(start, ang - EPSILON_5, ang + EPSILON_5, 0.15f, D3DCOLOR_XRGB(0, 255, 0), false, 1);
	DBG_DrawRotationY(start, l, h, 0.15f, D3DCOLOR_ARGB(50, 0, 250, 0), true);
#endif // DEBUG
}

void	DBG_DrawRotationLimitsZ(const fMatrix4x4& start, f32 ang, f32 l, f32 h)
{
#ifdef DEBUG
	DBG_DrawRotationZ(start, ang - EPSILON_5, ang + EPSILON_5, 0.15f, D3DCOLOR_XRGB(0, 0, 255), false, 1);
	DBG_DrawRotationZ(start, l, h, 0.15f, D3DCOLOR_ARGB(50, 0, 0, 250), true);
#endif // DEBUG
}

void	DBG_DrawRotationLimitsX(const fMatrix4x4& start, f32 ang, f32 l, f32 h)
{
#ifdef DEBUG
	DBG_DrawRotationX(start, ang + EPSILON_5, ang - EPSILON_5, 0.15f, D3DCOLOR_XRGB(255, 0, 0), false, 1);
	DBG_DrawRotationX(start, l, h, 0.15f, D3DCOLOR_ARGB(50, 255, 0, 0), true);
#endif // DEBUG
}

void	DBG_DrawRotation3(const fMatrix4x4& start, const f32 angs[7], const AngleInt limits[7], u16 y, u16 z, u16 x)
{
	fMatrix4x4 DBGG = start;
	DBG_DrawRotationLimitsY(DBGG, -angs[y], -limits[y].Low( ), -limits[y].High( ));
	DBGG.mulB_43(fMatrix4x4( ).rotateY(-angs[y]));
	DBG_DrawRotationLimitsZ(DBGG, -angs[z], -limits[z].Low( ), -limits[z].High( ));
	DBGG.mulB_43(fMatrix4x4( ).rotateZ(-angs[z]));
	DBG_DrawRotationLimitsX(DBGG, -angs[x], -limits[x].Low( ), -limits[x].High( ));
}
IC void ang_evaluate(fMatrix4x4& M, const f32 ang[3])
{
	fMatrix4x4 ry;
	ry.rotateY(-ang[0]);
	fMatrix4x4 rz;
	rz.rotateZ(-ang[1]);
	fMatrix4x4 rx;
	rx.rotateX(-ang[2]);
	M.mul_43(fMatrix4x4( ).mul_43(ry, rz), rx);
}

IC void CIKLimb::get_start(fMatrix4x4& start, SCalculateData& D, u16 bone)
{
	CKinematics* K = D.m_K;
	VERIFY(K);
	CIKLimb& L = D.m_limb;
	CBoneData& BD = K->LL_GetData(L.m_bones[bone]);
	start.mul_43(K->LL_GetTransform(BD.GetParentID( )), BD.bind_transform);
}

void 	CIKLimb::BonesCallback0(CBoneInstance* B)
{
	SCalculateData* D = (SCalculateData*)B->Callback_Param;
	VERIFY(D);

	f32	const* x = D->m_angles;
	fMatrix4x4			bm;
	ang_evaluate(bm, x);
	fMatrix4x4 start;
	get_start(start, *D, 0);

	B->mTransform.mul_43(start, bm);

#ifdef DEBUG
	CIKLimb& L = D->m_limb;
	if (ph_dbg_draw_mask1.test(phDbgDrawIKLimits))
		DBG_DrawRotation3(fMatrix4x4( ).mul_43(D->m_obj, start), x, L.m_limb.jt_limits, 0, 1, 2);
	if (ph_dbg_draw_mask.test(phDbgDrawIKGoal))
	{
		DBG_DrawMatrix(fMatrix4x4( ).mul_43(D->m_obj, start), 1.0f);
		DBG_DrawMatrix(fMatrix4x4( ).mul_43(D->m_obj, fMatrix4x4( ).mul_43(start, bm)), 0.75f);
	}
#endif

}
void 	CIKLimb::BonesCallback1(CBoneInstance* B)
{
	SCalculateData* D = (SCalculateData*)B->Callback_Param;

	f32	const* x = D->m_angles;
	fMatrix4x4 		bm;
	bm.rotateY(x[3]);

	fMatrix4x4 start;
	get_start(start, *D, 1);
	B->mTransform.mul_43(start, bm);
}
void 	CIKLimb::BonesCallback2(CBoneInstance* B)
{
	SCalculateData* D = (SCalculateData*)B->Callback_Param;

	f32	const* x = D->m_angles;
	fMatrix4x4 		bm;
	ang_evaluate(bm, x + 4);

	fMatrix4x4 start;
	get_start(start, *D, 2);
	B->mTransform.mul_43(start, bm);

#ifdef DEBUG
	CIKLimb& L = D->m_limb;
	if (ph_dbg_draw_mask1.test(phDbgDrawIKLimits))
	{
		DBG_DrawRotation3(fMatrix4x4( ).mul_43(D->m_obj, start), x, L.m_limb.jt_limits, 4, 5, 6);
	}
	if (ph_dbg_draw_mask.test(phDbgDrawIKGoal))
	{
		DBG_DrawMatrix(fMatrix4x4( ).mul_43(D->m_obj, fMatrix4x4( ).mul_43(start, bm)), 0.3f);
		DBG_DrawMatrix(fMatrix4x4( ).mul_43(D->m_obj, start), 0.3f);
	}
#endif
}
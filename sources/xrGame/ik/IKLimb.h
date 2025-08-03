#pragma once


#include "limb.h"
#include "../..\XR_3DA\SkeletonAnimated.h"
#include "../ik_anim_state.h"
class	CKinematics;
class	CDB::TRI;
struct SCalculateData;
struct SIKCollideData;
class  CGameObject;
class motion_marks;
struct SIKCollideData
{
	//CDB::TRI		*m_tri		;
	fPlane3			m_plane;
	fVector3			m_collide;
	fVector3			m_anime;
	fVector3			m_pick_dir;
	bool			collided;
	bool			clamp_down;
	SIKCollideData( ) : collided(false), clamp_down(false)
	{ }
};

struct calculate_state
{
	u32		frame;

	fMatrix4x4 goal;
	fMatrix4x4 anim_pos;
	fMatrix4x4	obj_pos;
	fMatrix4x4	collide_pos;
	fVector3 pick;
	f32	speed_blend_l;
	f32	speed_blend_a;
	bool	foot_step;
	bool	blending;
#ifdef DEBUG
	s32		count;
#endif
	calculate_state( ) : frame(0), foot_step(false), blending(false),
		anim_pos(Fidentity), speed_blend_l(0), speed_blend_a(0),
		pick(fVector3( ).set(0.0f, 0.0f, 0.0f))
#ifdef DEBUG
		, count(-1)
#endif
	{ }
};

class CIKLimb
{
public:
	CIKLimb( );
	void		Create(u16 id, CKinematics* K, const u16 bones[3], const fVector3& toe_pos, bool collide_);
	void		Destroy( );
	void		Calculate(SCalculateData& cd);
	void		Update(CGameObject* O, const	CBlend* b, u16 interval);
	IC				u16			get_id( )
	{
		return m_id;
	}
private:
	void		Invalidate( );
	void		GetFootStepMatrix(fMatrix4x4& m, const fMatrix4x4& gl_anim, const  SIKCollideData& cld, bool collide);
	IC				f32		CollideFoot(f32 angle, const fMatrix4x4& gl_anim, fPlane3& p, fVector3& ax);
	IC				void		make_shift(fMatrix4x4& xm, const fPlane3& p, const fVector3& pick_dir);
	void		ApplyContext(SCalculateData& cd);
	void		Solve(SCalculateData& cd);
	void		Collide(SIKCollideData& cld, CGameObject* O, const fMatrix4x4& foot, bool foot_step);
	IC				void		AnimGoal(fMatrix4x4& gl, CKinematicsAnimated& K);
	void		SetAnimGoal(SCalculateData& cd);
	void		SetNewGoal(const SIKCollideData& cld, SCalculateData& cd);
	void		CalculateBones(SCalculateData& cd);
	Matrix& Goal(Matrix& gl, const fMatrix4x4& xm, SCalculateData& cd);
	fMatrix4x4& GetHipInvert(fMatrix4x4& ihip, const SCalculateData& cd);
	f32		SwivelAngle(const fMatrix4x4& ihip, const SCalculateData& cd);
	void		GetKnee(fVector3& knee, const SCalculateData& cd) const;
	void		GetPickDir(fVector3& v, const fMatrix4x4& gl_bone);
	IC		static	void		get_start(fMatrix4x4& start, SCalculateData& D, u16 bone);

private:
	static	void 		BonesCallback0(CBoneInstance* B);
	static	void 		BonesCallback1(CBoneInstance* B);
	static	void 		BonesCallback2(CBoneInstance* B);

private:
	Limb		m_limb;
	fVector3		m_toe_position;
	u16			m_bones[4];
	u16			m_id;
	bool		m_collide;
	SIKCollideData	collide_data;
	ik_anim_state	anim_state;
	calculate_state	sv_state;

#ifdef DEBUG
	calculate_state	sv_state_DBR;
#endif

};

#include <boost/noncopyable.hpp>
class	ik_anim_state;
struct SCalculateData : private boost::noncopyable
{
	f32	const* m_angles;
	CKinematicsAnimated* m_K;
	CIKLimb& m_limb;
	const fMatrix4x4& m_obj;

	bool				do_collide;

	fMatrix4x4				goal;
	bool				apply;
	bool				foot_step;


	//	const BlendSVec		&anim_base			;
	//	const motion_vec	&uneffected_motions	;

	SCalculateData(CIKLimb& l, CKinematicsAnimated* K, const fMatrix4x4& o) :
		m_limb(l), m_obj(o), m_K(K), m_angles(0), apply(false),
		do_collide(false), foot_step(false)
	{ }
};

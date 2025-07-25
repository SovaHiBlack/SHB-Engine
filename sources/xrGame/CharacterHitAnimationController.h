#pragma once
class CEntityAlive;

#include "..\XR_3DA\SkeletonAnimated.h"

class CCharacterHitAnimationController
{
public:
	void								SetupHitMotions(CKinematicsAnimated& ca);
	void								PlayHitMotion(const fVector3& dir, const fVector3& bone_pos, u16 bi, CEntityAlive& ea) const;
	void								GetBaseMatrix(fMatrix4x4& m, CEntityAlive& ea) const;

private:
	bool								IsEffected(u16	bi, CKinematics& ca) const;

	//
	u16									base_bone;
	MotionID							bkhit_motion;
	MotionID							fvhit_motion;
	MotionID							rthit_motion;
	MotionID							lthit_motion;
	MotionID							turn_right;
	MotionID							turn_left;
	MotionID							all_shift_down;
	MotionID							hit_downl;
	MotionID							hit_downr;
	static const u16					num_anims = 7;
	mutable u32							block_times[num_anims];
	//
};

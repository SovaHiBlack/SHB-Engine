#pragma once

class CEntityAlive;

#include "..\ENGINE\SkeletonAnimated.h"

class character_hit_animation_controller
{
public:
	void								SetupHitMotions(CKinematicsAnimated& ca);
	void								PlayHitMotion(const Fvector3& dir, const Fvector3& bone_pos, unsigned short bi, CEntityAlive& ea)const;
	void								GetBaseMatrix(Fmatrix& m, CEntityAlive& ea)const;

private:
	bool								IsEffected(unsigned short bi, CKinematics& ca)const;

	unsigned short									base_bone;
	MotionID							bkhit_motion;
	MotionID							fvhit_motion;
	MotionID							rthit_motion;
	MotionID							lthit_motion;
	MotionID							turn_right;
	MotionID							turn_left;
	MotionID							all_shift_down;
	MotionID							hit_downl;
	MotionID							hit_downr;
	static const unsigned short					num_anims = 7;
	mutable u32							block_times[num_anims];
	//
};

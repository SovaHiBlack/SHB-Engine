#pragma once

//////////////////////////////////////////////////////////////////////////
// SStateDataAction
//////////////////////////////////////////////////////////////////////////
struct SStateDataAction {
	EAction		action;
	u32			spec_params;
	u32			time_out;
	u32			sound_type;
	u32			sound_delay;

	SStateDataAction() {
		action		= ACT_STAND_IDLE;
		spec_params	= 0;
		time_out	= 0;
		sound_type	= u32(-1);
		sound_delay	= u32(-1);
	}
};

//////////////////////////////////////////////////////////////////////////
// SStateDataMoveToPoint
//////////////////////////////////////////////////////////////////////////
struct SStateDataMoveToPoint {
	Fvector				point;
	u32					vertex;
	
	bool				accelerated;
	bool				braking;
	u8					accel_type;
	
	F32				completion_dist;
	
	SStateDataAction	action;

	SStateDataMoveToPoint() {
		point.set			(0.f,0.f,0.f);
		vertex				= u32(-1);
		accelerated			= false;
		completion_dist		= 0.f;
	}
};

//////////////////////////////////////////////////////////////////////////
// SStateDataMoveToPointEx
//////////////////////////////////////////////////////////////////////////
struct SStateDataMoveToPointEx : public SStateDataMoveToPoint {
	
	u32 time_to_rebuild;		//u32(-1) - �� �������������, 0-��-���������, ...

	SStateDataMoveToPointEx() {
		time_to_rebuild = u32(-1);
	}
};

//////////////////////////////////////////////////////////////////////////
// SStateHideFromPoint
//////////////////////////////////////////////////////////////////////////
struct SStateHideFromPoint {
	Fvector				point;
	
	bool				accelerated;
	bool				braking;
	u8					accel_type;
	
	F32				distance;
	
	F32				cover_min_dist;
	F32				cover_max_dist;
	F32				cover_search_radius;
	
	SStateDataAction	action;

	SStateHideFromPoint() {
		point.set			(0.f,0.f,0.f);
		
		accelerated			= false;
		
		distance			= 1.f;
		
		cover_min_dist		= 10.f;
		cover_max_dist		= 30.f;
		cover_search_radius = 20.f;
	}
};		

//////////////////////////////////////////////////////////////////////////
// SStateDataLookToPoint
//////////////////////////////////////////////////////////////////////////
struct SStateDataLookToPoint {
	Fvector				point;
	u32					face_delay;
	SStateDataAction	action;

	SStateDataLookToPoint() {
		point.set		(0.f,0.f,0.f);
		face_delay		= 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// SStateDataMoveAroundPoint
//////////////////////////////////////////////////////////////////////////
struct SStateDataMoveAroundPoint {
	Fvector				point;
	u32					vertex;
	
	F32				radius;

	bool				accelerated;
	bool				braking;
	u8					accel_type;
	
	SStateDataAction	action;

	SStateDataMoveAroundPoint() {
		point.set			(0.f,0.f,0.f);
		vertex				= u32(-1);
		accelerated			= false;
		radius				= 10.f;
	}
};

//////////////////////////////////////////////////////////////////////////
// SStateDataActionLook
//////////////////////////////////////////////////////////////////////////
struct SStateDataActionLook : public SStateDataAction {
	Fvector		point;			

	SStateDataActionLook() {
		point.set	(0.f,0.f,0.f);
	}
};


#pragma once

struct SOneDDOParams
{
	void				Mul(f32 v);
	f32					velocity;
	f32					acceleration;
};

struct SAllDDOParams
{
	void				Reset( );
	void				Load(CIniFile* ini);
	SOneDDOParams		translational;
	SOneDDOParams		rotational;
	u16					L2frames;
};

struct SAllDDWParams
{
	SAllDDOParams		objects_params;
	f32					reanable_factor;
};

extern SAllDDWParams	worldDisablingParams;

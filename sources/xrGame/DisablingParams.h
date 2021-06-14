#pragma once

struct SOneDDOParams
{
	void				Mul		(float v);
	float						velocity;
	float						acceleration;
};

struct SAllDDOParams
{
	void				Reset	( );
	void				Load	(CIniFile* ini);
	SOneDDOParams				translational;
	SOneDDOParams				rotational;
	U16							L2frames;
};

struct SAllDDWParams
{
	SAllDDOParams				objects_params;
	float						reanable_factor;
};

extern SAllDDWParams			worldDisablingParams;

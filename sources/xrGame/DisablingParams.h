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
	void				Load	(CConfigurationFile* ini);
	SOneDDOParams				translational;
	SOneDDOParams				rotational;
	unsigned short							L2frames;
};

struct SAllDDWParams
{
	SAllDDOParams				objects_params;
	float						reanable_factor;
};

extern SAllDDWParams			worldDisablingParams;

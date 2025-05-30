// Tracer.h: interface for the CTracer class.
#pragma once

class CBulletManager;

class CTracer
{
	friend CBulletManager;

protected:
	ref_shader			sh_Tracer;
	ref_geom			sh_Geom;
	xr_vector<u32>		m_aColors;

public:
	CTracer( );
	~CTracer( );

	void				Render(FVF::LIT*& verts, const fVector3& pos, const fVector3& center, const fVector3& dir, f32 length, f32 width, u8 colorID);
};

// Tracer.cpp: implementation of the CTracer class.

#include "stdafx.h"
#include "Tracer.h"
#include "..\XR_3DA\Render.h"

const u32 MAX_TRACERS = (1024 * 5);
const f32 TRACER_SIZE = 0.13f;

#define TRACERS_COLOR_TABLE "tracers_color_table"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTracer::CTracer( )
{
	sh_Tracer.create("effects\\bullet_tracer", "fx\\fx_tracer");
	sh_Geom.create(FVF::F_LIT, RCache.Vertex.Buffer( ), RCache.QuadIB);
	m_aColors.clear( );
	for (u8 i = 0; i < 255; i++)
	{
		shared_str LineName;
		LineName.sprintf("color_%d", i);
		if (!pSettings->line_exist(TRACERS_COLOR_TABLE, LineName))
		{
			break;
		}

		f32 r;
		f32 g;
		f32 b;
		sscanf(pSettings->r_string(TRACERS_COLOR_TABLE, *LineName), "%f,%f,%f", &r, &g, &b);
		m_aColors.push_back(color_argb_f(1.0f, r, g, b));
	}
}

CTracer::~CTracer( )
{ }

IC void FillSprite_Circle(FVF::LIT*& pv, const fVector3& pos, const f32 r1, f32 r2, u32 color)
{
	const fVector3& T = Device.vCameraTop;
	const fVector3& R = Device.vCameraRight;
	fVector3 Vr;
	fVector3 Vt;
	Vr.x = R.x * r1;
	Vr.y = R.y * r1;
	Vr.z = R.z * r1;
	Vt.x = T.x * r2;
	Vt.y = T.y * r2;
	Vt.z = T.z * r2;

	fVector3 a;
	fVector3 b;
	fVector3 c;
	fVector3 d;
	a.sub(Vt, Vr);
	b.add(Vt, Vr);
	c.invert(a);
	d.invert(b);
	pv->set(d.x + pos.x, d.y + pos.y, d.z + pos.z, color, 0.0f, 1.0f);
	pv++;
	pv->set(a.x + pos.x, a.y + pos.y, a.z + pos.z, color, 0.0f, 0.0f);
	pv++;
	pv->set(c.x + pos.x, c.y + pos.y, c.z + pos.z, color, 1.0f, 1.0f);
	pv++;
	pv->set(b.x + pos.x, b.y + pos.y, b.z + pos.z, color, 1.0f, 0.0f);
	pv++;
}

IC void FillSprite_Line(FVF::LIT*& pv, const fVector3& pos, const fVector3& dir, f32 r1, f32 r2, u32 color)
{
	const fVector3& T = dir;

	fVector3 R;
	R.crossproduct(T, Device.vCameraDirection).normalize_safe( );

	fVector3 Vr;
	fVector3 Vt;
	Vr.x = R.x * r1;
	Vr.y = R.y * r1;
	Vr.z = R.z * r1;
	Vt.x = T.x * r2;
	Vt.y = T.y * r2;
	Vt.z = T.z * r2;

	fVector3 a;
	fVector3 b;
	fVector3 c;
	fVector3 d;
	a.sub(Vt, Vr);
	b.add(Vt, Vr);
	c.invert(a);
	d.invert(b);
	pv->set(d.x + pos.x, d.y + pos.y, d.z + pos.z, color, 0.0f, 1.0f);
	pv++;
	pv->set(a.x + pos.x, a.y + pos.y, a.z + pos.z, color, 0.0f, 0.5f);
	pv++;
	pv->set(c.x + pos.x, c.y + pos.y, c.z + pos.z, color, 1.0f, 1.0f);
	pv++;
	pv->set(b.x + pos.x, b.y + pos.y, b.z + pos.z, color, 1.0f, 0.5f);
	pv++;
}

void CTracer::Render(FVF::LIT*& verts, const fVector3& pos, const fVector3& center, const fVector3& dir, f32 length, f32 width, u8 colorID)
{
	if (::Render->ViewBase.testSphere_dirty((fVector3&) center, length * .5f))
	{
		if (colorID >= m_aColors.size( ))
		{
			colorID = 0;
		}

		FillSprite_Circle(verts, pos, width * 0.5f, width * 0.5f, m_aColors[colorID]);
		FillSprite_Line(verts, center, dir, width * 0.5f, length * 0.5f, m_aColors[colorID]);
	}
}

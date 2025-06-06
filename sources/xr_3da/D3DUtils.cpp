#include "stdafx.h"

#include "gamefont.h"
#include "d3dutils.h"
#include "du_box.h"
#include "du_sphere.h"
#include "du_sphere_part.h"
#include "du_cone.h"
#include "du_cylinder.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include "d3dx9.h"
#pragma warning(pop)

CDrawUtilities DU;

#define LINE_DIVISION  32  // �� ������ 6!!!!!
// for drawing sphere
static fVector3 circledef1[LINE_DIVISION];
static fVector3 circledef2[LINE_DIVISION];
static fVector3 circledef3[LINE_DIVISION];

const u32 boxcolor = D3DCOLOR_RGBA(255, 255, 255, 0);
static const s32 boxvertcount = 48;
static fVector3 boxvert[boxvertcount];

#define DU_DRAW_RS	RCache.dbg_SetRS
#define DU_DRAW_SH_C(sh,c)		{ RCache.set_Shader(sh);	RCache.set_c("tfactor", f32(color_get_R(c)) / 255.0f, f32(color_get_G(c)) / 255.0f, f32(color_get_B(c)) / 255.0f, f32(color_get_A(c)) / 255.0f); }
#define DU_DRAW_SH(sh)			{ RCache.set_Shader(sh);	RCache.set_c("tfactor", 1.0f, 1.0f, 1.0f, 1.0f); }

#define FILL_MODE D3DFILL_SOLID
#define SHADE_MODE D3DSHADE_GOURAUD
#define SCREEN_QUALITY 1.0f

// identity box
const u32 identboxcolor = D3DCOLOR_RGBA(255, 255, 255, 0);
static const s32 identboxwirecount = 24;
static fVector3 identboxwire[identboxwirecount] =
{
	{-0.5f, -0.5f, -0.5f},	{-0.5f, +0.5f, -0.5f},	{-0.5f, +0.5f, -0.5f},	{+0.5f, +0.5f, -0.5f},
	{+0.5f, +0.5f, -0.5f},	{+0.5f, -0.5f, -0.5f},	{+0.5f, -0.5f, -0.5f},	{-0.5f, -0.5f, -0.5f},
	{-0.5f, +0.5f, +0.5f},	{+0.5f, +0.5f, +0.5f},	{+0.5f, +0.5f, +0.5f},	{+0.5f, -0.5f, +0.5f},
	{+0.5f, -0.5f, +0.5f},	{-0.5f, -0.5f, +0.5f},	{-0.5f, -0.5f, +0.5f},	{-0.5f, +0.5f, +0.5f},
	{-0.5f, +0.5f, -0.5f},	{-0.5f, +0.5f, +0.5f},	{+0.5f, +0.5f, -0.5f},	{+0.5f, +0.5f, +0.5f},
	{+0.5f, -0.5f, -0.5f},	{+0.5f, -0.5f, +0.5f},	{-0.5f, -0.5f, -0.5f},	{-0.5f, -0.5f, +0.5f}
};

#define SIGN(x) ((x < 0) ? -1 : 1)

DEFINE_VECTOR(FVF::L, FLvertexVec, FLvertexIt)

static FLvertexVec 	m_GridPoints;

u32 m_ColorAxis = 0xff000000;
u32 m_ColorGrid = 0xff909090;
u32 m_ColorGridTh = 0xffb4b4b4;
u32 m_SelectionRect = D3DCOLOR_RGBA(127, 255, 127, 64);

u32 m_ColorSafeRect = 0xffB040B0;

void SPrimitiveBuffer::CreateFromData(D3DPRIMITIVETYPE _pt, u32 _p_cnt, u32 FVF, LPVOID vertices, u32 _v_cnt, u16* indices, u32 _i_cnt)
{
	IDirect3DVertexBuffer9* pVB = 0;
	IDirect3DIndexBuffer9* pIB = 0;
	p_cnt = _p_cnt;
	p_type = _pt;
	v_cnt = _v_cnt;
	i_cnt = _i_cnt;
	u32 stride = D3DXGetFVFVertexSize(FVF);
	R_CHK(HW.pDevice->CreateVertexBuffer(v_cnt * stride, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVB, 0));
	u8* bytes;
	R_CHK(pVB->Lock(0, 0, (pvoid*)&bytes, 0));
	FLvertexVec verts(v_cnt);
	for (u32 k = 0; k < v_cnt; ++k)
	{
		verts[k].set(((fVector3*)vertices)[k], 0xFFFFFFFF);
	}

	Memory.mem_copy(bytes, &*verts.begin( ), v_cnt * stride);
	R_CHK(pVB->Unlock( ));
	if (i_cnt)
	{
		R_CHK(HW.pDevice->CreateIndexBuffer(i_cnt * sizeof(u16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, NULL));
		R_CHK(pIB->Lock(0, 0, (pvoid*)&bytes, 0));
		Memory.mem_copy(bytes, indices, i_cnt * sizeof(u16));
		R_CHK(pIB->Unlock( ));
		OnRender.bind(this, &SPrimitiveBuffer::RenderDIP);
	}
	else
	{
		OnRender.bind(this, &SPrimitiveBuffer::RenderDP);
	}

	pGeom.create(FVF, pVB, pIB);
}
void SPrimitiveBuffer::Destroy( )
{
	if (pGeom)
	{
		_RELEASE(pGeom->vb);
		_RELEASE(pGeom->ib);
		pGeom.destroy( );
	}
}

void CDrawUtilities::UpdateGrid(s32 number_of_cell, f32 square_size, s32 subdiv)
{
	m_GridPoints.clear( );
	// grid
	s32 m_GridSubDiv[2];
	s32 m_GridCounts[2];
	fVector2 m_GridStep;

	m_GridStep.set(square_size, square_size);
	m_GridSubDiv[0] = subdiv;
	m_GridSubDiv[1] = subdiv;
	m_GridCounts[0] = number_of_cell;//iFloor(size/step)*subdiv;
	m_GridCounts[1] = number_of_cell;//iFloor(size/step)*subdiv;

	FVF::L left;
	FVF::L right;
	left.p.y = right.p.y = 0;

	for (s32 thin = 0; thin < 2; thin++)
	{
		for (s32 i = -m_GridCounts[0]; i <= m_GridCounts[0]; i++)
		{
			if ((!!thin) != !!(i % m_GridSubDiv[0]))
			{
				left.p.z = -m_GridCounts[1] * m_GridStep.y;
				right.p.z = m_GridCounts[1] * m_GridStep.y;
				left.p.x = i * m_GridStep.x;
				right.p.x = left.p.x;
				left.color = (i % m_GridSubDiv[0]) ? m_ColorGrid : m_ColorGridTh;
				right.color = left.color;
				m_GridPoints.push_back(left);
				m_GridPoints.push_back(right);
			}
		}

		for (i = -m_GridCounts[1]; i <= m_GridCounts[1]; i++)
		{
			if ((!!thin) != !!(i % m_GridSubDiv[1]))
			{
				left.p.x = -m_GridCounts[0] * m_GridStep.x;
				right.p.x = m_GridCounts[0] * m_GridStep.x;
				left.p.z = i * m_GridStep.y;
				right.p.z = left.p.z;
				left.color = (i % m_GridSubDiv[1]) ? m_ColorGrid : m_ColorGridTh;
				right.color = left.color;
				m_GridPoints.push_back(left);
				m_GridPoints.push_back(right);
			}
		}
	}
}

void CDrawUtilities::OnDeviceCreate( )
{
	Device.seqRender.Add(this, REG_PRIORITY_LOW - 1000);

	m_SolidBox.CreateFromData(D3DPT_TRIANGLELIST, DU_BOX_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_box_vertices, DU_BOX_NUMVERTEX, du_box_faces, DU_BOX_NUMFACES * 3);
	m_SolidCone.CreateFromData(D3DPT_TRIANGLELIST, DU_CONE_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_cone_vertices, DU_CONE_NUMVERTEX, du_cone_faces, DU_CONE_NUMFACES * 3);
	m_SolidSphere.CreateFromData(D3DPT_TRIANGLELIST, DU_SPHERE_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_sphere_vertices, DU_SPHERE_NUMVERTEX, du_sphere_faces, DU_SPHERE_NUMFACES * 3);
	m_SolidSpherePart.CreateFromData(D3DPT_TRIANGLELIST, DU_SPHERE_PART_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_sphere_part_vertices, DU_SPHERE_PART_NUMVERTEX, du_sphere_part_faces, DU_SPHERE_PART_NUMFACES * 3);
	m_SolidCylinder.CreateFromData(D3DPT_TRIANGLELIST, DU_CYLINDER_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_cylinder_vertices, DU_CYLINDER_NUMVERTEX, du_cylinder_faces, DU_CYLINDER_NUMFACES * 3);
	m_WireBox.CreateFromData(D3DPT_LINELIST, DU_BOX_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_box_vertices, DU_BOX_NUMVERTEX, du_box_lines, DU_BOX_NUMLINES * 2);
	m_WireCone.CreateFromData(D3DPT_LINELIST, DU_CONE_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_cone_vertices, DU_CONE_NUMVERTEX, du_cone_lines, DU_CONE_NUMLINES * 2);
	m_WireSphere.CreateFromData(D3DPT_LINELIST, DU_SPHERE_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_sphere_verticesl, DU_SPHERE_NUMVERTEXL, du_sphere_lines, DU_SPHERE_NUMLINES * 2);
	m_WireSpherePart.CreateFromData(D3DPT_LINELIST, DU_SPHERE_PART_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_sphere_part_vertices, DU_SPHERE_PART_NUMVERTEX, du_sphere_part_lines, DU_SPHERE_PART_NUMLINES * 2);
	m_WireCylinder.CreateFromData(D3DPT_LINELIST, DU_CYLINDER_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_cylinder_vertices, DU_CYLINDER_NUMVERTEX, du_cylinder_lines, DU_CYLINDER_NUMLINES * 2);

	for (s32 i = 0; i < LINE_DIVISION; i++)
	{
		f32 angle = M_PI * 2.f * (i / (f32)LINE_DIVISION);
		f32 _sa = _sin(angle);
		f32 _ca = _cos(angle);
		circledef1[i].x = _ca;
		circledef1[i].y = _sa;
		circledef1[i].z = 0;
		circledef2[i].x = 0;
		circledef2[i].y = _ca;
		circledef2[i].z = _sa;
		circledef3[i].x = _sa;
		circledef3[i].y = 0;
		circledef3[i].z = _ca;
	}

	// initialize identity box
	fBox3 bb;
	bb.set(-0.505f, -0.505f, -0.505f, 0.505f, 0.505f, 0.505f);
	for (i = 0; i < 8; i++)
	{
		fVector3 S;
		fVector3 p;
		bb.getpoint(i, p);
		S.set((f32)SIGN(p.x), (f32)SIGN(p.y), (f32)SIGN(p.z));
		boxvert[i * 6 + 0].set(p);
		boxvert[i * 6 + 1].set(p.x - S.x * 0.25f, p.y, p.z);
		boxvert[i * 6 + 2].set(p);
		boxvert[i * 6 + 3].set(p.x, p.y - S.y * 0.25f, p.z);
		boxvert[i * 6 + 4].set(p);
		boxvert[i * 6 + 5].set(p.x, p.y, p.z - S.z * 0.25f);
	}

	// create render stream
	vs_L.create(FVF::F_L, RCache.Vertex.Buffer( ), RCache.Index.Buffer( ));
	vs_TL.create(FVF::F_TL, RCache.Vertex.Buffer( ), RCache.Index.Buffer( ));
	vs_LIT.create(FVF::F_LIT, RCache.Vertex.Buffer( ), RCache.Index.Buffer( ));

	m_Font = xr_new<CGameFont>("hud_font_small");
}

void CDrawUtilities::OnDeviceDestroy( )
{
	Device.seqRender.Remove(this);
	xr_delete(m_Font);
	m_SolidBox.Destroy( );
	m_SolidCone.Destroy( );
	m_SolidSphere.Destroy( );
	m_SolidSpherePart.Destroy( );
	m_SolidCylinder.Destroy( );
	m_WireBox.Destroy( );
	m_WireCone.Destroy( );
	m_WireSphere.Destroy( );
	m_WireSpherePart.Destroy( );
	m_WireCylinder.Destroy( );

	vs_L.destroy( );
	vs_TL.destroy( );
	vs_LIT.destroy( );
}
//----------------

void CDrawUtilities::DrawSpotLight(const fVector3& p, const fVector3& d, f32 range, f32 phi, u32 clr)
{
	fMatrix4x4 T;
	fVector3 p1;
	f32 H;
	f32 P;
	f32 da = PI_MUL_2 / LINE_DIVISION;
	f32 b = range * _cos(PI_DIV_2 - phi / 2);
	f32 a = range * _sin(PI_DIV_2 - phi / 2);
	d.getHP(H, P);
	T.setHPB(H, P, 0);
	T.translate_over(p);
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	FVF::L* pv = (FVF::L*)Stream->Lock(LINE_DIVISION * 2 + 2, vs_L->vb_stride, vBase);
	for (f32 angle = 0; angle < PI_MUL_2; angle += da)
	{
		f32 _sa = _sin(angle);
		f32 _ca = _cos(angle);
		p1.x = b * _ca;
		p1.y = b * _sa;
		p1.z = a;
		T.transform_tiny(p1);
		// fill VB
		pv->set(p, clr);
		pv++;
		pv->set(p1, clr);
		pv++;
	}

	p1.mad(p, d, range);
	pv->set(p, clr);
	pv++;
	pv->set(p1, clr);
	pv++;
	Stream->Unlock(LINE_DIVISION * 2 + 2, vs_L->vb_stride);
	// and Render it as triangle list
	DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, LINE_DIVISION + 1);
}

void CDrawUtilities::DrawDirectionalLight(const fVector3& p, const fVector3& d, f32 radius, f32 range, u32 c)
{
	f32 r = radius * 0.71f;
	fVector3 R;
	fVector3 N;
	fVector3 D;
	D.normalize(d);
	fMatrix4x4 rot;

	N.set(0.0f, 1.0f, 0.0f);
	if (_abs(D.y) > 0.99f)
	{
		N.set(1.0f, 0.0f, 0.0f);
	}

	R.crossproduct(N, D);
	R.normalize( );
	N.crossproduct(D, R);
	N.normalize( );
	rot.set(R, N, D, p);
	f32 sz = radius + range;

	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	FVF::L* pv = (FVF::L*)Stream->Lock(6, vs_L->vb_stride, vBase);
	pv->set(0.0f, 0.0f, r, c);
	rot.transform_tiny(pv->p);
	pv++;
	pv->set(0.0f, 0.0f, sz, c);
	rot.transform_tiny(pv->p);
	pv++;
	pv->set(-r, 0.0f, r, c);
	rot.transform_tiny(pv->p);
	pv++;
	pv->set(-r, 0.0f, sz, c);
	rot.transform_tiny(pv->p);
	pv++;
	pv->set(r, 0.0f, r, c);
	rot.transform_tiny(pv->p);
	pv++;
	pv->set(r, 0.0f, sz, c);
	rot.transform_tiny(pv->p);
	pv++;
	Stream->Unlock(6, vs_L->vb_stride);

	// and Render it as triangle list
	DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, 3);

	fBox3 b;
	b.min.set(-r, -r, -r);
	b.max.set(r, r, r);

	DrawLineSphere(p, radius, c, true);
}

void CDrawUtilities::DrawPointLight(const fVector3& p, f32 radius, u32 c)
{
	RCache.set_xform_world(Fidentity);
	DrawCross(p, radius, radius, radius, radius, radius, radius, c, true);
}

void CDrawUtilities::DrawEntity(u32 clr, ref_shader s)
{
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	FVF::L* pv = (FVF::L*)Stream->Lock(5, vs_L->vb_stride, vBase);
	pv->set(0.0f, 0.0f, 0.0f, clr);
	pv++;
	pv->set(0.0f, 1.0f, 0.0f, clr);
	pv++;
	pv->set(0.0f, 1.0f, 0.5f, clr);
	pv++;
	pv->set(0.0f, 0.5f, 0.5f, clr);
	pv++;
	pv->set(0.0f, 0.5f, 0.0f, clr);
	pv++;
	Stream->Unlock(5, vs_L->vb_stride);
	// render flagshtok
	DU_DRAW_SH(Device.m_WireShader);
	DU_DRAW_DP(D3DPT_LINESTRIP, vs_L, vBase, 4);

	if (s) DU_DRAW_SH(s);
	{
		// fill VB
		FVF::LIT* pv = (FVF::LIT*)Stream->Lock(6, vs_LIT->vb_stride, vBase);
		pv->set(0.0f, 1.0f, 0.0f, clr, 0.0f, 0.0f);
		pv++;
		pv->set(0.0f, 1.0f, 0.5f, clr, 1.0f, 0.0f);
		pv++;
		pv->set(0.0f, 0.5f, 0.5f, clr, 1.0f, 1.0f);
		pv++;
		pv->set(0.0f, 0.5f, 0.0f, clr, 0.0f, 1.0f);
		pv++;
		pv->set(0.0f, 0.5f, 0.5f, clr, 1.0f, 1.0f);
		pv++;
		pv->set(0.0f, 1.0f, 0.5f, clr, 1.0f, 0.0f);
		pv++;
		Stream->Unlock(6, vs_LIT->vb_stride);
		// and Render it as line list
		DU_DRAW_DP(D3DPT_TRIANGLEFAN, vs_LIT, vBase, 4);
	}
}

void CDrawUtilities::DrawFlag(const fVector3& p, f32 heading, f32 height, f32 sz, f32 sz_fl, u32 clr, BOOL bDrawEntity)
{
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	FVF::L* pv = (FVF::L*)Stream->Lock(2, vs_L->vb_stride, vBase);
	pv->set(p, clr);
	pv++;
	pv->set(p.x, p.y + height, p.z, clr);
	pv++;
	Stream->Unlock(2, vs_L->vb_stride);
	// and Render it as triangle list
	DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, 1);

	if (bDrawEntity)
	{
		// fill VB
		f32 rx = _sin(heading);
		f32 rz = _cos(heading);
		FVF::L* pv = (FVF::L*)Stream->Lock(6, vs_L->vb_stride, vBase);
		sz *= 0.8f;
		pv->set(p.x, p.y + height, p.z, clr);
		pv++;
		pv->set(p.x + rx * sz, p.y + height, p.z + rz * sz, clr);
		pv++;
		sz *= 0.5f;
		pv->set(p.x, p.y + height * (1.0f - sz_fl * 0.5f), p.z, clr);
		pv++;
		pv->set(p.x + rx * sz * 0.6f, p.y + height * (1.0f - sz_fl * 0.5f), p.z + rz * sz * 0.75f, clr);
		pv++;
		pv->set(p.x, p.y + height * (1.0f - sz_fl), p.z, clr);
		pv++;
		pv->set(p.x + rx * sz, p.y + height * (1.0f - sz_fl), p.z + rz * sz, clr);
		pv++;
		Stream->Unlock(6, vs_L->vb_stride);
		// and Render it as line list
		DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, 3);
	}
	else
	{
		// fill VB
		FVF::L* pv = (FVF::L*)Stream->Lock(6, vs_L->vb_stride, vBase);
		pv->set(p.x, p.y + height * (1.0f - sz_fl), p.z, clr);
		pv++;
		pv->set(p.x, p.y + height, p.z, clr);
		pv++;
		pv->set(p.x + _sin(heading) * sz, ((pv - 2)->p.y + (pv - 1)->p.y) / 2, p.z + _cos(heading) * sz, clr);
		pv++;
		pv->set(*(pv - 3));
		pv++;
		pv->set(*(pv - 2));
		pv++;
		pv->set(*(pv - 4));
		pv++;
		Stream->Unlock(6, vs_L->vb_stride);
		// and Render it as triangle list
		DU_DRAW_DP(D3DPT_TRIANGLELIST, vs_L, vBase, 2);
	}
}

//------------------------------------------------------------------------------
void CDrawUtilities::DrawRomboid(const fVector3& p, f32 r, u32 c)
{
	static const WORD IL[24] = { 0,2, 2,5, 0,5, 3,5, 3,0, 4,3, 4,0, 4,2, 1,2, 1,5, 1,3, 1,4 };
	static const WORD IT[24] = { 2,4,0, 4,3,0, 3,5,0, 5,2,0, 4,2,1, 2,5,1, 5,3,1, 3,4,1 };
	u32 vBase;
	u32 iBase;

	fColor C;
	C.set(c);
	C.mul_rgb(0.75f);
	u32 c1 = C.get( );

	s32 k;
	FVF::L* pv;
	WORD* i;
	_VertexStream* Stream = &RCache.Vertex;
	_IndexStream* StreamI = &RCache.Index;

	// fill VB
	pv = (FVF::L*)Stream->Lock(6, vs_L->vb_stride, vBase);
	pv->set(p.x, p.y + r, p.z, c1);
	pv++;
	pv->set(p.x, p.y - r, p.z, c1);
	pv++;
	pv->set(p.x, p.y, p.z - r, c1);
	pv++;
	pv->set(p.x, p.y, p.z + r, c1);
	pv++;
	pv->set(p.x - r, p.y, p.z, c1);
	pv++;
	pv->set(p.x + r, p.y, p.z, c1);
	pv++;
	Stream->Unlock(6, vs_L->vb_stride);

	i = StreamI->Lock(24, iBase);
	for (k = 0; k < 24; k++, i++)
	{
		*i = IT[k];
	}

	StreamI->Unlock(24);

	// and Render it as triangle list
	DU_DRAW_DIP(D3DPT_TRIANGLELIST, vs_L, vBase, 0, 6, iBase, 12);

	// draw lines
	pv = (FVF::L*)Stream->Lock(6, vs_L->vb_stride, vBase);
	pv->set(p.x, p.y + r, p.z, c);
	pv++;
	pv->set(p.x, p.y - r, p.z, c);
	pv++;
	pv->set(p.x, p.y, p.z - r, c);
	pv++;
	pv->set(p.x, p.y, p.z + r, c);
	pv++;
	pv->set(p.x - r, p.y, p.z, c);
	pv++;
	pv->set(p.x + r, p.y, p.z, c);
	pv++;
	Stream->Unlock(6, vs_L->vb_stride);

	i = StreamI->Lock(24, iBase);
	for (k = 0; k < 24; k++, i++)
	{
		*i = IL[k];
	}

	StreamI->Unlock(24);

	DU_DRAW_DIP(D3DPT_LINELIST, vs_L, vBase, 0, 6, iBase, 12);
}
//------------------------------------------------------------------------------

void CDrawUtilities::DrawSound(const fVector3& p, f32 r, u32 c)
{
	DrawCross(p, r, r, r, r, r, r, c, true);
}
//------------------------------------------------------------------------------
void CDrawUtilities::DrawIdentCone(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
	if (bWire)
	{
		DU_DRAW_SH_C(Device.m_WireShader, clr_w);
		m_WireCone.Render( );
	}

	if (bSolid)
	{
		DU_DRAW_SH_C(color_get_A(clr_s) >= 254 ? Device.m_WireShader : Device.m_SelectionShader, clr_s);
		m_SolidCone.Render( );
	}

	DU_DRAW_RS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawIdentSphere(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
	if (bWire)
	{
		DU_DRAW_SH_C(Device.m_WireShader, clr_w);
		m_WireSphere.Render( );
	}

	if (bSolid)
	{
		DU_DRAW_SH_C(color_get_A(clr_s) >= 254 ? Device.m_WireShader : Device.m_SelectionShader, clr_s);
		m_SolidSphere.Render( );
	}

	DU_DRAW_RS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawIdentSpherePart(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
	if (bWire)
	{
		DU_DRAW_SH_C(Device.m_WireShader, clr_w);
		m_WireSpherePart.Render( );
	}

	if (bSolid)
	{
		DU_DRAW_SH_C(color_get_A(clr_s) >= 254 ? Device.m_WireShader : Device.m_SelectionShader, clr_s);
		m_SolidSpherePart.Render( );
	}

	DU_DRAW_RS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawIdentCylinder(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
	if (bWire)
	{
		DU_DRAW_SH_C(Device.m_WireShader, clr_w);
		m_WireCylinder.Render( );
	}

	if (bSolid)
	{
		DU_DRAW_SH_C(color_get_A(clr_s) >= 254 ? Device.m_WireShader : Device.m_SelectionShader, clr_s);
		m_SolidCylinder.Render( );
	}

	DU_DRAW_RS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawIdentBox(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
	if (bWire)
	{
		DU_DRAW_SH_C(Device.m_WireShader, clr_w);
		m_WireBox.Render( );
	}

	if (bSolid)
	{
		DU_DRAW_SH_C(color_get_A(clr_s) >= 254 ? Device.m_WireShader : Device.m_SelectionShader, clr_s);
		m_SolidBox.Render( );
	}

	DU_DRAW_RS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawLineSphere(const fVector3& p, f32 radius, u32 c, BOOL bCross)
{
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	s32 i;
	FVF::L* pv;
	// seg 0
	pv = (FVF::L*)Stream->Lock(LINE_DIVISION + 1, vs_L->vb_stride, vBase);
	for (i = 0; i < LINE_DIVISION; i++, pv++)
	{
		pv->p.mad(p, circledef1[i], radius);
		pv->color = c;
	}

	pv->set(*(pv - LINE_DIVISION));
	Stream->Unlock(LINE_DIVISION + 1, vs_L->vb_stride);
	DU_DRAW_DP(D3DPT_LINESTRIP, vs_L, vBase, LINE_DIVISION);
	// seg 1
	pv = (FVF::L*)Stream->Lock(LINE_DIVISION + 1, vs_L->vb_stride, vBase);
	for (i = 0; i < LINE_DIVISION; i++)
	{
		pv->p.mad(p, circledef2[i], radius);
		pv->color = c;
		pv++;
	}

	pv->set(*(pv - LINE_DIVISION));
	pv++;
	Stream->Unlock(LINE_DIVISION + 1, vs_L->vb_stride);
	DU_DRAW_DP(D3DPT_LINESTRIP, vs_L, vBase, LINE_DIVISION);
	// seg 2
	pv = (FVF::L*)Stream->Lock(LINE_DIVISION + 1, vs_L->vb_stride, vBase);
	for (i = 0; i < LINE_DIVISION; i++)
	{
		pv->p.mad(p, circledef3[i], radius);
		pv->color = c;
		pv++;
	}

	pv->set(*(pv - LINE_DIVISION));
	pv++;
	Stream->Unlock(LINE_DIVISION + 1, vs_L->vb_stride);
	DU_DRAW_DP(D3DPT_LINESTRIP, vs_L, vBase, LINE_DIVISION);

	if (bCross)
	{
		DrawCross(p, radius, radius, radius, radius, radius, radius, c);
	}
}

//----------------------------------------------------
IC f32 _x2real(f32 x)
{
	return ((x + 1) * Device.dwWidth * 0.5f);
}
IC f32 _y2real(f32 y)
{
	return ((y + 1) * Device.dwHeight * 0.5f);
}

void CDrawUtilities::dbgDrawPlacement(const fVector3& p, s32 sz, u32 clr, pcstr caption, u32 clr_font)
{
	VERIFY(Device.b_is_Ready);
	fVector3 c;
	f32 w = p.x * Device.mFullTransform._14 + p.y * Device.mFullTransform._24 + p.z * Device.mFullTransform._34 + Device.mFullTransform._44;
	if (w < 0)
	{
		// culling
		return;
	}

	f32 s = (f32)sz;
	Device.mFullTransform.transform(c, p);
	c.x = (f32)iFloor(_x2real(c.x));
	c.y = (f32)iFloor(_y2real(-c.y));

	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	FVF::TL* pv = (FVF::TL*)Stream->Lock(5, vs_TL->vb_stride, vBase);
	pv->p.set(c.x - s, c.y - s, 0.0f, 1.0f);
	pv->color = clr;
	pv++;
	pv->p.set(c.x + s, c.y - s, 0.0f, 1.0f);
	pv->color = clr;
	pv++;
	pv->p.set(c.x + s, c.y + s, 0.0f, 1.0f);
	pv->color = clr;
	pv++;
	pv->p.set(c.x - s, c.y + s, 0.0f, 1.0f);
	pv->color = clr;
	pv++;
	pv->p.set(c.x - s, c.y - s, 0.0f, 1.0f);
	pv->color = clr;
	pv++;
	Stream->Unlock(5, vs_TL->vb_stride);

	// Render it as line strip
	DU_DRAW_DP(D3DPT_LINESTRIP, vs_TL, vBase, 4);
	if (caption)
	{
		m_Font->SetColor(clr_font);
		m_Font->Out(c.x, c.y + s, "%s", caption);
	}
}

void CDrawUtilities::dbgDrawVert(const fVector3& p0, u32 clr, pcstr caption)
{
	dbgDrawPlacement(p0, 1, clr, caption);
	DrawCross(p0, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
}

void CDrawUtilities::dbgDrawEdge(const fVector3& p0, const fVector3& p1, u32 clr, pcstr caption)
{
	dbgDrawPlacement(p0, 1, clr, caption);
	DrawCross(p0, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
	DrawCross(p1, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
	DrawLine(p0, p1, clr);
}

void CDrawUtilities::dbgDrawFace(const fVector3& p0, const fVector3& p1, const fVector3& p2, u32 clr, pcstr caption)
{
	dbgDrawPlacement(p0, 1, clr, caption);
	DrawCross(p0, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
	DrawCross(p1, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
	DrawCross(p2, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
	DrawLine(p0, p1, clr);
	DrawLine(p1, p2, clr);
	DrawLine(p2, p0, clr);
}
//----------------------------------------------------

void CDrawUtilities::DrawLine(const fVector3& p0, const fVector3& p1, u32 c)
{
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	FVF::L* pv = (FVF::L*)Stream->Lock(2, vs_L->vb_stride, vBase);
	pv->set(p0, c);
	pv++;
	pv->set(p1, c);
	pv++;
	Stream->Unlock(2, vs_L->vb_stride);
	// and Render it as triangle list
	DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, 1);
}

//----------------------------------------------------
void CDrawUtilities::DrawSelectionBox(const fVector3& C, const fVector3& S, u32* c)
{
	u32 cc = (c) ? *c : boxcolor;

	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	FVF::L* pv = (FVF::L*)Stream->Lock(boxvertcount, vs_L->vb_stride, vBase);
	for (s32 i = 0; i < boxvertcount; i++, pv++)
	{
		pv->p.mul(boxvert[i], S);
		pv->p.add(C);
		pv->color = cc;
	}

	Stream->Unlock(boxvertcount, vs_L->vb_stride);

	// and Render it as triangle list
	DU_DRAW_RS(D3DRS_FILLMODE, D3DFILL_SOLID);
	DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, boxvertcount / 2);
	DU_DRAW_RS(D3DRS_FILLMODE, FILL_MODE);
}

void CDrawUtilities::DrawBox(const fVector3& offs, const fVector3& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
	_VertexStream* Stream = &RCache.Vertex;
	if (bWire)
	{
		u32 vBase;
		FVF::L* pv = (FVF::L*)Stream->Lock(identboxwirecount, vs_L->vb_stride, vBase);
		for (s32 i = 0; i < identboxwirecount; i++, pv++)
		{
			pv->p.mul(identboxwire[i], Size);
			pv->p.mul(2);
			pv->p.add(offs);
			pv->color = clr_w;
		}

		Stream->Unlock(identboxwirecount, vs_L->vb_stride);

		DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, identboxwirecount / 2);
	}

	if (bSolid)
	{
		u32 vBase;
		FVF::L* pv = (FVF::L*)Stream->Lock(DU_BOX_NUMVERTEX2, vs_L->vb_stride, vBase);
		for (s32 i = 0; i < DU_BOX_NUMVERTEX2; i++, pv++)
		{
			pv->p.mul(du_box_vertices2[i], Size);
			pv->p.mul(2);
			pv->p.add(offs);
			pv->color = clr_s;
		}

		Stream->Unlock(DU_BOX_NUMVERTEX2, vs_L->vb_stride);

		DU_DRAW_DP(D3DPT_TRIANGLELIST, vs_L, vBase, DU_BOX_NUMFACES);
	}
}
//----------------------------------------------------

void CDrawUtilities::DrawOBB(const fMatrix4x4& parent, const fObb& box, u32 clr_s, u32 clr_w)
{
	fMatrix4x4 R;
	fMatrix4x4 S;
	fMatrix4x4 X;
	box.xform_get(R);
	S.scale(box.m_halfsize.x * 2.0f, box.m_halfsize.y * 2.0f, box.m_halfsize.z * 2.0f);
	X.mul_43(R, S);
	R.mul_43(parent, X);
	RCache.set_xform_world(R);
	DrawIdentBox(true, true, clr_s, clr_w);
}
//----------------------------------------------------

void CDrawUtilities::DrawAABB(const fMatrix4x4& parent, const fVector3& center, const fVector3& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
	fMatrix4x4 R;
	fMatrix4x4 S;
	S.scale(size.x * 2.0f, size.y * 2.0f, size.z * 2.0f);
	S.translate_over(center);
	R.mul_43(parent, S);
	RCache.set_xform_world(R);
	DrawIdentBox(bSolid, bWire, clr_s, clr_w);
}

void CDrawUtilities::DrawAABB(const fVector3& p0, const fVector3& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
	fMatrix4x4 R;
	fVector3 C;
	C.set((p1.x + p0.x) * 0.5f, (p1.y + p0.y) * 0.5f, (p1.z + p0.z) * 0.5f);
	R.scale(_abs(p1.x - p0.x), _abs(p1.y - p0.y), _abs(p1.z - p0.z));
	R.translate_over(C);
	RCache.set_xform_world(R);
	DrawIdentBox(bSolid, bWire, clr_s, clr_w);
}

void CDrawUtilities::DrawSphere(const fMatrix4x4& parent, const fVector3& center, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
	fMatrix4x4 B;
	B.scale(radius, radius, radius);
	B.translate_over(center);
	B.mulA_43(parent);
	RCache.set_xform_world(B);
	DrawIdentSphere(bSolid, bWire, clr_s, clr_w);
}
//----------------------------------------------------

void CDrawUtilities::DrawFace(const fVector3& p0, const fVector3& p1, const fVector3& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
	_VertexStream* Stream = &RCache.Vertex;

	u32 vBase;
	if (bSolid)
	{
		FVF::L* pv = (FVF::L*)Stream->Lock(3, vs_L->vb_stride, vBase);
		pv->set(p0, clr_s);
		pv++;
		pv->set(p1, clr_s);
		pv++;
		pv->set(p2, clr_s);
		pv++;
		Stream->Unlock(3, vs_L->vb_stride);
		DU_DRAW_DP(D3DPT_TRIANGLELIST, vs_L, vBase, 1);
	}

	if (bWire)
	{
		FVF::L* pv = (FVF::L*)Stream->Lock(4, vs_L->vb_stride, vBase);
		pv->set(p0, clr_w);
		pv++;
		pv->set(p1, clr_w);
		pv++;
		pv->set(p2, clr_w);
		pv++;
		pv->set(p0, clr_w);
		pv++;
		Stream->Unlock(4, vs_L->vb_stride);
		DU_DRAW_DP(D3DPT_LINESTRIP, vs_L, vBase, 3);
	}
}
//----------------------------------------------------

static const u32 MAX_VERT_COUNT = 0xFFFF;
void CDrawUtilities::DD_DrawFace_begin(BOOL bWire)
{
	VERIFY(m_DD_pv_start == 0);
	m_DD_wire = bWire;
	m_DD_pv_start = (FVF::L*)RCache.Vertex.Lock(MAX_VERT_COUNT, vs_L->vb_stride, m_DD_base);
	m_DD_pv = m_DD_pv_start;
}
void CDrawUtilities::DD_DrawFace_flush(BOOL try_again)
{
	RCache.Vertex.Unlock((u32)(m_DD_pv - m_DD_pv_start), vs_L->vb_stride);
	if (m_DD_wire)
	{
		DU_DRAW_RS(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	DU_DRAW_DP(D3DPT_TRIANGLELIST, vs_L, m_DD_base, u32(m_DD_pv - m_DD_pv_start) / 3);
	if (m_DD_wire)
	{
		DU_DRAW_RS(D3DRS_FILLMODE, FILL_MODE);
	}

	if (try_again)
	{
		m_DD_pv_start = (FVF::L*)RCache.Vertex.Lock(MAX_VERT_COUNT, vs_L->vb_stride, m_DD_base);
		m_DD_pv = m_DD_pv_start;
	}
}
void CDrawUtilities::DD_DrawFace_push(const fVector3& p0, const fVector3& p1, const fVector3& p2, u32 clr)
{
	m_DD_pv->set(p0, clr); m_DD_pv++;
	m_DD_pv->set(p1, clr); m_DD_pv++;
	m_DD_pv->set(p2, clr); m_DD_pv++;
	if (m_DD_pv - m_DD_pv_start == MAX_VERT_COUNT)
	{
		DD_DrawFace_flush(TRUE);
	}
}
void CDrawUtilities::DD_DrawFace_end( )
{
	DD_DrawFace_flush(FALSE);
	m_DD_pv_start = 0;
}
//----------------------------------------------------

void CDrawUtilities::DrawCylinder(const fMatrix4x4& parent, const fVector3& center, const fVector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
	fMatrix4x4 mScale;
	mScale.scale(2.0f * radius, 2.0f * radius, height);

	// build final rotation / translation
	fVector3 L_dir;
	fVector3 L_up;
	fVector3 L_right;
	L_dir.set(dir);
	L_dir.normalize( );
	L_up.set(0.0f, 1.0f, 0.0f);
	if (_abs(L_up.dotproduct(L_dir)) > 0.99f)
	{
		L_up.set(0.0f, 0.0f, 1.0f);
	}

	L_right.crossproduct(L_up, L_dir);
	L_right.normalize( );
	L_up.crossproduct(L_dir, L_right);
	L_up.normalize( );

	fMatrix4x4         	mR;
	mR.i = L_right;
	mR._14 = 0;
	mR.j = L_up;
	mR._24 = 0;
	mR.k = L_dir;
	mR._34 = 0;
	mR.c = center;
	mR._44 = 1;

	// final xform
	fMatrix4x4 xf;
	xf.mul(mR, mScale);
	xf.mulA_43(parent);
	RCache.set_xform_world(xf);
	DrawIdentCylinder(bSolid, bWire, clr_s, clr_w);
}
//----------------------------------------------------

void CDrawUtilities::DrawCone(const fMatrix4x4& parent, const fVector3& apex, const fVector3& dir, f32 height, f32 radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
	fMatrix4x4 mScale;
	mScale.scale(2.0f * radius, 2.0f * radius, height);

	// build final rotation / translation
	fVector3 L_dir;
	fVector3 L_up;
	fVector3 L_right;
	L_dir.set(dir);
	L_dir.normalize( );
	L_up.set(0.0f, 1.0f, 0.0f);
	if (_abs(L_up.dotproduct(L_dir)) > 0.99f)
	{
		L_up.set(0.0f, 0.0f, 1.0f);
	}

	L_right.crossproduct(L_up, L_dir);
	L_right.normalize( );
	L_up.crossproduct(L_dir, L_right);
	L_up.normalize( );

	fMatrix4x4 mR;
	mR.i = L_right;
	mR._14 = 0;
	mR.j = L_up;
	mR._24 = 0;
	mR.k = L_dir;
	mR._34 = 0;
	mR.c = apex;
	mR._44 = 1;

	// final xform
	fMatrix4x4 xf;
	xf.mul(mR, mScale);
	xf.mulA_43(parent);
	RCache.set_xform_world(xf);
	DrawIdentCone(bSolid, bWire, clr_s, clr_w);
}
//----------------------------------------------------

void CDrawUtilities::DrawPlane(const fVector3& p, const fVector3& n, const fVector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire)
{
	if (n.square_magnitude( ) < EPSILON_7)
	{
		return;
	}

	// build final rotation / translation
	fVector3 L_dir;
	fVector3 L_up = n;
	fVector3 L_right;
	L_dir.set(0.0f, 0.0f, 1.0f);
	if (_abs(L_up.dotproduct(L_dir)) > 0.99f)
	{
		L_dir.set(1.0f, 0.0f, 0.0f);
	}

	L_right.crossproduct(L_up, L_dir);
	L_right.normalize( );
	L_dir.crossproduct(L_right, L_up);
	L_dir.normalize( );

	fMatrix4x4 mR;
	mR.i = L_right;
	mR._14 = 0;
	mR.j = L_up;
	mR._24 = 0;
	mR.k = L_dir;
	mR._34 = 0;
	mR.c = p;
	mR._44 = 1;

	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;

	if (bSolid)
	{
		DU_DRAW_SH(Device.m_SelectionShader);
		FVF::L* pv = (FVF::L*)Stream->Lock(5, vs_L->vb_stride, vBase);
		pv->set(-scale.x, 0.0f, -scale.y, clr_s);
		mR.transform_tiny(pv->p);
		pv++;
		pv->set(-scale.x, 0.0f, +scale.y, clr_s);
		mR.transform_tiny(pv->p);
		pv++;
		pv->set(+scale.x, 0.0f, +scale.y, clr_s);
		mR.transform_tiny(pv->p);
		pv++;
		pv->set(+scale.x, 0.0f, -scale.y, clr_s);
		mR.transform_tiny(pv->p);
		pv++;
		pv->set(*(pv - 4));
		Stream->Unlock(5, vs_L->vb_stride);
		if (!bCull)
		{
			DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_NONE);
		}

		DU_DRAW_DP(D3DPT_TRIANGLEFAN, vs_L, vBase, 2);
		if (!bCull)
		{
			DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_CCW);
		}
	}

	if (bWire)
	{
		DU_DRAW_SH(Device.m_WireShader);
		FVF::L* pv = (FVF::L*)Stream->Lock(5, vs_L->vb_stride, vBase);
		pv->set(-scale.x, 0.0f, -scale.y, clr_w);
		mR.transform_tiny(pv->p);
		pv++;
		pv->set(+scale.x, 0.0f, -scale.y, clr_w);
		mR.transform_tiny(pv->p);
		pv++;
		pv->set(+scale.x, 0.0f, +scale.y, clr_w);
		mR.transform_tiny(pv->p);
		pv++;
		pv->set(-scale.x, 0.0f, +scale.y, clr_w);
		mR.transform_tiny(pv->p);
		pv++;
		pv->set(*(pv - 4));
		Stream->Unlock(5, vs_L->vb_stride);
		DU_DRAW_DP(D3DPT_LINESTRIP, vs_L, vBase, 4);
	}
}
//----------------------------------------------------

void CDrawUtilities::DrawPlane(const fVector3& center, const fVector2& scale, const fVector3& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire)
{
	fMatrix4x4 M;
	M.setHPB(rotate.y, rotate.x, rotate.z);
	M.translate_over(center);
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	if (bSolid)
	{
		DU_DRAW_SH(Device.m_SelectionShader);
		FVF::L* pv = (FVF::L*)Stream->Lock(5, vs_L->vb_stride, vBase);
		pv->set(-scale.x, 0.0f, -scale.y, clr_s);
		M.transform_tiny(pv->p);
		pv++;
		pv->set(-scale.x, 0.0f, +scale.y, clr_s);
		M.transform_tiny(pv->p);
		pv++;
		pv->set(+scale.x, 0.0f, +scale.y, clr_s);
		M.transform_tiny(pv->p);
		pv++;
		pv->set(+scale.x, 0.0f, -scale.y, clr_s);
		M.transform_tiny(pv->p);
		pv++;
		pv->set(*(pv - 4));
		Stream->Unlock(5, vs_L->vb_stride);
		if (!bCull)
		{
			DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_NONE);
		}

		DU_DRAW_DP(D3DPT_TRIANGLEFAN, vs_L, vBase, 2);
		if (!bCull)
		{
			DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_CCW);
		}
	}

	if (bWire)
	{
		DU_DRAW_SH(Device.m_WireShader);
		FVF::L* pv = (FVF::L*)Stream->Lock(5, vs_L->vb_stride, vBase);
		pv->set(-scale.x, 0.0f, -scale.y, clr_w);
		M.transform_tiny(pv->p);
		pv++;
		pv->set(+scale.x, 0.0f, -scale.y, clr_w);
		M.transform_tiny(pv->p);
		pv++;
		pv->set(+scale.x, 0.0f, +scale.y, clr_w);
		M.transform_tiny(pv->p);
		pv++;
		pv->set(-scale.x, 0.0f, +scale.y, clr_w);
		M.transform_tiny(pv->p);
		pv++;
		pv->set(*(pv - 4));
		Stream->Unlock(5, vs_L->vb_stride);
		DU_DRAW_DP(D3DPT_LINESTRIP, vs_L, vBase, 4);
	}
}
//----------------------------------------------------

void CDrawUtilities::DrawRectangle(const fVector3& o, const fVector3& u, const fVector3& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
	_VertexStream* Stream = &RCache.Vertex;

	u32 vBase;
	if (bSolid)
	{
		DU_DRAW_SH(Device.m_SelectionShader);
		FVF::L* pv = (FVF::L*)Stream->Lock(6, vs_L->vb_stride, vBase);
		pv->set(o.x, o.y, o.z, clr_s);
		pv++;
		pv->set(o.x + u.x + v.x, o.y + u.y + v.y, o.z + u.z + v.z, clr_s);
		pv++;
		pv->set(o.x + v.x, o.y + v.y, o.z + v.z, clr_s);
		pv++;
		pv->set(o.x, o.y, o.z, clr_s);
		pv++;
		pv->set(o.x + u.x, o.y + u.y, o.z + u.z, clr_s);
		pv++;
		pv->set(o.x + u.x + v.x, o.y + u.y + v.y, o.z + u.z + v.z, clr_s);
		pv++;
		Stream->Unlock(6, vs_L->vb_stride);
		DU_DRAW_DP(D3DPT_TRIANGLELIST, vs_L, vBase, 2);
	}

	if (bWire)
	{
		DU_DRAW_SH(Device.m_WireShader);
		FVF::L* pv = (FVF::L*)Stream->Lock(5, vs_L->vb_stride, vBase);
		pv->set(o.x, o.y, o.z, clr_w);
		pv++;
		pv->set(o.x + u.x, o.y + u.y, o.z + u.z, clr_w);
		pv++;
		pv->set(o.x + u.x + v.x, o.y + u.y + v.y, o.z + u.z + v.z, clr_w);
		pv++;
		pv->set(o.x + v.x, o.y + v.y, o.z + v.z, clr_w);
		pv++;
		pv->set(o.x, o.y, o.z, clr_w);
		pv++;
		Stream->Unlock(5, vs_L->vb_stride);
		DU_DRAW_DP(D3DPT_LINESTRIP, vs_L, vBase, 4);
	}
}
//----------------------------------------------------

void CDrawUtilities::DrawCross(const fVector3& p, f32 szx1, f32 szy1, f32 szz1, f32 szx2, f32 szy2, f32 szz2, u32 clr, BOOL bRot45)
{
	_VertexStream* Stream = &RCache.Vertex;
	// actual rendering
	u32 vBase;
	FVF::L* pv = (FVF::L*)Stream->Lock(bRot45 ? 12 : 6, vs_L->vb_stride, vBase);
	pv->set(p.x + szx2, p.y, p.z, clr);
	pv++;
	pv->set(p.x - szx1, p.y, p.z, clr);
	pv++;
	pv->set(p.x, p.y + szy2, p.z, clr);
	pv++;
	pv->set(p.x, p.y - szy1, p.z, clr);
	pv++;
	pv->set(p.x, p.y, p.z + szz2, clr);
	pv++;
	pv->set(p.x, p.y, p.z - szz1, clr);
	pv++;
	if (bRot45)
	{
		fMatrix4x4 M;
		M.setHPB(PI_DIV_4, PI_DIV_4, PI_DIV_4);
		for (s32 i = 0; i < 6; i++, pv++)
		{
			pv->p.sub((pv - 6)->p, p);
			M.transform_dir(pv->p);
			pv->p.add(p);
			pv->color = clr;
		}
	}

	// unlock VB and Render it as triangle list
	Stream->Unlock(bRot45 ? 12 : 6, vs_L->vb_stride);
	DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, bRot45 ? 6 : 3);
}

void CDrawUtilities::DrawPivot(const fVector3& pos, f32 sz)
{
	DU_DRAW_SH(Device.m_WireShader);
	DrawCross(pos, sz, sz, sz, sz, sz, sz, 0xFF7FFF7F);
}

void CDrawUtilities::DrawAxis(const fMatrix4x4& T)
{
	_VertexStream* Stream = &RCache.Vertex;
	fVector3 p[6];
	u32 c[6];

	// colors
	c[0] = c[2] = c[4] = 0x00222222;
	c[1] = 0x00FF0000;
	c[3] = 0x0000FF00;
	c[5] = 0x000000FF;

	// position
	p[0].mad(T.c, T.k, 0.25f);
	p[1].set(p[0]);
	p[1].x += 0.015f;
	p[2].set(p[0]);
	p[3].set(p[0]);
	p[3].y += 0.015f;
	p[4].set(p[0]);
	p[5].set(p[0]); 
	p[5].z += 0.015f;

	u32 vBase;
	FVF::TL* pv = (FVF::TL*)Stream->Lock(6, vs_TL->vb_stride, vBase);
	// transform to screen
	f32 dx = -f32(Device.dwWidth) / 2.2f;
	f32 dy = f32(Device.dwHeight) / 2.25f;

	for (s32 i = 0; i < 6; i++, pv++)
	{
		pv->color = c[i];
		pv->transform(p[i], Device.mFullTransform);
		pv->p.set((f32)iFloor(_x2real(pv->p.x) + dx), (f32)iFloor(_y2real(pv->p.y) + dy), 0.0f, 1.0f);
		p[i].set(pv->p.x, pv->p.y, 0.0f);
	}

	// unlock VB and Render it as triangle list
	Stream->Unlock(6, vs_TL->vb_stride);
	DU_DRAW_RS(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	DU_DRAW_SH(Device.m_WireShader);
	DU_DRAW_DP(D3DPT_LINELIST, vs_TL, vBase, 3);
	DU_DRAW_RS(D3DRS_SHADEMODE, SHADE_MODE);

	m_Font->SetColor(0xFF909090);
	m_Font->Out(p[1].x, p[1].y, "x");
	m_Font->Out(p[3].x, p[3].y, "y");
	m_Font->Out(p[5].x, p[5].y, "z");
	m_Font->SetColor(0xFF000000);
	m_Font->Out(p[1].x - 1, p[1].y - 1, "x");
	m_Font->Out(p[3].x - 1, p[3].y - 1, "y");
	m_Font->Out(p[5].x - 1, p[5].y - 1, "z");
}

void CDrawUtilities::DrawObjectAxis(const fMatrix4x4& T, f32 sz, BOOL sel)
{
	VERIFY(Device.b_is_Ready);
	_VertexStream* Stream = &RCache.Vertex;
	fVector3 c;
	fVector3 r;
	fVector3 n;
	fVector3 d;
	f32 w = T.c.x * Device.mFullTransform._14 + T.c.y * Device.mFullTransform._24 + T.c.z * Device.mFullTransform._34 + Device.mFullTransform._44;
	if (w < 0)
	{
		// culling
		return;
	}

	f32 s = w * sz;
	Device.mFullTransform.transform(c, T.c);
	r.mul(T.i, s);
	r.add(T.c);
	Device.mFullTransform.transform(r);
	n.mul(T.j, s);
	n.add(T.c);
	Device.mFullTransform.transform(n);
	d.mul(T.k, s);
	d.add(T.c);
	Device.mFullTransform.transform(d);
	c.x = (f32)iFloor(_x2real(c.x));
	c.y = (f32)iFloor(_y2real(-c.y));
	r.x = (f32)iFloor(_x2real(r.x));
	r.y = (f32)iFloor(_y2real(-r.y));
	n.x = (f32)iFloor(_x2real(n.x));
	n.y = (f32)iFloor(_y2real(-n.y));
	d.x = (f32)iFloor(_x2real(d.x));
	d.y = (f32)iFloor(_y2real(-d.y));

	u32 vBase;
	FVF::TL* pv = (FVF::TL*)Stream->Lock(6, vs_TL->vb_stride, vBase);
	pv->set(c.x, c.y, 0.0f, 1.0f, 0xFF222222, 0.0f, 0.0f);
	pv++;
	pv->set(d.x, d.y, 0.0f, 1.0f, sel ? 0xFF0000FF : 0xFF000080, 0.0f, 0.0f);
	pv++;
	pv->set(c.x, c.y, 0.0f, 1.0f, 0xFF222222, 0.0f, 0.0f);
	pv++;
	pv->set(r.x, r.y, 0.0f, 1.0f, sel ? 0xFFFF0000 : 0xFF800000, 0.0f, 0.0f); 
	pv++;
	pv->set(c.x, c.y, 0.0f, 1.0f, 0xFF222222, 0.0f, 0.0f);
	pv++;
	pv->set(n.x, n.y, 0.0f, 1.0f, sel ? 0xFF00FF00 : 0xFF008000, 0.0f, 0.0f);
	Stream->Unlock(6, vs_TL->vb_stride);

	// Render it as line list
	DU_DRAW_RS(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	DU_DRAW_SH(Device.m_WireShader);
	DU_DRAW_DP(D3DPT_LINELIST, vs_TL, vBase, 3);
	DU_DRAW_RS(D3DRS_SHADEMODE, SHADE_MODE);

	m_Font->SetColor(sel ? 0xFF000000 : 0xFF909090);
	m_Font->Out(r.x, r.y, "x");
	m_Font->Out(n.x, n.y, "y");
	m_Font->Out(d.x, d.y, "z");
	m_Font->SetColor(sel ? 0xFFFFFFFF : 0xFF000000);
	m_Font->Out(r.x - 1, r.y - 1, "x");
	m_Font->Out(n.x - 1, n.y - 1, "y");
	m_Font->Out(d.x - 1, d.y - 1, "z");
}

void CDrawUtilities::DrawGrid( )
{
	VERIFY(Device.b_is_Ready);
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	// fill VB
	FVF::L* pv = (FVF::L*)Stream->Lock(m_GridPoints.size( ), vs_L->vb_stride, vBase);
	for (FLvertexIt v_it = m_GridPoints.begin( ); v_it != m_GridPoints.end( ); v_it++, pv++)
	{
		pv->set(*v_it);
	}

	Stream->Unlock(m_GridPoints.size( ), vs_L->vb_stride);
	// Render it as triangle list
	fMatrix4x4 ddd;
	ddd.identity( );
	RCache.set_xform_world(ddd);
	DU_DRAW_SH(Device.m_WireShader);
	DU_DRAW_DP(D3DPT_LINELIST, vs_L, vBase, m_GridPoints.size( ) / 2);
}

void CDrawUtilities::DrawSelectionRect(const iVector2& m_SelStart, const iVector2& m_SelEnd)
{
	VERIFY(Device.b_is_Ready);
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	FVF::TL* pv = (FVF::TL*)Stream->Lock(4, vs_TL->vb_stride, vBase);
	pv->set(m_SelStart.x * SCREEN_QUALITY, m_SelStart.y * SCREEN_QUALITY, m_SelectionRect, 0.0f, 0.0f);
	pv++;
	pv->set(m_SelStart.x * SCREEN_QUALITY, m_SelEnd.y * SCREEN_QUALITY, m_SelectionRect, 0.0f, 0.0f);
	pv++;
	pv->set(m_SelEnd.x * SCREEN_QUALITY, m_SelEnd.y * SCREEN_QUALITY, m_SelectionRect, 0.0f, 0.0f);
	pv++;
	pv->set(m_SelEnd.x * SCREEN_QUALITY, m_SelStart.y * SCREEN_QUALITY, m_SelectionRect, 0.0f, 0.0f);
	pv++;
	Stream->Unlock(4, vs_TL->vb_stride);
	// Render it as triangle list
	DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_NONE);
	DU_DRAW_SH(Device.m_SelectionShader);
	DU_DRAW_DP(D3DPT_TRIANGLEFAN, vs_TL, vBase, 2);
	DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDrawUtilities::DrawPrimitiveL(D3DPRIMITIVETYPE pt, u32 pc, fVector3* vertices, s32 vc, u32 color, BOOL bCull, BOOL bCycle)
{
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	u32 dwNeed = (bCycle) ? vc + 1 : vc;
	FVF::L* pv = (FVF::L*)Stream->Lock(dwNeed, vs_L->vb_stride, vBase);
	for (s32 k = 0; k < vc; k++, pv++)
	{
		pv->set(vertices[k], color);
	}

	if (bCycle)
	{
		pv->set(*(pv - vc));
	}

	Stream->Unlock(dwNeed, vs_L->vb_stride);

	if (!bCull)
	{
		DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_NONE);
	}

	DU_DRAW_DP(pt, vs_L, vBase, pc);
	if (!bCull)
	{
		DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}

void CDrawUtilities::DrawPrimitiveTL(D3DPRIMITIVETYPE pt, u32 pc, FVF::TL* vertices, s32 vc, BOOL bCull, BOOL bCycle)
{
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	u32 dwNeed = (bCycle) ? vc + 1 : vc;
	FVF::TL* pv = (FVF::TL*)Stream->Lock(dwNeed, vs_TL->vb_stride, vBase);
	for (s32 k = 0; k < vc; k++, pv++)
	{
		pv->set(vertices[k]);
	}

	if (bCycle)
	{
		pv->set(*(pv - vc));
	}

	Stream->Unlock(dwNeed, vs_TL->vb_stride);

	if (!bCull)
	{
		DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_NONE);
	}

	DU_DRAW_DP(pt, vs_TL, vBase, pc);
	if (!bCull)
	{
		DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}

void CDrawUtilities::DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, u32 pc, FVF::LIT* vertices, s32 vc, BOOL bCull, BOOL bCycle)
{
	// fill VB
	_VertexStream* Stream = &RCache.Vertex;
	u32 vBase;
	u32 dwNeed = (bCycle) ? vc + 1 : vc;
	FVF::LIT* pv = (FVF::LIT*)Stream->Lock(dwNeed, vs_LIT->vb_stride, vBase);
	for (s32 k = 0; k < vc; k++, pv++)
	{
		pv->set(vertices[k]);
	}

	if (bCycle)
	{
		pv->set(*(pv - vc));
	}

	Stream->Unlock(dwNeed, vs_LIT->vb_stride);

	if (!bCull)
	{
		DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_NONE);
	}

	DU_DRAW_DP(pt, vs_LIT, vBase, pc);
	if (!bCull)
	{
		DU_DRAW_RS(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}

void CDrawUtilities::DrawLink(const fVector3& p0, const fVector3& p1, f32 sz, u32 clr)
{
	DrawLine(p1, p0, clr);
	fVector3 pp[2];
	fVector3 D;
	fVector3 R;
	fVector3 N = { 0.0f, 1.0f, 0.0f };
	D.sub(p1, p0);
	D.normalize( );
	R.crossproduct(N, D);
	R.mul(0.5f);
	D.mul(2.0f);
	N.mul(0.5f);
	// LR
	pp[0].add(R, D);
	pp[0].mul(sz * -0.5f);
	pp[0].add(p1);
	R.invert( );
	pp[1].add(R, D);
	pp[1].mul(sz * -0.5f);
	pp[1].add(p1);
	DrawLine(p1, pp[0], clr);
	DrawLine(p1, pp[1], clr);
	// UB
	pp[0].add(N, D);
	pp[0].mul(sz * -0.5f);
	pp[0].add(p1);
	N.invert( );
	pp[1].add(N, D);
	pp[1].mul(sz * -0.5f);
	pp[1].add(p1);
	DrawLine(p1, pp[0], clr);
	DrawLine(p1, pp[1], clr);
}

void CDrawUtilities::DrawJoint(const fVector3& p, f32 radius, u32 clr)
{
	DrawLineSphere(p, radius, clr, false);
}

void CDrawUtilities::OnRender( )
{
	m_Font->OnRender( );
}

void CDrawUtilities::OutText(const fVector3& pos, pcstr text, u32 color, u32 shadow_color)
{
	fVector3 p;
	f32 w = pos.x * Device.mFullTransform._14 + pos.y * Device.mFullTransform._24 + pos.z * Device.mFullTransform._34 + Device.mFullTransform._44;
	if (w >= 0.0f)
	{
		Device.mFullTransform.transform(p, pos);
		p.x = (f32)iFloor(_x2real(p.x));
		p.y = (f32)iFloor(_y2real(-p.y));

		m_Font->SetColor(shadow_color);
		m_Font->Out(p.x, p.y, (pstr)text);
		m_Font->SetColor(color);
		m_Font->Out(p.x - 1, p.y - 1, (pstr)text);
	}
}

#include "stdafx.h"
#ifdef DEBUG
#include "physics.h"
#include "MathUtils.h"
#include "..\XR_3DA\StatGraph.h"
#include "PHDebug.h"
#include "PHObject.h"
#include "ExtendedGeom.h"
#include "Level.h"
#include "Hudmanager.h"

#include "DebugRenderer.h"

flags32		ph_dbg_draw_mask;
flags32		ph_dbg_draw_mask1;
bool		draw_frame = 0;

pcstr	dbg_trace_object = NULL;
string64 s_dbg_tsrace_obj;
u32	 	dbg_bodies_num = 0;
u32	 	dbg_joints_num = 0;
u32	 	dbg_islands_num = 0;
u32	 	dbg_contacts_num = 0;
u32	 	dbg_tries_num = 0;
u32	 	dbg_saved_tries_for_active_objects = 0;
u32	 	dbg_total_saved_tries = 0;
u32	 	dbg_reused_queries_per_step = 0;
u32	 	dbg_new_queries_per_step = 0;
f32	dbg_vel_collid_damage_to_display = 7.0f;

#ifdef DRAW_CONTACTS
CONTACT_VECTOR Contacts0;
CONTACT_VECTOR Contacts1;
#endif

PHOBJ_DBG_V	dbg_draw_objects0;
PHOBJ_DBG_V	dbg_draw_objects1;

PHABS_DBG_V	dbg_draw_abstruct0;
PHABS_DBG_V	dbg_draw_abstruct1;

PHABS_DBG_V	dbg_draw_cashed;
PHABS_DBG_V	dbg_draw_simple;

enum		EDBGPHDrawMode
{
	dmSecondaryThread,
	dmCashed,
	dmSimple
} dbg_ph_draw_mode = dmSecondaryThread;
u32			cash_draw_remove_time = u32(-1);

struct SPHDBGDrawTri :public SPHDBGDrawAbsract
{
	fVector3 v[3];
	u32		c;
	bool solid;
	SPHDBGDrawTri(CDB::RESULT* T, u32 ac)
	{
		v[0].set(T->verts[0]);
		v[1].set(T->verts[1]);
		v[2].set(T->verts[2]);
		c = ac;
		solid = false;
	}
	SPHDBGDrawTri(CDB::TRI* T, const fVector3* V_array, u32 ac)
	{

		v[0].set(V_array[T->verts[0]]);
		v[1].set(V_array[T->verts[1]]);
		v[2].set(V_array[T->verts[2]]);
		c = ac;
		solid = false;
	}
	SPHDBGDrawTri(const fVector3& v0, const fVector3& v1, const fVector3& v2, u32 ac, bool solid_)
	{
		v[0].set(v0); v[1].set(v1); v[2].set(v2);
		c = ac;
		solid = solid_;
	}
	virtual void render( )
	{
		if (solid)
		{
			RCache.dbg_DrawTRI(Fidentity, v[0], v[1], v[2], c);
			RCache.dbg_DrawTRI(Fidentity, v[2], v[1], v[0], c);
		}
		else
		{
			Level( ).debug_renderer( ).draw_line(Fidentity, v[0], v[1], c);
			Level( ).debug_renderer( ).draw_line(Fidentity, v[1], v[2], c);
			Level( ).debug_renderer( ).draw_line(Fidentity, v[2], v[0], c);
		}
	}
};

static void clear_vector(PHABS_DBG_V& v)
{
	PHABS_DBG_I i, e; i = v.begin( ); e = v.end( );
	for (; e != i; ++i)
	{
		xr_delete(*i);
	}
	v.clear( );
}

void DBG_DrawTri(CDB::RESULT* T, u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawTri>(T, c));
}

void DBG_DrawTri(CDB::TRI* T, const fVector3* V_verts, u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawTri>(T, V_verts, c));
}

struct SPHDBGDrawLine : public SPHDBGDrawAbsract
{
	fVector3 p[2];
	u32 c;
	SPHDBGDrawLine(const fVector3& p0, const fVector3& p1, u32 ca)
	{
		p[0].set(p0); p[1].set(p1); c = ca;
	}
	virtual void render( )
	{
		Level( ).debug_renderer( ).draw_line(Fidentity, p[0], p[1], c);
	}
};

void DBG_DrawLine(const fVector3& p0, const fVector3& p1, u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawLine>(p0, p1, c));
}

void DBG_DrawMatrix(const fMatrix4x4& m, f32 size, u8 a/* = 255*/)
{
	fVector3 to;
	to.add(m.c, fVector3( ).mul(m.i, size));
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawLine>(m.c, to, D3DCOLOR_XRGB(a, 0, 0)));
	to.add(m.c, fVector3( ).mul(m.j, size));
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawLine>(m.c, to, D3DCOLOR_XRGB(0, a, 0)));
	to.add(m.c, fVector3( ).mul(m.k, size));
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawLine>(m.c, to, D3DCOLOR_XRGB(0, 0, a)));
}

template<s32>
IC	void rotate(fMatrix4x4& m, f32 ang);

template<>
IC	void rotate<0>(fMatrix4x4& m, f32 ang)
{
	m.rotateX(ang);
}

template<>
IC	void rotate<1>(fMatrix4x4& m, f32 ang)
{
	m.rotateY(ang);
}

template<>
IC	void rotate<2>(fMatrix4x4& m, f32 ang)
{
	m.rotateZ(ang);
}

template<s32 ax>
void DBG_DrawRotation(f32 ang0, f32 ang1, const fMatrix4x4& m, const fVector3& l, f32 size, u32 ac, bool solid, u32 tessel)
{
	fVector3 from;
	from.set(m.c);
	fVector3 ln;
	ln.set(l);
	ln.mul(size);

	const f32 ftess = (f32)tessel;
	fMatrix4x4 mm;
	rotate<ax>(mm, ang0);
	mm.mulA_43(m);
	fMatrix4x4 r;
	rotate<ax>(r, (ang1 - ang0) / ftess);
	for (u32 i = 0; tessel > i; ++i)
	{
		fVector3 tmp;
		mm.transform_dir(tmp, ln);
		fVector3 to0;
		to0.add(from, tmp);
		mm.mulB_43(r);
		mm.transform_dir(tmp, ln);
		fVector3 to1;
		to1.add(from, tmp);
		DBG_DrawPHAbstruct(xr_new<SPHDBGDrawTri>(from, to0, to1, ac, solid));
	}
}

void	DBG_DrawRotationX(const fMatrix4x4& m, f32 ang0, f32 ang1, f32 size, u32 ac, bool solid, u32 tessel)
{
	DBG_DrawRotation<0>(ang0, ang1, m, fVector3( ).set(0.0f, 0.0f, 1.0f), size, ac, solid, tessel);
}

void	DBG_DrawRotationY(const fMatrix4x4& m, f32 ang0, f32 ang1, f32 size, u32 ac, bool solid, u32 tessel)
{
	DBG_DrawRotation<1>(ang0, ang1, m, fVector3( ).set(1.0f, 0.0f, 0.0f), size, ac, solid, tessel);
}

void	DBG_DrawRotationZ(const fMatrix4x4& m, f32 ang0, f32 ang1, f32 size, u32 ac, bool solid, u32 tessel)
{
	DBG_DrawRotation<2>(ang0, ang1, m, fVector3( ).set(0.0f, 1.0f, 0.0f), size, ac, solid, tessel);
}

struct SPHDBGDrawAABB :public SPHDBGDrawAbsract
{
	fVector3 p[2];
	u32 c;
	SPHDBGDrawAABB(const fVector3& center, const fVector3& AABB, u32 ac)
	{
		p[0].set(center);
		p[1].set(AABB);
		c = ac;
	}
	virtual void render( )
	{
		Level( ).debug_renderer( ).draw_aabb(p[0], p[1].x, p[1].y, p[1].z, c);
	}
};

void DBG_DrawAABB(const fVector3& center, const fVector3& AABB, u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawAABB>(center, AABB, c));
}

struct SPHDBGDrawOBB : public SPHDBGDrawAbsract
{
	fMatrix4x4 m;
	fVector3 h;
	u32 c;
	SPHDBGDrawOBB(const fMatrix4x4 am, const fVector3 ah, u32 ac)
	{
		m.set(am);
		h.set(ah); c = ac;
	}
	virtual void render( )
	{
		Level( ).debug_renderer( ).draw_obb(m, h, c);
	}
};

void DBG_DrawOBB(const fMatrix4x4& m, const fVector3 h, u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawOBB>(m, h, c));
}

struct SPHDBGDrawPoint :public SPHDBGDrawAbsract
{
	fVector3 p;
	f32 size;
	u32 c;
	SPHDBGDrawPoint(const fVector3 ap, f32 s, u32 ac)
	{
		p.set(ap), size = s;
		c = ac;
	}
	virtual void render( )
	{
		//Level().debug_renderer().draw_aabb(p,size,size,size,c);
		fMatrix4x4 m;
		m.identity( );
		m.scale(size, size, size);
		m.c.set(p);
		Level( ).debug_renderer( ).draw_ellipse(m, c);
	}
};

void DBG_DrawPoint(const fVector3& p, f32 size, u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawPoint>(p, size, c));
}

struct SPHDBGOutText : public SPHDBGDrawAbsract
{
	string64 s;
	bool	 rendered;
	SPHDBGOutText(pcstr t)
	{
		strcpy(s, t);
		rendered = false;
	}
	virtual void render( )
	{
		//if(rendered) return;
		HUD( ).Font( ).pFontSmall->OutNext(s);
		rendered = true;
	}
};

void _cdecl DBG_OutText(pcstr s, ...)
{
	string64 t;
	va_list   marker;
	va_start(marker, s);
	vsprintf(t, s, marker);
	va_end(marker);
	DBG_DrawPHAbstruct(xr_new<SPHDBGOutText>(t));
}

void DBG_OpenCashedDraw( )
{
	dbg_ph_draw_mode = dmCashed;
}

void DBG_ClosedCashedDraw(u32 remove_time)
{
	dbg_ph_draw_mode = dmSecondaryThread;
	cash_draw_remove_time = remove_time + Device.dwTimeGlobal;
}

IC void push(PHABS_DBG_V& v, SPHDBGDrawAbsract* a)
{
	if (v.size( ) < 500)
		v.push_back(a);
}

void DBG_DrawPHAbstruct(SPHDBGDrawAbsract* a)
{
	if (dbg_ph_draw_mode != dmCashed)
	{
		if (ph_world->Processing( ))
		{
			dbg_ph_draw_mode = dmSecondaryThread;
		}
		else
		{
			dbg_ph_draw_mode = dmSimple;
		}
	}
	switch (dbg_ph_draw_mode)
	{
		case dmSecondaryThread:
		{
			if (draw_frame)
			{
				push(dbg_draw_abstruct0, a);
			}
			else
			{
				push(dbg_draw_abstruct1, a);
			}
		}
		break;
		case dmCashed:
		{
			push(dbg_draw_cashed, a);
		}
		break;
		case dmSimple:
		{
			push(dbg_draw_simple, a);
		}
		break;
	}
}

void DBG_PHAbstruactStartFrame(bool dr_frame)
{
	PHABS_DBG_I i, e;
	if (dr_frame)
	{
		i = dbg_draw_abstruct0.begin( );
		e = dbg_draw_abstruct0.end( );
	}
	else
	{
		i = dbg_draw_abstruct1.begin( );
		e = dbg_draw_abstruct1.end( );
	}

	for (; e != i; ++i)
	{
		xr_delete(*i);
	}

	if (dr_frame)
	{
		dbg_draw_abstruct0.clear( );
	}
	else
	{
		dbg_draw_abstruct1.clear( );
	}
}

void capped_cylinder_ray_collision_test( );
void DBG_PHAbstructRender( )
{
	PHABS_DBG_I i, e;
	if (!draw_frame)
	{
		i = dbg_draw_abstruct0.begin( );
		e = dbg_draw_abstruct0.end( );
	}
	else
	{
		i = dbg_draw_abstruct1.begin( );
		e = dbg_draw_abstruct1.end( );
	}

	for (; e != i; ++i)
	{
		(*i)->render( );
	}

	if (dbg_ph_draw_mode != dmCashed)
	{
		PHABS_DBG_I i, e;
		i = dbg_draw_cashed.begin( ); e = dbg_draw_cashed.end( );
		for (; e != i; ++i)
		{
			(*i)->render( );
		}

		if (cash_draw_remove_time < Device.dwTimeGlobal)
		{
			clear_vector(dbg_draw_cashed);
		}
	}

	{
		PHABS_DBG_I i, e;
		i = dbg_draw_simple.begin( ); e = dbg_draw_simple.end( );
		for (; e != i; ++i)
		{
			(*i)->render( );
		}

		clear_vector(dbg_draw_simple);
	}
	//capped_cylinder_ray_collision_test();
}

void DBG_PHAbstructClear( )
{
	DBG_PHAbstruactStartFrame(true);
	DBG_PHAbstruactStartFrame(false);
	clear_vector(dbg_draw_cashed);
	clear_vector(dbg_draw_simple);
}

void DBG_DrawPHObject(CPHObject* obj)
{
	if (ph_dbg_draw_mask.test(phDbgDrawEnabledAABBS))
	{
		SPHObjDBGDraw obj_draw;
		obj_draw.AABB.set(obj->AABB);
		obj_draw.AABB_center.set(obj->spatial.sphere.P);
		if (draw_frame)
		{
			dbg_draw_objects0.push_back(obj_draw);
		}
		else
		{
			dbg_draw_objects1.push_back(obj_draw);
		}
	}
}

void DBG_DrawContact(dContact& c)
{

#ifdef DRAW_CONTACTS
	SPHContactDBGDraw dbc;
	if (dGeomGetBody(c.geom.g1))
	{
		dbc.geomClass = dGeomGetClass(retrieveGeom(c.geom.g1));
	}
	else
	{
		dbc.geomClass = dGeomGetClass(retrieveGeom(c.geom.g2));
	}

	dbc.norm.set(cast_fv(c.geom.normal));
	dbc.pos.set(cast_fv(c.geom.pos));
	dbc.depth = c.geom.depth;
	if (ph_dbg_draw_mask.test(phDbgDrawContacts))
	{
		if (draw_frame)Contacts0.push_back(dbc);
		else		  Contacts1.push_back(dbc);
	}
#endif

}

void DBG_DrawFrameStart( )
{
	if (draw_frame)
	{

#ifdef DRAW_CONTACTS
		Contacts0.clear( );
#endif

		dbg_draw_objects0.clear( );
		dbg_draw_abstruct0.clear( );
	}
	else
	{

#ifdef DRAW_CONTACTS
		Contacts1.clear( );
#endif

		dbg_draw_objects1.clear( );
		dbg_draw_abstruct1.clear( );
	}

	DBG_PHAbstruactStartFrame(draw_frame);

	dbg_tries_num = 0;
	dbg_saved_tries_for_active_objects = 0;
}

void PH_DBG_Clear( )
{
	DBG_PHAbstructClear( );
	dbg_draw_objects0.clear( );
	dbg_draw_objects1.clear( );

#ifdef DRAW_CONTACTS
	Contacts0.clear( );
	Contacts1.clear( );
#endif

}

void PH_DBG_Render( )
{
	if (ph_dbg_draw_mask.test(phDbgDrawZDisable))
	{
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, 0));
	}

	HUD( ).Font( ).pFontSmall->OutSet(550, 250);

	if (ph_dbg_draw_mask.test(phDbgDrawEnabledAABBS))
	{
		PHOBJ_DBG_I i, e;
		if (!draw_frame)
		{
			i = dbg_draw_objects0.begin( );
			e = dbg_draw_objects0.end( );
		}
		else
		{
			i = dbg_draw_objects1.begin( );
			e = dbg_draw_objects1.end( );
		}

		for (; e != i; ++i)
		{
			SPHObjDBGDraw& ds = *i;
			Level( ).debug_renderer( ).draw_aabb(ds.AABB_center, ds.AABB.x, ds.AABB.y, ds.AABB.z, D3DCOLOR_XRGB(255, 0, 0));
		}
	}

	DBG_PHAbstructRender( );

#ifdef DRAW_CONTACTS
	if (ph_dbg_draw_mask.test(phDbgDrawContacts))
	{
		CONTACT_I i, e;
		if (!draw_frame)
		{
			i = Contacts0.begin( );
			e = Contacts0.end( );
		}
		else
		{
			i = Contacts1.begin( );
			e = Contacts1.end( );
		}

		for (; i != e; i++)
		{
			SPHContactDBGDraw& c = *i;
			bool is_cyl = c.geomClass == dCylinderClassUser;
			Level( ).debug_renderer( ).draw_aabb(c.pos, .01f, .01f, .01f, D3DCOLOR_XRGB(255 * is_cyl, 0, 255 * !is_cyl));
			fVector3 dir;
			dir.set(c.norm);
			dir.mul(c.depth * 100.f);
			dir.add(c.pos);
			Level( ).debug_renderer( ).draw_line(Fidentity, c.pos, dir, D3DCOLOR_XRGB(255 * is_cyl, 0, 255 * !is_cyl));
		}
	}
	//	HUD().Font().pFontSmall->OutNext("---------------------");
#endif

	if (ph_dbg_draw_mask.test(phDbgDrawZDisable))
	{
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZENABLE, 1));
	}
}

void DBG_DrawStatBeforeFrameStep( )
{
	if (ph_dbg_draw_mask.test(phDbgDrawObjectStatistics))
	{
		static f32 obj_count = 0.f;
		static f32 update_obj_count = 0.f;
		obj_count = obj_count * 0.9f + f32(ph_world->ObjectsNumber( )) * 0.1f;
		update_obj_count = update_obj_count * 0.9f + f32(ph_world->UpdateObjectsNumber( )) * 0.1f;
		DBG_OutText("Active Phys Objects %3.0f", obj_count);
		DBG_OutText("Active Phys Update Objects %3.0f", update_obj_count);
	}
}

void DBG_DrawStatAfterFrameStep( )
{
	if (ph_dbg_draw_mask.test(phDbgDrawObjectStatistics))
	{
		DBG_OutText("------------------------------");
		static f32  fdbg_bodies_num = 0.f;
		static f32  fdbg_joints_num = 0.f;
		static f32  fdbg_islands_num = 0.f;
		static f32  fdbg_contacts_num = 0.f;
		static f32  fdbg_tries_num = 0.f;
		fdbg_islands_num = 0.9f * fdbg_islands_num + 0.1f * f32(dbg_islands_num);
		fdbg_bodies_num = 0.9f * fdbg_bodies_num + 0.1f * f32(dbg_bodies_num);
		fdbg_joints_num = 0.9f * fdbg_joints_num + 0.1f * f32(dbg_joints_num);
		fdbg_contacts_num = 0.9f * fdbg_contacts_num + 0.1f * f32(dbg_contacts_num);
		fdbg_tries_num = 0.9f * fdbg_tries_num + 0.1f * f32(dbg_tries_num);
		DBG_OutText("Ph Number of active islands %3.0f", fdbg_islands_num);
		DBG_OutText("Ph Number of active bodies %3.0f", fdbg_bodies_num);
		DBG_OutText("Ph Number of active joints %4.0f", fdbg_joints_num);
		DBG_OutText("Ph Number of contacts %4.0f", fdbg_contacts_num);
		DBG_OutText("Ph Number of tries %5.0f", fdbg_tries_num);
		DBG_OutText("------------------------------");
	}

	if (ph_dbg_draw_mask.test(phDbgDrawCashedTriesStat))
	{
		DBG_OutText("------------------------------");
		static f32 fdbg_saved_tries_for_active_objects = 0;
		static f32 fdbg_total_saved_tries = 0;

		fdbg_saved_tries_for_active_objects = 0.9f * fdbg_saved_tries_for_active_objects + 0.1f * f32(dbg_saved_tries_for_active_objects);
		fdbg_total_saved_tries = 0.9f * fdbg_total_saved_tries + 0.1f * f32(dbg_total_saved_tries);
		DBG_OutText("Ph Number of cashed tries in active objects %5.0f", fdbg_saved_tries_for_active_objects);
		DBG_OutText("Ph Total number cashed %5.0f", fdbg_total_saved_tries);

		static SInertVal fdbg_reused_queries_per_step(0.9f);
		static SInertVal fdbg_new_queries_per_step(0.9f);
		fdbg_reused_queries_per_step.new_val(f32(dbg_reused_queries_per_step));
		fdbg_new_queries_per_step.new_val(f32(dbg_new_queries_per_step));
		DBG_OutText("Ph tri_queries_per_step %5.2f", fdbg_new_queries_per_step.val);
		DBG_OutText("Ph reused_tri_queries_per_step %5.2f", fdbg_reused_queries_per_step.val);
		DBG_OutText("------------------------------");
	}

	draw_frame = !draw_frame;
}

CFunctionGraph::CFunctionGraph( )
{
	m_stat_graph = NULL;
	m_function.clear( );
}
CFunctionGraph::~CFunctionGraph( )
{
	xr_delete(m_stat_graph);
	m_function.clear( );
}
void CFunctionGraph::Init(type_function fun, f32 x0, f32 x1, s32 l, s32 t, s32 w, s32 h, s32 points_num/*=500*/, u32 color/*=*/, u32 bk_color)
{
	x_min = x0; x_max = x1;
	m_stat_graph = xr_new<CStatGraph>( );
	m_function = fun;
	R_ASSERT(!m_function.empty( ) && m_stat_graph);
	R_ASSERT(x1 > x0);
	s = (x_max - x_min) / points_num;
	R_ASSERT(s > 0.f);
	m_stat_graph->SetRect(l, t, w, h, bk_color, bk_color);
	f32 min = dInfinity;
	f32 max = -dInfinity;
	for (f32 x = x_min; x < x_max; x += s)
	{
		f32 val = m_function(x);
		save_min(min, val); save_max(max, val);
	}

	R_ASSERT(min<dInfinity && max>-dInfinity && min <= max);
	m_stat_graph->SetMinMax(min, max, points_num);

	for (f32 x = x_min; x < x_max; x += s)
	{
		f32 val = m_function(x);
		m_stat_graph->AppendItem(val, color);
	}
	//m_stat_graph->AddMarker(CStatGraph::stVert, 0, D3DCOLOR_XRGB(255, 0, 0));
	//m_stat_graph->AddMarker(CStatGraph::stHor, 0, D3DCOLOR_XRGB(255, 0, 0));
}

void CFunctionGraph::AddMarker(CStatGraph::EStyle Style, f32 pos, u32 Color)
{
	VERIFY(IsActive( ));
	ScaleMarkerPos(Style, pos);
	m_stat_graph->AddMarker(Style, pos, Color);
}
void CFunctionGraph::UpdateMarker(u32 ID, f32 M)
{
	VERIFY(IsActive( ));
	ScaleMarkerPos(ID, M);
	m_stat_graph->UpdateMarkerPos(ID, M);
}
void CFunctionGraph::ScaleMarkerPos(u32 ID, f32& p)
{
	VERIFY(IsActive( ));
	ScaleMarkerPos(m_stat_graph->Marker(ID).m_eStyle, p);
}
void CFunctionGraph::ScaleMarkerPos(CStatGraph::EStyle Style, f32& p)
{
	VERIFY(IsActive( ));
	if (Style == CStatGraph::stVert)
	{
		p = ScaleX(p);
	}
}
void CFunctionGraph::Clear( )
{
	xr_delete(m_stat_graph);
	m_function.clear( );
}

bool CFunctionGraph::IsActive( )
{
	VERIFY((m_stat_graph == 0) == m_function.empty( ));
	return !!m_stat_graph;
}

pcstr PH_DBG_ObjectTrack( )
{
	return dbg_trace_object;
}
void PH_DBG_SetTrackObject(pcstr obj)
{
	strcpy(s_dbg_tsrace_obj, obj);
	dbg_trace_object = s_dbg_tsrace_obj;
}
#endif

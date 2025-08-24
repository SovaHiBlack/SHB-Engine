////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_debug2.cpp
//	Created 	: 02.10.2001
//  Modified 	: 11.11.2003
//	Author		: Oles Shihkovtsov, Dmitriy Iassenev
//	Description : Level graph debug functions
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#ifdef DEBUG
#include "level_graph.h"
#include "..\XR_3DA\CustomHUD.h"
#include "AISpace.h"
#include "hudmanager.h"
#include "game_graph.h"
#include "game_sv_single.h"
#include "custommonster.h"
#include "ai/stalker/Stalker.h"
#include "xrserver_objects_alife_monsters.h"
#include "cover_point.h"
#include "cover_manager.h"
#include "cover_evaluators.h"
#include "TeamBaseZone.h"
#include "alife_simulator.h"
#include "alife_graph_registry.h"
#include "alife_object_registry.h"
#include "game_cl_base.h"
#include "space_restriction_manager.h"
#include "space_restriction.h"
#include "SpaceRestrictor.h"
#include "space_restriction_base.h"
#include "detail_path_manager.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "memory_space.h"
#include "Level.h"
#include "ai_object_location.h"
#include "movement_manager.h"
#include "graph_engine.h"
#include "DebugRenderer.h"

void CLevelGraph::draw_nodes	()
{
	CGameObject*	O	= smart_cast<CGameObject*> (Level().CurrentEntity());
	fVector3	POSITION	= O->Position();
	POSITION.y += 0.5f;

	// display
	fVector3 P			= POSITION;

//	CPosition			Local;
//	vertex_position		(Local,P);

	u32 ID				= O->ai_location().level_vertex_id();

	CGameFont* F		= HUD().Font().pFontDI;
	F->SetHeightI		(.02f);
	F->OutI				(0.f,0.5f,"%f,%f,%f",VPUSH(P));
//	f32				x,z;
//	unpack_xz			(Local,x,z);
//	F->Out				(0.f,0.55f,"%3d,%4d,%3d -> %d",	iFloor(x),iFloor(Local.y()),iFloor(z),u32(ID));

	svector<u32,128>	linked;
	{
		const_iterator	i,e;
		begin			(ID,i,e);
		for(; i != e; ++i)
			linked.push_back(value(ID,i));
	}

	// render
	f32	sc		= header().cell_size()/16;
	f32	st		= 0.98f*header().cell_size()/2;
	f32	tt		= 0.01f;

	fVector3	DUP;
	DUP.set(0.0f,1.0f,0.0f);

	RCache.set_Shader	(sh_debug);
	F->SetColor			(color_rgba(255,255,255,255));

	// если включён ai_dbg_frustum раскрасить ноды по light
	// иначе раскрашивать по cover
	bool			b_light = false;
	
	//////////////////////////////////////////////////////////////////////////
	fVector3 min_position;
	fVector3 max_position;
	max_position = min_position = Device.vCameraPosition;
	min_position.sub(30.0f);
	max_position.add(30.0f);
	
	CLevelGraph::const_vertex_iterator	 I, E;
	if (valid_vertex_position(min_position))
		I = std::lower_bound(begin(),end(),vertex_position(min_position).xz(),&vertex::predicate2);
	else
		I = begin();

	if (valid_vertex_position(max_position)) {
		E = std::upper_bound(begin(),end(),vertex_position(max_position).xz(),&vertex::predicate);
		if (E != end()) ++E;
	}
	else
		E = end();

	//////////////////////////////////////////////////////////////////////////

	for ( ; I != E; ++I)
	{
		const CLevelGraph::CVertex&	N	= *I;
		fVector3			PC;
		PC				= vertex_position(N);

		u32 Nid			= vertex_id(I);

		if (Device.vCameraPosition.distance_to(PC)>30) continue;

		f32			sr	= header().cell_size();
		if (::Render->ViewBase.testSphere_dirty(PC,sr)) {
			
			u32	LL = ((b_light) ?	iFloor(f32(N.light())/15.f*255.f) :
									iFloor(vertex_cover(I)/4*255.f));
			
			u32	CC		= D3DCOLOR_XRGB(0,0,255);
			u32	CT		= D3DCOLOR_XRGB(LL,LL,LL);
			u32	CH		= D3DCOLOR_XRGB(0,128,0);

			BOOL	bHL		= FALSE;
			if (Nid==u32(ID))	{ bHL = TRUE; CT = D3DCOLOR_XRGB(0,255,0); }
			else {
				for (u32 t=0; t<linked.size(); ++t) {
					if (linked[t]==Nid) { bHL = TRUE; CT = CH; break; }
				}
			}

			// unpack plane
			fPlane3 PL;
			fVector3 vNorm;
			pvDecompress(vNorm,N.plane());
			PL.build	(PC,vNorm);

			// create vertices
			fVector3		v;
			fVector3 v1;
			fVector3 v2;
			fVector3 v3;
			fVector3 v4;
			v.set(PC.x-st,PC.y,PC.z-st);	PL.intersectRayPoint(v,DUP,v1);	v1.mad(v1,PL.n,tt);	// minX,minZ
			v.set(PC.x+st,PC.y,PC.z-st);	PL.intersectRayPoint(v,DUP,v2);	v2.mad(v2,PL.n,tt);	// maxX,minZ
			v.set(PC.x+st,PC.y,PC.z+st);	PL.intersectRayPoint(v,DUP,v3);	v3.mad(v3,PL.n,tt);	// maxX,maxZ
			v.set(PC.x-st,PC.y,PC.z+st);	PL.intersectRayPoint(v,DUP,v4);	v4.mad(v4,PL.n,tt);	// minX,maxZ

			// render quad
			RCache.dbg_DrawTRI	(Fidentity,v3,v2,v1,CT);
			RCache.dbg_DrawTRI	(Fidentity,v1,v4,v3,CT);

			// render center
			Level().debug_renderer().draw_aabb	(PC,sc,sc,sc,CC);

			// render id
			if (bHL) {
				fVector3		T;
				fVector4	S;
				T.set		(PC); T.y+=0.3f;
				Device.mFullTransform.transform	(S,T);
				if (S.z < 0 || S.z < 0)												continue;
				if (S.x < -1.f || S.x > 1.f || S.y<-1.f || S.x>1.f)					continue;
				F->SetHeightI	(0.05f/_sqrt(_abs(S.w)));
				F->SetColor	(0xffffffff);
				F->OutI		(S.x,-S.y,"~%d",Nid);
			}
		}
	}
}

void CLevelGraph::draw_restrictions	()
{
	CSpaceRestrictionManager::SPACE_RESTRICTIONS::const_iterator	I = Level().space_restriction_manager().restrictions().begin();
	CSpaceRestrictionManager::SPACE_RESTRICTIONS::const_iterator	E = Level().space_restriction_manager().restrictions().end();

	CRandom R;

	for ( ; I != E; ++I) {
		if (!(*I).second->m_ref_count)
			continue;
		if (!(*I).second->initialized()) continue;

		u8 b = u8(R.randI(255));
		u8 g = u8(R.randI(255));
		u8 r = u8(R.randI(255));

		xr_vector<u32>::const_iterator	i = (*I).second->border().begin();
		xr_vector<u32>::const_iterator	e = (*I).second->border().end();
		for ( ; i != e; ++i) {
			fVector3 temp = ai().level_graph().vertex_position(*i);
			temp.y += .1f;
			Level().debug_renderer().draw_aabb(temp,.05f,.05f,.05f,D3DCOLOR_XRGB(r,g,b));
		}

#ifdef USE_FREE_IN_RESTRICTIONS
		CSpaceRestriction::FREE_IN_RESTRICTIONS::const_iterator II = (*I).second->m_free_in_restrictions.begin();
		CSpaceRestriction::FREE_IN_RESTRICTIONS::const_iterator EE = (*I).second->m_free_in_restrictions.end();
		for ( ; II != EE; ++II) {
			xr_vector<u32>::const_iterator	i = (*II).m_restriction->border().begin();
			xr_vector<u32>::const_iterator	e = (*II).m_restriction->border().end();
			for ( ; i != e; ++i) {
				fVector3 temp = ai().level_graph().vertex_position(*i);
				temp.y += .1f;
				Level().debug_renderer().draw_aabb(temp,.05f,.05f,.05f,D3DCOLOR_XRGB(255,0,0));
			}
			{
				xr_vector<u32>::const_iterator	i = (*II).m_restriction->border().begin();
				xr_vector<u32>::const_iterator	e = (*II).m_restriction->border().end();
				for ( ; i != e; ++i) {
					fVector3 temp = ai().level_graph().vertex_position(*i);
					temp.y += .1f;
					Level().debug_renderer().draw_aabb(temp,.05f,.05f,.05f,D3DCOLOR_XRGB(0,255,0));
				}
			}
		}
#endif
	}
}

void CLevelGraph::draw_covers	()
{
	f32					half_size = ai().level_graph().header().cell_size()*.5f;
	xr_vector<CCoverPoint*>	nearest;
	nearest.reserve			(1000);
	ai().cover_manager().covers().nearest(Device.vCameraPosition,5.f,nearest);
	xr_vector<CCoverPoint*>::const_iterator	I = nearest.begin();
	xr_vector<CCoverPoint*>::const_iterator	E = nearest.end();
	for ( ; I != E; ++I) {
		fVector3				position = (*I)->position();
		position.y			+= 1.f;
		Level().debug_renderer().draw_aabb	(position,half_size - .01f,1.f,ai().level_graph().header().cell_size()*.5f-.01f,D3DCOLOR_XRGB(0*255,255,0*255));

		CVertex				*v = vertex((*I)->level_vertex_id());
		fVector3				direction;
		f32				best_value = -1.f;

		for (u32 i=0, j = 0; i<36; ++i) {
			f32				value = cover_in_direction(f32(10*i)/180.f*PI,v);
			direction.setHP		(f32(10*i)/180.f*PI,0);
			direction.normalize	();
			direction.mul		(value*half_size);
			direction.add		(position);
			direction.y			= position.y;
			Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(0,0,255));
			value				= compute_square(f32(10*i)/180.f*PI,PI/2.f,v);
			if (value > best_value) {
				best_value		= value;
				j				= i;
			}
		}

		direction.set		(position.x - half_size* f32(v->cover(0))/15.f,position.y,position.z);
		Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set		(position.x,position.y,position.z + half_size* f32(v->cover(1))/15.f);
		Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set		(position.x + half_size* f32(v->cover(2))/15.f,position.y,position.z);
		Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		direction.set		(position.x,position.y,position.z - half_size* f32(v->cover(3))/15.f);
		Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

		f32				value = cover_in_direction(f32(10*j)/180.f*PI,v);
		direction.setHP		(f32(10*j)/180.f*PI,0);
		direction.normalize	();
		direction.mul		(value*half_size);
		direction.add		(position);
		direction.y			= position.y;
		Level().debug_renderer().draw_line	(Fidentity,position,direction,D3DCOLOR_XRGB(0,0,0));
	}
}

void CLevelGraph::draw_objects	()
{
	u32					I = 0;
	u32					E = Level().Objects.o_count	();
	for ( ; I < E; ++I) {
		CObject			*_O = Level().Objects.o_get_by_iterator(I);
		CTeamBaseZone	*team_base_zone = smart_cast<CTeamBaseZone*>(_O);
		if (team_base_zone) {
			team_base_zone->OnRender();
			continue;
		}

		CCustomMonster	*tpCustomMonster = smart_cast<CCustomMonster*>(_O);
		if (tpCustomMonster) {
			tpCustomMonster->OnRender();
			if (!tpCustomMonster->movement().detail().path().empty()) {
				fVector3				temp = tpCustomMonster->movement().detail().path()[tpCustomMonster->movement().detail().path().size() - 1].position;
				Level().debug_renderer().draw_aabb	(temp,1.f,1.f,1.f,D3DCOLOR_XRGB(0,0,255));
			}
		}
	}
}

void CLevelGraph::draw_debug_node()
{
	if (g_bDebugNode) {
		fVector3 pos_src;
		fVector3 pos_dest;

		if (ai().level_graph().valid_vertex_id(g_dwDebugNodeSource)) {
			pos_src		= ai().level_graph().vertex_position(g_dwDebugNodeSource);
			pos_dest	= pos_src;
			pos_dest.y	+= 10.0f;

			Level().debug_renderer().draw_aabb(pos_src,0.35f,0.35f,0.35f,D3DCOLOR_XRGB(0,0,255));
			Level().debug_renderer().draw_line(Fidentity,pos_src,pos_dest,D3DCOLOR_XRGB(0,0,255));
		}

		if (ai().level_graph().valid_vertex_id(g_dwDebugNodeDest)) {
			pos_src		= ai().level_graph().vertex_position(g_dwDebugNodeDest);
			pos_dest	= pos_src;
			pos_dest.y	+= 10.0f;

			Level().debug_renderer().draw_aabb(pos_src,0.35f,0.35f,0.35f,D3DCOLOR_XRGB(255,0,0));
			Level().debug_renderer().draw_line(Fidentity,pos_src,pos_dest,D3DCOLOR_XRGB(255,0,0));
		}
	}
}

#endif // DEBUG
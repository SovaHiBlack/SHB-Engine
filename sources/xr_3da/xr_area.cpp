#include "stdafx.h"
#include "igame_level.h"

#include "xr_area.h"
#include "Object.h"
#include "xrLevel.h"
#include "feel_sound.h"
//#include "x_ray.h"
//#include "GameFont.h"

using namespace collide;

extern BOOL g_bLoaded;

void IGame_Level::SoundEvent_Register(ref_sound_data_ptr S, f32 range)
{
	if (!g_bLoaded)
	{
		return;
	}

	if (!S)
	{
		return;
	}

	if (S->g_object && S->g_object->getDestroy( ))
	{
		S->g_object = 0; return;
	}

	if (0 == S->feedback)
	{
		return;
	}

	clamp(range, 0.1f, 500.0f);

	const CSound_params* p = S->feedback->get_params( );
	fVector3 snd_position = p->position;
	if (S->feedback->is_2D( ))
	{
		snd_position.add(Sound->listener_position( ));
	}

	VERIFY(p && _valid(range));
	range = _min(range, p->max_ai_distance);
	VERIFY(_valid(snd_position));
	VERIFY(_valid(p->max_ai_distance));
	VERIFY(_valid(p->volume));

	// Query objects
	fVector3 bb_size = {range,range,range};
	g_SpatialSpace->q_box(snd_ER, 0, STYPE_REACTTOSOUND, snd_position, bb_size);

	// Iterate
	xr_vector<ISpatial*>::iterator	it = snd_ER.begin( );
	xr_vector<ISpatial*>::iterator	end = snd_ER.end( );
	for (; it != end; it++)
	{
		Feel::Sound* L = (*it)->dcast_FeelSound( );
		if (0 == L)
		{
			continue;
		}

		CObject* CO = (*it)->dcast_CObject( );
		VERIFY(CO);
		if (CO->getDestroy( ))
		{
			continue;
		}

		// Energy and signal
		VERIFY(_valid((*it)->spatial.sphere.P));
		f32 dist = snd_position.distance_to((*it)->spatial.sphere.P);
		if (dist > p->max_ai_distance)
		{
			continue;
		}

		VERIFY(_valid(dist));
		VERIFY2(!fis_zero(p->max_ai_distance), S->handle->file_name( ));
		f32 Power = (1.0f - dist / p->max_ai_distance) * p->volume;
		VERIFY(_valid(Power));
		if (Power > EPSILON_7)
		{
			f32 occ = Sound->get_occlusion_to((*it)->spatial.sphere.P, snd_position);
			VERIFY(_valid(occ));
			Power *= occ;
			if (Power > EPSILON_7)
			{
				_esound_delegate	D = {L, S, Power};
				snd_Events.push_back(D);
			}
		}
	}

	snd_ER.clear_not_free( );
}

void IGame_Level::SoundEvent_Dispatch( )
{
	while (!snd_Events.empty( ))
	{
		_esound_delegate& D = snd_Events.back( );
		VERIFY(D.dest && D.source);
		if (D.source->feedback)
		{
			D.dest->feel_sound_new(
				D.source->g_object,
				D.source->g_type,
				D.source->g_userdata,
				D.source->feedback->get_params( )->position,
				D.power
			);
		}

		snd_Events.pop_back( );
	}
}

void __stdcall _sound_event(ref_sound_data_ptr S, f32 range)
{
	if (g_pGameLevel && S && S->feedback)
	{
		g_pGameLevel->SoundEvent_Register(S, range);
	}
}

//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------
CObjectSpace::CObjectSpace( )
{

#ifdef DEBUG
	sh_debug.create("debug\\wireframe", "$null");
#endif

	m_BoundingVolume.invalidate( );
}
//----------------------------------------------------------------------
CObjectSpace::~CObjectSpace( )
{
	Sound->set_geometry_occ(NULL);
	Sound->set_handler(NULL);

#ifdef DEBUG
	sh_debug.destroy( );
#endif

}
//----------------------------------------------------------------------
IC s32 CObjectSpace::GetNearest(xr_vector<CObject*>& q_nearest, const fVector3& point, f32 range, CObject* ignore_object)
{
	// Query objects
	q_nearest.clear_not_free( );
	fSphere				Q;
	Q.set(point, range);
	fVector3				B;
	B.set(range, range, range);
	g_SpatialSpace->q_box(r_spatial, 0, STYPE_COLLIDEABLE, point, B);

	// Iterate
	xr_vector<ISpatial*>::iterator	it = r_spatial.begin( );
	xr_vector<ISpatial*>::iterator	end = r_spatial.end( );
	for (; it != end; it++)
	{
		CObject* O = (*it)->dcast_CObject( );
		if (0 == O)
		{
			continue;
		}

		if (O == ignore_object)
		{
			continue;
		}

		fSphere mS = {O->spatial.sphere.P, O->spatial.sphere.R};
		if (Q.intersect(mS))
		{
			q_nearest.push_back(O);
		}
	}

	return q_nearest.size( );
}
//----------------------------------------------------------------------
IC s32 CObjectSpace::GetNearest(xr_vector<CObject*>& q_nearest, ICollisionForm* obj, f32 range)
{
	CObject* O = obj->Owner( );
	return				GetNearest(q_nearest, O->spatial.sphere.P, range + O->spatial.sphere.R, O);
}

//----------------------------------------------------------------------
static void __stdcall	build_callback(fVector3* V, s32 Vcnt, CDB::TRI* T, s32 Tcnt, pvoid params)
{
	g_pGameLevel->Load_GameSpecific_CFORM(T, Tcnt);
}
void CObjectSpace::Load( )
{
	IReader* F = FS.r_open("$level$", "level.cform");
	R_ASSERT(F);

	hdrCFORM					H;
	F->r(&H, sizeof(hdrCFORM));
	fVector3* verts = (fVector3*)F->pointer( );
	CDB::TRI* tris = (CDB::TRI*)(verts + H.vertcount);
	R_ASSERT(CFORM_CURRENT_VERSION == H.version);
	Static.build(verts, H.vertcount, tris, H.facecount, build_callback);

	m_BoundingVolume.set(H.aabb);
	g_SpatialSpace->initialize(H.aabb);
	g_SpatialSpacePhysic->initialize(H.aabb);
	Sound->set_geometry_occ(&Static);
	Sound->set_handler(_sound_event);

	FS.r_close(F);
}
//----------------------------------------------------------------------
#ifdef DEBUG
void CObjectSpace::dbgRender( )
{
	R_ASSERT(bDebug);

	RCache.set_Shader(sh_debug);
	for (u32 i = 0; i < q_debug.boxes.size( ); i++)
	{
		fObb& obb = q_debug.boxes[i];
		fMatrix4x4		X;
		fMatrix4x4		S;
		fMatrix4x4		R;
		obb.xform_get(X);
		RCache.dbg_DrawOBB(X, obb.m_halfsize, D3DCOLOR_XRGB(255, 0, 0));
		S.scale(obb.m_halfsize);
		R.mul(X, S);
		RCache.dbg_DrawEllipse(R, D3DCOLOR_XRGB(0, 0, 255));
	}

	q_debug.boxes.clear( );

	for (i = 0; i < dbg_S.size( ); i++)
	{
		std::pair<fSphere, u32>& P = dbg_S[i];
		fSphere& S = P.first;
		fMatrix4x4		M;
		M.scale(S.R, S.R, S.R);
		M.translate_over(S.P);
		RCache.dbg_DrawEllipse(M, P.second);
	}

	dbg_S.clear( );
}
#endif

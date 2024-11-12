#include "stdafx.h"

#include "Render.h"
#include "Thunderbolt.h"
#include "igame_persistent.h"
#include "LightAnimLibrary.h"

#include "igame_level.h"
#include "xr_area.h"
#include "xr_object.h"

static const f32 MAX_DIST_FACTOR = 0.95f;

SThunderboltDesc::SThunderboltDesc(CInifile* pIni, pcstr sect)
{
	name = sect;
	color_anim = LALib.FindItem(pIni->r_string(sect, "color_anim"));
	VERIFY(color_anim);
	color_anim->fFPS = (f32) color_anim->iFrameCount;
	m_GradientTop.shader = pIni->r_string(sect, "gradient_top_shader");
	m_GradientTop.texture = pIni->r_string(sect, "gradient_top_texture");
	m_GradientTop.fRadius = pIni->r_fvector2(sect, "gradient_top_radius");
	m_GradientTop.fOpacity = pIni->r_float(sect, "gradient_top_opacity");
	m_GradientTop.hShader.create(*m_GradientTop.shader, *m_GradientTop.texture);
	m_GradientCenter.shader = pIni->r_string(sect, "gradient_center_shader");
	m_GradientCenter.texture = pIni->r_string(sect, "gradient_center_texture");
	m_GradientCenter.fRadius = pIni->r_fvector2(sect, "gradient_center_radius");
	m_GradientCenter.fOpacity = pIni->r_float(sect, "gradient_center_opacity");
	m_GradientCenter.hShader.create(*m_GradientCenter.shader, *m_GradientCenter.texture);

	// models
	IReader* F = 0;
	pcstr m_name;
	m_name = pSettings->r_string(sect, "lightning_model");
	F = FS.r_open("$game_meshes$", m_name);
	R_ASSERT2(F, "Empty 'lightning_model'.");
	l_model = ::Render->model_CreateDM(F);
	FS.r_close(F);

	// sound
	m_name = pSettings->r_string(sect, "sound");
	if (m_name && m_name[0])
	{
		snd.create(m_name, st_Effect, sg_Undefined);
	}
}

SThunderboltDesc::~SThunderboltDesc( )
{
	::Render->model_Delete(l_model);
	m_GradientTop.hShader.destroy( );
	m_GradientCenter.hShader.destroy( );
	snd.destroy( );
}

//----------------------------------------------------------------------------------------------
// collection
//----------------------------------------------------------------------------------------------
SThunderboltCollection::SThunderboltCollection(CInifile* pIni, pcstr sect)
{
	section = sect;
	s32 tb_count = pIni->line_count(sect);
	for (s32 tb_idx = 0; tb_idx < tb_count; tb_idx++)
	{
		pcstr N;
		pcstr V;
		if (pIni->r_line(sect, tb_idx, &N, &V))
		{
			palette.push_back(xr_new<SThunderboltDesc>(pIni, N));
		}
	}
}
SThunderboltCollection::~SThunderboltCollection( )
{
	for (DescIt d_it = palette.begin( ); d_it != palette.end( ); d_it++)
	{
		xr_delete(*d_it);
	}

	palette.clear( );
}

//----------------------------------------------------------------------------------------------
// thunderbolt effect
//----------------------------------------------------------------------------------------------
CEffect_Thunderbolt::CEffect_Thunderbolt( )
{
	current = 0;
	life_time = 0.0f;
	state = stIdle;
	next_lightning_time = 0.0f;
	bEnabled = FALSE;

	// geom
	hGeom_model.create(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RCache.Vertex.Buffer( ), RCache.Index.Buffer( ));
	hGeom_gradient.create(FVF::F_LIT, RCache.Vertex.Buffer( ), RCache.QuadIB);

	// params
	p_var_alt = pSettings->r_fvector2("thunderbolt_common", "altitude");
	p_var_alt.x = deg2rad(p_var_alt.x); p_var_alt.y = deg2rad(p_var_alt.y);
	p_var_long = deg2rad(pSettings->r_float("thunderbolt_common", "delta_longitude"));
	p_min_dist = _min(MAX_DIST_FACTOR, pSettings->r_float("thunderbolt_common", "min_dist_factor"));
	p_tilt = deg2rad(pSettings->r_float("thunderbolt_common", "tilt"));
	p_second_prop = pSettings->r_float("thunderbolt_common", "second_propability");
	clamp(p_second_prop, 0.f, 1.f);
	p_sky_color = pSettings->r_float("thunderbolt_common", "sky_color");
	p_sun_color = pSettings->r_float("thunderbolt_common", "sun_color");
	p_fog_color = pSettings->r_float("thunderbolt_common", "fog_color");
}

CEffect_Thunderbolt::~CEffect_Thunderbolt( )
{
	for (CollectionVecIt d_it = collection.begin( ); d_it != collection.end( ); d_it++)
	{
		xr_delete(*d_it);
	}

	collection.clear( );
	hGeom_model.destroy( );
	hGeom_gradient.destroy( );
}

s32 CEffect_Thunderbolt::AppendDef(CInifile* pIni, pcstr sect)
{
	if (!sect || (0 == sect[0]))
	{
		return -1;
	}

	for (CollectionVecIt it = collection.begin( ); it != collection.end( ); it++)
	{
		if ((*it)->section == sect)
		{
			return s32(it - collection.begin( ));
		}
	}

	collection.push_back(xr_new<SThunderboltCollection>(pIni, sect));
	return collection.size( ) - 1;
}

BOOL CEffect_Thunderbolt::RayPick(const fVector3& s, const fVector3& d, f32& dist)
{
	BOOL bRes = TRUE;

	collide::rq_result RQ;
	CObject* E = g_pGameLevel->CurrentViewEntity( );
	bRes = g_pGameLevel->ObjectSpace.RayPick(s, d, dist, collide::rqtBoth, RQ, E);
	if (bRes)
	{
		dist = RQ.range;
	}
	else
	{
		fVector3 N = { 0.0f, -1.0f, 0.0f };
		fVector3 P = { 0.0f, 0.0f, 0.0f };
		fPlane3 PL;
		PL.build(P, N);
		f32 dst = dist;
		if (PL.intersectRayDist(s, d, dst) && (dst <= dist))
		{
			dist = dst;
			return true;
		}
		else
		{
			return false;
		}
	}

	return bRes;
}

#define FAR_DIST g_pGamePersistent->Environment().CurrentEnv.far_plane

void CEffect_Thunderbolt::Bolt(s32 id, f32 period, f32 lt)
{
	VERIFY(id >= 0 && id < (s32) collection.size( ));
	state = stWorking;
	life_time = lt + Random.randF(-lt * 0.5f, lt * 0.5f);
	current_time = 0.0f;
	current = collection[id]->GetRandomDesc( );
	VERIFY(current);

	fMatrix4x4 XF;
	fMatrix4x4 S;
	fVector3 pos;
	fVector3 dev;
	f32 sun_h;
	f32 sun_p;
	g_pGamePersistent->Environment( ).CurrentEnv.sun_dir.getHP(sun_h, sun_p);
	f32 alt = Random.randF(p_var_alt.x, p_var_alt.y);
	f32 lng = Random.randF(sun_h - p_var_long + PI, sun_h + p_var_long + PI);
	f32 dist = Random.randF(FAR_DIST * p_min_dist, FAR_DIST * MAX_DIST_FACTOR);
	current_direction.setHP(lng, alt);
	pos.mad(Device.vCameraPosition, current_direction, dist);
	dev.x = Random.randF(-p_tilt, p_tilt);
	dev.y = Random.randF(0, PI_MUL_2);
	dev.z = Random.randF(-p_tilt, p_tilt);
	XF.setXYZi(dev);

	fVector3 light_dir = { 0.0f,-1.0f,0.0f };
	XF.transform_dir(light_dir);
	lightning_size = FAR_DIST * 2.f;
	RayPick(pos, light_dir, lightning_size);

	lightning_center.mad(pos, light_dir, lightning_size * 0.5f);

	S.scale(lightning_size, lightning_size, lightning_size);
	XF.translate_over(pos);
	current_xform.mul_43(XF, S);

	f32 next_v = Random.randF( );

	if (next_v < p_second_prop)
	{
		next_lightning_time = Device.fTimeGlobal + lt + EPSILON_3;
	}
	else
	{
		next_lightning_time = Device.fTimeGlobal + period + Random.randF(-period * 0.3f, period * 0.3f);
		current->snd.play_no_feedback(0, 0, dist / 300.f, &pos, 0, 0, &fVector2( ).set(dist / 2, dist * 2.0f));
	}

	current_direction.invert( );	// for env-sun
}

void CEffect_Thunderbolt::OnFrame(int id, f32 period, f32 duration)
{
	BOOL enabled = (id >= 0);
	if (bEnabled != enabled)
	{
		bEnabled = enabled;
		next_lightning_time = Device.fTimeGlobal + period + Random.randF(-period * 0.5f, period * 0.5f);
	}
	else if (bEnabled && (Device.fTimeGlobal > next_lightning_time))
	{
		if (state == stIdle && (id >= 0)) Bolt(id, period, duration);
	}
	if (state == stWorking)
	{
		if (current_time > life_time) state = stIdle;
		current_time += Device.fTimeDelta;
		fVector3 fClr;
		int frame;
		u32 uClr = current->color_anim->CalculateRGB(current_time / life_time, frame);
		fClr.set(f32(color_get_R(uClr)) / 255.f, f32(color_get_G(uClr) / 255.f), f32(color_get_B(uClr) / 255.f));

		lightning_phase = 1.5f * (current_time / life_time);
		clamp(lightning_phase, 0.f, 1.f);

		g_pGamePersistent->Environment( ).CurrentEnv.sky_color.mad(fClr, p_sky_color);
		g_pGamePersistent->Environment( ).CurrentEnv.sun_color.mad(fClr, p_sun_color);
		g_pGamePersistent->Environment( ).CurrentEnv.fog_color.mad(fClr, p_fog_color);

		if (::Render->get_generation( ) == IRender_interface::GENERATION_R2)
		{
			g_pGamePersistent->Environment( ).CurrentEnv.sun_dir = current_direction;
			VERIFY2(g_pGamePersistent->Environment( ).CurrentEnv.sun_dir.y < 0, "Invalid sun direction settings while CEffect_Thunderbolt");
		}
	}
}

void CEffect_Thunderbolt::Render( )
{
	if (state == stWorking)
	{
		VERIFY(current);

		// lightning model
		f32 dv = lightning_phase * 0.5f;
		dv = (lightning_phase > 0.5f) ? Random.randI(2) * 0.5f : dv;

		RCache.set_CullMode(CULL_NONE);
		u32					v_offset, i_offset;
		u32					vCount_Lock = current->l_model->number_vertices;
		u32					iCount_Lock = current->l_model->number_indices;
		IRender_DetailModel::fvfVertexOut* v_ptr = (IRender_DetailModel::fvfVertexOut*) RCache.Vertex.Lock(vCount_Lock, hGeom_model->vb_stride, v_offset);
		u16* i_ptr = RCache.Index.Lock(iCount_Lock, i_offset);
		// XForm verts
		current->l_model->transfer(current_xform, v_ptr, 0xffffffff, i_ptr, 0, 0.f, dv);
		// Flush if needed
		RCache.Vertex.Unlock(vCount_Lock, hGeom_model->vb_stride);
		RCache.Index.Unlock(iCount_Lock);
		RCache.set_xform_world(Fidentity);
		RCache.set_Shader(current->l_model->shader);
		RCache.set_Geometry(hGeom_model);
		RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, iCount_Lock / 3);
		RCache.set_CullMode(CULL_CCW);

		// gradient
		fVector3				vecSx;
		fVector3			vecSy;
		u32					VS_Offset;
		FVF::LIT* pv = (FVF::LIT*) RCache.Vertex.Lock(8, hGeom_gradient.stride( ), VS_Offset);
		// top
		{
			u32 c_val = iFloor(current->m_GradientTop.fOpacity * lightning_phase * 255.f);
			u32 c = color_rgba(c_val, c_val, c_val, c_val);
			vecSx.mul(Device.vCameraRight, current->m_GradientTop.fRadius.x * lightning_size);
			vecSy.mul(Device.vCameraTop, -current->m_GradientTop.fRadius.y * lightning_size);
			pv->set(current_xform.c.x + vecSx.x - vecSy.x, current_xform.c.y + vecSx.y - vecSy.y, current_xform.c.z + vecSx.z - vecSy.z, c, 0, 0); pv++;
			pv->set(current_xform.c.x + vecSx.x + vecSy.x, current_xform.c.y + vecSx.y + vecSy.y, current_xform.c.z + vecSx.z + vecSy.z, c, 0, 1); pv++;
			pv->set(current_xform.c.x - vecSx.x - vecSy.x, current_xform.c.y - vecSx.y - vecSy.y, current_xform.c.z - vecSx.z - vecSy.z, c, 1, 0); pv++;
			pv->set(current_xform.c.x - vecSx.x + vecSy.x, current_xform.c.y - vecSx.y + vecSy.y, current_xform.c.z - vecSx.z + vecSy.z, c, 1, 1); pv++;
		}
		// center
		{
			u32 c_val = iFloor(current->m_GradientTop.fOpacity * lightning_phase * 255.f);
			u32 c = color_rgba(c_val, c_val, c_val, c_val);
			vecSx.mul(Device.vCameraRight, current->m_GradientCenter.fRadius.x * lightning_size);
			vecSy.mul(Device.vCameraTop, -current->m_GradientCenter.fRadius.y * lightning_size);
			pv->set(lightning_center.x + vecSx.x - vecSy.x, lightning_center.y + vecSx.y - vecSy.y, lightning_center.z + vecSx.z - vecSy.z, c, 0, 0); pv++;
			pv->set(lightning_center.x + vecSx.x + vecSy.x, lightning_center.y + vecSx.y + vecSy.y, lightning_center.z + vecSx.z + vecSy.z, c, 0, 1); pv++;
			pv->set(lightning_center.x - vecSx.x - vecSy.x, lightning_center.y - vecSx.y - vecSy.y, lightning_center.z - vecSx.z - vecSy.z, c, 1, 0); pv++;
			pv->set(lightning_center.x - vecSx.x + vecSy.x, lightning_center.y - vecSx.y + vecSy.y, lightning_center.z - vecSx.z + vecSy.z, c, 1, 1); pv++;
		}
		RCache.Vertex.Unlock(8, hGeom_gradient.stride( ));
		RCache.set_xform_world(Fidentity);
		RCache.set_Geometry(hGeom_gradient);
		RCache.set_Shader(current->m_GradientTop.hShader);
		RCache.Render(D3DPT_TRIANGLELIST, VS_Offset, 0, 4, 0, 2);
		RCache.set_Shader(current->m_GradientCenter.hShader);
		RCache.Render(D3DPT_TRIANGLELIST, VS_Offset + 4, 0, 4, 0, 2);
	}
}

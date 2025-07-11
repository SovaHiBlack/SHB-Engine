#include "stdafx.h"

#include "xr_efflensflare.h"

#include "igame_persistent.h"
#include "Environment.h"
#include "SkeletonCustom.h"
#include "cl_intersect.h"

#include "Object.h"
#include "igame_level.h"

#define FAR_DIST g_pGamePersistent->Environment().CurrentEnv.far_plane

#define MAX_Flares	24
//////////////////////////////////////////////////////////////////////////////
// Globals ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define BLEND_INC_SPEED 8.0f
#define BLEND_DEC_SPEED 4.0f

//------------------------------------------------------------------------------
void CLensFlareDescriptor::SetSource(f32 fRadius, BOOL ign_color, pcstr tex_name, pcstr sh_name)
{
	m_Source.fRadius = fRadius;
	m_Source.shader = sh_name;
	m_Source.texture = tex_name;
	m_Source.ignore_color = ign_color;
}

void CLensFlareDescriptor::SetGradient(f32 fMaxRadius, f32 fOpacity, pcstr tex_name, pcstr sh_name)
{
	m_Gradient.fRadius = fMaxRadius;
	m_Gradient.fOpacity = fOpacity;
	m_Gradient.shader = sh_name;
	m_Gradient.texture = tex_name;
}

void CLensFlareDescriptor::AddFlare(f32 fRadius, f32 fOpacity, f32 fPosition, pcstr tex_name, pcstr sh_name)
{
	SFlare F;
	F.fRadius = fRadius;
	F.fOpacity = fOpacity;
	F.fPosition = fPosition;
	F.shader = sh_name;
	F.texture = tex_name;
	m_Flares.push_back(F);
}

ref_shader CLensFlareDescriptor::CreateShader(pcstr tex_name, pcstr sh_name)
{
	ref_shader	R;
	if (tex_name && tex_name[0])
	{
		R.create(sh_name, tex_name);
	}

	return R;
}

void CLensFlareDescriptor::load(CIniFile* pIni, pcstr sect)
{
	section = sect;
	m_Flags.set(flSource, pIni->r_bool(sect, "source"));
	if (m_Flags.is(flSource))
	{
		pcstr S = pIni->r_string(sect, "source_shader");
		pcstr T = pIni->r_string(sect, "source_texture");
		f32 r = pIni->r_float(sect, "source_radius");
		BOOL i = pIni->r_bool(sect, "source_ignore_color");
		SetSource(r, i, T, S);
	}

	m_Flags.set(flFlare, pIni->r_bool(sect, "flares"));
	if (m_Flags.is(flFlare))
	{
		pcstr S = pIni->r_string(sect, "flare_shader");
		pcstr T = pIni->r_string(sect, "flare_textures");
		pcstr R = pIni->r_string(sect, "flare_radius");
		pcstr O = pIni->r_string(sect, "flare_opacity");
		pcstr P = pIni->r_string(sect, "flare_position");
		u32 tcnt = _GetItemCount(T);
		string256 name;
		for (u32 i = 0; i < tcnt; i++)
		{
			_GetItem(R, i, name);
			f32 r = (f32)atof(name);
			_GetItem(O, i, name);
			f32 o = (f32)atof(name);
			_GetItem(P, i, name);
			f32 p = (f32)atof(name);
			_GetItem(T, i, name);
			AddFlare(r, o, p, name, S);
		}
	}

	m_Flags.set(flGradient, CIniFile::IsBOOL(pIni->r_string(sect, "gradient")));
	if (m_Flags.is(flGradient))
	{
		pcstr S = pIni->r_string(sect, "gradient_shader");
		pcstr T = pIni->r_string(sect, "gradient_texture");
		f32 r = pIni->r_float(sect, "gradient_radius");
		f32 o = pIni->r_float(sect, "gradient_opacity");
		SetGradient(r, o, T, S);
	}

	m_StateBlendUpSpeed = 1.0f / (_max(pIni->r_float(sect, "blend_rise_time"), 0.0f) + EPSILON_7);
	m_StateBlendDnSpeed = 1.0f / (_max(pIni->r_float(sect, "blend_down_time"), 0.0f) + EPSILON_7);

	OnDeviceCreate( );
}

void CLensFlareDescriptor::OnDeviceCreate( )
{
	// shaders
	m_Gradient.hShader = CreateShader(*m_Gradient.texture, *m_Gradient.shader);
	m_Source.hShader = CreateShader(*m_Source.texture, *m_Source.shader);
	for (FlareIt it = m_Flares.begin( ); it != m_Flares.end( ); it++)
	{
		it->hShader = CreateShader(*it->texture, *it->shader);
	}
}

void CLensFlareDescriptor::OnDeviceDestroy( )
{
	// shaders
	m_Gradient.hShader.destroy( );
	m_Source.hShader.destroy( );
	for (FlareIt it = m_Flares.begin( ); it != m_Flares.end( ); it++)
	{
		it->hShader.destroy( );
	}
}

//------------------------------------------------------------------------------
CLensFlare::CLensFlare( )
{
	// Device
	dwFrame = 0xfffffffe;

	fBlend = 0.0f;

	LightColor.set(0xFFFFFFFF);
	fGradientValue = 0.0f;

	hGeom = 0;
	m_Current = 0;

	m_State = lfsNone;
	m_StateBlend = 0.0f;

	m_ray_cache.verts[0].set(0.0f, 0.0f, 0.0f);
	m_ray_cache.verts[1].set(0.0f, 0.0f, 0.0f);
	m_ray_cache.verts[2].set(0.0f, 0.0f, 0.0f);

	OnDeviceCreate( );
}

CLensFlare::~CLensFlare( )
{
	OnDeviceDestroy( );
}

struct STranspParam
{
	fVector3 P;
	fVector3 D;
	f32 f;
	CLensFlare* parent;
	f32 vis;
	f32 vis_threshold;
	STranspParam(CLensFlare* p, const fVector3& _P, const fVector3& _D, f32 _f, f32 _vis_threshold) : P(_P), D(_D), f(_f), parent(p), vis(1.0f), vis_threshold(_vis_threshold)
	{ }
};

IC BOOL material_callback(collide::rq_result& result, pvoid params)
{
	STranspParam* fp = (STranspParam*)params;
	f32 vis = 1.0f;
	if (result.O)
	{
		vis = 0.0f;
		CKinematics* K = PKinematics(result.O->renderable.visual);
		if (K && (result.element > 0))
		{
			vis = g_pGamePersistent->MtlTransparent(K->LL_GetData(u16(result.element)).game_mtl_idx);
		}
	}
	else
	{
		CDB::TRI* T = g_pGameLevel->ObjectSpace.GetStaticTris( ) + result.element;
		vis = g_pGamePersistent->MtlTransparent(T->material);
		if (fis_zero(vis))
		{
			fVector3* V = g_pGameLevel->ObjectSpace.GetStaticVerts( );
			fp->parent->m_ray_cache.set(fp->P, fp->D, fp->f, TRUE);
			fp->parent->m_ray_cache.verts[0].set(V[T->verts[0]]);
			fp->parent->m_ray_cache.verts[1].set(V[T->verts[1]]);
			fp->parent->m_ray_cache.verts[2].set(V[T->verts[2]]);
		}
	}

	fp->vis *= vis;
	return (fp->vis > fp->vis_threshold);
}

IC void	blend_lerp(f32& cur, f32 tgt, f32 speed, f32 dt)
{
	f32 diff = tgt - cur;
	f32 diff_a = _abs(diff);
	if (diff_a < EPSILON_7)
	{
		return;
	}

	f32 mot = speed * dt;
	if (mot > diff_a)
	{
		mot = diff_a;
	}

	cur += (diff / diff_a) * mot;
}

void CLensFlare::OnFrame(s32 id)
{
	if (dwFrame == Device.dwFrame)
	{
		return;
	}

	if (!g_pGameLevel)
	{
		return;
	}

	dwFrame = Device.dwFrame;

	vSunDir.mul(g_pGamePersistent->Environment( ).CurrentEnv.sun_dir, -1.0f);

	// color
	f32 tf = g_pGamePersistent->Environment( ).fTimeFactor;
	fVector3& c = g_pGamePersistent->Environment( ).CurrentEnv.sun_color;
	LightColor.set(c.x, c.y, c.z, 1.0f);

	CLensFlareDescriptor* desc = (id == -1) ? 0 : &m_Palette[id];

	switch (m_State)
	{
		case lfsNone:
		{
			m_State = lfsShow;
			m_Current = desc;
		}
		break;
		case lfsIdle:
		{
			if (desc != m_Current)
			{
				m_State = lfsHide;
			}
		}
		break;
		case lfsShow:
		{
			m_StateBlend = m_Current ? (m_StateBlend + m_Current->m_StateBlendUpSpeed * Device.fTimeDelta * tf) : 1.0f + EPSILON_5;
			if (m_StateBlend >= 1.0f)
			{
				m_State = lfsIdle;
			}
		}
		break;
		case lfsHide:
		{
			m_StateBlend = m_Current ? (m_StateBlend - m_Current->m_StateBlendDnSpeed * Device.fTimeDelta * tf) : 0.0f - EPSILON_5;
			if (m_StateBlend <= 0.0f)
			{
				m_State = lfsShow;
				m_Current = desc;
				m_StateBlend = m_Current ? m_Current->m_StateBlendUpSpeed * Device.fTimeDelta * tf : 0.0f;
			}
		}
		break;
	}

	clamp(m_StateBlend, 0.0f, 1.0f);

	if ((m_Current == 0) || (LightColor.magnitude_rgb( ) == 0.0f))
	{
		bRender = false; return;
	}

	//
	// Compute center and axis of flares
	//
	f32 fDot;

	fVector3 vecPos;

	fMatrix4x4	matEffCamPos;
	matEffCamPos.identity( );
	// Calculate our position and direction

	matEffCamPos.i.set(Device.vCameraRight);
	matEffCamPos.j.set(Device.vCameraTop);
	matEffCamPos.k.set(Device.vCameraDirection);
	vecPos.set(Device.vCameraPosition);

	vecDir.set(0.0f, 0.0f, 1.0f);
	matEffCamPos.transform_dir(vecDir);
	vecDir.normalize( );

	// Figure out of light (or flare) might be visible
	vecLight.set(vSunDir);
	vecLight.normalize( );

	fDot = vecLight.dotproduct(vecDir);

	if (fDot <= 0.01f)
	{
		bRender = false;
		return;
	}
	else
	{
		bRender = true;
	}

	 // Calculate the point directly in front of us, on the far clip plane
	f32 fDistance = FAR_DIST * 0.75f;
	vecCenter.mul(vecDir, fDistance);
	vecCenter.add(vecPos);
	// Calculate position of light on the far clip plane
	vecLight.mul(fDistance / fDot);
	vecLight.add(vecPos);
	// Compute axis which goes from light through the center of the screen
	vecAxis.sub(vecLight, vecCenter);

	//
	// Figure out if light is behind something else
	vecX.set(1.0f, 0.0f, 0.0f);
	matEffCamPos.transform_dir(vecX);
	vecX.normalize( );
	vecY.crossproduct(vecX, vecDir);

	CObject* o_main = g_pGameLevel->CurrentViewEntity( );
	STranspParam TP(this, Device.vCameraPosition, vSunDir, 1000.f, EPSILON_3);
	collide::ray_defs RD(TP.P, TP.D, TP.f, CDB::OPT_CULL, collide::rqtBoth);
	if (m_ray_cache.result && m_ray_cache.similar(TP.P, TP.D, TP.f))
	{
		// similar with previous query == 0
		TP.vis = 0.0f;
	}
	else
	{
		f32 _u;
		f32 _v;
		f32 _range;
		if (CDB::TestRayTri(TP.P, TP.D, m_ray_cache.verts, _u, _v, _range, false) && (_range > 0 && _range < TP.f))
		{
			TP.vis = 0.0f;
		}
		else
		{
			// cache outdated. real query.
			r_dest.r_clear( );
			if (g_pGameLevel->ObjectSpace.RayQuery(r_dest, RD, material_callback, &TP, NULL, o_main))
			{
				m_ray_cache.result = FALSE;
			}
		}
	}

	blend_lerp(fBlend, TP.vis, BLEND_DEC_SPEED, Device.fTimeDelta);

	clamp(fBlend, 0.0f, 1.0f);

	// gradient
	if (m_Current->m_Flags.is(CLensFlareDescriptor::flGradient))
	{
		fVector3 scr_pos;
		Device.mFullTransform.transform(scr_pos, vecLight);
		f32 kx = 1.0f;
		f32 ky = 1.0f;
		f32 sun_blend = 0.5f;
		f32 sun_max = 2.5f;
		scr_pos.y *= -1.0f;

		if (_abs(scr_pos.x) > sun_blend)
		{
			kx = ((sun_max - (f32)_abs(scr_pos.x))) / (sun_max - sun_blend);
		}

		if (_abs(scr_pos.y) > sun_blend)
		{
			ky = ((sun_max - (f32)_abs(scr_pos.y))) / (sun_max - sun_blend);
		}

		if (!((_abs(scr_pos.x) > sun_max) || (_abs(scr_pos.y) > sun_max)))
		{
			f32 op = m_StateBlend * m_Current->m_Gradient.fOpacity;
			fGradientValue = kx * ky * op * fBlend;
		}
		else
		{
			fGradientValue = 0.0f;
		}
	}
}

void CLensFlare::Render(BOOL bSun, BOOL bFlares, BOOL bGradient)
{
	if (!bRender)
	{
		return;
	}

	if (!m_Current)
	{
		return;
	}

	VERIFY(m_Current);

	fColor dwLight;
	fColor color;
	fVector3 vec;
	fVector3 vecSx;
	fVector3 vecSy;
	fVector3 vecDx;
	fVector3 vecDy;

	dwLight.set(LightColor);
	svector<ref_shader, MAX_Flares> _2render;

	u32 VS_Offset;
	FVF::LIT* pv = (FVF::LIT*)RCache.Vertex.Lock(MAX_Flares * 4, hGeom.stride( ), VS_Offset);

	f32 fDistance = FAR_DIST * 0.75f;

	if (bSun)
	{
		if (m_Current->m_Flags.is(CLensFlareDescriptor::flSource))
		{
			vecSx.mul(vecX, m_Current->m_Source.fRadius * fDistance);
			vecSy.mul(vecY, m_Current->m_Source.fRadius * fDistance);
			if (m_Current->m_Source.ignore_color)
			{
				color.set(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{
				color.set(dwLight);
			}

			color.a *= m_StateBlend;
			u32 c = color.get( );
			pv->set(vecLight.x + vecSx.x - vecSy.x, vecLight.y + vecSx.y - vecSy.y, vecLight.z + vecSx.z - vecSy.z, c, 0.0f, 0.0f);
			pv++;
			pv->set(vecLight.x + vecSx.x + vecSy.x, vecLight.y + vecSx.y + vecSy.y, vecLight.z + vecSx.z + vecSy.z, c, 0.0f, 1.0f);
			pv++;
			pv->set(vecLight.x - vecSx.x - vecSy.x, vecLight.y - vecSx.y - vecSy.y, vecLight.z - vecSx.z - vecSy.z, c, 1.0f, 0.0f);
			pv++;
			pv->set(vecLight.x - vecSx.x + vecSy.x, vecLight.y - vecSx.y + vecSy.y, vecLight.z - vecSx.z + vecSy.z, c, 1.0f, 1.0f);
			pv++;
			_2render.push_back(m_Current->m_Source.hShader);
		}
	}

	if (fBlend >= EPSILON_3)
	{
		if (bFlares)
		{
			vecDx.normalize(vecAxis);
			vecDy.crossproduct(vecDx, vecDir);
			if (m_Current->m_Flags.is(CLensFlareDescriptor::flFlare))
			{
				for (CLensFlareDescriptor::FlareIt it = m_Current->m_Flares.begin( ); it != m_Current->m_Flares.end( ); it++)
				{
					CLensFlareDescriptor::SFlare& F = *it;
					vec.mul(vecAxis, F.fPosition);
					vec.add(vecCenter);
					vecSx.mul(vecDx, F.fRadius * fDistance);
					vecSy.mul(vecDy, F.fRadius * fDistance);
					f32 cl = F.fOpacity * fBlend * m_StateBlend;
					color.set(dwLight);
					color.mul_rgba(cl);
					u32 c = color.get( );
					pv->set(vec.x + vecSx.x - vecSy.x, vec.y + vecSx.y - vecSy.y, vec.z + vecSx.z - vecSy.z, c, 0.0f, 0.0f);
					pv++;
					pv->set(vec.x + vecSx.x + vecSy.x, vec.y + vecSx.y + vecSy.y, vec.z + vecSx.z + vecSy.z, c, 0.0f, 1.0f);
					pv++;
					pv->set(vec.x - vecSx.x - vecSy.x, vec.y - vecSx.y - vecSy.y, vec.z - vecSx.z - vecSy.z, c, 1.0f, 0.0f);
					pv++;
					pv->set(vec.x - vecSx.x + vecSy.x, vec.y - vecSx.y + vecSy.y, vec.z - vecSx.z + vecSy.z, c, 1.0f, 1.0f);
					pv++;
					_2render.push_back(it->hShader);
				}
			}
		}

		// gradient
		if (bGradient && (fGradientValue >= EPSILON_3))
		{
			if (m_Current->m_Flags.is(CLensFlareDescriptor::flGradient))
			{
				vecSx.mul(vecX, m_Current->m_Gradient.fRadius * fGradientValue * fDistance);
				vecSy.mul(vecY, m_Current->m_Gradient.fRadius * fGradientValue * fDistance);

				color.set(dwLight);
				color.mul_rgba(fGradientValue * m_StateBlend);

				u32 c = color.get( );
				pv->set(vecLight.x + vecSx.x - vecSy.x, vecLight.y + vecSx.y - vecSy.y, vecLight.z + vecSx.z - vecSy.z, c, 0.0f, 0.0f);
				pv++;
				pv->set(vecLight.x + vecSx.x + vecSy.x, vecLight.y + vecSx.y + vecSy.y, vecLight.z + vecSx.z + vecSy.z, c, 0.0f, 1.0f);
				pv++;
				pv->set(vecLight.x - vecSx.x - vecSy.x, vecLight.y - vecSx.y - vecSy.y, vecLight.z - vecSx.z - vecSy.z, c, 1.0f, 0.0f);
				pv++;
				pv->set(vecLight.x - vecSx.x + vecSy.x, vecLight.y - vecSx.y + vecSy.y, vecLight.z - vecSx.z + vecSy.z, c, 1.0f, 1.0f);
				pv++;
				_2render.push_back(m_Current->m_Gradient.hShader);
			}
		}
	}

	RCache.Vertex.Unlock(_2render.size( ) * 4, hGeom.stride( ));

	RCache.set_xform_world(Fidentity);
	RCache.set_Geometry(hGeom);
	for (u32 i = 0; i < _2render.size( ); i++)
	{
		if (_2render[i])
		{
			u32 vBase = i * 4 + VS_Offset;
			RCache.set_Shader(_2render[i]);
			RCache.Render(D3DPT_TRIANGLELIST, vBase, 0, 4, 0, 2);
		}
	}
}

s32	CLensFlare::AppendDef(CIniFile* pIni, pcstr sect)
{
	if (!sect || (0 == sect[0]))
	{
		return -1;
	}

	for (LensFlareDescIt it = m_Palette.begin( ); it != m_Palette.end( ); it++)
	{
		if (0 == xr_strcmp(*it->section, sect))
		{
			return s32(it - m_Palette.begin( ));
		}
	}

	m_Palette.push_back(CLensFlareDescriptor( ));
	CLensFlareDescriptor& lf = m_Palette.back( );
	lf.load(pIni, sect);
	return (m_Palette.size( ) - 1);
}

void CLensFlare::OnDeviceCreate( )
{
	// VS
	hGeom.create(FVF::F_LIT, RCache.Vertex.Buffer( ), RCache.QuadIB);

	// palette
	for (LensFlareDescIt it = m_Palette.begin( ); it != m_Palette.end( ); it++)
	{
		it->OnDeviceCreate( );
	}
}

void CLensFlare::OnDeviceDestroy( )
{
	// palette
	for (LensFlareDescIt it = m_Palette.begin( ); it != m_Palette.end( ); it++)
	{
		it->OnDeviceDestroy( );
	}

	// VS
	hGeom.destroy( );
}

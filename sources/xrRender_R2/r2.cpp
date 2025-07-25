#include "stdafx.h"
#include "r2.h"
#include "..\XR_3DA\BasicVisual.h"
#include "..\XR_3DA\Object.h"
#include "..\XR_3DA\CustomHUD.h"
#include "..\XR_3DA\igame_persistent.h"
#include "..\XR_3DA\environment.h"
#include "..\XR_3DA\SkeletonCustom.h"
#include "..\xrRender\LightTrack.h"
#include "..\XR_3DA\GameFont.h"

CRender										RImplementation;

//////////////////////////////////////////////////////////////////////////
class CGlow : public IRender_Glow
{
public:
	bool				bActive;
public:
	CGlow( ) : bActive(false)
	{ }
	virtual void					set_active(bool b)
	{
		bActive = b;
	}
	virtual bool					get_active( )
	{
		return bActive;
	}
	virtual void					set_position(const fVector3& P)
	{ }
	virtual void					set_direction(const fVector3& D)
	{ }
	virtual void					set_radius(f32 R)
	{ }
	virtual void					set_texture(pcstr name)
	{ }
	virtual void					set_color(const fColor& C)
	{ }
	virtual void					set_color(f32 r, f32 g, f32 b)
	{ }
};

f32		r_dtex_range = 50.0f;
//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_dynamic(IRenderVisual* pVisual, f32 cdist_sq)
{
	int		id = SE_R2_SHADOW;
	if (CRender::PHASE_NORMAL == RImplementation.phase)
	{
		id = ((_sqrt(cdist_sq) - pVisual->vis.sphere.R) < r_dtex_range) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
	}
	return pVisual->shader->E[id]._get( );
}
//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_static(IRenderVisual* pVisual, f32 cdist_sq)
{
	int		id = SE_R2_SHADOW;
	if (CRender::PHASE_NORMAL == RImplementation.phase)
	{
		id = ((_sqrt(cdist_sq) - pVisual->vis.sphere.R) < r_dtex_range) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
	}
	return pVisual->shader->E[id]._get( );
}
static class cl_parallax : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		f32			h = ps_r2_df_parallax_h;
		RCache.set_c(C, h, -h / 2.f, 1.f / r_dtex_range, 1.f / r_dtex_range);
	}
}	binder_parallax;

extern ENGINE_API BOOL r2_sun_static;
//////////////////////////////////////////////////////////////////////////
// Just two static storage
void					CRender::create( )
{
	Device.seqFrame.Add(this, REG_PRIORITY_HIGH + 0x12345678);

	m_skinning = -1;

	// hardware
	o.smapsize = 2048;
	o.mrt = (HW.Caps.raster.dwMRT_count >= 3);
	o.mrtmixdepth = (HW.Caps.raster.b_MRT_mixdepth);

	// Check for NULL render target support
	D3DFORMAT	nullrt = (D3DFORMAT)MAKEFOURCC('N', 'U', 'L', 'L');
	o.nullrt = HW.support(nullrt, D3DRTYPE_SURFACE, D3DUSAGE_RENDERTARGET);
	/*
	if (o.nullrt)		{
	Msg				("* NULLRT supported and used");
	};
	*/
	if (o.nullrt)
	{
		Msg("* NULLRT supported");

		//.	    _tzset			();
		//.		??? _strdate	( date, 128 );	???
		//.		??? if (date < 22-march-07)		
		if (0)
		{
			u32 device_id = HW.Caps.id_device;
			bool disable_nullrt = false;
			switch (device_id)
			{
				case 0x190:
				case 0x191:
				case 0x192:
				case 0x193:
				case 0x194:
				case 0x197:
				case 0x19D:
				case 0x19E:
				{
					disable_nullrt = true;	//G80
					break;
				}
				case 0x400:
				case 0x401:
				case 0x402:
				case 0x403:
				case 0x404:
				case 0x405:
				case 0x40E:
				case 0x40F:
				{
					disable_nullrt = true;	//G84
					break;
				}
				case 0x420:
				case 0x421:
				case 0x422:
				case 0x423:
				case 0x424:
				case 0x42D:
				case 0x42E:
				case 0x42F:
				{
					disable_nullrt = true;	// G86
					break;
				}
			}
			if (disable_nullrt)	o.nullrt = false;
		};
		if (o.nullrt)	Msg("* ...and used");
	};


	// SMAP / DST
	o.HW_smap_FETCH4 = FALSE;
	o.HW_smap = HW.support(D3DFMT_D24X8, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL);
	o.HW_smap_PCF = o.HW_smap;
	if (o.HW_smap)
	{
		o.HW_smap_FORMAT = D3DFMT_D24X8;
		Msg("* HWDST/PCF supported and used");
	}

	o.fp16_filter = HW.support(D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER);
	o.fp16_blend = HW.support(D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING);

	// search for ATI formats
	if (!o.HW_smap && (0 == strstr(Core.Params, "-nodf24")))
	{
		o.HW_smap = HW.support((D3DFORMAT)(MAKEFOURCC('D', 'F', '2', '4')), D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL);
		if (o.HW_smap)
		{
			o.HW_smap_FORMAT = MAKEFOURCC('D', 'F', '2', '4');
			o.HW_smap_PCF = FALSE;
			o.HW_smap_FETCH4 = TRUE;
		}
		Msg("* DF24/F4 supported and used [%X]", o.HW_smap_FORMAT);
	}

	// emulate ATI-R4xx series
	if (strstr(Core.Params, "-r4xx"))
	{
		o.mrtmixdepth = FALSE;
		o.HW_smap = FALSE;
		o.HW_smap_PCF = FALSE;
		o.fp16_filter = FALSE;
		o.fp16_blend = FALSE;
	}

	VERIFY2(o.mrt && (HW.Caps.raster.dwInstructions >= 256), "Hardware doesn't meet minimum feature-level");
	if (o.mrtmixdepth)		o.albedo_wo = FALSE;
	else if (o.fp16_blend)	o.albedo_wo = FALSE;
	else					o.albedo_wo = TRUE;

	// nvstencil on NV40 and up
	o.nvstencil = FALSE;
	if ((HW.Caps.id_vendor == 0x10DE) && (HW.Caps.id_device >= 0x40))	o.nvstencil = TRUE;
	if (strstr(Core.Params, "-nonvs"))		o.nvstencil = FALSE;

	// nv-dbt
	o.nvdbt = HW.support((D3DFORMAT)MAKEFOURCC('N', 'V', 'D', 'B'), D3DRTYPE_SURFACE, 0);
	if (o.nvdbt)		Msg("* NV-DBT supported and used");

	// options (smap-pool-size)
	if (strstr(Core.Params, "-smap1536"))	o.smapsize = 1536;
	if (strstr(Core.Params, "-smap2048"))	o.smapsize = 2048;
	if (strstr(Core.Params, "-smap2560"))	o.smapsize = 2560;
	if (strstr(Core.Params, "-smap3072"))	o.smapsize = 3072;
	if (strstr(Core.Params, "-smap4096"))	o.smapsize = 4096;

	// gloss
	char* g = strstr(Core.Params, "-gloss ");
	o.forcegloss = g ? TRUE : FALSE;
	if (g)
	{
		o.forcegloss_v = f32(atoi(g + xr_strlen("-gloss "))) / 255.f;
	}

	// options
	o.bug = (strstr(Core.Params, "-bug")) ? TRUE : FALSE;
	o.sunfilter = (strstr(Core.Params, "-sunfilter")) ? TRUE : FALSE;
	//.	o.sunstatic			= (strstr(Core.Params,"-sunstatic"))?	TRUE	:FALSE	;
	o.sunstatic = r2_sun_static;
	o.sjitter = (strstr(Core.Params, "-sjitter")) ? TRUE : FALSE;
	o.depth16 = (strstr(Core.Params, "-depth16")) ? TRUE : FALSE;
	o.noshadows = (strstr(Core.Params, "-noshadows")) ? TRUE : FALSE;
	o.Tshadows = (strstr(Core.Params, "-tsh")) ? TRUE : FALSE;
	o.mblur = (strstr(Core.Params, "-mblur")) ? TRUE : FALSE;
	o.distortion_enabled = (strstr(Core.Params, "-nodistort")) ? FALSE : TRUE;
	o.distortion = o.distortion_enabled;
	o.disasm = (strstr(Core.Params, "-disasm")) ? TRUE : FALSE;
	o.forceskinw = (strstr(Core.Params, "-skinw")) ? TRUE : FALSE;

	// constants
	::Device.Resources->RegisterConstantSetup("parallax", &binder_parallax);

	c_lmaterial = "L_material";
	c_sbase = "s_base";

	Target = xr_new<CRenderTarget>( );	// Main target

	Models = xr_new<CModelPool>( );
	PSLibrary.OnCreate( );
	HWOCC.occq_create(occq_size);

	//rmNormal					();
	marker = 0;
	R_CHK(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &q_sync_point[0]));
	R_CHK(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &q_sync_point[1]));

	xrRender_apply_tf( );
	::PortalTraverser.initialize( );
}

void					CRender::destroy( )
{
	::PortalTraverser.destroy( );
	_RELEASE(q_sync_point[1]);
	_RELEASE(q_sync_point[0]);
	HWOCC.occq_destroy( );
	xr_delete(Models);
	xr_delete(Target);
	PSLibrary.OnDestroy( );
	Device.seqFrame.Remove(this);
}

void CRender::reset_begin( )
{
	// Update incremental shadowmap-visibility solver
	// BUG-ID: 10646
	{
		u32 it = 0;
		for (it = 0; it < Lights_LastFrame.size( ); it++)
		{
			if (0 == Lights_LastFrame[it])	continue;
			try
			{
				Lights_LastFrame[it]->svis.resetoccq( );
			}
			catch (...)
			{
				Msg("! Failed to flush-OCCq on light [%d] %X", it, *(u32*)(&Lights_LastFrame[it]));
			}
		}
		Lights_LastFrame.clear( );
	}

	xr_delete(Target);
	HWOCC.occq_destroy( );
	_RELEASE(q_sync_point[1]);
	_RELEASE(q_sync_point[0]);
}

void CRender::reset_end( )
{
	R_CHK(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &q_sync_point[0]));
	R_CHK(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &q_sync_point[1]));
	HWOCC.occq_create(occq_size);

	Target = xr_new<CRenderTarget>( );

	xrRender_apply_tf( );
}
/*
void CRender::OnFrame()
{
	Models->DeleteQueue			();
	if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_CALC))	{
		Device.seqParallel.insert	(Device.seqParallel.begin(),
			fastdelegate::FastDelegate0<>(&HOM,&CHOM::MT_RENDER));
	}
}*/
void CRender::OnFrame( )
{
	Models->DeleteQueue( );
	if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_CALC))
	{
// MT-details (@front)
		Device.seqParallel.insert(Device.seqParallel.begin( ),
								  fastdelegate::FastDelegate0<>(Details, &CDetailManager::MT_CALC));

							  // MT-HOM (@front)
		Device.seqParallel.insert(Device.seqParallel.begin( ),
								  fastdelegate::FastDelegate0<>(&HOM, &CHOM::MT_RENDER));
	}
}


// Implementation
IRender_ObjectSpecific* CRender::ros_create(IRenderable* parent)
{
	return xr_new<CROS_impl>( );
}
void					CRender::ros_destroy(IRender_ObjectSpecific*& p)
{
	xr_delete(p);
}
IRenderVisual* CRender::model_Create(pcstr name, IReader* data)
{
	return Models->Create(name, data);
}
IRenderVisual* CRender::model_CreateChild(pcstr name, IReader* data)
{
	return Models->CreateChild(name, data);
}
IRenderVisual* CRender::model_Duplicate(IRenderVisual* V)
{
	return Models->Instance_Duplicate(V);
}
void					CRender::model_Delete(IRenderVisual*& V, BOOL bDiscard)
{
	Models->Delete(V, bDiscard);
}
IRender_DetailModel* CRender::model_CreateDM(IReader* F)
{
	CDetail* D = xr_new<CDetail>( );
	D->Load(F);
	return D;
}
void					CRender::model_Delete(IRender_DetailModel*& F)
{
	if (F)
	{
		CDetail* D = (CDetail*)F;
		D->Unload( );
		xr_delete(D);
		F = NULL;
	}
}
IRenderVisual* CRender::model_CreatePE(pcstr name)
{
	PS::CPEDef* SE = PSLibrary.FindPED(name);
	R_ASSERT3(SE, "Particle effect doesn't exist", name);
	return					Models->CreatePE(SE);
}
IRenderVisual* CRender::model_CreateParticles(pcstr name)
{
	PS::CPEDef* SE = PSLibrary.FindPED(name);
	if (SE) return			Models->CreatePE(SE);
	else
	{
		PS::CPGDef* SG = PSLibrary.FindPGD(name);
		R_ASSERT3(SG, "Particle effect or group doesn't exist", name);
		return				Models->CreatePG(SG);
	}
}
void					CRender::models_Prefetch( )
{
	Models->Prefetch( );
}
void					CRender::models_Clear(BOOL b_complete)
{
	Models->ClearPool(b_complete);
}

ref_shader				CRender::getShader(int id)
{
	VERIFY(id<int(Shaders.size( )));
	return Shaders[id];
}
IRender_Portal* CRender::getPortal(int id)
{
	VERIFY(id<int(Portals.size( )));
	return Portals[id];
}
IRender_Sector* CRender::getSector(int id)
{
	VERIFY(id<int(Sectors.size( )));
	return Sectors[id];
}
IRender_Sector* CRender::getSectorActive( )
{
	return pLastSector;
}
IRenderVisual* CRender::getVisual(int id)
{
	VERIFY(id<int(Visuals.size( )));
	return Visuals[id];
}
D3DVERTEXELEMENT9* CRender::getVB_Format(int id, BOOL	_alt)
{
	if (_alt)
	{
		VERIFY(id<int(xDC.size( )));
		return xDC[id].begin( );
	}
	else
	{
		VERIFY(id<int(nDC.size( )));
		return nDC[id].begin( );
	}
}
IDirect3DVertexBuffer9* CRender::getVB(int id, BOOL	_alt)
{
	if (_alt)
	{
		VERIFY(id<int(xVB.size( )));
		return xVB[id];
	}
	else
	{
		VERIFY(id<int(nVB.size( )));
		return nVB[id];
	}
}
IDirect3DIndexBuffer9* CRender::getIB(int id, BOOL	_alt)
{
	if (_alt)
	{
		VERIFY(id<int(xIB.size( )));
		return xIB[id];
	}
	else
	{
		VERIFY(id<int(nIB.size( )));
		return nIB[id];
	}
}
FSlideWindowItem* CRender::getSWI(int id)
{
	VERIFY(id<int(SWIs.size( )));
	return &SWIs[id];
}
IRender_Target* CRender::getTarget( )
{
	return Target;
}

IRender_Light* CRender::light_create( )
{
	return Lights.Create( );
}
IRender_Glow* CRender::glow_create( )
{
	return xr_new<CGlow>( );
}

void					CRender::flush( )
{
	r_dsgraph_render_graph(0);
}

BOOL					CRender::occ_visible(vis_data& P)
{
	return HOM.visible(P);
}
BOOL					CRender::occ_visible(sPoly& P)
{
	return HOM.visible(P);
}
BOOL					CRender::occ_visible(fBox3& P)
{
	return HOM.visible(P);
}

void					CRender::add_Visual(IRenderVisual* V)
{
	add_leafs_Dynamic(V);
}
void					CRender::add_Geometry(IRenderVisual* V)
{
	add_Static(V, View->getMask( ));
}
void					CRender::add_StaticWallmark(ref_shader& S, const fVector3& P, f32 s, CDB::TRI* T, fVector3* verts)
{
	if (T->suppress_wm)	return;
	VERIFY2(_valid(P) && _valid(s) && T && verts && (s > EPSILON_3), "Invalid static wallmark params");
	Wallmarks->AddStaticWallmark(T, verts, P, &*S, s);
}

void					CRender::clear_static_wallmarks( )
{
	Wallmarks->clear( );
}

void					CRender::add_SkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm)
{
	Wallmarks->AddSkeletonWallmark(wm);
}
void					CRender::add_SkeletonWallmark(const fMatrix4x4* xf, CKinematics* obj, ref_shader& sh, const fVector3& start, const fVector3& dir, f32 size)
{
	Wallmarks->AddSkeletonWallmark(xf, obj, sh, start, dir, size);
}
void					CRender::add_Occluder(fBox2& bb_screenspace)
{
	HOM.occlude(bb_screenspace);
}
void					CRender::set_Object(IRenderable* O)
{
	val_pObject = O;
}
void					CRender::rmNear( )
{
	IRender_Target* T = getTarget( );
	D3DVIEWPORT9 VP = {0,0,T->get_width( ),T->get_height( ),0,0.02f};
	CHK_DX(HW.pDevice->SetViewport(&VP));
}
void					CRender::rmFar( )
{
	IRender_Target* T = getTarget( );
	D3DVIEWPORT9 VP = {0,0,T->get_width( ),T->get_height( ),0.99999f,1.f};
	CHK_DX(HW.pDevice->SetViewport(&VP));
}
void					CRender::rmNormal( )
{
	IRender_Target* T = getTarget( );
	D3DVIEWPORT9 VP = {0,0,T->get_width( ),T->get_height( ),0,1.f};
	CHK_DX(HW.pDevice->SetViewport(&VP));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender( )
{ }

CRender::~CRender( )
{ }

void	CRender::Statistics(CGameFont* _F)
{
	CGameFont& F = *_F;
	F.OutNext(" **** LT:%2d,LV:%2d **** ", stats.l_total, stats.l_visible);	stats.l_visible = 0;
	F.OutNext("    S(%2d)   | (%2d)NS   ", stats.l_shadowed, stats.l_unshadowed);
	F.OutNext("smap use[%2d], merge[%2d], finalclip[%2d]", stats.s_used, stats.s_merged - stats.s_used, stats.s_finalclip);
	stats.s_used = 0; stats.s_merged = 0; stats.s_finalclip = 0;
	F.OutSkip( );
	F.OutNext(" **** Occ-Q(%03.1f) **** ", 100.0f * f32(stats.o_culled) / f32(stats.o_queries ? stats.o_queries : 1));
	F.OutNext(" total  : %2d", stats.o_queries);	stats.o_queries = 0;
	F.OutNext(" culled : %2d", stats.o_culled);	stats.o_culled = 0;
	F.OutSkip( );
	u32	ict = stats.ic_total + stats.ic_culled;
	F.OutNext(" **** iCULL(%03.1f) **** ", 100.0f * f32(stats.ic_culled) / f32(ict ? ict : 1));
	F.OutNext(" visible: %2d", stats.ic_total);	stats.ic_total = 0;
	F.OutNext(" culled : %2d", stats.ic_culled);	stats.ic_culled = 0;
#ifdef DEBUG
	HOM.stats( );
#endif
}

#pragma comment(lib,"d3dx9.lib")
HRESULT	CRender::shader_compile(
	pcstr							name,
	pcstr                          pSrcData,
	UINT                            SrcDataLen,
	void* _pDefines,
	void* _pInclude,
	pcstr                          pFunctionName,
	pcstr                          pTarget,
	DWORD                           Flags,
	void* _ppShader,
	void* _ppErrorMsgs,
	void* _ppConstantTable)
{
	D3DXMACRO						defines[128];
	int								def_it = 0;
	CONST D3DXMACRO* pDefines = (CONST D3DXMACRO*)	_pDefines;
	char							c_smapsize[32];
	char							c_gloss[32];
	if (pDefines)
	{
// transfer existing defines
		for (;; def_it++)
		{
			if (0 == pDefines[def_it].Name)	break;
			defines[def_it] = pDefines[def_it];
		}
	}
	// options
	{
		sprintf(c_smapsize, "%d", u32(o.smapsize));
		defines[def_it].Name = "SMAP_size";
		defines[def_it].Definition = c_smapsize;
		def_it++;
	}
	if (o.fp16_filter)
	{
		defines[def_it].Name = "FP16_FILTER";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.fp16_blend)
	{
		defines[def_it].Name = "FP16_BLEND";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.HW_smap)
	{
		defines[def_it].Name = "USE_HWSMAP";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.HW_smap_PCF)
	{
		defines[def_it].Name = "USE_HWSMAP_PCF";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.HW_smap_FETCH4)
	{
		defines[def_it].Name = "USE_FETCH4";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.sjitter)
	{
		defines[def_it].Name = "USE_SJITTER";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (HW.Caps.raster_major >= 3)
	{
		defines[def_it].Name = "USE_BRANCHING";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (HW.Caps.geometry.bVTF)
	{
		defines[def_it].Name = "USE_VTF";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.Tshadows)
	{
		defines[def_it].Name = "USE_TSHADOWS";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.mblur)
	{
		defines[def_it].Name = "USE_MBLUR";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.sunfilter)
	{
		defines[def_it].Name = "USE_SUNFILTER";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.sunstatic)
	{
		defines[def_it].Name = "USE_R2_STATIC_SUN";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.forcegloss)
	{
		sprintf(c_gloss, "%f", o.forcegloss_v);
		defines[def_it].Name = "FORCE_GLOSS";
		defines[def_it].Definition = c_gloss;
		def_it++;
	}
	if (o.forceskinw)
	{
		defines[def_it].Name = "SKIN_COLOR";
		defines[def_it].Definition = "1";
		def_it++;
	}

	// skinning
	if (m_skinning < 0)
	{
		defines[def_it].Name = "SKIN_NONE";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (0 == m_skinning)
	{
		defines[def_it].Name = "SKIN_0";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (1 == m_skinning)
	{
		defines[def_it].Name = "SKIN_1";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (2 == m_skinning)
	{
		defines[def_it].Name = "SKIN_2";
		defines[def_it].Definition = "1";
		def_it++;
	}

	// finish
	defines[def_it].Name = 0;
	defines[def_it].Definition = 0;
	def_it++;

	// 
	if (0 == xr_strcmp(pFunctionName, "main"))
	{
		if ('v' == pTarget[0])			pTarget = D3DXGetVertexShaderProfile(HW.pDevice);	// vertex	"vs_2_a"; //	
		else							pTarget = D3DXGetPixelShaderProfile(HW.pDevice);	// pixel	"ps_2_a"; //	
	}

	LPD3DXINCLUDE                   pInclude = (LPD3DXINCLUDE)_pInclude;
	LPD3DXBUFFER* ppShader = (LPD3DXBUFFER*)_ppShader;
	LPD3DXBUFFER* ppErrorMsgs = (LPD3DXBUFFER*)_ppErrorMsgs;
	LPD3DXCONSTANTTABLE* ppConstantTable = (LPD3DXCONSTANTTABLE*)_ppConstantTable;

#ifdef	D3DXSHADER_USE_LEGACY_D3DX9_31_DLL	//	December 2006 and later
	HRESULT		_result = D3DXCompileShader(pSrcData, SrcDataLen, defines, pInclude, pFunctionName, pTarget, Flags | D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, ppShader, ppErrorMsgs, ppConstantTable);
#else
	HRESULT		_result = D3DXCompileShader(pSrcData, SrcDataLen, defines, pInclude, pFunctionName, pTarget, Flags, ppShader, ppErrorMsgs, ppConstantTable);
#endif
	if (SUCCEEDED(_result) && o.disasm)
	{
		ID3DXBuffer* code = *((LPD3DXBUFFER*)_ppShader);
		ID3DXBuffer* disasm = 0;
		D3DXDisassembleShader(LPDWORD(code->GetBufferPointer( )), FALSE, 0, &disasm);
		string_path			dname;
		strconcat(sizeof(dname), dname, "disasm\\", name, ('v' == pTarget[0]) ? ".vs" : ".ps");
		IWriter* W = FS.w_open("$logs$", dname);
		W->w(disasm->GetBufferPointer( ), disasm->GetBufferSize( ));
		FS.w_close(W);
		_RELEASE(disasm);
	}
	return		_result;
}

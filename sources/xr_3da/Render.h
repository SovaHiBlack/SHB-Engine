#pragma once

#include "frustum.h"
#include "vis_common.h"
#include "IRenderDetailModel.h"

// refs
class ENGINE_API	IRenderable;
class ENGINE_API	IRenderVisual;
class ENGINE_API	IBlender;
class ENGINE_API	CSkeletonWallmark;
class ENGINE_API	CKinematics;
struct ENGINE_API	FSlideWindowItem;

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Light)
class	ENGINE_API	IRender_Light : public xr_resource
{
public:
	enum LT
	{
		DIRECT = 0,
		POINT = 1,
		SPOT = 2,
		OMNIPART = 3,
		REFLECTED = 4
	};

public:
	virtual void					set_type(LT type) = 0;
	virtual void					set_active(bool) = 0;
	virtual bool					get_active( ) = 0;
	virtual void					set_shadow(bool) = 0;
	virtual void					set_indirect(bool)
	{ }
	virtual void					set_position(const fVector3& P) = 0;
	virtual void					set_rotation(const fVector3& D, const fVector3& R) = 0;
	virtual void					set_cone(f32 angle) = 0;
	virtual void					set_range(f32 R) = 0;
	virtual void					set_virtual_size(f32 R) = 0;
	virtual void					set_texture(pcstr name) = 0;
	virtual void					set_color(const fColor& C) = 0;
	virtual void					set_color(f32 r, f32 g, f32 b) = 0;
	virtual ~IRender_Light( );
};
struct ENGINE_API		resptrcode_light : public resptr_base<IRender_Light>
{
	void				destroy( )
	{
		_set(NULL);
	}
};
typedef	resptr_core<IRender_Light, resptrcode_light >	ref_light;

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Glow)
class	ENGINE_API		IRender_Glow : public xr_resource
{
public:
	virtual void					set_active(bool) = 0;
	virtual bool					get_active( ) = 0;
	virtual void					set_position(const fVector3& P) = 0;
	virtual void					set_direction(const fVector3& P) = 0;
	virtual void					set_radius(f32 R) = 0;
	virtual void					set_texture(pcstr name) = 0;
	virtual void					set_color(const fColor& C) = 0;
	virtual void					set_color(f32 r, f32 g, f32 b) = 0;
	virtual ~IRender_Glow( );
};
struct ENGINE_API		resptrcode_glow : public resptr_base<IRender_Glow>
{
	void				destroy( )
	{
		_set(NULL);
	}
};
typedef	resptr_core<IRender_Glow, resptrcode_glow >		ref_glow;

//////////////////////////////////////////////////////////////////////////
// definition (Per-object render-specific data)
class	ENGINE_API	IRender_ObjectSpecific
{
public:
	enum mode
	{
		TRACE_LIGHTS = (1 << 0),
		TRACE_SUN = (1 << 1),
		TRACE_HEMI = (1 << 2),
		TRACE_ALL = (TRACE_LIGHTS | TRACE_SUN | TRACE_HEMI)
	};

public:
	virtual void					force_mode(u32 mode) = 0;
	virtual f32						get_luminocity( ) = 0;
	virtual f32						get_luminocity_hemi( ) = 0;

	virtual ~IRender_ObjectSpecific( )
	{ }
};

//////////////////////////////////////////////////////////////////////////
// definition (Portal)
class	ENGINE_API	IRender_Portal
{
public:
	virtual ~IRender_Portal( )
	{ }
};

//////////////////////////////////////////////////////////////////////////
// definition (Sector)
class	ENGINE_API	IRender_Sector
{
public:
	virtual ~IRender_Sector( )
	{ }
};

//////////////////////////////////////////////////////////////////////////
// definition (Target)
class	ENGINE_API	IRender_Target
{
public:
	virtual	void					set_blur(f32 f) = 0;
	virtual	void					set_gray(f32 f) = 0;
	virtual void					set_duality_h(f32 f) = 0;
	virtual void					set_duality_v(f32 f) = 0;
	virtual void					set_noise(f32 f) = 0;
	virtual void					set_noise_scale(f32 f) = 0;
	virtual void					set_noise_fps(f32 f) = 0;
	virtual void					set_color_base(u32 f) = 0;
	virtual void					set_color_gray(u32 f) = 0;
	virtual void					set_color_add(u32 f) = 0;
	virtual u32						get_width( ) = 0;
	virtual u32						get_height( ) = 0;
	virtual ~IRender_Target( )
	{ }
};

//////////////////////////////////////////////////////////////////////////
// definition (Renderer)
class	ENGINE_API	IRender_interface
{
public:
	enum GenerationLevel
	{
		GENERATION_R1 = 81,
		GENERATION_DX81 = 81,
		GENERATION_R2 = 90,
		GENERATION_DX90 = 90,
		GENERATION_forcedword = u32(-1)
	};
	enum ScreenshotMode
	{
		SM_NORMAL = 0,				// jpeg,	name ignored
		SM_FOR_CUBEMAP = 1,			// tga,		name used as postfix
		SM_FOR_GAMESAVE = 2,		// dds/dxt1,name used as full-path
		SM_FOR_LEVELMAP = 3,		// tga,		name used as postfix (level_name)
		SM_forcedword = u32(-1)
	};

public:
	// options
	s32								m_skinning;
	// data
	CFrustum						ViewBase;
	CFrustum* View;

public:
	// feature level
	virtual GenerationLevel			get_generation( ) = 0;
	// Loading / Unloading
	virtual void					create( ) = 0;
	virtual void					destroy( ) = 0;
	virtual void					reset_begin( ) = 0;
	virtual void					reset_end( ) = 0;

	virtual void					level_Load(IReader*) = 0;
	virtual void					level_Unload( ) = 0;

	virtual IDirect3DBaseTexture9* texture_load(pcstr	fname, u32& msize) = 0;
	void					shader_option_skinning(s32 mode)
	{
		m_skinning = mode;
	}
	virtual HRESULT					shader_compile(
		pcstr							name,
		pcstr                          pSrcData,
		UINT                            SrcDataLen,
		pvoid pDefines,
		pvoid pInclude,
		pcstr                          pFunctionName,
		pcstr                          pTarget,
		DWORD                           Flags,
		pvoid ppShader,
		pvoid ppErrorMsgs,
		pvoid ppConstantTable) = 0;

	// Information
	virtual	void					Statistics(CGameFont* F)
	{ }

	virtual pcstr					getShaderPath( ) = 0;
	virtual ref_shader				getShader(s32 id) = 0;
	virtual IRender_Sector* getSector(s32 id) = 0;
	virtual IRenderVisual* getVisual(s32 id) = 0;
	virtual IRender_Sector* detectSector(const fVector3& P) = 0;
	virtual IRender_Target* getTarget( ) = 0;

	// Main
	IC		void					set_Frustum(CFrustum* O)
	{
		VERIFY(O);
		View = O;
	}
	virtual void					set_Transform(fMatrix4x4* M) = 0;
	virtual void					set_HUD(BOOL V) = 0;
	virtual BOOL					get_HUD( ) = 0;
	virtual void					set_Invisible(BOOL V) = 0;
	virtual void					flush( ) = 0;
	virtual void					set_Object(IRenderable* O) = 0;
	virtual void					add_Occluder(fBox2& bb_screenspace) = 0;	// mask screen region as oclluded (-1..1, -1..1)
	virtual void					add_Visual(IRenderVisual* V) = 0;	// add visual leaf	(no culling performed at all)
	virtual void					add_Geometry(IRenderVisual* V) = 0;	// add visual(s)	(all culling performed)
	virtual void					add_StaticWallmark(ref_shader& S, const fVector3& P, f32 s, CDB::TRI* T, fVector3* V) = 0;
	virtual void					clear_static_wallmarks( ) = 0;
	virtual void					add_SkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm) = 0;
	virtual void					add_SkeletonWallmark(const fMatrix4x4* xf, CKinematics* obj, ref_shader& sh, const fVector3& start, const fVector3& dir, f32 size) = 0;

	virtual IBlender* blender_create(CLASS_ID cls) = 0;
	virtual void					blender_destroy(IBlender*&) = 0;

	virtual IRender_ObjectSpecific* ros_create(IRenderable* parent) = 0;
	virtual void					ros_destroy(IRender_ObjectSpecific*&) = 0;

	// Lighting/glowing
	virtual IRender_Light* light_create( ) = 0;
	virtual void					light_destroy(IRender_Light* p_)
	{ }
	virtual IRender_Glow* glow_create( ) = 0;
	virtual void					glow_destroy(IRender_Glow* p_)
	{ }

	// Models
	virtual IRenderVisual* model_CreateParticles(pcstr name) = 0;
	virtual IRender_DetailModel* model_CreateDM(IReader* F) = 0;
	virtual IRenderVisual* model_Create(pcstr name, IReader* data = 0) = 0;
	virtual IRenderVisual* model_CreateChild(pcstr name, IReader* data) = 0;
	virtual IRenderVisual* model_Duplicate(IRenderVisual* V) = 0;
	virtual void					model_Delete(IRenderVisual*& V, BOOL bDiscard = FALSE) = 0;
	virtual void 					model_Delete(IRender_DetailModel*& F) = 0;
	virtual void					model_Logging(BOOL bEnable) = 0;
	virtual void					models_Prefetch( ) = 0;
	virtual void					models_Clear(BOOL b_complete) = 0;

	// Occlusion culling
	virtual BOOL					occ_visible(vis_data& V) = 0;
	virtual BOOL					occ_visible(fBox3& B) = 0;
	virtual BOOL					occ_visible(sPoly& P) = 0;

	// Main
	virtual void					Calculate( ) = 0;
	virtual void					Render( ) = 0;
	virtual void					Screenshot(ScreenshotMode mode = SM_NORMAL, pcstr name = 0) = 0;

	// Render mode
	virtual void					rmNear( ) = 0;
	virtual void					rmFar( ) = 0;
	virtual void					rmNormal( ) = 0;
	virtual u32						memory_usage( ) = 0;

	// Constructor/destructor
	virtual ~IRender_interface( );
};

extern ENGINE_API	IRender_interface* Render;

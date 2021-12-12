#pragma once

//#define RBackend_PGO

#ifdef	RBackend_PGO
#define PGO(a)	a
#else
#define PGO(a)
#endif

#include "r_DStreams.h"
#include "r_constants_cache.h"
#include "r_backend_xform.h"
#include "fvf.h"

const unsigned int		CULL_CCW = D3DCULL_CCW;
const unsigned int		CULL_CW = D3DCULL_CW;
const unsigned int		CULL_NONE = D3DCULL_NONE;

///		detailed statistic
struct	R_statistics_element
{
	unsigned int verts;
	unsigned int dips;
	__forceinline void	add(unsigned int _verts)
	{
		verts += _verts;
		dips++;
	}
};

struct	R_statistics
{
	R_statistics_element		s_static;
	R_statistics_element		s_flora;
	R_statistics_element		s_flora_lods;
	R_statistics_element		s_details;
	R_statistics_element		s_ui;
	R_statistics_element		s_dynamic;
	R_statistics_element		s_dynamic_sw;
	R_statistics_element		s_dynamic_inst;
	R_statistics_element		s_dynamic_1B;
	R_statistics_element		s_dynamic_2B;
};

class ENGINE_API CBackend
{
public:
	// Dynamic geometry streams
	_VertexStream					Vertex;
	_IndexStream					Index;
	IDirect3DIndexBuffer9*		QuadIB;
	IDirect3DIndexBuffer9*		old_QuadIB;
	R_xforms						xforms;

private:
	// Render-targets
	IDirect3DSurface9* pRT[4];
	IDirect3DSurface9* pZB;

	// Vertices/Indices/etc
	IDirect3DVertexDeclaration9* decl;
	IDirect3DVertexBuffer9* vb;
	IDirect3DIndexBuffer9* ib;
	unsigned int								vb_stride;

	// Pixel/Vertex constants
	ALIGN(16)	R_constants			constants;
	R_constant_table* ctable;

	// Shaders/State
	IDirect3DStateBlock9* state;
	IDirect3DPixelShader9* ps;
	IDirect3DVertexShader9* vs;

#ifdef DEBUG
	const char* ps_name;
	const char* vs_name;
#endif // DEBUG

	unsigned int								stencil_enable;
	unsigned int								stencil_func;
	unsigned int								stencil_ref;
	unsigned int								stencil_mask;
	unsigned int								stencil_writemask;
	unsigned int								stencil_fail;
	unsigned int								stencil_pass;
	unsigned int								stencil_zfail;
	unsigned int								colorwrite_mask;
	unsigned int								cull_mode;

	// Lists
	STextureList* T;
	SMatrixList* M;
	SConstantList* C;

	// Lists-expanded
	CTexture* textures_ps[16];	// stages
	CTexture* textures_vs[5];	// dmap + 4 vs

	void							Invalidate( );

public:
	struct _stats
	{
		unsigned int								polys;
		unsigned int								verts;
		unsigned int								calls;
		unsigned int								vs;
		unsigned int								ps;

#ifdef DEBUG
		unsigned int								decl;
		unsigned int								vb;
		unsigned int								ib;
		unsigned int								states;			// Number of times the shader-state changes
		unsigned int								textures;		// Number of times the shader-tex changes
		unsigned int								matrices;		// Number of times the shader-xform changes
		unsigned int								constants;		// Number of times the shader-consts changes
#endif // DEBUG

		unsigned int								xforms;
		unsigned int								target_rt;
		unsigned int								target_zb;

		R_statistics					r;
	}									stat;

public:
	inline CTexture* get_ActiveTexture(unsigned int stage)
	{
		if (stage >= 256)
		{
			return textures_vs[stage - 256];
		}
		else
		{
			return textures_ps[stage];
		}
	}
	inline R_constant_array& get_ConstantCache_Vertex( )
	{
		return constants.a_vertex;
	}
	inline R_constant_array& get_ConstantCache_Pixel( )
	{
		return constants.a_pixel;
	}

// API
	inline void						set_xform(unsigned int ID, const Fmatrix& M);
	inline void						set_xform_world(const Fmatrix& M);
	inline void						set_xform_view(const Fmatrix& M);
	inline void						set_xform_project(const Fmatrix& M);
	inline const Fmatrix& get_xform_world( );
	inline const Fmatrix& get_xform_view( );
	inline const Fmatrix& get_xform_project( );

	inline void						set_RT(IDirect3DSurface9* RT, unsigned int ID = 0);
	inline void						set_ZB(IDirect3DSurface9* ZB);

	inline void						set_Constants(R_constant_table* C);
	inline void						set_Constants(ref_ctable& C)
	{
		set_Constants(&*C);
	}

	void						set_Textures(STextureList* T);
	inline void						set_Textures(ref_texture_list& T)
	{
		set_Textures(&*T);
	}

	inline void						set_Element(ShaderElement* S, unsigned int	pass = 0);
	inline void						set_Element(ref_selement& S, unsigned int	pass = 0)
	{
		set_Element(&*S, pass);
	}

	inline void						set_Shader(Shader* S, unsigned int pass = 0);
	inline void						set_Shader(ref_shader& S, unsigned int pass = 0)
	{
		set_Shader(&*S, pass);
	}

	__forceinline void						set_States(IDirect3DStateBlock9* _state);
	__forceinline void						set_States(ref_state& _state)
	{
		set_States(_state->state);
	}

	__forceinline void						set_Format(IDirect3DVertexDeclaration9* _decl);

	__forceinline void						set_PS(IDirect3DPixelShader9* _ps, const char* _n = 0);
	__forceinline void						set_PS(ref_ps& _ps)
	{
		set_PS(_ps->ps, _ps->cName.c_str( ));
	}

	__forceinline void						set_VS(IDirect3DVertexShader9* _vs, const char* _n = 0);
	__forceinline void						set_VS(ref_vs& _vs)
	{
		set_VS(_vs->vs, _vs->cName.c_str( ));
	}

	__forceinline void						set_Vertices(IDirect3DVertexBuffer9* _vb, unsigned int _vb_stride);
	__forceinline void						set_Indices(IDirect3DIndexBuffer9* _ib);
	__forceinline void						set_Geometry(SGeometry* _geom);
	__forceinline void						set_Geometry(ref_geom& _geom)
	{
		set_Geometry(&*_geom);
	}
	inline void						set_Stencil(unsigned int _enable, unsigned int _func = D3DCMP_ALWAYS, unsigned int _ref = 0x00, unsigned int _mask = 0x00, unsigned int _writemask = 0x00, unsigned int _fail = D3DSTENCILOP_KEEP, unsigned int _pass = D3DSTENCILOP_KEEP, unsigned int _zfail = D3DSTENCILOP_KEEP);
	inline void						set_ColorWriteEnable(unsigned int _mask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	inline void						set_CullMode(unsigned int _mode);
	inline unsigned int							get_CullMode( )
	{
		return cull_mode;
	}
	void							set_ClipPlanes(unsigned int _enable, Fplane* _planes = NULL, unsigned int count = 0);
	void							set_ClipPlanes(unsigned int _enable, Fmatrix* _xform = NULL, unsigned int fmask = 0xff);
	inline void						set_Scissor(Irect* rect = NULL);

	// constants
	__forceinline ref_constant				get_c(const char* n)
	{
		if (ctable)
		{
			return ctable->get(n);
		}
		else
		{
			return 0;
		}
	}
	__forceinline ref_constant				get_c(CSharedString& n)
	{
		if (ctable)
		{
			return ctable->get(n);
		}
		else
		{
			return 0;
		}
	}

// constants - direct (fast)
	__forceinline void						set_c(R_constant* C, const Fmatrix& A)
	{
		if (C)
		{
			constants.set(C, A);
		}
	}
	__forceinline void						set_c(R_constant* C, const Fvector4& A)
	{
		if (C)
		{
			constants.set(C, A);
		}
	}
	__forceinline void						set_c(R_constant* C, float x, float y, float z, float w)
	{
		if (C)
		{
			constants.set(C, x, y, z, w);
		}
	}
	__forceinline void						set_ca(R_constant* C, unsigned int e, const Fmatrix& A)
	{
		if (C)
		{
			constants.seta(C, e, A);
		}
	}
	__forceinline void						set_ca(R_constant* C, unsigned int e, const Fvector4& A)
	{
		if (C)
		{
			constants.seta(C, e, A);
		}
	}
	__forceinline void						set_ca(R_constant* C, unsigned int e, float x, float y, float z, float w)
	{
		if (C)
		{
			constants.seta(C, e, x, y, z, w);
		}
	}

// constants - const char* (slow)
	__forceinline void						set_c(const char* n, const Fmatrix& A)
	{
		if (ctable)
		{
			set_c(&*ctable->get(n), A);
		}
	}
	__forceinline void						set_c(const char* n, const Fvector4& A)
	{
		if (ctable)
		{
			set_c(&*ctable->get(n), A);
		}
	}
	__forceinline void						set_c(const char* n, float x, float y, float z, float w)
	{
		if (ctable)
		{
			set_c(&*ctable->get(n), x, y, z, w);
		}
	}
	__forceinline void						set_ca(const char* n, unsigned int e, const Fmatrix& A)
	{
		if (ctable)
		{
			set_ca(&*ctable->get(n), e, A);
		}
	}
	__forceinline void						set_ca(const char* n, unsigned int e, const Fvector4& A)
	{
		if (ctable)
		{
			set_ca(&*ctable->get(n), e, A);
		}
	}
	__forceinline void						set_ca(const char* n, unsigned int e, float x, float y, float z, float w)
	{
		if (ctable)
		{
			set_ca(&*ctable->get(n), e, x, y, z, w);
		}
	}

// constants - CSharedString (average)
	__forceinline void						set_c(CSharedString& n, const Fmatrix& A)
	{
		if (ctable)
		{
			set_c(&*ctable->get(n), A);
		}
	}
	__forceinline void						set_c(CSharedString& n, const Fvector4& A)
	{
		if (ctable)
		{
			set_c(&*ctable->get(n), A);
		}
	}
	__forceinline void						set_c(CSharedString& n, float x, float y, float z, float w)
	{
		if (ctable)
		{
			set_c(&*ctable->get(n), x, y, z, w);
		}
	}
	__forceinline void						set_ca(CSharedString& n, unsigned int e, const Fmatrix& A)
	{
		if (ctable)
		{
			set_ca(&*ctable->get(n), e, A);
		}
	}
	__forceinline void						set_ca(CSharedString& n, unsigned int e, const Fvector4& A)
	{
		if (ctable)
		{
			set_ca(&*ctable->get(n), e, A);
		}
	}
	__forceinline void						set_ca(CSharedString& n, unsigned int e, float x, float y, float z, float w)
	{
		if (ctable)
		{
			set_ca(&*ctable->get(n), e, x, y, z, w);
		}
	}

	__forceinline void						Render(D3DPRIMITIVETYPE T, unsigned int baseV, unsigned int startV, unsigned int countV, unsigned int startI, unsigned int PC);
	__forceinline void						Render(D3DPRIMITIVETYPE T, unsigned int startV, unsigned int PC);

	// Device create / destroy / frame signaling
	void							CreateQuadIB( );
	void							OnFrameBegin( );
	void							OnFrameEnd( );
	void							OnDeviceCreate( );
	void							OnDeviceDestroy( );

	// Debug render
	void dbg_DP(D3DPRIMITIVETYPE pt, ref_geom geom, unsigned int vBase, unsigned int pc);
	void dbg_DIP(D3DPRIMITIVETYPE pt, ref_geom geom, unsigned int baseV, unsigned int startV, unsigned int countV, unsigned int startI, unsigned int PC);
	inline void	dbg_SetRS(D3DRENDERSTATETYPE p1, unsigned int p2)
	{
		CHK_DX(HW.pDevice->SetRenderState(p1, p2));
	}
	inline void	dbg_SetSS(unsigned int sampler, D3DSAMPLERSTATETYPE type, unsigned int value)
	{
		CHK_DX(HW.pDevice->SetSamplerState(sampler, type, value));
	}

#ifdef DEBUG
	void dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, unsigned short* pIdx, int pcnt);
	void dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int pcnt);
	inline void dbg_DrawAABB(Fvector3& T, float sx, float sy, float sz, unsigned int C)
	{
		Fvector3 half_dim;
		half_dim.set(sx, sy, sz);
		Fmatrix	TM;
		TM.translate(T);
		dbg_DrawOBB(TM, half_dim, C);
	}
	void dbg_DrawOBB(Fmatrix& T, Fvector3& half_dim, unsigned int C);
	inline void dbg_DrawTRI(Fmatrix& T, Fvector3* p, unsigned int C)
	{
		dbg_DrawTRI(T, p[0], p[1], p[2], C);
	}
	void dbg_DrawTRI(Fmatrix& T, Fvector3& p1, Fvector3& p2, Fvector3& p3, unsigned int C);
	void dbg_DrawLINE(Fmatrix& T, Fvector3& p1, Fvector3& p2, unsigned int C);
	void dbg_DrawEllipse(Fmatrix& T, unsigned int C);
#endif // DEBUG

	CBackend( )
	{
		Invalidate( );
	}
};

extern ENGINE_API CBackend			RCache;

#include "D3DUtils.h"

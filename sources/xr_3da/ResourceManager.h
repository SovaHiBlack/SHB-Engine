// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include	"shader.h"
#include	"tss_def.h"
#include	"TextureDescrManager.h"
// refs
struct		lua_State;

// defs
class ENGINE_API CResourceManager
{
private:
	struct str_pred : public std::binary_function<char*, char*, bool>	{
		IC bool operator()(pcstr x, pcstr y) const
		{	return xr_strcmp(x,y)<0;	}
	};
	struct texture_detail	{
		pcstr				T;
		R_constant_setup*	cs;
	};
public:
	DEFINE_MAP_PRED(pcstr,IBlender*,		map_Blender,	map_BlenderIt,		str_pred);
	DEFINE_MAP_PRED(pcstr,CTexture*,		map_Texture,	map_TextureIt,		str_pred);
	DEFINE_MAP_PRED(pcstr,CMatrix*,			map_Matrix,		map_MatrixIt,		str_pred);
	DEFINE_MAP_PRED(pcstr,CConstant*,		map_Constant,	map_ConstantIt,		str_pred);
	DEFINE_MAP_PRED(pcstr,CRT*,				map_RT,			map_RTIt,			str_pred);
	DEFINE_MAP_PRED(pcstr,CRTC*,			map_RTC,		map_RTCIt,			str_pred);
	DEFINE_MAP_PRED(pcstr,SVS*,				map_VS,			map_VSIt,			str_pred);
	DEFINE_MAP_PRED(pcstr,SPS*,				map_PS,			map_PSIt,			str_pred);
	DEFINE_MAP_PRED(pcstr,texture_detail,	map_TD,			map_TDIt,			str_pred);

private:
	// data
	map_Blender											m_blenders;
	map_Texture											m_textures;
	map_Matrix											m_matrices;
	map_Constant										m_constants;
	map_RT												m_rtargets;
	map_RTC												m_rtargets_c;
	map_VS												m_vs;
	map_PS												m_ps;
	map_TD												m_td;

	xr_vector<SState*>									v_states;
	xr_vector<SDeclaration*>							v_declarations;
	xr_vector<SGeometry*>								v_geoms;
	xr_vector<R_constant_table*>						v_constant_tables;

	// lists
	xr_vector<STextureList*>							lst_textures;
	xr_vector<SMatrixList*>								lst_matrices;
	xr_vector<SConstantList*>							lst_constants;

	// main shader-array
	xr_vector<SPass*>									v_passes;
	xr_vector<ShaderElement*>							v_elements;
	xr_vector<Shader*>									v_shaders;
	
	xr_vector<ref_texture>								m_necessary;
	// misc

public:
	CTextureDescrManager								m_textures_description;
	xr_vector<std::pair<shared_str,R_constant_setup*> >	v_constant_setup;
	lua_State*											LSVM;
	BOOL												bDeferredLoad;

private:
	void							LS_Load				();
	void							LS_Unload			();

public:
	// Miscelaneous
	void							_ParseList			(sh_list& dest, pcstr names);
	IBlender*						_GetBlender			(pcstr Name);
	IBlender* 						_FindBlender		(pcstr Name);
	void							_GetMemoryUsage		(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
	void							_DumpMemoryUsage	();

	map_Blender&					_GetBlenders		()		{	return m_blenders;	}

	// Debug
	void							DBG_VerifyGeoms		();
	void							DBG_VerifyTextures	();

	// Low level resource creation
	CTexture*						_CreateTexture		(pcstr Name);
	void							_DeleteTexture		(const CTexture* T);

	CMatrix*						_CreateMatrix		(pcstr Name);
	void							_DeleteMatrix		(const CMatrix*  M);

	CConstant*						_CreateConstant		(pcstr Name);
	void							_DeleteConstant		(const CConstant* C);

	R_constant_table*				_CreateConstantTable(R_constant_table& C);
	void							_DeleteConstantTable(const R_constant_table* C);

	CRT*							_CreateRT			(pcstr Name, u32 w, u32 h,	D3DFORMAT f);
	void							_DeleteRT			(const CRT*	RT	);

	CRTC*							_CreateRTC			(pcstr Name, u32 size,	D3DFORMAT f);
	void							_DeleteRTC			(const CRTC*	RT	);

	SPS*							_CreatePS			(pcstr Name);
	void							_DeletePS			(const SPS*	PS	);

	SVS*							_CreateVS			(pcstr Name);
	void							_DeleteVS			(const SVS*	VS	);

	SPass*							_CreatePass			(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C);
	void							_DeletePass			(const SPass* P	);

	// Shader compiling / optimizing
	SState*							_CreateState		(SimulatorStates& Code);
	void							_DeleteState		(const SState* SB);

	SDeclaration*					_CreateDecl			(D3DVERTEXELEMENT9* dcl);
	void							_DeleteDecl			(const SDeclaration* dcl);

	STextureList*					_CreateTextureList	(STextureList& L);
	void							_DeleteTextureList	(const STextureList* L);

	SMatrixList*					_CreateMatrixList	(SMatrixList& L);
	void							_DeleteMatrixList	(const SMatrixList* L);

	SConstantList*					_CreateConstantList	(SConstantList& L);
	void							_DeleteConstantList	(const SConstantList* L);

	ShaderElement*					_CreateElement		(ShaderElement& L);
	void							_DeleteElement		(const ShaderElement* L);

	Shader*							_cpp_Create			(pcstr		s_shader, pcstr s_textures=0, pcstr s_constants=0, pcstr s_matrices=0);
	Shader*							_cpp_Create			(IBlender*	B, pcstr s_shader=0, pcstr s_textures=0, pcstr s_constants=0, pcstr s_matrices=0);
	Shader*							_lua_Create			(pcstr		s_shader, pcstr s_textures);
	BOOL							_lua_HasShader		(pcstr		s_shader);

	CResourceManager						()	: bDeferredLoad(TRUE) {	}
	~CResourceManager						()	;

	void			OnDeviceCreate			(IReader* F);
	void			OnDeviceCreate			(pcstr name);
	void			OnDeviceDestroy			(BOOL   bKeepTextures);

	void			reset_begin				();
	void			reset_end				();

	// Creation/Destroying
	Shader*			Create					(pcstr s_shader=0, pcstr s_textures=0, pcstr s_constants=0, pcstr s_matrices=0);
	Shader*			Create					(IBlender*	B, pcstr s_shader=0, pcstr s_textures=0, pcstr s_constants=0, pcstr s_matrices=0);
	void			Delete					(const Shader*		S	);
	void			RegisterConstantSetup	(pcstr name,		R_constant_setup* s)	{	v_constant_setup.push_back(mk_pair(shared_str(name),s));	}

	SGeometry*		CreateGeom				(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	SGeometry*		CreateGeom				(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void			DeleteGeom				(const SGeometry* VS		);
	void			DeferredLoad			(BOOL E)					{ bDeferredLoad=E;	}
	void			DeferredUpload			();
	void			Evict					();
	void			StoreNecessaryTextures	();
	void			DestroyNecessaryTextures();
	void			Dump					(bool bBrief);
};

#include "stdafx.h"

#include "Render.h"
#include "ResourceManager.h"
#include "tss.h"
#include "blenders\blender.h"
#include "blenders\blender_recorder.h"
#include "ai_script_space.h"
#include "ai_script_lua_extension.h"

#include <luabind/return_reference_to_policy.hpp>

using namespace				luabind;

#ifdef	DEBUG
#define MDB	Memory.dbg_check()
#else // DEBUG
#define MDB
#endif // DEBUG

// wrapper
class	adopt_sampler
{
	CBlender_Compile* C;
	u32						stage;

public:
	adopt_sampler(CBlender_Compile* _C, u32 _stage) : C(_C), stage(_stage)
	{
		if (u32(-1) == stage) C = 0;
	}
	adopt_sampler(const adopt_sampler& _C) : C(_C.C), stage(_C.stage)
	{
		if (u32(-1) == stage) C = 0;
	}

	adopt_sampler& _texture(pcstr texture)
	{
		if (C) C->i_Texture(stage, texture);											return *this;
	}
	adopt_sampler& _projective(bool _b)
	{
		if (C) C->i_Projective(stage, _b);													return *this;
	}
	adopt_sampler& _clamp( )
	{
		if (C) C->i_Address(stage, D3DTADDRESS_CLAMP);									return *this;
	}
	adopt_sampler& _wrap( )
	{
		if (C) C->i_Address(stage, D3DTADDRESS_WRAP);									return *this;
	}
	adopt_sampler& _mirror( )
	{
		if (C) C->i_Address(stage, D3DTADDRESS_MIRROR);									return *this;
	}
	adopt_sampler& _f_anisotropic( )
	{
		if (C) C->i_Filter(stage, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);	return *this;
	}
	adopt_sampler& _f_trilinear( )
	{
		if (C) C->i_Filter(stage, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR);		return *this;
	}
	adopt_sampler& _f_bilinear( )
	{
		if (C) C->i_Filter(stage, D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTEXF_LINEAR);		return *this;
	}
	adopt_sampler& _f_linear( )
	{
		if (C) C->i_Filter(stage, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);		return *this;
	}
	adopt_sampler& _f_none( )
	{
		if (C) C->i_Filter(stage, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);		return *this;
	}
	adopt_sampler& _fmin_none( )
	{
		if (C) C->i_Filter_Min(stage, D3DTEXF_NONE);										return *this;
	}
	adopt_sampler& _fmin_point( )
	{
		if (C) C->i_Filter_Min(stage, D3DTEXF_POINT);										return *this;
	}
	adopt_sampler& _fmin_linear( )
	{
		if (C) C->i_Filter_Min(stage, D3DTEXF_LINEAR);										return *this;
	}
	adopt_sampler& _fmin_aniso( )
	{
		if (C) C->i_Filter_Min(stage, D3DTEXF_ANISOTROPIC);								return *this;
	}
	adopt_sampler& _fmip_none( )
	{
		if (C) C->i_Filter_Mip(stage, D3DTEXF_NONE);										return *this;
	}
	adopt_sampler& _fmip_point( )
	{
		if (C) C->i_Filter_Mip(stage, D3DTEXF_POINT);										return *this;
	}
	adopt_sampler& _fmip_linear( )
	{
		if (C) C->i_Filter_Mip(stage, D3DTEXF_LINEAR);										return *this;
	}
	adopt_sampler& _fmag_none( )
	{
		if (C) C->i_Filter_Mag(stage, D3DTEXF_NONE);										return *this;
	}
	adopt_sampler& _fmag_point( )
	{
		if (C) C->i_Filter_Mag(stage, D3DTEXF_POINT);										return *this;
	}
	adopt_sampler& _fmag_linear( )
	{
		if (C) C->i_Filter_Mag(stage, D3DTEXF_LINEAR);										return *this;
	}
};

// wrapper																																					
class	adopt_compiler
{
	CBlender_Compile* C;
public:
	adopt_compiler(CBlender_Compile* _C) : C(_C)
	{ }
	adopt_compiler(const adopt_compiler& _C) : C(_C.C)
	{ }

	adopt_compiler& _options(s32	P, bool	S)
	{
		C->SetParams(P, S);					return	*this;
	}
	adopt_compiler& _o_emissive(bool	E)
	{
		C->SH->flags.bEmissive = E;					return	*this;
	}
	adopt_compiler& _o_distort(bool	E)
	{
		C->SH->flags.bDistort = E;					return	*this;
	}
	adopt_compiler& _o_wmark(bool	E)
	{
		C->SH->flags.bWmark = E;						return	*this;
	}
	adopt_compiler& _pass(pcstr	vs, pcstr ps)
	{
		C->r_Pass(vs, ps, true);			return	*this;
	}
	adopt_compiler& _fog(bool	_fog)
	{
		C->PassSET_LightFog(FALSE, _fog);			return	*this;
	}
	adopt_compiler& _ZB(bool	_test, bool _write)
	{
		C->PassSET_ZB(_test, _write);			return	*this;
	}
	adopt_compiler& _blend(bool	_blend, u32 abSRC, u32 abDST)
	{
		C->PassSET_ablend_mode(_blend, abSRC, abDST);	return 	*this;
	}
	adopt_compiler& _aref(bool	_aref, u32 aref)
	{
		C->PassSET_ablend_aref(_aref, aref);			return 	*this;
	}
	adopt_sampler			_sampler(pcstr _name)
	{
		u32 s = C->r_Sampler(_name, 0);				return	adopt_sampler(C, s);
	}
};

class	adopt_blend
{
public:
};

void LuaLog(pcstr caMessage)
{
	MDB;
	Lua::LuaOut(Lua::eLuaMessageTypeMessage, "%s", caMessage);
}
void LuaError(lua_State* L)
{
	Debug.fatal(DEBUG_INFO, "LUA error: %s", lua_tostring(L, -1));
}

#ifndef PURE_ALLOC
#	ifndef USE_MEMORY_MONITOR
#		define USE_DL_ALLOCATOR
#	endif // USE_MEMORY_MONITOR
#endif // PURE_ALLOC

#ifndef USE_DL_ALLOCATOR
static void* lua_alloc_xr(void* ud, void* ptr, size_t osize, size_t nsize)
{
	(void)ud;
	(void)osize;
	if (nsize == 0)
	{
		xr_free(ptr);
		return	NULL;
	}
	else
#	ifdef DEBUG_MEMORY_NAME
		return Memory.mem_realloc(ptr, nsize, "LUA:Render");
#	else // DEBUG_MEMORY_NAME
		return Memory.mem_realloc(ptr, nsize);
#	endif // DEBUG_MEMORY_NAME
}
#else // USE_DL_ALLOCATOR
#	include "doug_lea_memory_allocator.h"

static void* lua_alloc_dl(void* ud, void* ptr, size_t osize, size_t nsize)
{
	(void)ud;
	(void)osize;
	if (nsize == 0)
	{
		dlfree(ptr);	 return	NULL;
	}
	else				return dlrealloc(ptr, nsize);
}

ENGINE_API u32 engine_lua_memory_usage( )
{
	return			((u32)dlmallinfo( ).uordblks);
}
#endif // USE_DL_ALLOCATOR

// export
void	CResourceManager::LS_Load( )
{
#ifndef USE_DL_ALLOCATOR
	LSVM = lua_newstate(lua_alloc_xr, NULL);
#else // USE_XR_ALLOCAOR
	LSVM = lua_newstate(lua_alloc_dl, NULL);
#endif // USE_XR_ALLOCAOR
	if (!LSVM)
	{
		Msg("! ERROR : Cannot initialize LUA VM!");
		return;
	}

	// initialize lua standard library functions 
	luaopen_base(LSVM);
	luaopen_table(LSVM);
	luaopen_string(LSVM);
	luaopen_math(LSVM);
#ifdef USE_JIT
	luaopen_jit(LSVM);
#endif

	luabind::open(LSVM);
#if !XRAY_EXCEPTIONS
	if (0 == luabind::get_error_callback( ))
		luabind::set_error_callback(LuaError);
#endif

	function(LSVM, "log", LuaLog);

	module(LSVM)
		[
			class_<adopt_sampler>("_sampler")
				.def(constructor<const adopt_sampler&>( ))
				.def("texture", &adopt_sampler::_texture, return_reference_to(_1))
				.def("project", &adopt_sampler::_projective, return_reference_to(_1))
				.def("clamp", &adopt_sampler::_clamp, return_reference_to(_1))
				.def("wrap", &adopt_sampler::_wrap, return_reference_to(_1))
				.def("mirror", &adopt_sampler::_mirror, return_reference_to(_1))
				.def("f_anisotropic", &adopt_sampler::_f_anisotropic, return_reference_to(_1))
				.def("f_trilinear", &adopt_sampler::_f_trilinear, return_reference_to(_1))
				.def("f_bilinear", &adopt_sampler::_f_bilinear, return_reference_to(_1))
				.def("f_linear", &adopt_sampler::_f_linear, return_reference_to(_1))
				.def("f_none", &adopt_sampler::_f_none, return_reference_to(_1))
				.def("fmin_none", &adopt_sampler::_fmin_none, return_reference_to(_1))
				.def("fmin_point", &adopt_sampler::_fmin_point, return_reference_to(_1))
				.def("fmin_linear", &adopt_sampler::_fmin_linear, return_reference_to(_1))
				.def("fmin_aniso", &adopt_sampler::_fmin_aniso, return_reference_to(_1))
				.def("fmip_none", &adopt_sampler::_fmip_none, return_reference_to(_1))
				.def("fmip_point", &adopt_sampler::_fmip_point, return_reference_to(_1))
				.def("fmip_linear", &adopt_sampler::_fmip_linear, return_reference_to(_1))
				.def("fmag_none", &adopt_sampler::_fmag_none, return_reference_to(_1))
				.def("fmag_point", &adopt_sampler::_fmag_point, return_reference_to(_1))
				.def("fmag_linear", &adopt_sampler::_fmag_linear, return_reference_to(_1)),

				class_<adopt_compiler>("_compiler")
				.def(constructor<const adopt_compiler&>( ))
				.def("begin", &adopt_compiler::_pass, return_reference_to(_1))
				.def("sorting", &adopt_compiler::_options, return_reference_to(_1))
				.def("emissive", &adopt_compiler::_o_emissive, return_reference_to(_1))
				.def("distort", &adopt_compiler::_o_distort, return_reference_to(_1))
				.def("wmark", &adopt_compiler::_o_wmark, return_reference_to(_1))
				.def("fog", &adopt_compiler::_fog, return_reference_to(_1))
				.def("zb", &adopt_compiler::_ZB, return_reference_to(_1))
				.def("blend", &adopt_compiler::_blend, return_reference_to(_1))
				.def("aref", &adopt_compiler::_aref, return_reference_to(_1))
				.def("sampler", &adopt_compiler::_sampler),	// returns sampler-object

				class_<adopt_blend>("blend")
				.enum_("blend")
				[
					value("zero", s32(D3DBLEND_ZERO)),
					value("one", s32(D3DBLEND_ONE)),
					value("srccolor", s32(D3DBLEND_SRCCOLOR)),
					value("invsrccolor", s32(D3DBLEND_INVSRCCOLOR)),
					value("srcalpha", s32(D3DBLEND_SRCALPHA)),
					value("invsrcalpha", s32(D3DBLEND_INVSRCALPHA)),
					value("destalpha", s32(D3DBLEND_DESTALPHA)),
					value("invdestalpha", s32(D3DBLEND_INVDESTALPHA)),
					value("destcolor", s32(D3DBLEND_DESTCOLOR)),
					value("invdestcolor", s32(D3DBLEND_INVDESTCOLOR)),
					value("srcalphasat", s32(D3DBLEND_SRCALPHASAT))
				]
		];

		// load shaders
	xr_vector<char*>* folder = FS.file_list_open("$game_shaders$", ::Render->getShaderPath( ), FS_ListFiles | FS_RootOnly);
	VERIFY(folder);
	for (u32 it = 0; it < folder->size( ); it++)
	{
		string_path						namesp, fn;
		strcpy_s(namesp, (*folder)[it]);
		if (0 == strext(namesp) || 0 != xr_strcmp(strext(namesp), ".s"))	continue;
		*strext(namesp) = 0;
		if (0 == namesp[0])			strcpy_s(namesp, "_G");
		strconcat(sizeof(fn), fn, ::Render->getShaderPath( ), (*folder)[it]);
		FS.update_path(fn, "$game_shaders$", fn);
		try
		{
			Script::bfLoadFileIntoNamespace(LSVM, fn, namesp, true);
		}
		catch (...)
		{
			Log(lua_tostring(LSVM, -1));
		}
	}
	FS.file_list_close(folder);

#ifdef USE_JIT
	luaJIT_setmode(LSVM, LUAJIT_MODE_ENGINE, LUAJIT_MODE_ON);
#endif
}

void	CResourceManager::LS_Unload( )
{
	lua_close(LSVM);
	LSVM = NULL;
}

BOOL	CResourceManager::_lua_HasShader(pcstr s_shader)
{
	string256	undercorated;
	for (s32 i = 0, l = xr_strlen(s_shader) + 1; i < l; i++)
	{
		undercorated[i] = ('\\' == s_shader[i]) ? '_' : s_shader[i];
	}

	return	(Script::bfIsObjectPresent(LSVM, undercorated, "normal", LUA_TFUNCTION) || Script::bfIsObjectPresent(LSVM, undercorated, "l_special", LUA_TFUNCTION));
}

Shader* CResourceManager::_lua_Create(pcstr d_shader, pcstr s_textures)
{
	CBlender_Compile	C;
	Shader				S;

	// undecorate
	string256	undercorated;
	for (s32 i = 0, l = xr_strlen(d_shader) + 1; i < l; i++)
	{
		undercorated[i] = ('\\' == d_shader[i]) ? '_' : d_shader[i];
	}

	pcstr		s_shader = undercorated;

	// Access to template
	C.BT = NULL;
	C.bEditor = FALSE;
	C.bDetail = FALSE;

	// Prepare
	_ParseList(C.L_textures, s_textures);
	C.detail_texture = NULL;
	C.detail_scaler = NULL;

	// Compile element	(LOD0 - HQ)
	if (Script::bfIsObjectPresent(LSVM, s_shader, "normal_hq", LUA_TFUNCTION))
	{
		// Analyze possibility to detail this shader
		C.iElement = 0;
		C.bDetail = Device.Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);

		if (C.bDetail)
		{
			S.E[0] = C._lua_Compile(s_shader, "normal_hq");
		}
		else
		{
			S.E[0] = C._lua_Compile(s_shader, "normal");
		}
	}
	else
	{
		if (Script::bfIsObjectPresent(LSVM, s_shader, "normal", LUA_TFUNCTION))
		{
			C.iElement = 0;
			C.bDetail = Device.Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
			S.E[0] = C._lua_Compile(s_shader, "normal");
		}
	}

	// Compile element	(LOD1)
	if (Script::bfIsObjectPresent(LSVM, s_shader, "normal", LUA_TFUNCTION))
	{
		C.iElement = 1;
		C.bDetail = Device.Resources->m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
		S.E[1] = C._lua_Compile(s_shader, "normal");
	}

	// Compile element
	if (Script::bfIsObjectPresent(LSVM, s_shader, "l_point", LUA_TFUNCTION))
	{
		C.iElement = 2;
		C.bDetail = FALSE;
		S.E[2] = C._lua_Compile(s_shader, "l_point");;
	}

	// Compile element
	if (Script::bfIsObjectPresent(LSVM, s_shader, "l_spot", LUA_TFUNCTION))
	{
		C.iElement = 3;
		C.bDetail = FALSE;
		S.E[3] = C._lua_Compile(s_shader, "l_spot");;
	}

	// Compile element
	if (Script::bfIsObjectPresent(LSVM, s_shader, "l_special", LUA_TFUNCTION))
	{
		C.iElement = 4;
		C.bDetail = FALSE;
		S.E[4] = C._lua_Compile(s_shader, "l_special");
	}

	// Search equal in shaders array
	for (u32 it = 0; it < v_shaders.size( ); it++)
	{
		if (S.equal(v_shaders[it]))
		{
			return v_shaders[it];
		}
	}

	// Create _new_ entry
	Shader* N = xr_new<Shader>(S);
	N->dwFlags |= xr_resource_flagged::RF_REGISTERED;
	v_shaders.push_back(N);
	return N;
}

ShaderElement* CBlender_Compile::_lua_Compile(pcstr namesp, pcstr name)
{
	ShaderElement E;
	SH = &E;
	RS.Invalidate( );

	// Compile
	pcstr				t_0 = *L_textures[0] ? *L_textures[0] : "null";
	pcstr				t_1 = (L_textures.size( ) > 1) ? *L_textures[1] : "null";
	pcstr				t_d = detail_texture ? detail_texture : "null";
	lua_State* LSVM = Device.Resources->LSVM;
	object				shader = get_globals(LSVM)[namesp];
	functor<void>		element = object_cast<functor<void>>(shader[name]);
	adopt_compiler		ac = adopt_compiler(this);
	element(ac, t_0, t_1, t_d);
	r_End( );
	ShaderElement* _r = Device.Resources->_CreateElement(E);
	return			_r;
}

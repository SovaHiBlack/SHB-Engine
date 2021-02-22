////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file.cpp
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_ini_file.h"
#include "script_engine.h"
#include "ai_space.h"
#include "ObjectFactory.h"

CScriptIniFile::CScriptIniFile		(IReader *F, const char* path) :
	inherited	(F,path)
{
}

CScriptIniFile::CScriptIniFile		(const char* szFileName, BOOL ReadOnly, BOOL bLoadAtStart, BOOL SaveAtEnd) :
	inherited	(update(szFileName), ReadOnly, bLoadAtStart, SaveAtEnd)
{
}

CScriptIniFile::~CScriptIniFile		()
{
}

const char* CScriptIniFile::update		(const char* file_name)
{
	string_path			S1;
	FS.update_path		(S1,"$game_config$",file_name);
	return				(*shared_str(S1));
}

bool CScriptIniFile::line_exist		(const char* S, const char* L)
{
	return		(!!inherited::line_exist(S,L));
}

bool CScriptIniFile::section_exist	(const char* S)
{
	return		(!!inherited::section_exist(S));
}

int	 CScriptIniFile::r_clsid		(const char* S, const char* L)
{
	return		(object_factory().script_clsid(inherited::r_clsid(S,L)));
}

bool CScriptIniFile::r_bool			(const char* S, const char* L)
{
	return		(!!inherited::r_bool(S,L));
}

int	 CScriptIniFile::r_token		(const char* S, const char* L, const CScriptTokenList &token_list)
{
	return		(inherited::r_token(S,L,&*token_list.tokens().begin()));
}

const char* CScriptIniFile::r_string_wb	(const char* S, const char* L)
{
	return		(*inherited::r_string_wb(S,L));
}

u32	 CScriptIniFile::line_count			(const char* S)
{
	THROW3		(inherited::section_exist(S),"Cannot find section",S);
	return		(inherited::line_count(S));
}

const char* CScriptIniFile::r_string			(const char* S, const char* L)
{
	THROW3		(inherited::section_exist(S),"Cannot find section",S);
	THROW3		(inherited::line_exist(S,L),"Cannot find line",L);
	return		(inherited::r_string(S,L));
}

u32	 CScriptIniFile::r_u32				(const char* S, const char* L)
{
	THROW3		(inherited::section_exist(S),"Cannot find section",S);
	THROW3		(inherited::line_exist(S,L),"Cannot find line",L);
	return		(inherited::r_u32(S,L));
}

int	 CScriptIniFile::r_s32				(const char* S, const char* L)
{
	THROW3		(inherited::section_exist(S),"Cannot find section",S);
	THROW3		(inherited::line_exist(S,L),"Cannot find line",L);
	return		(inherited::r_s32(S,L));
}

float CScriptIniFile::r_float			(const char* S, const char* L)
{
	THROW3		(inherited::section_exist(S),"Cannot find section",S);
	THROW3		(inherited::line_exist(S,L),"Cannot find line",L);
	return		(inherited::r_float(S,L));
}

Fvector CScriptIniFile::r_fvector3		(const char* S, const char* L)
{
	THROW3		(inherited::section_exist(S),"Cannot find section",S);
	THROW3		(inherited::line_exist(S,L),"Cannot find line",L);
	return		(inherited::r_fvector3(S,L));
}
using namespace luabind;

CScriptIniFile* get_system_ini( )
{
	return	((CScriptIniFile*) pSettings);
}

CScriptIniFile* get_game_ini( )
{
	return	((CScriptIniFile*) pGameIni);
}

bool r_line(CScriptIniFile* self, const char* S, int L, xr_string& N, xr_string& V)
{
	THROW3(self->section_exist(S), "Cannot find section", S);
	THROW2((int) self->line_count(S) > L, "Invalid line number");

	N = "";
	V = "";

	const char* n;
	const char* v;
	bool			result = !!self->r_line(S, L, &n, &v);
	if (!result)
		return		(false);

	N = n;
	if (v)
		V = v;
	return			(true);
}

#pragma warning(push)
#pragma warning(disable:4238)
CScriptIniFile* create_ini_file(const char* ini_string)
{
	return			(
		(CScriptIniFile*)
		xr_new<CIniFile>(
			&IReader(
				(void*) ini_string,
				xr_strlen(ini_string)
			),
			FS.get_path("$game_config$")->m_Path
			)
		);
}
#pragma warning(pop)

#pragma optimize("s",on)
void CScriptIniFile::script_register(lua_State* L)
{
	module(L)
		[
			class_<CScriptIniFile>("ini_file")
			.def(constructor<const char*>( ))
		.def("section_exist", &CScriptIniFile::section_exist)
		.def("line_exist", &CScriptIniFile::line_exist)
		.def("r_clsid", &CScriptIniFile::r_clsid)
		.def("r_bool", &CScriptIniFile::r_bool)
		.def("r_token", &CScriptIniFile::r_token)
		.def("r_string_wq", &CScriptIniFile::r_string_wb)
		.def("line_count", &CScriptIniFile::line_count)
		.def("r_string", &CScriptIniFile::r_string)
		.def("r_u32", &CScriptIniFile::r_u32)
		.def("r_s32", &CScriptIniFile::r_s32)
		.def("r_float", &CScriptIniFile::r_float)
		.def("r_vector", &CScriptIniFile::r_fvector3)
		.def("r_line", &::r_line, out_value(_4) + out_value(_5)),

		def("system_ini", &get_system_ini),
		def("game_ini", &get_game_ini),
		def("create_ini_file", &create_ini_file, adopt(result))
		];
}

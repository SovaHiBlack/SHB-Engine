////////////////////////////////////////////////////////////////////////////
//	Module 		: script_ini_file.h
//	Created 	: 21.05.2004
//  Modified 	: 21.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Script ini file class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_token_list.h"
#include "script_export_space.h"

class CScriptIniFile : public CInifile {
protected:
	typedef CInifile inherited;

public:
						CScriptIniFile		(IReader *F, pcstr path=0);
						CScriptIniFile		(pcstr szFileName, BOOL ReadOnly=TRUE, BOOL bLoadAtStart=TRUE, BOOL SaveAtEnd=TRUE);
	virtual 			~CScriptIniFile		();
			bool		line_exist			(pcstr S, pcstr L);
			bool		section_exist		(pcstr S);
			int			r_clsid				(pcstr S, pcstr L);
			bool		r_bool				(pcstr S, pcstr L);
			int			r_token				(pcstr S, pcstr L, const CScriptTokenList &token_list);
			pcstr		r_string_wb			(pcstr S, pcstr L);
			pcstr		update				(pcstr file_name);
			u32			line_count			(pcstr S);
			pcstr		r_string			(pcstr S, pcstr L);
			u32			r_u32				(pcstr S, pcstr L);
			int			r_s32				(pcstr S, pcstr L);
			float		r_float				(pcstr S, pcstr L);
			Fvector		r_fvector3			(pcstr S, pcstr L);
			DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptIniFile)
#undef script_type_list
#define script_type_list save_type_list(CScriptIniFile)

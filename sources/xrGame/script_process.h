////////////////////////////////////////////////////////////////////////////
//	Module 		: script_process.h
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script process class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CScriptThread;

class CScriptProcess {
public:
	typedef xr_vector<CScriptThread*> SCRIPT_REGISTRY;

private:
	struct CScriptToRun {
		char* m_script_name;
		bool						m_do_string;
		bool						m_reload;

		inline		CScriptToRun		(const char* script_name, bool do_string, bool reload = false)
		{
			m_script_name			= xr_strdup(script_name);
			m_do_string				= do_string;
			m_reload				= reload;
		}

		inline		CScriptToRun		(const CScriptToRun &script)
		{
			m_script_name			= xr_strdup(script.m_script_name);
			m_do_string				= script.m_do_string;
			m_reload				= script.m_reload;
		}

		virtual ~CScriptToRun		()
		{
			xr_free					(m_script_name);
		}
	};

public:
	typedef xr_vector<CScriptToRun> SCRIPTS_TO_RUN;

protected:
	SCRIPT_REGISTRY					m_scripts;
	SCRIPTS_TO_RUN					m_scripts_to_run;
	CSharedString						m_name;

protected:
	u32								m_iterator;				// Oles: iterative update

protected:
			void					run_scripts		();

public:
									CScriptProcess	(CSharedString anme, CSharedString scripts);
	virtual							~CScriptProcess	();
			void					update			();
			void					add_script		(const char* script_name, bool string, bool reload);
	inline		const SCRIPT_REGISTRY	&scripts		() const;
	inline		CSharedString				name			() const;
};

#include "script_process_inline.h"
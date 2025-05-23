////////////////////////////////////////////////////////////////////////////
//	Module 		: script_process.cpp
//	Created 	: 19.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script process class
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_engine.h"
#include "script_process.h"
#include "script_thread.h"
#include "AISpace.h"
#include "object_broker.h"

string4096			g_ca_stdout;

#ifdef USE_DEBUGGER
#	include "script_debugger.h"
#endif

CScriptProcess::CScriptProcess	(shared_str name, shared_str scripts) :
	m_name						(name)
{
#ifdef DEBUG
	Msg				("* Initializing %s script process",*m_name);
#endif
	
	string256		I;
	for (u32 i=0, n = _GetItemCount(*scripts); i<n; ++i)
		add_script	(_GetItem(*scripts,i,I),false,false);

	m_iterator		= 0;
}

CScriptProcess::~CScriptProcess()
{
	delete_data		(m_scripts);
}

void CScriptProcess::run_scripts()
{
	pstr						S;
	for ( ; !m_scripts_to_run.empty(); ) {
		pstr					I = m_scripts_to_run.back().m_script_name;
		bool					do_string = m_scripts_to_run.back().m_do_string;
		bool					reload = m_scripts_to_run.back().m_reload;
		S						= xr_strdup(I);
		m_scripts_to_run.pop_back();

		CScriptThread			*script = xr_new<CScriptThread>(S,do_string,reload);
		xr_free					(S);

		if (script->active())
			m_scripts.push_back	(script);
		else
			xr_delete			(script);
	}
}

// Oles: 
//		changed to process one script per-frame
//		changed log-output to stack-based buffer (avoid persistent 4K storage)
void CScriptProcess::update()
{
#ifdef DBG_DISABLE_SCRIPTS
	m_scripts_to_run.clear();
	return;
#endif

	run_scripts			();

	if (m_scripts.empty())
		return;

	// update script
	g_ca_stdout[0]		= 0;
	u32					_id	= (++m_iterator)%m_scripts.size();
	if (!m_scripts[_id]->update()) {
		xr_delete			(m_scripts[_id]);
		m_scripts.erase	(m_scripts.begin() + _id);
		--m_iterator;		// try to avoid skipping
	}

	if (g_ca_stdout[0]) {
		fputc							(0,stderr);
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeInfo,"%s",g_ca_stdout);
		fflush							(stderr);
	}

#if defined(_DEBUG) && !defined(USE_JIT)
	try {
		lua_setgcthreshold	(ai().script_engine().lua(),0);
	}
	catch(...) {
	}
#endif
}

void CScriptProcess::add_script	(pcstr	script_name,bool do_string, bool reload)
{
	m_scripts_to_run.push_back(CScriptToRun(script_name,do_string,reload));
}

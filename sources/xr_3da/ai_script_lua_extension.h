////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.h
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_script_space.h"
//struct CLuaVirtualMachine;

namespace Script {
#ifndef ENGINE_BUILD
	void				vfExportGlobals				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportFvector				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportFmatrix				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportGame				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportLevel				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportDevice				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportParticles			(CLuaVirtualMachine *tpLuaVM);
	void				vfExportSound				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportHit					(CLuaVirtualMachine *tpLuaVM);
	void				vfExportActions				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportObject				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportEffector			(CLuaVirtualMachine *tpLuaVM);
	void				vfExportArtifactMerger		(CLuaVirtualMachine *tpLuaVM);
	void				vfLoadStandardScripts		(CLuaVirtualMachine *tpLuaVM);
	void				vfExportMemoryObjects		(CLuaVirtualMachine *tpLuaVM);
	void				vfExportToLua				(CLuaVirtualMachine *tpLuaVM);
	void				vfExportActionManagement	(CLuaVirtualMachine *tpLuaVM);
	void				vfExportMotivationManagement(CLuaVirtualMachine *tpLuaVM);
	bool				bfLoadFile					(CLuaVirtualMachine *tpLuaVM, pcstr	caScriptName,	bool	bCall = true);
	void				LuaHookCall					(CLuaVirtualMachine *tpLuaVM, lua_Debug *tpLuaDebug);
	int					LuaPanic					(CLuaVirtualMachine *tpLuaVM);
#endif
	bool				bfPrintOutput				(CLuaVirtualMachine *tpLuaVM, pcstr	caScriptName, int iErorCode = 0);
	pcstr				cafEventToString			(int				iEventCode);
	void				vfPrintError				(CLuaVirtualMachine *tpLuaVM, int		iErrorCode);
	bool				bfListLevelVars				(CLuaVirtualMachine *tpLuaVM, int		iStackLevel);
	bool				bfLoadBuffer				(CLuaVirtualMachine *tpLuaVM, pcstr	caBuffer,		size_t	tSize, pcstr	caScriptName, pcstr caNameSpaceName = 0);
	bool				bfLoadFileIntoNamespace		(CLuaVirtualMachine *tpLuaVM, pcstr	caScriptName, pcstr	caNamespaceName,	bool	bCall);
	bool				bfGetNamespaceTable			(CLuaVirtualMachine *tpLuaVM, pcstr	caName);
	CLuaVirtualMachine	*get_namespace_table		(CLuaVirtualMachine *tpLuaVM, pcstr	caName);
	bool				bfIsObjectPresent			(CLuaVirtualMachine *tpLuaVM, pcstr	caIdentifier,	int type);
	bool				bfIsObjectPresent			(CLuaVirtualMachine *tpLuaVM, pcstr	caNamespaceName, pcstr	caIdentifier, int type);
	luabind::object		lua_namespace_table			(CLuaVirtualMachine *tpLuaVM, pcstr namespace_name);
};

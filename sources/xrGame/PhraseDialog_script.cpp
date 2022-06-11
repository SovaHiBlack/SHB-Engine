#include "stdafx.h"

#include "PhraseDialog_script.h"
#include "PhraseDialog.h"

using namespace luabind;

void CPhraseScript::AddPrecondition	(pcstr str)
{
	m_Preconditions.push_back	(str);
}
void CPhraseScript::AddAction		(pcstr str)
{
	m_ScriptActions.push_back	(str);
}
void CPhraseScript::AddHasInfo		(pcstr str)
{
	m_HasInfo.push_back	(str);
}
void CPhraseScript::AddDontHasInfo	(pcstr str)
{
	m_DontHasInfo.push_back	(str);
}
void CPhraseScript::AddGiveInfo		(pcstr str)
{
	m_GiveInfo.push_back	(str);
}
void CPhraseScript::AddDisableInfo	(pcstr str)
{
	m_DisableInfo.push_back	(str);
}


#pragma optimize("s",on)
void CPhraseDialogExporter::script_register(lua_State *L)
{
	module(L)
	[
		class_<CPhrase>("CPhrase")
		.def("GetPhraseScript",		&CPhrase::GetPhraseScript),

		class_<CPhraseDialog>("CPhraseDialog")
		.def("AddPhrase",			&CPhraseDialog::AddPhrase_script ),

		class_<CPhraseScript>("CPhraseScript")
		.def("AddPrecondition",		&CPhraseScript::AddPrecondition)
		.def("AddAction",			&CPhraseScript::AddAction)
		.def("AddHasInfo",			&CPhraseScript::AddHasInfo)
		.def("AddDontHasInfo",		&CPhraseScript::AddDontHasInfo)
		.def("AddGiveInfo",			&CPhraseScript::AddGiveInfo)
		.def("AddDisableInfo",		&CPhraseScript::AddDisableInfo)
	];
}

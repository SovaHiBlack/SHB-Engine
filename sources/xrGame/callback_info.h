#pragma once

#include "../script_callback_ex.h"

class CUIWindow;

struct SCallbackInfo
{
	CScriptCallbackEx<void>									m_callback;
	fastdelegate::FastDelegate2<CUIWindow*, void*, void>	m_cpp_callback;
	CSharedString											m_controlName;
	signed short											m_event;
								SCallbackInfo				( ) : m_controlName(""), m_event(-1)
	{ }
};

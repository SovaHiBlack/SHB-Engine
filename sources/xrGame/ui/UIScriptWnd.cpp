#include "stdafx.h"

#include "UIScriptWnd.h"
#include "../HUDManager.h"//
#include "../object_broker.h"
#include "../callback_info.h"

struct event_comparer
{
	CSharedString name;
	signed short event;

	event_comparer(CSharedString n, signed short e) : name(n), event(e)
	{ }
	bool operator ()(SCallbackInfo* i)
	{
		return((i->m_controlName == name) && (i->m_event == event));
	}
};

CUIDialogWndEx::CUIDialogWndEx( ) :inherited( )
{
	Hide( );
}

CUIDialogWndEx::~CUIDialogWndEx( )
{
	try
	{
		delete_data(m_callbacks);
	}
	catch (...)
	{
	}
}

void CUIDialogWndEx::Register(CUIWindow* pChild)
{
	pChild->SetMessageTarget(this);
}

void CUIDialogWndEx::Register(CUIWindow* pChild, const char* name)
{
	pChild->SetWindowName(name);
	pChild->SetMessageTarget(this);
}

void CUIDialogWndEx::SendMessage(CUIWindow* pWnd, signed short msg, void* pData)
{
	event_comparer ec(pWnd->WindowName( ), msg);

	CALLBACK_IT it = std::find_if(m_callbacks.begin( ), m_callbacks.end( ), ec);
	if (it == m_callbacks.end( ))
	{
		return inherited::SendMessage(pWnd, msg, pData);
	}

	((*it)->m_callback)();

//	if ((*it)->m_cpp_callback)
//	{
//	(*it)->m_cpp_callback(pData);
//	}
}

bool CUIDialogWndEx::Load(const char* xml_name)
{
	return true;
}

SCallbackInfo* CUIDialogWndEx::NewCallback( )
{
	m_callbacks.push_back(xr_new<SCallbackInfo>( ));
	return m_callbacks.back( );
}

void CUIDialogWndEx::AddCallback(const char* control_id, signed short event, const luabind::functor<void>& lua_function)
{
	SCallbackInfo* c = NewCallback( );
	c->m_callback.set(lua_function);
	c->m_controlName = control_id;
	c->m_event = event;
}

void CUIDialogWndEx::AddCallback(const char* control_id, signed short event, const luabind::functor<void>& functor, const luabind::object& object)
{
	SCallbackInfo* c = NewCallback( );
	c->m_callback.set(functor, object);
	c->m_controlName = control_id;
	c->m_event = event;
}

bool CUIDialogWndEx::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	return inherited::OnKeyboard(dik, keyboard_action);
}

void CUIDialogWndEx::Update( )
{
	inherited::Update( );
}

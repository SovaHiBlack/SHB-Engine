#include "stdafx.h"

#include "UIWndCallback.h"
#include "UIWindow.h"//
#include "../object_broker.h"
#include "../callback_info.h"

struct event_comparer
{
	CSharedString name;
	signed short event;

	event_comparer(CSharedString n, signed short e) : name(n), event(e)
	{ }
	bool operator ( ) (SCallbackInfo* i)
	{
		return ((i->m_controlName == name) && (i->m_event == event));
	}
};

CUIWndCallback::~CUIWndCallback( )
{
	delete_data(m_callbacks);
}

void CUIWndCallback::Register(CUIWindow* pChild)
{
	pChild->SetMessageTarget(smart_cast<CUIWindow*>(this));
}

void CUIWndCallback::OnEvent(CUIWindow* pWnd, signed short msg, void* pData)
{
	if (!pWnd)
	{
		return;
	}

	event_comparer ec(pWnd->WindowName( ), msg);

	CALLBACK_IT it = std::find_if(m_callbacks.begin( ), m_callbacks.end( ), ec);
	if (it == m_callbacks.end( ))
	{
		return;
	}

	(*it)->m_callback( );

	if ((*it)->m_cpp_callback)
	{
		(*it)->m_cpp_callback(pWnd, pData);
	}
}

SCallbackInfo* CUIWndCallback::NewCallback( )
{
	m_callbacks.push_back(xr_new<SCallbackInfo>( ));
	return m_callbacks.back( );
}

void CUIWndCallback::AddCallback(const char* control_id, signed short event, const void_function& f)
{
	SCallbackInfo* c = NewCallback( );
	c->m_cpp_callback = f;
	c->m_controlName = control_id;
	c->m_event = event;
}

void CUIWndCallback::AddCallback(const CSharedString& control_id, signed short event, const void_function& f)
{
	SCallbackInfo* c = NewCallback( );
	c->m_cpp_callback = f;
	c->m_controlName = control_id;
	c->m_event = event;
}

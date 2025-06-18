#include "stdafx.h"

#include "UIMessageBox.h"
#include "UIMessageBoxEx.h"
#include "UIDialogHolder.h"

CUIMessageBoxEx::CUIMessageBoxEx( )
{
	m_pMessageBox = xr_new<CUIMessageBox>( );
	m_pMessageBox->SetWindowName("msg_box");
	AttachChild(m_pMessageBox);
}

CUIMessageBoxEx::~CUIMessageBoxEx( )
{
	xr_delete(m_pMessageBox);
}

void CUIMessageBoxEx::Init(pcstr xml_template)
{
	CUIDialogWnd::Init(0.0f, 0.0f, 1024.0f, 768.0f);
	m_pMessageBox->Init(xml_template);
}

void CUIMessageBoxEx::SetText(pcstr text)
{
	m_pMessageBox->SetText(text);
}

pcstr CUIMessageBoxEx::GetText( )
{
	return m_pMessageBox->GetText( );
}

void CUIMessageBoxEx::SendMessage(CUIWindow* pWnd, s16 msg, pvoid pData /* = NULL */)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
	if (pWnd == m_pMessageBox)
	{
		switch (msg)
		{
			case MESSAGE_BOX_OK_CLICKED:
			case MESSAGE_BOX_YES_CLICKED:
			case MESSAGE_BOX_NO_CLICKED:
			case MESSAGE_BOX_CANCEL_CLICKED:
			case MESSAGE_BOX_QUIT_WIN_CLICKED:
			case MESSAGE_BOX_QUIT_GAME_CLICKED:
			{
				GetHolder( )->StartStopMenu(this, true);
			}
			default:
			{
			}
			break;
		}

		if (GetMessageTarget( ))
		{
			GetMessageTarget( )->SendMessage(this, msg, pData);
		}
	}
}

pcstr CUIMessageBoxEx::GetHost( )
{
	return m_pMessageBox->GetHost( );
}

pcstr CUIMessageBoxEx::GetPassword( )
{
	return m_pMessageBox->GetPassword( );
}

#pragma once

#include "UIDialogWnd.h"//
#include "UIWndCallback.h"
//#include "UIMessageBox.h"//

class CUIMessageBox;

class CUIMessageBoxEx : public CUIDialogWnd, public CUIWndCallback
{
public:
	CUIMessageBoxEx( );
	virtual ~CUIMessageBoxEx( );
	void SetText(const char* text);
	const char* GetText( );
	virtual void Init(const char* xml_template);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr);

	CUIMessageBox* m_pMessageBox;
};

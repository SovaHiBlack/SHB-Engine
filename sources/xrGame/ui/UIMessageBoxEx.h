#pragma once

#include "UIDialogWnd.h"
#include "UIWndCallback.h"
//#include "UIMessageBox.h"

class CUIMessageBox;

class CUIMessageBoxEx : public CUIDialogWnd, public CUIWndCallback{
public:
	CUIMessageBoxEx();
	virtual ~CUIMessageBoxEx();
			void SetText(pcstr text);
			pcstr GetText ();
	virtual void Init(pcstr xml_template);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	pcstr		 GetHost			();
	pcstr		 GetPassword		();

    CUIMessageBox*	m_pMessageBox;
};

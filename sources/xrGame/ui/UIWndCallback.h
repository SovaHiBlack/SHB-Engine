#pragma once

struct SCallbackInfo;
class CUIWindow;

class CUIWndCallback
{
public:
	typedef fastdelegate::FastDelegate2<CUIWindow*, pvoid, void>				void_function;

private:
	typedef xr_vector<SCallbackInfo*>	CALLBACKS;
	typedef CALLBACKS::iterator			CALLBACK_IT;

private:
	CALLBACKS			m_callbacks;
	SCallbackInfo* NewCallback( );

public:
	virtual						~CUIWndCallback( );
	virtual void				OnEvent(CUIWindow* pWnd, s16 msg, pvoid pData = NULL);
	void				Register(CUIWindow* pChild);
	void				AddCallback(pcstr control_id, s16 event, const void_function& f);
	void				AddCallback(const shared_str& control_id, s16 event, const void_function& f);
};

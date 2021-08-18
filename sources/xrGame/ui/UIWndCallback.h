#pragma once

struct SCallbackInfo;
class CUIWindow;

class CUIWndCallback
{
public:
	using void_function						= fastdelegate::FastDelegate2<CUIWindow*, void*, void>;

private:
	using CALLBACKS							= xr_vector<SCallbackInfo*>;
	using CALLBACK_IT						= CALLBACKS::iterator;

	CALLBACKS								m_callbacks;
	SCallbackInfo*		NewCallback			( );

public:
	virtual				~CUIWndCallback		( );
	virtual void		OnEvent				(CUIWindow* pWnd, S16 msg, void* pData = nullptr);
	void				Register			(CUIWindow* pChild);
	void				AddCallback			(const char* control_id, S16 event, const void_function& f);
	void				AddCallback			(const CSharedString& control_id, S16 event, const void_function& f);
};

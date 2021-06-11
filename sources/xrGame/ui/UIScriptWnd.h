#pragma once

#include "UIDialogWnd.h"//
#include "../script_space_forward.h"
#include "../script_export_space.h"

struct SCallbackInfo;

class CUIDialogWndEx : public CUIDialogWnd, public DLL_Pure
{
	using inherited							= CUIDialogWnd;
	using CALLBACKS							= xr_vector<SCallbackInfo*>;
	using CALLBACK_IT						= CALLBACKS::iterator;

private:
	CALLBACKS								m_callbacks;
	virtual void		SendMessage			(CUIWindow* pWnd, S16 msg, void* pData = nullptr);
	SCallbackInfo*		NewCallback			( );

protected:
	bool				Load				(const char* xml_name);

public:
	void				Register			(CUIWindow* pChild);
	void				Register			(CUIWindow* pChild, const char* name);
						CUIDialogWndEx		( );
	virtual				~CUIDialogWndEx		( );
	void				AddCallback			(const char* control_id, S16 event, const luabind::functor<void>& lua_function);
	void				AddCallback			(const char* control_id, S16 event, const luabind::functor<void>& functor, const luabind::object& object);
	virtual void		Update				( );
	virtual bool		OnKeyboard			(int dik, EUIMessages keyboard_action);
	virtual bool		Dispatch			(int cmd, int param)
	{
		return true;
	}

	template<typename T>
	inline T*			GetControl			(const char* name);

	static void			script_register		(lua_State*);
};

add_to_type_list(CUIDialogWndEx)
#undef script_type_list
#define script_type_list save_type_list(CUIDialogWndEx)

#pragma once

#include "script_export_space.h"
#include "object_interfaces.h"
#include "game_base_space.h"

// refs
class CUI;
class CTeamBaseZone;
class game_cl_GameState;
class CUIDialogWnd;
class CUICaption;
class CUIStatic;
class CUIWindow;
class CUIXml;

struct SDrawStaticStruct :public IPureDestroyableObject
{
	SDrawStaticStruct( );
	virtual	void	destroy( );
	CUIStatic* m_static;
	f32			m_endTime;
	shared_str		m_name;
	void			Draw( );
	void			Update( );
	CUIStatic* wnd( )
	{
		return m_static;
	}
	bool			IsActual( );
	bool operator ==(pcstr str)
	{
		return (m_name == str);
	}
};

typedef xr_vector<SDrawStaticStruct>	st_vec;
struct SGameTypeMaps
{
	shared_str				m_game_type_name;
	EGameTypes				m_game_type_id;
	xr_vector<shared_str>	m_map_names;
};

struct SGameWeathers
{
	shared_str				m_weather_name;
	shared_str				m_start_time;
};

typedef xr_vector<SGameWeathers>					GAME_WEATHERS;
typedef xr_vector<SGameWeathers>::iterator			GAME_WEATHERS_IT;
typedef xr_vector<SGameWeathers>::const_iterator	GAME_WEATHERS_CIT;

class CUIGameCustom :public DLL_Pure, public ISheduled
{
	typedef ISheduled inherited;

protected:
	u32					uFlags;

	void				SetFlag(u32 mask, BOOL flag)
	{
		if (flag)
		{
			uFlags |= mask;
		}
		else
		{
			uFlags &= ~mask;
		}
	}
	void				InvertFlag(u32 mask)
	{
		if (uFlags & mask)
		{
			uFlags &= ~mask;
		}
		else
		{
			uFlags |= mask;
		}
	}
	BOOL				GetFlag(u32 mask)
	{
		return uFlags & mask;
	}
	CUICaption* GameCaptions( )
	{
		return m_pgameCaptions;
	}
	CUICaption* m_pgameCaptions;
	CUIXml* m_msgs_xml;
	st_vec										m_custom_statics;

public:
	virtual void		SetClGame(game_cl_GameState* g)
	{ }

	virtual				f32					shedule_Scale( );
	virtual				void					shedule_Update(u32 dt);

	CUIGameCustom( );
	virtual				~CUIGameCustom( );

	virtual	void		Init( )
	{ }

	virtual void		Render( );
	virtual void		OnFrame( );
	virtual	void		reset_ui( );

	virtual bool		IR_OnKeyboardPress(s32 dik);
	virtual bool		IR_OnKeyboardRelease(s32 dik);
	virtual bool		IR_OnMouseMove(s32 dx, s32 dy);
	virtual bool		IR_OnMouseWheel(s32 direction);

	void				AddDialogToRender(CUIWindow* pDialog);
	void				RemoveDialogToRender(CUIWindow* pDialog);

	CUIDialogWnd* MainInputReceiver( );
	virtual void		ReInitShownUI( ) = 0;
	virtual void		HideShownDialogs( )
	{ }

	void		AddCustomMessage(pcstr id, f32 x, f32 y, f32 font_size, CGameFont* pFont, u16 alignment, u32 color);
	void		AddCustomMessage(pcstr id, f32 x, f32 y, f32 font_size, CGameFont* pFont, u16 alignment, u32 color, f32 flicker);
	void		CustomMessageOut(pcstr id, pcstr msg, u32 color);
	void		RemoveCustomMessage(pcstr id);

	SDrawStaticStruct* AddCustomStatic(pcstr id, bool bSingleInstance);
	SDrawStaticStruct* GetCustomStatic(pcstr id);
	void				RemoveCustomStatic(pcstr id);

	virtual	shared_str	shedule_Name( ) const
	{
		return shared_str("CUIGameCustom");
	}
	virtual bool		shedule_Needed( )
	{
		return true;
	}

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIGameCustom)
#undef script_type_list
#define script_type_list save_type_list(CUIGameCustom)

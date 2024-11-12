// Console.h: interface for the CConsole class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "IInputReceiver.h"

//refs
class ENGINE_API CGameFont;
class ENGINE_API IConsoleCommand;

class ENGINE_API CConsole : public IInputReceiver, public pureRender, public pureFrame
{
public:
	//t-defs
	struct str_pred : public std::binary_function<pstr, pstr, bool>
	{
		IC bool operator()(pcstr x, pcstr y) const
		{
			return xr_strcmp(x, y) < 0;
		}
	};
	typedef xr_map<pcstr, IConsoleCommand*, str_pred>	vecCMD;
	typedef vecCMD::iterator							vecCMD_IT;
	enum
	{
		MAX_LEN = 1024
	};

private:
	u32				last_mm_timer;
	f32			cur_time;
	f32			rep_time;
	f32			fAccel;

	s32				cmd_delta;
	s32				old_cmd_delta;

	pstr editor_last;
	BOOL			bShift;

	BOOL			bRepeat;
	BOOL			RecordCommands;

protected:
	s32				scroll_delta;
	char			editor[MAX_LEN];
	BOOL			bCursor;

	CGameFont* pFont;

public:
	virtual			~CConsole( )
	{ }
	string64		ConfigFile;
	BOOL			bVisible;
	vecCMD			Commands;

	void			AddCommand(IConsoleCommand*);
	void			RemoveCommand(IConsoleCommand*);
	void			Reset( );

	void			Show( );
	void			Hide( );

//	void			Save( );
	void			Execute(pcstr cmd);
	void			ExecuteScript(pcstr name);
	void			ExecuteCommand( );

	// get
	BOOL			GetBool(pcstr cmd, BOOL& val);
	f32				GetFloat(pcstr cmd, f32& val, f32& min, f32& max);
	pstr			GetString(pcstr cmd);
	s32				GetInteger(pcstr cmd, s32& val, s32& min, s32& max);
	pstr			GetToken(pcstr cmd);
	xr_token*		GetXRToken(pcstr cmd);

	void			SelectCommand( );

	// keyboard
	void			OnPressKey(s32 dik, BOOL bHold = false);
	virtual void	IR_OnKeyboardPress(s32 dik);
	virtual void	IR_OnKeyboardHold(s32 dik);
	virtual void	IR_OnKeyboardRelease(s32 dik);

	// render & onmove
	virtual void	OnRender( );
	virtual void	OnFrame( );

	virtual	void	Initialize( );
	virtual void	Destroy( );
};

ENGINE_API extern CConsole* Console;

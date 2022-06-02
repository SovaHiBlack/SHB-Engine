// XR_IOConsole.h: interface for the CConsole class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "iinputreceiver.h"

//refs
class ENGINE_API CGameFont;
class ENGINE_API IConsole_Command;

class ENGINE_API CConsole  :
	public IInputReceiver,
	public pureRender,
	public pureFrame
{
public:
	//t-defs
	struct str_pred : public std::binary_function<char*, char*, bool> {	
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return xr_strcmp(x,y)<0;	}
	};
	typedef xr_map<LPCSTR,IConsole_Command*,str_pred>	vecCMD;
	typedef vecCMD::iterator							vecCMD_IT;
	enum			{ MAX_LEN = 1024 };
private:
	u32				last_mm_timer;
	F32			cur_time;
	F32			rep_time;
	F32			fAccel;
	
	int				cmd_delta;
	int				old_cmd_delta;
	
	char*			editor_last;
	BOOL			bShift;
	
	BOOL			bRepeat;
	BOOL			RecordCommands;
protected:
	int				scroll_delta;
	char			editor[MAX_LEN];
	BOOL			bCursor;

	CGameFont*		pFont;
public:
	virtual			~CConsole			(){};
	string64		ConfigFile;
	BOOL			bVisible;
	vecCMD			Commands;

	void			AddCommand			(IConsole_Command*);
	void			RemoveCommand		(IConsole_Command*);
	void			Reset				();

	void			Show				();
	void			Hide				();

	void			Save				();
	void			Execute				(LPCSTR cmd);
	void			ExecuteScript		(LPCSTR name);
	void			ExecuteCommand		();

	// get
	BOOL			GetBool				(LPCSTR cmd, BOOL &val);
	F32			GetFloat			(LPCSTR cmd, F32& val, F32& min, F32& max);
	char *			GetString			(LPCSTR cmd);
	int				GetInteger			(LPCSTR cmd, int& val, int& min, int& max);
	char *			GetToken			(LPCSTR cmd);
	xr_token*		GetXRToken			(LPCSTR cmd);
//	char *			GetNextValue		(LPCSTR cmd);
//	char *			GetPrevValue		(LPCSTR cmd);

	void			SelectCommand		();

	// keyboard
	void			OnPressKey			(int dik, BOOL bHold=false);
	virtual void	IR_OnKeyboardPress		(int dik);
	virtual void	IR_OnKeyboardHold		(int dik);
	virtual void	IR_OnKeyboardRelease	(int dik);

	// render & onmove
	virtual void	OnRender			(void);
	virtual void	OnFrame				(void);

	virtual	void	Initialize	();
	virtual void	Destroy		();
};

ENGINE_API extern CConsole* Console;

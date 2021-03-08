#pragma once

#include "IInputReceiver.h"

class ENGINE_API CGameFont;
class ENGINE_API IConsoleCommand;

class ENGINE_API CConsole : public IInputReceiver, public pureRender, public pureFrame
{
public:
	struct str_pred : public std::binary_function<char*, char*, bool>
	{
		inline bool operator ( )							(const char* x, const char* y) const
		{
			return xr_strcmp(x, y) < 0;
		}
	};

	using vecCMD				= xr_map<const char*, IConsoleCommand*, str_pred>;
	using vecCMD_IT				= vecCMD::iterator;

	enum
	{
		MAX_LEN					= 1024
	};

private:
	u32							last_mm_timer;
	float						cur_time;
	float						rep_time;
	float						fAccel;

	int							cmd_delta;
	int							old_cmd_delta;

	char*						editor_last;
	bool						bShift;

	bool						bRepeat;
	bool						bRecordCommands;

protected:
	int							scroll_delta;
	char						editor[MAX_LEN];
	bool						bCursor;

	CGameFont*					pFont;

public:
	virtual						~CConsole				( )
	{ };
	string64					ConfigFile;
	bool						bVisible;
	vecCMD						Commands;

	void						AddCommand				(IConsoleCommand*);
	void						RemoveCommand			(IConsoleCommand*);
	void						Reset					( );

	void						Show					( );
	void						Hide					( );

//	void						Save					( );
	void						Execute					(const char* cmd);
	void						ExecuteScript			(const char* name);
	void						ExecuteCommand			( );

	// get
	BOOL						GetBool					(const char* cmd, BOOL& val);
	float						GetFloat				(const char* cmd, float& val, float& min, float& max);
	char*						GetString				(const char* cmd);
	int							GetInteger				(const char* cmd, int& val, int& min, int& max);
	char*						GetToken				(const char* cmd);
	xr_token*					GetXRToken				(const char* cmd);

	void						SelectCommand			( );

	// keyboard
	void						OnPressKey				(int dik, bool bHold = false);
	virtual void				IR_OnKeyboardPress		(int dik);
	virtual void				IR_OnKeyboardHold		(int dik);
	virtual void				IR_OnKeyboardRelease	(int dik);

	// render & onmove
	virtual void				OnRender				( );
	virtual void				OnFrame					( );

	virtual void				Initialize				( );
	virtual void				Destroy					( );
};

ENGINE_API extern CConsole*		Console;

#include "stdafx.h"

#include <dinput.h>
#include "..\ENGINE\Console.h"
#include "..\ENGINE\IConsoleCommand.h"
#include "..\ENGINE\Input.h"
#include "xr_level_controller.h"
#include "StringTable.h"

_binding g_key_bindings[bindings_count];

_action actions[ ] =
{
	{ "left",				kLEFT },
	{ "right",				kRIGHT },
	{ "up",					kUP },
	{ "down",				kDOWN },
	{ "jump",				kJUMP },
	{ "crouch",				kCROUCH },
	{ "crouch_toggle",		kCROUCH_TOGGLE },
	{ "accel",				kACCEL },
	{ "sprint_toggle",		kSPRINT_TOGGLE },

	{ "forward",			kFWD },
	{ "back",				kBACK },
	{ "lstrafe",			kL_STRAFE },
	{ "rstrafe",			kR_STRAFE },

	{ "llookout",			kL_LOOKOUT },
	{ "rlookout",			kR_LOOKOUT },

	{ "turn_engine",		kENGINE },

	{ "cam_1",				kCAM_1 },
	{ "cam_2",				kCAM_2 },
	{ "cam_3",				kCAM_3 },
	{ "cam_4",				kCAM_4 },
	{ "cam_zoom_in",		kCAM_ZOOM_IN },
	{ "cam_zoom_out",		kCAM_ZOOM_OUT },

	{ "torch",				kTORCH },
	{ "night_vision",		kNIGHT_VISION },
	{ "wpn_1",				kWPN_1 },
	{ "wpn_2",				kWPN_2 },
	{ "wpn_3",				kWPN_3 },
	{ "wpn_4",				kWPN_4 },
	{ "wpn_5",				kWPN_5 },
	{ "wpn_6",				kWPN_6 },
	{ "wpn_next",			kWPN_NEXT },
	{ "wpn_fire",			kWPN_FIRE },
	{ "wpn_zoom",			kWPN_ZOOM },
	{ "wpn_zoom_inc",		kWPN_ZOOM_INC },
	{ "wpn_zoom_dec",		kWPN_ZOOM_DEC },
	{ "wpn_reload",			kWPN_RELOAD },
	{ "wpn_func",			kWPN_FUNC },
	{ "wpn_firemode_prev",	kWPN_FIREMODE_PREV },
	{ "wpn_firemode_next",	kWPN_FIREMODE_NEXT },

	{ "pause",				kPAUSE },
	{ "drop",				kDROP },
	{ "use",				kUSE },
	{ "scores",				kSCORES },
	{ "screenshot",			kSCREENSHOT },
	{ "quit",				kQUIT },
	{ "console",			kCONSOLE },
	{ "inventory",			kINVENTORY },
	{ "active_jobs",		kACTIVE_JOBS },
	{ "map",				kMAP },
	{ "contacts",			kCONTACTS },
	{ "ext_1",				kEXT_1 },

	{ "next_slot",			kNEXT_SLOT },
	{ "prev_slot",			kPREV_SLOT },

	{ "use_bandage",		kUSE_BANDAGE },
	{ "use_medkit",			kUSE_MEDKIT },
	{ "quick_save",			kQUICK_SAVE },
	{ "quick_load",			kQUICK_LOAD },

	{ nullptr,				kLASTACTION }
};

_keyboard keyboards[ ] =
{
	{ "kESCAPE",		DIK_ESCAPE		},
	{ "k1",				DIK_1			},
	{ "k2",				DIK_2			},
	{ "k3",				DIK_3			},
	{ "k4",				DIK_4			},
	{ "k5",				DIK_5			},
	{ "k6",				DIK_6			},
	{ "k7",				DIK_7			},
	{ "k8",				DIK_8			},
	{ "k9",				DIK_9			},
	{ "k0",				DIK_0			},
	{ "kMINUS",			DIK_MINUS		},
	{ "kEQUALS",		DIK_EQUALS		},
	{ "kBACK",			DIK_BACK		},
	{ "kTAB",			DIK_TAB 		},
	{ "kQ",				DIK_Q			},
	{ "kW",				DIK_W			},
	{ "kE",				DIK_E			},
	{ "kR",				DIK_R			},
	{ "kT",				DIK_T			},
	{ "kY",				DIK_Y			},
	{ "kU",				DIK_U			},
	{ "kI",				DIK_I			},
	{ "kO",				DIK_O			},
	{ "kP",				DIK_P			},
	{ "kLBRACKET",		DIK_LBRACKET	},
	{ "kRBRACKET",		DIK_RBRACKET	},
	{ "kRETURN",		DIK_RETURN		},
	{ "kLCONTROL",		DIK_LCONTROL	},
	{ "kA",				DIK_A			},
	{ "kS",				DIK_S			},
	{ "kD",				DIK_D			},
	{ "kF",				DIK_F			},
	{ "kG",				DIK_G			},
	{ "kH",				DIK_H			},
	{ "kJ",				DIK_J			},
	{ "kK",				DIK_K			},
	{ "kL",				DIK_L			},
	{ "kSEMICOLON",		DIK_SEMICOLON	},
	{ "kAPOSTROPHE",	DIK_APOSTROPHE	},
	{ "kGRAVE",			DIK_GRAVE		},
	{ "kLSHIFT",		DIK_LSHIFT		},
	{ "kBACKSLASH",		DIK_BACKSLASH	},
	{ "kZ",				DIK_Z			},
	{ "kX",				DIK_X			},
	{ "kC",				DIK_C			},
	{ "kV",				DIK_V			},
	{ "kB",				DIK_B			},
	{ "kN",				DIK_N			},
	{ "kM",				DIK_M			},
	{ "kCOMMA",			DIK_COMMA		},
	{ "kPERIOD",		DIK_PERIOD		},
	{ "kSLASH",			DIK_SLASH		},
	{ "kRSHIFT",		DIK_RSHIFT		},
	{ "kMULTIPLY",		DIK_MULTIPLY	},
	{ "kLMENU",			DIK_LMENU		},
	{ "kSPACE",			DIK_SPACE		},
	{ "kCAPITAL",		DIK_CAPITAL		},
	{ "kF1",			DIK_F1			},
	{ "kF2",			DIK_F2			},
	{ "kF3",			DIK_F3			},
	{ "kF4",			DIK_F4			},
	{ "kF5",			DIK_F5			},
	{ "kF6",			DIK_F6			},
	{ "kF7",			DIK_F7			},
	{ "kF8",			DIK_F8			},
	{ "kF9",			DIK_F9			},
	{ "kF10",			DIK_F10			},
	{ "kNUMLOCK",		DIK_NUMLOCK		},
	{ "kSCROLL",		DIK_SCROLL		},
	{ "kNUMPAD7",		DIK_NUMPAD7		},
	{ "kNUMPAD8",		DIK_NUMPAD8		},
	{ "kNUMPAD9",		DIK_NUMPAD9		},
	{ "kSUBTRACT",		DIK_SUBTRACT	},
	{ "kNUMPAD4",		DIK_NUMPAD4		},
	{ "kNUMPAD5",		DIK_NUMPAD5		},
	{ "kNUMPAD6",		DIK_NUMPAD6		},
	{ "kADD",			DIK_ADD			},
	{ "kNUMPAD1",		DIK_NUMPAD1		},
	{ "kNUMPAD2",		DIK_NUMPAD2		},
	{ "kNUMPAD3",		DIK_NUMPAD3		},
	{ "kNUMPAD0",		DIK_NUMPAD0		},
	{ "kDECIMAL",		DIK_DECIMAL		},
	{ "kF11",			DIK_F11			},
	{ "kF12",			DIK_F12			},
	{ "kF13",			DIK_F13			},
	{ "kF14",			DIK_F14			},
	{ "kF15",			DIK_F15			},
	{ "kKANA",			DIK_KANA		},
	{ "kCONVERT",		DIK_CONVERT		},
	{ "kNOCONVERT",		DIK_NOCONVERT	},
	{ "kYEN",			DIK_YEN			},
	{ "kNUMPADEQUALS",	DIK_NUMPADEQUALS},
	{ "kCIRCUMFLEX",	DIK_CIRCUMFLEX	},
	{ "kAT",			DIK_AT			},
	{ "kCOLON",			DIK_COLON		},
	{ "kUNDERLINE",		DIK_UNDERLINE	},
	{ "kKANJI",			DIK_KANJI		},
	{ "kSTOP",			DIK_STOP		},
	{ "kAX",			DIK_AX			},
	{ "kUNLABELED",		DIK_UNLABELED	},
	{ "kNUMPADENTER",	DIK_NUMPADENTER	},
	{ "kRCONTROL",		DIK_RCONTROL	},
	{ "kNUMPADCOMMA",	DIK_NUMPADCOMMA	},
	{ "kDIVIDE",		DIK_DIVIDE		},
	{ "kSYSRQ",			DIK_SYSRQ		},
	{ "kRMENU",			DIK_RMENU		},
	{ "kHOME",			DIK_HOME		},
	{ "kUP",			DIK_UP			},
	{ "kPRIOR",			DIK_PRIOR		},
	{ "kLEFT",			DIK_LEFT		},
	{ "kRIGHT",			DIK_RIGHT		},
	{ "kEND",			DIK_END			},
	{ "kDOWN",			DIK_DOWN		},
	{ "kNEXT",			DIK_NEXT		},
	{ "kINSERT",		DIK_INSERT		},
	{ "kDELETE",		DIK_DELETE		},
	{ "kLWIN",			DIK_LWIN		},
	{ "kRWIN",			DIK_RWIN		},
	{ "kAPPS",			DIK_APPS		},
	{ "kPAUSE",			DIK_PAUSE		},
	{ "mouse1",			MOUSE_1			},
	{ "mouse2",			MOUSE_2			},
	{ "mouse3",			MOUSE_3			},
	{ "mouse4",			MOUSE_4			},
	{ "mouse5",			MOUSE_5			},
	{ "mouse6",			MOUSE_6			},
	{ "mouse7",			MOUSE_7			},
	{ "mouse8",			MOUSE_8			},
	{ nullptr,			0				}
};

void initialize_bindings( )
{

#ifdef DEBUG
	int i1 = 0;
	while (true)
	{
		_keyboard& _k1 = keyboards[i1];
		if (_k1.key_name == NULL)	break;
		int i2 = i1;
		while (true)
		{
			_keyboard& _k2 = keyboards[i2];
			if (_k2.key_name == NULL)	break;
			if (_k1.dik == _k2.dik && i1 != i2)
			{
				Msg("%s==%s", _k1.key_name, _k2.key_name);
			}
			++i2;
		}
		++i1;
	}
#endif

	for (int idx = 0; idx < bindings_count; ++idx)
		g_key_bindings[idx].m_action = &actions[idx];
}

void remap_keys( )
{
	int idx = 0;
	string128			buff;
	while (keyboards[idx].key_name)
	{
		buff[0] = 0;
		_keyboard& kb = keyboards[idx];
		bool res = pInput->get_dik_name(kb.dik, buff, 128);
		if (res)
			kb.key_local_name = buff;
		else
			kb.key_local_name = kb.key_name;

//.		Msg("[%s]-[%s]",kb.key_name, kb.key_local_name.c_str());
		++idx;
	}
}

EGameActions action_name_to_id(const char* _name)
{
	_action* action = action_name_to_ptr(_name);
	if (action)
	{
		return action->id;
	}
	else
	{
		return kNOTBINDED;
	}
}

_action* action_name_to_ptr(const char* _name)
{
	int idx = 0;
	while (actions[idx].action_name)
	{
		if (!stricmp(_name, actions[idx].action_name))
			return &actions[idx];
		++idx;
	}
	Msg("! cant find corresponding [id] for action_name %s", _name);
	return			NULL;
}

const char* dik_to_keyname(int _dik)
{
	_keyboard* kb = dik_to_ptr(_dik, true);
	if (kb)
	{
		return kb->key_name;
	}
	else
	{
		return nullptr;
	}
}

_keyboard* dik_to_ptr(int _dik, bool bSafe)
{
	int idx = 0;
	while (keyboards[idx].key_name)
	{
		_keyboard& kb = keyboards[idx];
		if (kb.dik == _dik)
			return &keyboards[idx];
		++idx;
	}
	if (!bSafe)
		Msg("! cant find corresponding [_keyboard] for dik");
	return			nullptr;
}

int	keyname_to_dik(const char* _name)
{
	_keyboard* _kb = keyname_to_ptr(_name);
	return _kb->dik;
}

_keyboard* keyname_to_ptr(const char* _name)
{
	int idx = 0;
	while (keyboards[idx].key_name)
	{
		_keyboard& kb = keyboards[idx];
		if (!stricmp(_name, kb.key_name))
			return &keyboards[idx];
		++idx;
	}

	Msg("! cant find corresponding [_keyboard*] for keyname %s", _name);
	return			nullptr;
}

bool is_binded(EGameActions _action_id, int _dik)
{
	_binding* pbinding = &g_key_bindings[_action_id];
	if (pbinding->m_keyboard[0] && pbinding->m_keyboard[0]->dik == _dik)
		return true;

	if (pbinding->m_keyboard[1] && pbinding->m_keyboard[1]->dik == _dik)
		return true;

	return false;
}

int get_action_dik(EGameActions _action_id)
{
	_binding* pbinding = &g_key_bindings[_action_id];

	if (pbinding->m_keyboard[0])
		return pbinding->m_keyboard[0]->dik;

	if (pbinding->m_keyboard[1])
		return pbinding->m_keyboard[1]->dik;

	return 0;
}

EGameActions get_binded_action(int _dik)
{
	for (int idx = 0; idx < bindings_count; ++idx)
	{
		_binding* binding = &g_key_bindings[idx];

		if (binding->m_keyboard[0] && binding->m_keyboard[0]->dik == _dik)
			return binding->m_action->id;

		if (binding->m_keyboard[1] && binding->m_keyboard[1]->dik == _dik)
			return binding->m_action->id;
	}

	return kNOTBINDED;
}

void GetActionAllBinding(const char* _action, char* dst_buff, int dst_buff_sz)
{
	int action_id = action_name_to_id(_action);
	_binding* pbinding = &g_key_bindings[action_id];

	string128	prim;
	string128	sec;
	prim[0] = 0;
	sec[0] = 0;

	if (pbinding->m_keyboard[0])
	{
		strcpy_s(prim, pbinding->m_keyboard[0]->key_local_name.c_str( ));
	}

	if (pbinding->m_keyboard[1])
	{
		strcpy_s(sec, pbinding->m_keyboard[1]->key_local_name.c_str( ));
	}

	sprintf_s(dst_buff, dst_buff_sz, "%s%s%s", prim[0] ? prim : "", (sec[0] && prim[0]) ? " , " : "", sec[0] ? sec : "");
}

ConsoleBindCmds	bindConsoleCmds;
bool bRemapped = false;

class CCC_Bind : public IConsoleCommand
{
	int m_work_idx;
public:
	CCC_Bind(const char* N, int idx) : IConsoleCommand(N), m_work_idx(idx)
	{ };
	virtual void Execute(const char* args)
	{
		string256							action;
		string256							key;
		*action = 0;
		*key = 0;

		sscanf(args, "%s %s", action, key);
		if (!*action)
			return;

		if (!*key)
			return;

		if (!bRemapped)
		{
			remap_keys( );
			bRemapped = true;
		}

		if (!action_name_to_ptr(action))
			return;

		int action_id = action_name_to_id(action);
		if (action_id == kNOTBINDED)
			return;

		_keyboard* pkeyboard = keyname_to_ptr(key);
		if (!pkeyboard)
			return;

		_binding* curr_pbinding = &g_key_bindings[action_id];

		curr_pbinding->m_keyboard[m_work_idx] = pkeyboard;

		{
			for (int idx = 0; idx < bindings_count; ++idx)
			{
				_binding* binding = &g_key_bindings[idx];
				if (binding == curr_pbinding)	continue;

				if (binding->m_keyboard[0] == pkeyboard)
					binding->m_keyboard[0] = NULL;

				if (binding->m_keyboard[1] == pkeyboard)
					binding->m_keyboard[1] = NULL;
			}
		}

		CStringTable::ReparseKeyBindings( );
	}
	virtual void Save(IWriter* F)
	{
		if (m_work_idx == 0)
			F->w_printf("unbindall\r\n");

		for (int idx = 0; idx < bindings_count; ++idx)
		{
			_binding* pbinding = &g_key_bindings[idx];
			if (pbinding->m_keyboard[m_work_idx])
			{
				F->w_printf("%s %s %s\r\n",
							cName,
							pbinding->m_action->action_name,
							pbinding->m_keyboard[m_work_idx]->key_name);
			}
		}
	}
};

class CCC_UnBind : public IConsoleCommand
{
	int m_work_idx;
public:
	CCC_UnBind(const char* N, int idx) : IConsoleCommand(N), m_work_idx(idx)
	{
		bEmptyArgsHandled = true;
	};
	virtual void Execute(const char* args)
	{
		int action_id = action_name_to_id(args);
		_binding* pbinding = &g_key_bindings[action_id];
		pbinding->m_keyboard[m_work_idx] = NULL;

		CStringTable::ReparseKeyBindings( );
	}
};

class CCC_ListActions : public IConsoleCommand
{
public:
	CCC_ListActions(const char* N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	};

	virtual void Execute(const char* args)
	{
		Log("- --- Action list start ---");
		for (int idx = 0; idx < bindings_count; ++idx)
		{
			_binding* pbinding = &g_key_bindings[idx];
			Log("-", pbinding->m_action->action_name);
		}

		Log("- --- Action list end   ---");
	}
};

class CCC_UnBindAll : public IConsoleCommand
{
public:
	CCC_UnBindAll(const char* N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	};

	virtual void Execute(const char* args)
	{
		for (int idx = 0; idx < bindings_count; ++idx)
		{
			_binding* pbinding = &g_key_bindings[idx];
			pbinding->m_keyboard[0] = NULL;
			pbinding->m_keyboard[1] = NULL;
		}

		bindConsoleCmds.clear( );

		string_path				_cfg;
		string_path				cmd;
		FS.update_path(_cfg, "$game_config$", "default_controls.ltx");
		strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
		Console->Execute(cmd);
	}
};

class CCC_BindList : public IConsoleCommand
{
public:
	CCC_BindList(const char* N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	};

	virtual void Execute(const char* args)
	{
		Log("- --- Bind list start ---");
		string512		buff;

		for (int idx = 0; idx < bindings_count; ++idx)
		{
			_binding* pbinding = &g_key_bindings[idx];
			sprintf_s(buff, "[%s] primary is[%s] secondary is[%s]",
					  pbinding->m_action->action_name,
					  (pbinding->m_keyboard[0]) ? pbinding->m_keyboard[0]->key_local_name.c_str( ) : "NULL",
					  (pbinding->m_keyboard[1]) ? pbinding->m_keyboard[1]->key_local_name.c_str( ) : "NULL");
			Log(buff);
		}

		Log("- --- Bind list end   ---");
	}
};

class CCC_BindConsoleCmd : public IConsoleCommand
{
public:
	CCC_BindConsoleCmd(const char* N) : IConsoleCommand(N)
	{ };
	virtual void Execute(const char* args)
	{
		string512				console_command;
		string256				key;
		int cnt = _GetItemCount(args, ' ');
		_GetItems(args, 0, cnt - 1, console_command, ' ');
		_GetItem(args, cnt - 1, key, ' ');

		int dik = keyname_to_dik(key);
		bindConsoleCmds.bind(dik, console_command);
	}

	virtual void Save(IWriter* F)
	{
		bindConsoleCmds.save(F);
	}
};

class CCC_UnBindConsoleCmd : public IConsoleCommand
{
public:
	CCC_UnBindConsoleCmd(const char* N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = false;
	};

	virtual void Execute(const char* args)
	{
		int _dik = keyname_to_dik(args);
		bindConsoleCmds.unbind(_dik);
	}
};

void ConsoleBindCmds::bind(int dik, const char* N)
{
	_conCmd& c = m_bindConsoleCmds[dik];
	c.cmd = N;
}

void ConsoleBindCmds::unbind(int dik)
{
	xr_map<int, _conCmd>::iterator it = m_bindConsoleCmds.find(dik);
	if (it == m_bindConsoleCmds.end( ))
		return;

	m_bindConsoleCmds.erase(it);
}

void ConsoleBindCmds::clear( )
{
	m_bindConsoleCmds.clear( );
}

bool ConsoleBindCmds::execute(int dik)
{
	xr_map<int, _conCmd>::iterator it = m_bindConsoleCmds.find(dik);
	if (it == m_bindConsoleCmds.end( ))
		return false;

	Console->Execute(it->second.cmd.c_str( ));
	return true;
}

void ConsoleBindCmds::save(IWriter* F)
{
	xr_map<int, _conCmd>::iterator it = m_bindConsoleCmds.begin( );

	for (; it != m_bindConsoleCmds.end( ); ++it)
	{
		const char* keyname = dik_to_keyname(it->first);
		F->w_printf("bind_console %s %s\n", *it->second.cmd, keyname);
	}
}

void CCC_RegisterInput( )
{
	initialize_bindings( );
	CMD2(CCC_Bind, "bind", 0);
	CMD2(CCC_Bind, "bind_sec", 1);
	CMD2(CCC_UnBind, "unbind", 0);
	CMD2(CCC_UnBind, "unbind_sec", 1);
	CMD1(CCC_UnBindAll, "unbindall");
	CMD1(CCC_ListActions, "list_actions");

	CMD1(CCC_BindList, "bind_list");
	CMD1(CCC_BindConsoleCmd, "bind_console");
	CMD1(CCC_UnBindConsoleCmd, "unbind_console");
};

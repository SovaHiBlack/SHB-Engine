#pragma once

enum EGameActions
{
	kLEFT,
	kRIGHT,
	kUP,
	kDOWN,
	kJUMP,
	kCROUCH,
	kCROUCH_TOGGLE,
	kACCEL,
	kSPRINT_TOGGLE,

	kFWD,
	kBACK,
	kL_STRAFE,
	kR_STRAFE,

	kL_LOOKOUT,
	kR_LOOKOUT,

	kENGINE,
	kCAM_1,
	kCAM_2,
	kCAM_3,
	kCAM_4,
	kCAM_ZOOM_IN,
	kCAM_ZOOM_OUT,

	kTORCH,
	kNIGHT_VISION,
	kWPN_1,
	kWPN_2,
	kWPN_3,
	kWPN_4,
	kWPN_5,
	kWPN_6,
	kWPN_NEXT,
	kWPN_FIRE,
	kWPN_ZOOM,
	kWPN_ZOOM_INC,
	kWPN_ZOOM_DEC,
	kWPN_RELOAD,
	kWPN_FUNC,
	kWPN_FIREMODE_PREV,
	kWPN_FIREMODE_NEXT,

	kPAUSE,
	kDROP,
	kUSE,
	kSCORES,
	kSCREENSHOT,
	kQUIT,
	kCONSOLE,
	kINVENTORY,
	kACTIVE_JOBS,
	kMAP,
	kCONTACTS,
	kEXT_1,

	kNEXT_SLOT,
	kPREV_SLOT,

	kUSE_BANDAGE,
	kUSE_MEDKIT,

	kQUICK_SAVE,
	kQUICK_LOAD,

	kLASTACTION,
	kNOTBINDED,
	kFORCEDWORD = u32(-1)
};

struct _keyboard
{
	const char*										key_name;
	int												dik;
	xr_string										key_local_name;
};

struct _action
{
	const char*										action_name;
	EGameActions									id;
};

const char*					dik_to_keyname			(int _dik);
int							keyname_to_dik			(const char* _name);
_keyboard*					keyname_to_ptr			(const char* _name);
_keyboard*					dik_to_ptr				(int _dik, bool bSafe);

EGameActions				action_name_to_id		(const char* _name);
_action*					action_name_to_ptr		(const char* _name);

extern _action										actions[ ];

#define bindings_count								kLASTACTION

struct _binding
{
	_action*										m_action;
	_keyboard*										m_keyboard[2];
};

extern _binding										g_key_bindings[ ];

bool						is_binded				(EGameActions action_id, int dik);
int							get_action_dik			(EGameActions action_id);
EGameActions				get_binded_action		(int dik);

extern void					CCC_RegisterInput		( );

struct _conCmd
{
	CSharedString										cmd;
};

class ConsoleBindCmds
{
public:
	xr_map<int, _conCmd>							m_bindConsoleCmds;

	void					bind					(int dik, const char* N);
	void					unbind					(int dik);
	bool					execute					(int dik);
	void					clear					( );
	void					save					(IWriter* F);
};

void						GetActionAllBinding		(const char* action, char* dst_buff, int dst_buff_sz);

extern ConsoleBindCmds								bindConsoleCmds;

// 0xED - max vavue in DIK* enum
#define MOUSE_1										(0xED + 100)
#define MOUSE_2										(0xED + 101)
#define MOUSE_3										(0xED + 102)
#define MOUSE_4										(0xED + 103)
#define MOUSE_5										(0xED + 104)
#define MOUSE_6										(0xED + 105)
#define MOUSE_7										(0xED + 106)
#define MOUSE_8										(0xED + 107)

#pragma once

// refs
class CConfigurationFile;
class CFontBase;

class CContextMenu
{
public:
	struct MenuItem
	{
		char*					Name;
		EVENT					Event;
		char*					Param;
	};
	char*						Name;
	xr_vector<MenuItem>			Items;

				~CContextMenu	( );
	void		Load			(CConfigurationFile* INI, const char* S);
	void		Render			(CGameFont* F, u32 cT, u32 cI, float s);
	void		Select			(int num);
};

#pragma once

// refs
class CInifile;
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
public:
				~CContextMenu	();
	void		Load			(CInifile* INI, pcstr S);
	void		Render			(CGameFont* F, u32 cT, u32 cI, float s);
	void		Select			(int num);
};

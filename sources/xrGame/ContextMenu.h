#pragma once

// refs
class CIniFile;
class CFontBase;

class CContextMenu
{
public:
	struct MenuItem
	{
		pstr					Name;
		EVENT					Event;
		pstr					Param;
	};
	pstr						Name;
	xr_vector<MenuItem>			Items;

public:
	~CContextMenu( );
	void		Load(CIniFile* INI, pcstr S);
	void		Render(CGameFont* F, u32 cT, u32 cI, f32 s);
	void		Select(s32 num);
};

#include "stdafx.h"
#include "contextmenu.h"
#include "..\XR_3DA\gamefont.h"

const f32 fade_speed = 8.0f;

CContextMenu::~CContextMenu( )
{
	for (xr_vector<MenuItem>::iterator I = Items.begin( ); Items.end( ) != I; ++I)
	{
		Engine.Event.Destroy(I->Event);
		xr_free(I->Name);
		xr_free(I->Param);
	}

	xr_free(Name);
}

void CContextMenu::Load(CIniFile* INI, pcstr SECT)
{
	CIniFile::Sect& S = INI->r_section(SECT);
	for (CIniFile::SectCIt I = S.Data.begin( ); S.Data.end( ) != I; ++I)
	{
		string128 Event;
		string128 Param;
		Event[0] = 0;
		Param[0] = 0;
		sscanf(*I->second, "%[^,],%s", Event, Param);
		MenuItem	Item;
		Item.Name = xr_strdup(*I->first);
		Item.Event = Engine.Event.Create(Event);
		Item.Param = xr_strdup(Param);
		Items.push_back(Item);
	}
}

void CContextMenu::Render(CGameFont* F, u32 cT, u32 cI, f32 s)
{
	F->SetHeightI(0.05f);
	F->SetColor(cT);
	F->OutNext("%s", Name);
	F->SetColor(cI);
	F->SetHeightI(0.03f);

	for (u32 i = 0; i < Items.size( ); ++i)
	{
		F->OutNext("%d. %s", i, (pstr)Items[i].Name);
	}
}

void CContextMenu::Select(s32 I)
{
	if (I >= 0 && I < (s32)(Items.size( )))
	{
		MenuItem& M = Items[I];
		Engine.Event.Signal(M.Event, u64(M.Param));
	}
}

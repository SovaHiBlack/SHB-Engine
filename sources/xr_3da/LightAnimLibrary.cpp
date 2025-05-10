#include "stdafx.h"

#include "LightAnimLibrary.h"

//---------------------------------------------------------
#define LANIM_VERSION						0x0001
//---------------------------------------------------------
#define CHUNK_VERSION						0x0000
#define CHUNK_ITEM_LIST						0x0001
//---------------------------------------------------------
#define CHUNK_ITEM_COMMON					0x0001
#define CHUNK_ITEM_KEYS						0x0002
//---------------------------------------------------------

CLightAnimLibrary							LALib;

//-------------------------------------------------------------------------------------------------
// Item
//-------------------------------------------------------------------------------------------------
CLightAnimItem::CLightAnimItem( )
{
	fFPS = 15.0f;
	iFrameCount = 1;
}

void CLightAnimItem::InitDefault( )
{
	Keys[0] = 0x00000000;
}

void CLightAnimItem::Load(IReader& F)
{
	R_ASSERT(F.find_chunk(CHUNK_ITEM_COMMON));
	F.r_stringZ(cName);
	fFPS = F.r_float( );
	iFrameCount = F.r_u32( );

	s32										key_cnt;
	s32										key;
	R_ASSERT(F.find_chunk(CHUNK_ITEM_KEYS));
	key_cnt = F.r_u32( );
	for (s32 i = 0; i < key_cnt; i++)
	{
		key = F.r_u32( );
		Keys[key] = F.r_u32( );
	}
}

void CLightAnimItem::Save(IWriter& F)
{
	F.open_chunk(CHUNK_ITEM_COMMON);
	F.w_stringZ(cName);
	F.w_float(fFPS);
	F.w_u32(iFrameCount);
	F.close_chunk( );

	F.open_chunk(CHUNK_ITEM_KEYS);
	F.w_u32(Keys.size( ));
	for (KeyPairIt it = Keys.begin( ); it != Keys.end( ); it++)
	{
		F.w_u32(it->first);
		F.w_u32(it->second);
	}

	F.close_chunk( );
}

void CLightAnimItem::InsertKey(s32 frame, u32 color)
{
	R_ASSERT(frame <= iFrameCount);
	Keys[frame] = color;
}

void CLightAnimItem::DeleteKey(s32 frame)
{
	R_ASSERT(frame <= iFrameCount);
	if (0 == frame)
	{
		return;
	}

	KeyPairIt it = Keys.find(frame);
	if (it != Keys.end( ))
	{
		Keys.erase(it);
	}
}

void CLightAnimItem::MoveKey(s32 from, s32 to)
{
	R_ASSERT(from <= iFrameCount);
	R_ASSERT(to <= iFrameCount);
	KeyPairIt it = Keys.find(from);
	if (it != Keys.end( ))
	{
		Keys[to] = it->second;
		Keys.erase(it);
	}
}

void CLightAnimItem::Resize(s32 new_len)
{
	VERIFY(new_len >= 1);
	if (new_len != iFrameCount)
	{
		if (new_len > iFrameCount)
		{
			s32 old_len = iFrameCount;
			iFrameCount = new_len;
			MoveKey(old_len, new_len);
		}
		else
		{
			KeyPairIt I = Keys.upper_bound(new_len - 1);
			if (I != Keys.end( ))
			{
				Keys.erase(I, Keys.end( ));
			}

			iFrameCount = new_len;
		}
	}
}

u32 CLightAnimItem::InterpolateRGB(s32 frame)
{
	R_ASSERT(frame <= iFrameCount);

	KeyPairIt A = Keys.find(frame);
	KeyPairIt B;
	if (A != Keys.end( ))
	{	// ключ - возвращаем цвет ключа
		return A->second;
	}
	else
	{	// не ключ
		B = Keys.upper_bound(frame);	// ищем следующий ключ
		if (B == Keys.end( ))
		{	// если его нет вернем цвет последнего ключа
			B--;
			return B->second;
		}

		A = B;						// иначе в A занесем предыдущий ключ
		A--;
	}

	R_ASSERT(Keys.size( ) > 1);
	// интерполируем цвет
	fColor									c;
	fColor									c0;
	fColor									c1;
	f32 a0 = (f32)A->first;
	f32 a1 = (f32)B->first;
	c0.set(A->second);
	c1.set(B->second);
	f32 t = f32(frame - a0) / f32(a1 - a0);
	c.lerp(c0, c1, t);
	return c.get( );
}

u32 CLightAnimItem::InterpolateBGR(s32 frame)
{
	u32 c = InterpolateRGB(frame);
	return color_rgba(color_get_B(c), color_get_G(c), color_get_R(c), color_get_A(c));
}

u32 CLightAnimItem::CalculateRGB(f32 T, s32& frame)
{
	frame = iFloor(fmodf(T, f32(iFrameCount) / fFPS) * fFPS);
	return InterpolateRGB(frame);
}

u32 CLightAnimItem::CalculateBGR(f32 T, s32& frame)
{
	frame = iFloor(fmodf(T, f32(iFrameCount) / fFPS) * fFPS);
	return InterpolateBGR(frame);
}

s32 CLightAnimItem::PrevKeyFrame(s32 frame)
{
	KeyPairIt A = Keys.lower_bound(frame);
	if (A != Keys.end( ))
	{
		KeyPairIt B = A;
		B--;
		if (B != Keys.end( ))
		{
			return B->first;
		}

		return A->first;
	}
	else
	{
		return Keys.rbegin( )->first;
	}
}

s32 CLightAnimItem::NextKeyFrame(s32 frame)
{
	KeyPairIt A = Keys.upper_bound(frame);
	if (A != Keys.end( ))
	{
		return A->first;
	}
	else
	{
		return Keys.rbegin( )->first;
	}
}

//-------------------------------------------------------------------------------------------------
// Library
//-------------------------------------------------------------------------------------------------
CLightAnimLibrary::CLightAnimLibrary( )
{ }

CLightAnimLibrary::~CLightAnimLibrary( )
{ }

void CLightAnimLibrary::OnCreate( )
{
	Load( );
}

void CLightAnimLibrary::OnDestroy( )
{
	Unload( );
}

void CLightAnimLibrary::Unload( )
{
	for (LightAnimItem_it it = Items.begin( ); it != Items.end( ); it++)
	{
		xr_delete(*it);
	}

	Items.clear( );
}

void CLightAnimLibrary::Load( )
{
	string_path								fn;
	FS.update_path(fn, "$game_data$", "lanims.xr");
	IReader* fs = FS.r_open(fn);
	if (fs)
	{
		u16 version = 0;
		if (fs->find_chunk(CHUNK_VERSION))
		{
			version = fs->r_u16( );
		}

		IReader* OBJ = fs->open_chunk(CHUNK_ITEM_LIST);
		if (OBJ)
		{
			IReader* O = OBJ->open_chunk(0);
			for (s32 count = 1; O; count++)
			{
				CLightAnimItem* I = xr_new<CLightAnimItem>( );
				I->Load(*O);
				if (version == 0)
				{
					for (CLightAnimItem::KeyPairIt it = I->Keys.begin( ); it != I->Keys.end( ); it++)
					{
						it->second = subst_alpha(bgr2rgb(it->second), color_get_A(it->second));
					}
				}

				Items.push_back(I);
				O->close( );
				O = OBJ->open_chunk(count);
			}

			OBJ->close( );
		}

		FS.r_close(fs);
	}
}

void CLightAnimLibrary::Save( )
{
	CMemoryWriter							F;
	F.open_chunk(CHUNK_VERSION);
	F.w_u16(LANIM_VERSION);
	F.close_chunk( );
	F.open_chunk(CHUNK_ITEM_LIST);
	s32 count = 0;
	for (LightAnimItem_it it = Items.begin( ); it != Items.end( ); it++)
	{
		F.open_chunk(count++);
		(*it)->Save(F);
		F.close_chunk( );
	}

	F.close_chunk( );

	string_path								fn;
	FS.update_path(fn, "$game_data$", "lanims.xr");

	if (!F.save_to(fn))
	{
		Log("!Can't save color animations:", fn);
	}
}

void CLightAnimLibrary::Reload( )
{
	Unload( );
	Load( );
}

LightAnimItem_it CLightAnimLibrary::FindItemI(pcstr name)
{
	if (name && name[0])
	{
		for (LightAnimItem_it it = Items.begin( ); it != Items.end( ); it++)
		{
			if (0 == xr_strcmp((*it)->cName, name))
			{
				return it;
			}
		}
	}

	return Items.end( );
}

CLightAnimItem* CLightAnimLibrary::FindItem(pcstr name)
{
	LightAnimItem_it it = FindItemI(name);
	return ((it != Items.end( )) ? *it : 0);
}

CLightAnimItem* CLightAnimLibrary::AppendItem(pcstr name, CLightAnimItem* src)
{
	VERIFY2(FindItem(name) == 0, "Duplicate name found.");
	CLightAnimItem* I = xr_new<CLightAnimItem>( );
	if (src)
	{
		*I = *src;
	}
	else
	{
		I->InitDefault( );
	}

	I->cName = name;
	Items.push_back(I);
	return I;
}

#pragma once

class ENGINE_API CLightAnimItem
{
public:
	shared_str										cName;
	f32												fFPS;
	DEFINE_MAP										(s32, u32, KeyMap, KeyPairIt);
	KeyMap											Keys;
	s32												iFrameCount;

public:
							CLightAnimItem			( );

	void					InitDefault				( );
	void					Load					(IReader& F);
	void					Save					(IWriter& F);
	f32						Length_sec				( )
	{
		return (f32(iFrameCount) / fFPS);
	}
	u32						Length_ms				( )
	{
		return iFloor(Length_sec( ) * 1000.0f);
	}
	u32						InterpolateRGB			(s32 frame);
	u32						InterpolateBGR			(s32 frame);
	u32						CalculateRGB			(f32 T, s32& frame);
	u32						CalculateBGR			(f32 T, s32& frame);
	void					Resize					(s32 new_len);
	void					InsertKey				(s32 frame, u32 color);
	void					DeleteKey				(s32 frame);
	void					MoveKey					(s32 from, s32 to);
	bool					IsKey					(s32 frame)
	{
		return (Keys.end( ) != Keys.find(frame));
	}
	s32						PrevKeyFrame			(s32 frame);
	s32						NextKeyFrame			(s32 frame);
	s32						FirstKeyFrame			( )
	{
		return Keys.rend( )->first;
	}
	s32						LastKeyFrame			( )
	{
		return Keys.rbegin( )->first;
	}
	u32*					GetKey					(s32 frame)
	{
		KeyPairIt it								= Keys.find(frame);
		return ((it != Keys.end( )) ? &(it->second) : 0);
	}
};

DEFINE_VECTOR										(CLightAnimItem*, LightAnimItem_vec, LightAnimItem_it);

class ENGINE_API CLightAnimLibrary
{
public:
	LightAnimItem_vec								Items;
	LightAnimItem_it		FindItemI				(pcstr name);
	CLightAnimItem*			FindItem				(pcstr name);

public:
							CLightAnimLibrary		( );
							~CLightAnimLibrary		( );

	void					OnCreate				( );
	void					OnDestroy				( );
	void					Load					( );
	void					Save					( );
	void					Reload					( );
	void					Unload					( );
	CLightAnimItem*			AppendItem				(pcstr name, CLightAnimItem* src);
	LightAnimItem_vec&		Objects					( )
	{
		return Items;
	}
};

extern ENGINE_API CLightAnimLibrary					LALib;

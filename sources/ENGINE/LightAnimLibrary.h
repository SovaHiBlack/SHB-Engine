#pragma once

class ENGINE_API CLAItem
{
public:
	CSharedString		cName;
	float			fFPS;
//	DEFINE_MAP(int, unsigned int, KeyMap, KeyPairIt);
	using KeyMap = xr_map<int, unsigned int>;
	using KeyPairIt = KeyMap::iterator;
	KeyMap			Keys;
	int				iFrameCount;

					CLAItem( );

	void			InitDefault( );
	void			Load(IReader& F);
	void			Save(IWriter& F);
	float			Length_sec( )
	{
		return float(iFrameCount) / fFPS;
	}
	unsigned int				Length_ms( )
	{
		return iFloor(Length_sec( ) * 1000.0f);
	}
	unsigned int				InterpolateRGB(int frame);
	unsigned int				InterpolateBGR(int frame);
	unsigned int				CalculateRGB(float T, int& frame);
	unsigned int				CalculateBGR(float T, int& frame);
	void			Resize(int new_len);
	void			InsertKey(int frame, unsigned int color);
	void			DeleteKey(int frame);
	void			MoveKey(int from, int to);
	bool			IsKey(int frame)
	{
		return (Keys.end( ) != Keys.find(frame));
	}
	int				PrevKeyFrame(int frame);
	int				NextKeyFrame(int frame);
	int				FirstKeyFrame( )
	{
		return Keys.rend( )->first;
	}
	int				LastKeyFrame( )
	{
		return Keys.rbegin( )->first;
	}
	unsigned int*			GetKey(int frame)
	{
		KeyPairIt it = Keys.find(frame);
		return (it != Keys.end( )) ? &(it->second) : 0;
	}
};

//DEFINE_VECTOR(CLAItem*, LAItemVec, LAItemIt);
using LAItemVec = xr_vector<CLAItem*>;
using LAItemIt = LAItemVec::iterator;

class ENGINE_API ELightAnimLibrary
{
public:
	LAItemVec		Items;
	LAItemIt		FindItemI(const char* name);
	CLAItem*		FindItem(const char* name);

public:
					ELightAnimLibrary( );
					~ELightAnimLibrary( );

	void			OnCreate( );
	void			OnDestroy( );
	void			Load( );
	void			Save( );
	void			Reload( );
	void			Unload( );
	CLAItem*		AppendItem(const char* name, CLAItem* src);
	LAItemVec&		Objects( )
	{
		return Items;
	}
};

extern ENGINE_API ELightAnimLibrary LALib;

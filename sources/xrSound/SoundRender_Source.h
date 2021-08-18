#pragma once

#include "SoundRender_Cache.h"

// refs
struct OggVorbis_File;

class CSoundRender_Source : public CSound_source
{
public:
	CSharedString				pname;
	CSharedString				fname;
	cache_cat				CAT;
	U32						dwTimeTotal;			// всего
	U32						dwBytesTotal;
	//	U32						dwBytesPerSec;
	U32						dwBytesPerMS;

	float					m_fBaseVolume;
	float					m_fMinDist;
	float					m_fMaxDist;
	float					m_fMaxAIDist;
	U32						m_uGameType;

private:
	void 					i_decompress_fr(OggVorbis_File* ovf, char* dest, U32 size);
	void 					i_decompress_hr(OggVorbis_File* ovf, char* dest, U32 size);
	void					LoadWave(const char* name);

public:
	CSoundRender_Source( );
	~CSoundRender_Source( );

	void					load(const char* name);
	void					unload( );
	void					decompress(U32 line, OggVorbis_File* ovf);

	virtual	U32				length_ms( )
	{
		return dwTimeTotal;
	}
	virtual U32				game_type( )
	{
		return m_uGameType;
	}
	virtual const char* file_name( )
	{
		return *fname;
	}
	virtual float			base_volume( )
	{
		return m_fBaseVolume;
	}
};

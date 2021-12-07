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
	unsigned int						dwTimeTotal;			// всего
	unsigned int						dwBytesTotal;
	//	unsigned int						dwBytesPerSec;
	unsigned int						dwBytesPerMS;

	float					m_fBaseVolume;
	float					m_fMinDist;
	float					m_fMaxDist;
	float					m_fMaxAIDist;
	unsigned int						m_uGameType;

private:
	void 					i_decompress_fr(OggVorbis_File* ovf, char* dest, unsigned int size);
	void 					i_decompress_hr(OggVorbis_File* ovf, char* dest, unsigned int size);
	void					LoadWave(const char* name);

public:
	CSoundRender_Source( );
	~CSoundRender_Source( );

	void					load(const char* name);
	void					unload( );
	void					decompress(unsigned int line, OggVorbis_File* ovf);

	virtual	unsigned int				length_ms( )
	{
		return dwTimeTotal;
	}
	virtual unsigned int				game_type( )
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

#ifndef SoundRender_SourceH
#define SoundRender_SourceH
#pragma once

#include "soundrender_cache.h"

// refs
struct OggVorbis_File;

class CSoundRender_Source	: public CSound_source
{
public:
	shared_str				pname;
	shared_str				fname;
	cache_cat				CAT;
	u32						dwTimeTotal;			// всего
	u32						dwBytesTotal;
//	u32						dwBytesPerSec;
	u32						dwBytesPerMS;

	F32					m_fBaseVolume;
	F32					m_fMinDist;
	F32					m_fMaxDist;
	F32					m_fMaxAIDist;
	u32						m_uGameType;
private:
	void 					i_decompress_fr			(OggVorbis_File* ovf, char* dest, u32 size);    
	void 					i_decompress_hr			(OggVorbis_File* ovf, char* dest, u32 size);
	void					LoadWave 				(LPCSTR name);
public:
							CSoundRender_Source		();
							~CSoundRender_Source	();

	void					load					(LPCSTR name);
    void					unload					();
	void					decompress				(u32 line, OggVorbis_File* ovf);
	
	virtual	u32				length_ms				()	{return dwTimeTotal;	}
	virtual u32				game_type				()	{return m_uGameType;	}
	virtual LPCSTR			file_name				()	{return *fname;	}
	virtual F32			base_volume				()	{return m_fBaseVolume; }
};
#endif
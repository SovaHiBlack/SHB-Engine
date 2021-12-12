#pragma once

#include "..\Core\xr_resource.h"

class ENGINE_API CAviPlayerCustom;
class ENGINE_API CTheoraSurface;

class ENGINE_API CTexture			: public xr_resource_named				{
public:
	struct 
	{
		unsigned int					bLoaded		: 1;
		unsigned int					bUser		: 1;
		unsigned int					seqCycles	: 1;
		unsigned int					MemoryUsage	: 28;

	}									flags;
	fastdelegate::FastDelegate1<unsigned int>	bind;

	IDirect3DBaseTexture9*				pSurface;
	CAviPlayerCustom*					pAVI;
	CTheoraSurface*						pTheora;
	float								m_material;
	CSharedString							m_bumpmap;

	union{
		unsigned int								m_play_time;		// sync theora time
		unsigned int								seqMSPF;			// Sequence data milliseconds per frame
	};

	// Sequence data
	xr_vector<IDirect3DBaseTexture9*>	seqDATA;

	// Description
	IDirect3DBaseTexture9*				desc_cache;
	D3DSURFACE_DESC						desc;
	inline BOOL								desc_valid		()		{ return pSurface==desc_cache; }
	inline void								desc_enshure	()		{ if (!desc_valid()) desc_update(); }
	void								desc_update		();
public:
	void	__stdcall					apply_load		(unsigned int	stage);
	void	__stdcall					apply_theora	(unsigned int	stage);
	void	__stdcall					apply_avi		(unsigned int	stage);
	void	__stdcall					apply_seq		(unsigned int	stage);
	void	__stdcall					apply_normal	(unsigned int	stage);

	void								Preload			();
	void								Load			();
	void								PostLoad		();
	void								Unload			(void);
//	void								Apply			(unsigned int dwStage);

	void								surface_set		(IDirect3DBaseTexture9* surf);
	IDirect3DBaseTexture9*				surface_get 	();

	inline BOOL								isUser			()		{ return flags.bUser;					}
	inline unsigned int								get_Width		()		{ desc_enshure(); return desc.Width;	}
	inline unsigned int								get_Height		()		{ desc_enshure(); return desc.Height;	}

	void								video_Sync		(unsigned int _time){m_play_time=_time;}
	void								video_Play		(BOOL looped, unsigned int _time=0xFFFFFFFF);
	void								video_Pause		(BOOL state);
	void								video_Stop		();
	BOOL								video_IsPlaying	();

	CTexture							();
	virtual ~CTexture					();
};
struct ENGINE_API		resptrcode_texture	: public resptr_base<CTexture>
{
	void				create			(const char* _name);
	void				destroy			()					{ _set(NULL);					}
	CSharedString			bump_get		()					{ return _get()->m_bumpmap;		}
	bool				bump_exist		()					{ return 0!=bump_get().size();	}
};
typedef	resptr_core<CTexture,resptrcode_texture >	
	ref_texture;

#ifndef SoundRender_TargetDH
#define SoundRender_TargetDH
#pragma once

#include "soundrender_Target.h"
#include "soundrender_CoreD.h"

class CSoundRender_TargetD: public CSoundRender_Target
{
	typedef CSoundRender_Target	inherited;

	IDirectSoundBuffer*			pBuffer_base;
	IDirectSoundBuffer*			pBuffer;
	IDirectSound3DBuffer*		pControl;

	BOOL						bDX7;

	U32							buf_size;		// bytes
	U32							buf_block;

	int							cache_hw_volume;
	int							cache_hw_freq;
	U32							pos_write;		// bytes

private:
	void						fill_block				();
	U32							calc_interval			(U32 ptr);

public:
								CSoundRender_TargetD	();
	virtual 					~CSoundRender_TargetD	();

	virtual BOOL				_initialize				();
	virtual void				_destroy				();

	virtual void				start					(CSoundRender_Emitter* E);
	virtual void				render					();
	virtual void				rewind					();
	virtual void				stop					();
	virtual void				update					();
	virtual void				fill_parameters			();
};
#endif
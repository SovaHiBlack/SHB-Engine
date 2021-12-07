#pragma once

#include "SoundRender_Target.h"
#include "SoundRender_CoreD.h"

class CSoundRender_TargetD: public CSoundRender_Target
{
	typedef CSoundRender_Target	inherited;

	IDirectSoundBuffer*			pBuffer_base;
	IDirectSoundBuffer*			pBuffer;
	IDirectSound3DBuffer*		pControl;

	BOOL						bDX7;

	unsigned int							buf_size;		// bytes
	unsigned int							buf_block;

	int							cache_hw_volume;
	int							cache_hw_freq;
	unsigned int							pos_write;		// bytes

private:
	void						fill_block				();
	unsigned int							calc_interval			(unsigned int ptr);

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

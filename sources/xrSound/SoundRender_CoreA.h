#ifndef SoundRender_CoreAH
#define SoundRender_CoreAH
#pragma once

#include "SoundRender_Core.h"            

#include <al.h>
#include <alc.h>
//.#include <alut.h>
#include <eax.h>

#include "OpenALDeviceList.h"

#ifdef DEBUG
#	define A_CHK(expr)		{ alGetError(); 		expr; ALenum error=alGetError(); 			VERIFY2(error==AL_NO_ERROR, (pcstr)alGetString(error)); }
#	define AC_CHK(expr)		{ alcGetError(pDevice); expr; ALCenum error=alcGetError(pDevice); 	VERIFY2(error==ALC_NO_ERROR,(pcstr)alcGetString(pDevice,error)); }
#else
#	define A_CHK(expr)		{ expr; }
#	define AC_CHK(expr)		{ expr; }
#endif

class CSoundRender_CoreA: public CSoundRender_Core
{
	typedef CSoundRender_Core inherited;
	EAXSet					eaxSet;					// EAXSet function, retrieved if EAX Extension is supported
	EAXGet					eaxGet;					// EAXGet function, retrieved if EAX Extension is supported
	ALCdevice* 				pDevice;
    ALCcontext*				pContext;
	ALDeviceList*			pDeviceList;

	struct SListener{
		fVector3				position;
		fVector3				orientation[2];
	};
	SListener				Listener;

    BOOL 					EAXQuerySupport			(BOOL bDeferred, const GUID* guid, u32 prop, void* val, u32 sz);
	BOOL 					EAXTestSupport			(BOOL bDeferred);
protected:
	virtual void			i_eax_set				(const GUID* guid, u32 prop, void* val, u32 sz);
	virtual void			i_eax_get				(const GUID* guid, u32 prop, void* val, u32 sz);
	virtual void			update_listener			( const fVector3& P, const fVector3& D, const fVector3& N, f32 dt );
public:	
						    CSoundRender_CoreA		();
    virtual					~CSoundRender_CoreA		();

	virtual void			_initialize				( u64 window );
	virtual void			_clear					( );
    
	virtual void			set_master_volume		(f32 f		);

	virtual const fVector3&	listener_position		( ){return Listener.position;}
};
extern CSoundRender_CoreA* SoundRenderA;
#endif

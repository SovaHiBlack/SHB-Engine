#include "stdafx.h"

#include "soundrender_coreA.h"
#include "soundrender_targetA.h"

CSoundRender_CoreA*	SoundRenderA= 0; 

CSoundRender_CoreA::CSoundRender_CoreA	():CSoundRender_Core()
{
	pDevice						= 0;
	pDeviceList					= 0;
	pContext					= 0;
    eaxSet						= 0;
    eaxGet						= 0;
}

CSoundRender_CoreA::~CSoundRender_CoreA	()
{
}

BOOL CSoundRender_CoreA::EAXQuerySupport(BOOL bDeferred, const GUID* guid, u32 prop, void* val, u32 sz)
{
	if (AL_NO_ERROR!=eaxGet(guid, prop, 0, val, sz)) return FALSE;
	if (AL_NO_ERROR!=eaxSet(guid, (bDeferred?DSPROPERTY_EAXLISTENER_DEFERRED:0) | prop, 0, val, sz)) return FALSE;
    return TRUE;
}

BOOL CSoundRender_CoreA::EAXTestSupport	(BOOL bDeferred)
{
    EAXLISTENERPROPERTIES 		ep;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOM, 				&ep.lRoom,					sizeof(LONG))) 	return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOMHF, 		  	&ep.lRoomHF,				sizeof(LONG))) 	return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, 	&ep.flRoomRolloffFactor,	sizeof(f32))) return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_DECAYTIME, 		  	&ep.flDecayTime,			sizeof(f32))) return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_DECAYHFRATIO,		&ep.flDecayHFRatio,			sizeof(f32))) return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REFLECTIONS, 		&ep.lReflections,			sizeof(LONG))) 	return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY,   &ep.flReflectionsDelay,		sizeof(f32))) return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REVERB, 		  	&ep.lReverb,				sizeof(LONG))) 	return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REVERBDELAY, 		&ep.flReverbDelay,			sizeof(f32))) return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION,&ep.flEnvironmentDiffusion,sizeof(f32))) return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, 	&ep.flAirAbsorptionHF,		sizeof(f32))) return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_FLAGS, 				&ep.dwFlags,				sizeof(DWORD))) return FALSE;
	return TRUE;
}

void CSoundRender_CoreA::_initialize	(u64 window)
{
	bPresent			        = FALSE;

	pDeviceList					= xr_new<ALDeviceList>();

	if (0==pDeviceList->GetNumDevices()){ 
		Log						("OpenAL: Can't create sound device.");
		xr_delete				(pDeviceList);
		return;
	}
	
	pDeviceList->SelectBestDevice();

	int defaultIdx					= pDeviceList->GetDefaultDevice();
	R_ASSERT						(defaultIdx>=0 && defaultIdx<pDeviceList->GetNumDevices());
	const ALDeviceDesc& deviceDesc	= pDeviceList->GetDeviceDesc(defaultIdx);


    // OpenAL device
    pDevice						= alcOpenDevice		(deviceDesc.name.c_str());
	if (pDevice == NULL){
		Log						("SOUND: OpenAL: Failed to create device.");
		bPresent				= FALSE;
		return;
	}

    // Get the device specifier.
    const ALCchar*		        deviceSpecifier;
    deviceSpecifier         	= alcGetString		(pDevice, ALC_DEVICE_SPECIFIER);
	Msg				        	("SOUND: OpenAL: Required device: %s. Created device: %s.", deviceDesc.name.c_str(), deviceSpecifier);

    // Create context
    pContext					= alcCreateContext	(pDevice,NULL);
	if (0==pContext){
		Log						("SOUND: OpenAL: Failed to create context.");
		bPresent				= FALSE;
		alcCloseDevice			(pDevice); pDevice = 0;
		return;
	}
    
    // clear errors
	alGetError					();
	alcGetError					(pDevice);
    
    // Set active context
    AC_CHK				        (alcMakeContextCurrent(pContext));

    // initialize listener
    A_CHK				        (alListener3f		(AL_POSITION,0.f,0.f,0.f));
    A_CHK				        (alListener3f		(AL_VELOCITY,0.f,0.f,0.f));
	fVector3	orient[2]	        = {{0.0f,0.0f,1.0f},{0.0f,1.0f,0.0f}};
    A_CHK				        (alListenerfv		(AL_ORIENTATION,&orient[0].x));
    A_CHK				        (alListenerf		(AL_GAIN,1.f));

    // Check for EAX extension
    bEAX 				        = deviceDesc.eax && !deviceDesc.eax_unwanted;
    eaxSet 				        = (EAXSet)alGetProcAddress	((const ALchar*)"EAXSet");
    if (eaxSet==NULL) bEAX 		= false;
    eaxGet 				        = (EAXGet)alGetProcAddress	((const ALchar*)"EAXGet");
    if (eaxGet==NULL) bEAX 		= false;

    if (bEAX){
		bDeferredEAX			= EAXTestSupport(TRUE);
        bEAX 					= EAXTestSupport(FALSE);
    }

	ZeroMemory					( &wfm, sizeof( WAVEFORMATEX ) );
	switch	( psSoundFreq ){            
	default:
	case sf_22K:	wfm.nSamplesPerSec = 22050; break;
	case sf_44K:	wfm.nSamplesPerSec = 44100; break;
	}
	wfm.wFormatTag				= WAVE_FORMAT_PCM;
	wfm.nChannels				= 2;	//(dsCaps.dwFlags&DSCAPS_PRIMARYSTEREO)?2:1;
	wfm.wBitsPerSample			= 16;	//(dsCaps.dwFlags&DSCAPS_PRIMARY16BIT)?16:8;
	wfm.nBlockAlign				= wfm.wBitsPerSample / 8 * wfm.nChannels;
	wfm.nAvgBytesPerSec			= wfm.nSamplesPerSec * wfm.nBlockAlign;

    // inherited initialize           
    inherited::_initialize		(window);

	// Pre-create targets
	CSoundRender_Target*	T	= 0;
	for (u32 tit=0; tit<u32(psSoundTargets); tit++)
	{
		T						=	xr_new<CSoundRender_TargetA>();
		if (T->_initialize()){	
			s_targets.push_back	(T);
        }else{
        	Log					("! SOUND: OpenAL: Max targets - ",tit);
            T->_destroy			();
        	xr_delete			(T);
        	break;
        }
	}
}

void CSoundRender_CoreA::set_master_volume(f32 f )
{
	if (bPresent)				{
		A_CHK				    (alListenerf	(AL_GAIN,f));
	}
}

void CSoundRender_CoreA::_clear	()
{
	inherited::_clear			();
    // remove targets
	CSoundRender_Target*	T	= 0;
	for (u32 tit=0; tit<s_targets.size(); tit++)
	{
		T						= s_targets[tit];
		T->_destroy				();
        xr_delete				(T);
	}
    // Reset the current context to NULL.
    alcMakeContextCurrent		(NULL);         
    // Release the context and the device.
    alcDestroyContext			(pContext);		pContext	= 0;
    alcCloseDevice				(pDevice);		pDevice		= 0;
	xr_delete					(pDeviceList);
}

void	CSoundRender_CoreA::i_eax_set			(const GUID* guid, u32 prop, void* val, u32 sz)
{
	eaxSet	     			 	(guid, prop, 0, val, sz);
}
void	CSoundRender_CoreA::i_eax_get			(const GUID* guid, u32 prop, void* val, u32 sz)
{
	eaxGet	    		  	    (guid, prop, 0, val, sz);
}

void CSoundRender_CoreA::update_listener		( const fVector3& P, const fVector3& D, const fVector3& N, f32 dt )
{
	inherited::update_listener(P,D,N,dt);

	if (!Listener.position.similar(P)){
		Listener.position.set	(P);
		bListenerMoved			= TRUE;
	}
	Listener.orientation[0].set	(D.x,D.y,-D.z);
	Listener.orientation[1].set	(N.x,N.y,-N.z);

	A_CHK						(alListener3f	(AL_POSITION,Listener.position.x,Listener.position.y,-Listener.position.z));
	A_CHK						(alListener3f	(AL_VELOCITY,0.f,0.f,0.f));
	A_CHK						(alListenerfv	(AL_ORIENTATION,&Listener.orientation[0].x));
}

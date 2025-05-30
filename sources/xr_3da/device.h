#pragma once

// Note:
// ZNear - always 0.0f
// ZFar  - always 1.0f

class	ENGINE_API	CResourceManager;
class	ENGINE_API	CGammaControl;

#include "pure.h"
#include "hw.h"
#include "..\xrCore\ftimer.h"
#include "stats.h"
#include "GammaControl.h"
#include "shader.h"
#include "R_Backend.h"
#include	"R_Backend_Runtime.h"

#define VIEWPORT_NEAR  0.2f

#define DEVICE_RESET_PRECACHE_FRAME_COUNT 10

// refs
class ENGINE_API CRenderDevice
{
private:
	// Main objects used for creating and rendering the 3D scene
	u32										m_dwWindowStyle;
	RECT									m_rcWindowBounds;
	RECT									m_rcWindowClient;

	u32										Timer_MM_Delta;
	CTimer_paused							Timer;
	CTimer_paused							TimerGlobal;
	CTimer									TimerMM;

	void									_Create(pcstr shName);
	void									_Destroy(BOOL	bKeepTextures);
	void									_SetupStates( );

public:
	HWND									m_hWnd;

	u32										dwFrame;
	u32										dwPrecacheFrame;
	u32										dwPrecacheTotal;

	u32										dwWidth, dwHeight;
	f32										fWidth_2;
	f32										fHeight_2;
	BOOL									b_is_Ready;
	BOOL									b_is_Active;
	void									OnWM_Activate(WPARAM wParam, LPARAM lParam);

public:
	ref_shader								m_WireShader;
	ref_shader								m_SelectionShader;

	BOOL									m_bNearer;
	void									SetNearer(BOOL enabled)
	{
		if (enabled && !m_bNearer)
		{
			m_bNearer = TRUE;
			mProject._43 -= EPSILON_3;
		}
		else if (!enabled && m_bNearer)
		{
			m_bNearer = FALSE;
			mProject._43 += EPSILON_3;
		}

		RCache.set_xform_project(mProject);
	}

public:
	// Registrators
	CRegistrator	<pureRender			>			seqRender;
	CRegistrator	<pureAppActivate	>			seqAppActivate;
	CRegistrator	<pureAppDeactivate	>			seqAppDeactivate;
	CRegistrator	<pureAppStart		>			seqAppStart;
	CRegistrator	<pureAppEnd			>			seqAppEnd;
	CRegistrator	<pureFrame			>			seqFrame;
	CRegistrator	<pureFrame			>			seqFrameMT;
	CRegistrator	<pureDeviceReset	>			seqDeviceReset;
	xr_vector		<fastdelegate::FastDelegate0<> >	seqParallel;

	// Dependent classes
	CResourceManager* Resources;
	CStats* Statistic;
	CGammaControl							Gamma;

	// Engine flow-control
	f32										fTimeDelta;
	f32										fTimeGlobal;
	u32										dwTimeDelta;
	u32										dwTimeGlobal;
	u32										dwTimeContinual;

	// Cameras & projection
	fVector3									vCameraPosition;
	fVector3									vCameraDirection;
	fVector3									vCameraTop;
	fVector3									vCameraRight;
	fMatrix4x4									mView;
	fMatrix4x4									mProject;
	fMatrix4x4									mFullTransform;
	fMatrix4x4									mInvFullTransform;
	f32										fFOV;
	f32										fASPECT;

	CRenderDevice( )
	{
		m_hWnd = NULL;
		b_is_Active = FALSE;
		b_is_Ready = FALSE;
		Timer.Start( );
		m_bNearer = FALSE;
	}

	void	Pause(BOOL bOn, BOOL bTimer, BOOL bSound, pcstr reason);
	BOOL	Paused( );

	// Scene control
	void PreCache(u32 frames);
	BOOL Begin( );
	void Clear( );
	void End( );
	void FrameMove( );

	void overdrawBegin( );
	void overdrawEnd( );

	// Mode control
	void DumpFlags( );
	IC	 CTimer_paused* GetTimerGlobal( )
	{
		return &TimerGlobal;
	}
	u32	 TimerAsync( )
	{
		return TimerGlobal.GetElapsed_ms( );
	}
	u32	 TimerAsync_MMT( )
	{
		return TimerMM.GetElapsed_ms( ) + Timer_MM_Delta;
	}

	// Creation & Destroying
	void Create( );
	void Run( );
	void Destroy( );
	void Reset(bool precache = true);

	void Initialize( );

public:
	void time_factor(const f32& time_factor)
	{
		Timer.time_factor(time_factor);
		TimerGlobal.time_factor(time_factor);
	}

	IC	const f32& time_factor( ) const
	{
		VERIFY(Timer.time_factor( ) == TimerGlobal.time_factor( ));
		return Timer.time_factor( );
	}

	// Multi-threading
	xrCriticalSection	mt_csEnter;
	xrCriticalSection	mt_csLeave;
	volatile BOOL		mt_bMustExit;

	ICF		void			remove_from_seq_parallel(const fastdelegate::FastDelegate0<>& delegate)
	{
		xr_vector<fastdelegate::FastDelegate0<> >::iterator I = std::find(
			seqParallel.begin( ),
			seqParallel.end( ),
			delegate
		);
		if (I != seqParallel.end( ))
		{
			seqParallel.erase(I);
		}
	}
};

extern		ENGINE_API		CRenderDevice		Device;
extern		ENGINE_API		bool				g_bBenchmark;

typedef fastdelegate::FastDelegate0<bool>		LOADING_EVENT;
extern	ENGINE_API xr_list<LOADING_EVENT>		g_loading_events;

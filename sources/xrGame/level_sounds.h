#pragma once

struct SStaticSound
{
	ref_sound		m_Source;
	iVector2		m_ActiveTime;
	iVector2		m_PlayTime;
	iVector2		m_PauseTime;
	u32				m_NextTime;
	u32				m_StopTime;
	fVector3			m_Position;
	f32			m_Volume;
	f32			m_Freq;

public:
	void 			Load			(IReader& F);
	void 			Update			(u32 gt, u32 rt);
};

// music interface
struct	SMusicTrack
{
#ifdef DEBUG
	shared_str		m_DbgName;
#endif
	ref_sound		m_SourceLeft;
	ref_sound		m_SourceRight;
	iVector2		m_ActiveTime;
	iVector2		m_PauseTime;
	f32			m_Volume;

public:
	void			Load			(pcstr fn, pcstr params);
	BOOL			IsPlaying		(){return m_SourceLeft._feedback() || m_SourceRight._feedback();}
	void			Play			();
	void			Stop			();
	void			SetVolume		(f32 volume);
};

class CLevelSoundManager
{
	DEFINE_VECTOR(SStaticSound,StaticSoundsVec,StaticSoundsVecIt);
	StaticSoundsVec	m_StaticSounds;
	DEFINE_VECTOR(SMusicTrack,MusicTrackVec,MusicTrackVecIt);
	MusicTrackVec	m_MusicTracks;
	u32				m_NextTrackTime;
	s32				m_CurrentTrack;

public:
					CLevelSoundManager();
	void			Load			();
	void			Unload			();
	void __stdcall	Update			();
};

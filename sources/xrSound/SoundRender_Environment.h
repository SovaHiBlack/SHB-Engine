#ifndef soundrender_environmentH
#define soundrender_environmentH
#pragma once

// refs
class CSoundRender_Environment		: public CSound_environment
{
public:
	u32				version;
	shared_str			name;

    u32				Environment;				// sorce environment
	F32           Room;                       // room effect level at low frequencies
	F32           RoomHF;                     // room effect high-frequency level re. low frequency level
	F32           RoomRolloffFactor;          // like DS3D flRolloffFactor but for room effect
	F32           DecayTime;                  // reverberation decay time at low frequencies
	F32           DecayHFRatio;               // high-frequency to low-frequency decay time ratio
	F32           Reflections;                // early reflections level relative to room effect
	F32           ReflectionsDelay;           // initial reflection delay time
	F32           Reverb;                     // late reverberation level relative to room effect
	F32           ReverbDelay;                // late reverberation delay time relative to initial reflection
	F32           EnvironmentSize;            // environment size in meters
	F32           EnvironmentDiffusion;       // environment diffusion
	F32           AirAbsorptionHF;            // change in level per meter at 5 kHz
public:
                    CSoundRender_Environment	(void);
                    ~CSoundRender_Environment	(void);
	void			set_identity	            ();
	void			set_default		            ();
	void			clamp			            ();
	void			lerp			            (CSoundRender_Environment& A, CSoundRender_Environment& B, F32 f);
	bool			load			            (IReader* fs);
	void			save			            (IWriter* fs);
};

class SoundEnvironment_LIB
{
public:
	DEFINE_VECTOR				(CSoundRender_Environment*,SE_VEC,SE_IT);
private:
	SE_VEC						library;
public:
	void						Load	(pcstr name);
	bool						Save	(pcstr name);
	void						Unload	();
	int							GetID	(pcstr name);
	CSoundRender_Environment*	Get		(pcstr name);
	CSoundRender_Environment*	Get		(int id);
	CSoundRender_Environment*	Append	(CSoundRender_Environment* parent=0);
	void						Remove	(pcstr name);
	void						Remove	(int id);
	SE_VEC&						Library	();
};
#endif

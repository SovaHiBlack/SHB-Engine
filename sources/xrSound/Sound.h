#pragma once

#ifdef XRSOUND_EXPORTS
	#define XRSOUND_API __declspec(dllexport)
#else
	#define XRSOUND_API __declspec(dllimport)
#endif

#define SNDENV_FILENAME				"sEnvironment.xr"
#define OGG_COMMENT_VERSION 		0x0003 

// refs
class	CObject;
class	XRSOUND_API					CSound_params;
class	XRSOUND_API					CSound_source;
class	XRSOUND_API					CSound_emitter;
class	XRSOUND_API					CSound_stream_interface;
class	XRSOUND_API					CSound_environment;

//
XRSOUND_API extern unsigned int				psSoundFreq				;
XRSOUND_API extern unsigned int				psSoundModel			;
XRSOUND_API extern float			psSoundVEffects			;
XRSOUND_API extern float			psSoundVFactor			;
XRSOUND_API extern float			psSoundVMusic			;
XRSOUND_API extern float			psSoundRolloff			;
XRSOUND_API extern float			psSoundOcclusionScale	;
XRSOUND_API extern Flags32			psSoundFlags			;
XRSOUND_API extern int				psSoundTargets			;
XRSOUND_API extern int				psSoundCacheSizeMB		;

// Flags
enum {
	ss_Hardware			= (1ul<<1ul),	//!< Use hardware mixing only
	ss_EAX				= (1ul<<2ul),	//!< Use eax
	ss_forcedword		= unsigned int(-1)
};
enum {
	sf_22K,
	sf_44K,
	sf_forcedword = unsigned int(-1)
};
enum {
	sq_DEFAULT,
	sq_NOVIRT,
	sq_LIGHT,
	sq_HIGH,
	sq_forcedword = unsigned int(-1)
};
enum {
	sg_Undefined		= 0,
	sg_SourceType		= unsigned int(-1),
	sg_forcedword		= unsigned int(-1),
};
enum {
	sm_Looped			= (1ul<<0ul),	//!< Looped
	sm_2D				= (1ul<<1ul),	//!< 2D mode
	sm_forcedword		= unsigned int(-1),
};
enum esound_type{
	st_Effect			= 0,
	st_Music			= 1,
	st_forcedword		= unsigned int(-1),
};

class CSound_UserDataVisitor;

class CSound_UserData	: public xr_resource		{
public:
	virtual							~CSound_UserData()							{}
	virtual void					accept			(CSound_UserDataVisitor*)	=0;
	virtual void					invalidate		()							=0;
};
typedef resptr_core<CSound_UserData,resptr_base<CSound_UserData> >	CSound_UserDataPtr;

class ref_sound_data	: public xr_resource		{
public:
	CSharedString						nm;
	CSound_source*					handle;			//!< Pointer to wave-source interface
	CSound_emitter*					feedback;		//!< Pointer to emitter, automaticaly clears on emitter-stop
	esound_type						s_type;
	int								g_type;			//!< Sound type, usually for AI
	CObject*						g_object;		//!< Game object that emitts ref_sound
	CSound_UserDataPtr				g_userdata;

									ref_sound_data	();
									ref_sound_data	(const char* fName, esound_type sound_type, int game_type);
	virtual							~ref_sound_data	();
};
typedef resptr_core<ref_sound_data,resptr_base<ref_sound_data> >	ref_sound_data_ptr;
/*! \class ref_sound
\brief Sound source + control

The main class respresenting source/emitter interface
This class infact just hides internals and redirect calls to 
specific sub-systems
*/
struct	ref_sound
{
	ref_sound_data_ptr		_p;
public:
	//! A constructor
	/*!
		\sa ~ref_sound()
	*/
							ref_sound				(){ }
							~ref_sound				(){ }

	inline CSound_source*		_handle					(){return _p?_p->handle:0;}
	inline CSound_emitter*		_feedback				(){return _p?_p->feedback:0;}
	inline CObject*				_g_object				(){VERIFY(_p); return _p->g_object;}
	inline int					_g_type					(){VERIFY(_p); return _p->g_type;}
	inline esound_type			_sound_type				(){VERIFY(_p); return _p->s_type;}
	inline CSound_UserDataPtr	_g_userdata				(){VERIFY(_p); return _p->g_userdata;}

	//! Loader/initializer
	/*!
		\sa clone()
		\sa destroy()
		\param name Name of wave-file
		\param type Sound type, usually for \a AI
	*/
	inline void					create					(const char* name, esound_type sound_type,	int	game_type);

	//! Clones ref_sound from another
	/*!
		\sa create()
		\sa destroy()
		\param from Source to clone.
		\param leave_type Controls whenewer to leave game/AI type as is
	*/
	inline void					clone					( const ref_sound& from, esound_type sound_type, int game_type);

	//! Destroys and unload wave
	/*!
		\sa create()
		\sa clone()
	*/
	inline void					destroy					( );

	//@{
	//! Starts playing this source
	/*!
		\sa stop()
	*/
	inline void					play					( CObject* O /*!< Object */, unsigned int flags=0 /*!< Looping */, float delay=0.f /*!< Delay */);
	inline void					play_at_pos				( CObject* O /*!< Object */,	const Fvector3& pos /*!< 3D position */, unsigned int flags=0 /*!< Looping */, float delay=0.f /*!< Delay */);
	inline void					play_no_feedback		( CObject* O /*!< Object */, unsigned int flags=0 /*!< Looping */, float delay=0.f /*!< Delay */, Fvector3* pos=0, float* vol=0, float* freq=0, Fvector2* range=0);
	//@}

	//! Stops playing this source
	/*!
		\sa play(), etc
	*/
	inline void					stop 					( );
	inline void					stop_deffered			( );
	inline void					set_position			( const Fvector3& pos);
	inline void					set_frequency			( float freq);
	inline void					set_range				( float min, float max );
	inline void					set_volume				( float vol );
	inline void					set_priority			( float vol );

	inline const CSound_params*	get_params				( );
	inline void					set_params				( CSound_params* p );
};

/// definition (Sound Source)
class XRSOUND_API	CSound_source
{
public:
	virtual	unsigned int				length_ms				( )		= 0;
	virtual unsigned int				game_type				( )		= 0;
	virtual const char* file_name				( )		= 0;
};

/// definition (Sound Source)
class XRSOUND_API			CSound_environment
{
public:
};

/// definition (Sound Params)
class XRSOUND_API			CSound_params
{
public:
	Fvector3				position;
	float					base_volume;
	float					volume;
	float					freq;
	float					min_distance;
	float					max_distance;
	float					max_ai_distance;
};

/// definition (Sound Interface)
class XRSOUND_API			CSound_emitter
{
public:
	virtual BOOL					is_2D					()															= 0;
	virtual void					switch_to_2D			()															= 0;
	virtual void					switch_to_3D			()															= 0;
	virtual void					set_position			(const Fvector3& pos)										= 0;
	virtual void					set_frequency			(float freq)												= 0;
	virtual void					set_range				(float min, float max)										= 0;
	virtual void					set_volume				(float vol)													= 0;
	virtual void					set_priority			(float vol)													= 0;
	virtual void					stop					(BOOL bDeffered)											= 0;
	virtual	const CSound_params*	get_params				( )															= 0;
	virtual unsigned int						play_time				( )															= 0;
};

/// definition (Sound Stream Interface)
class XRSOUND_API			CSound_stream_interface
{
public:
};

/// definition (Sound Stream Interface)
class XRSOUND_API			CSound_stats
{
public:
	unsigned int						_rendered;
	unsigned int						_simulated;
	unsigned int						_cache_hits;
	unsigned int						_cache_misses;
	unsigned int						_events;
};

class XRSOUND_API			CSound_stats_ext
{
public:
	struct SItem{
		CSharedString			name;
		CSound_params		params;
		float				volume;
		esound_type			type;
		int					game_type;
		CObject*			game_object;
		struct  {
			unsigned int				_3D			:1;
			unsigned int				_rendered	:1;
		};
	};
//	DEFINE_VECTOR			(SItem,item_vec,item_vec_it);
	using item_vec = xr_vector<SItem>;
	using item_vec_it = item_vec::iterator;
	item_vec				items;
public:
void						clear							()					{items.clear();}
void						append							(const SItem& itm)	{items.push_back(itm);}
};

/// definition (Sound Callback)
typedef		void __stdcall sound_event						(ref_sound_data_ptr S, float range);

/// definition (Sound Manager Interface)
class XRSOUND_API	CSound_manager_interface
{
	virtual void	  				_initialize				(unsigned __int64 window )																			= 0;
	virtual void					_clear					( )																						= 0;

protected:
	friend class 					ref_sound_data;
	virtual void					_create_data			( ref_sound_data& S, const char* fName, esound_type sound_type, int	game_type)				= 0;
	virtual void					_destroy_data			( ref_sound_data& S)																	= 0;
public:
	virtual							~CSound_manager_interface(){}
	//@{
	/// General
	static void						_create					(unsigned __int64 window );
	static void						_destroy				( );

	virtual void					_restart				( )																						= 0;
	virtual BOOL					i_locked 				( )																						= 0;
	//@}

	//@{
	/// Sound interface
	virtual void					create					( ref_sound& S, const char* fName,				esound_type sound_type, int		game_type)	= 0;
	virtual void					clone					( ref_sound& S, const ref_sound& from,		esound_type sound_type, int		game_type)	= 0;
	virtual void					destroy					( ref_sound& S)																			= 0;
	virtual void					stop_emitters			( )																						= 0;	
	virtual int						pause_emitters			( bool val )																			= 0;

	virtual void					play					( ref_sound& S, CObject* O, unsigned int flags=0, float delay=0.0f)			= 0;
	virtual void					play_at_pos				( ref_sound& S, CObject* O,	const Fvector3& pos, unsigned int flags=0, float delay=0.0f)			= 0;
	virtual void					play_no_feedback		( ref_sound& S, CObject* O, unsigned int flags=0, float delay=0.0f, Fvector3* pos=0, float* vol=0, float* freq=0, Fvector2* range=0)= 0;

	virtual void					set_master_volume		( float f=1.f )																			= 0;
	virtual void					set_geometry_env		( IReader* I )																			= 0;
	virtual void					set_geometry_som		( IReader* I )																			= 0;
	virtual void					set_geometry_occ		( CDB::MODEL* M )																		= 0;
	virtual void					set_handler				( sound_event* E )																		= 0;
	//@}

	virtual void					update					( const Fvector3& P, const Fvector3& D, const Fvector3& N)									= 0;
	virtual void					statistic				( CSound_stats*  s0, CSound_stats_ext* s1 )												= 0;

	virtual float					get_occlusion_to		( const Fvector3& hear_pt, const Fvector3& snd_pt, float dispersion=0.2f)					= 0;

	virtual void					object_relcase			( CObject* obj )																		= 0;
	virtual const Fvector3&			listener_position		()																						= 0;
};
extern XRSOUND_API CSound_manager_interface*		Sound;

/// ********* Sound ********* (utils, accessors, helpers)
inline ref_sound_data::ref_sound_data				()																{	handle=0;feedback=0;g_type=0;g_object=0;s_type=st_Effect;												}
inline ref_sound_data::ref_sound_data				(const char* fName, 		esound_type sound_type, int	game_type )	{	::Sound->_create_data			(*this,fName, sound_type, game_type);									}
inline ref_sound_data::~ref_sound_data				()																{	::Sound->_destroy_data			(*this);																}

inline void	ref_sound::create						(const char* name,			esound_type sound_type, int	game_type)	{	VERIFY(!::Sound->i_locked()); 	::Sound->create		(*this,name,sound_type,game_type);					}
inline void	ref_sound::clone						( const ref_sound& from,esound_type sound_type, int	game_type)	{	VERIFY(!::Sound->i_locked()); 	::Sound->clone		(*this,from,sound_type,game_type);					}
inline void	ref_sound::destroy						( )														{	VERIFY(!::Sound->i_locked()); 	::Sound->destroy	(*this);													}
inline void	ref_sound::play							( CObject* O, unsigned int flags, float d)	{	VERIFY(!::Sound->i_locked()); 	::Sound->play		(*this,O,flags,d);											}
inline void	ref_sound::play_at_pos					( CObject* O, const Fvector3& pos, unsigned int flags, float d)	{	VERIFY(!::Sound->i_locked()); 	::Sound->play_at_pos(*this,O,pos,flags,d);										}
inline void	ref_sound::play_no_feedback				( CObject* O, unsigned int flags, float d, Fvector3* pos, float* vol, float* freq, Fvector2* range){	VERIFY(!::Sound->i_locked()); ::Sound->play_no_feedback(*this,O,flags,d,pos,vol,freq,range);	}
inline void	ref_sound::set_position					( const Fvector3& pos)									{	VERIFY(!::Sound->i_locked()); 	VERIFY(_feedback());_feedback()->set_position(pos);								}
inline void	ref_sound::set_frequency				( float freq)											{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->set_frequency(freq);							}
inline void	ref_sound::set_range					( float min, float max )								{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->set_range(min,max);							}
inline void	ref_sound::set_volume					( float vol )											{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->set_volume(vol);								}
inline void	ref_sound::set_priority					( float p )												{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->set_priority(p);								}
inline void	ref_sound::stop							( )														{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->stop(FALSE);									}
inline void	ref_sound::stop_deffered				( )														{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->stop(TRUE);									}
inline const CSound_params*	ref_sound::get_params	( )														{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	return _feedback()->get_params(); else return NULL;			}
inline void	ref_sound::set_params					( CSound_params* p )									
{	
	VERIFY(!::Sound->i_locked()); 	
	if (_feedback()){
		_feedback()->set_position	(p->position);
		_feedback()->set_frequency	(p->freq);
		_feedback()->set_range   	(p->min_distance,p->max_distance);
		_feedback()->set_volume   	(p->volume);
	}
}

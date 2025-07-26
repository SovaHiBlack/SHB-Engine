#pragma once

#include "..\..\..\XR_3DA\skeletonanimated.h"
#include "..\..\..\XR_3DA\cameramanager.h"

typedef u32 TTime;

#define COLOR		D3DCOLOR_XRGB
#define COLOR_RED	COLOR(255,0,0)	
#define COLOR_GREEN	COLOR(0,255,0)	
#define COLOR_BLUE	COLOR(0,0,255)	

class CBlend;

// специальные параметры анимаций (animation spec params)
#define ASP_MOVE_BKWD			(1 << 0) 
#define ASP_DRAG_CORPSE			(1 << 1) 
#define ASP_CHECK_CORPSE		(1 << 2)
#define ASP_ATTACK_RAT			(1 << 3)
#define ASP_ATTACK_RAT_JUMP		(1 << 4)
#define	ASP_STAND_SCARED		(1 << 5)
#define ASP_THREATEN			(1 << 6)
#define ASP_BACK_ATTACK			(1 << 7)
#define ASP_ROTATION_JUMP		(1 << 8)
#define ASP_ROTATION_RUN_LEFT	(1 << 9)
#define ASP_ROTATION_RUN_RIGHT	(1 << 10)
#define ASP_ATTACK_RUN			(1 << 11)
#define ASP_PSI_ATTACK			(1 << 12)
#define ASP_UPPER_STATE			(1 << 13)

#define AA_FLAG_ATTACK_RAT		(1 << 0)			// аттака крыс?
#define AA_FLAG_FIRE_ANYWAY		(1 << 1)			// трассировка не нужна

#define CRITICAL_STAND_TIME		1400
#define TIME_STAND_RECHECK		2000

#define HIT_SIDE_COUNT			2
#define HIT_BACK				0
#define HIT_FRONT				1

#define HIT_HEIGHT_COUNT		2
#define HIT_LOW					0
#define HIT_HIGH				1

// Enemy flags
#define FLAG_ENEMY_DIE					( 1 << 0 )
#define FLAG_ENEMY_LOST_SIGHT			( 1 << 1 )
#define FLAG_ENEMY_GO_CLOSER			( 1 << 2 )
#define FLAG_ENEMY_GO_FARTHER			( 1 << 3 )
#define FLAG_ENEMY_GO_CLOSER_FAST		( 1 << 4 )
#define FLAG_ENEMY_GO_FARTHER_FAST		( 1 << 5 )
#define FLAG_ENEMY_STANDING				( 1 << 6 )
#define FLAG_ENEMY_HIDING				( 1 << 7 )
#define FLAG_ENEMY_RUN_AWAY				( 1 << 8 )
#define FLAG_ENEMY_DOESNT_KNOW_ABOUT_ME	( 1 << 9 )
#define FLAG_ENEMY_GO_OFFLINE			( 1 << 10 )
#define FLAG_ENEMY_DOESNT_SEE_ME		( 1 << 11 )
#define FLAG_ENEMY_STATS_NOT_READY		( 1 << 12 )

#define SOUND_ATTACK_HIT_MIN_DELAY		1000
#define MORALE_NORMAL					0.5f

#define STANDART_ATTACK					-PI_DIV_6,PI_DIV_6,-PI_DIV_6,PI_DIV_6,3.5f
#define SIMPLE_ENEMY_HIT_TEST


// StepSounds
struct SStepSound {
	f32	vol;
	f32	freq;
};

struct SAttackEffector {
	SPPInfo	ppi;
	f32	time;
	f32	time_attack;
	f32	time_release;

	// camera effects
	f32	ce_time;
	f32	ce_amplitude;
	f32	ce_period_number;
	f32	ce_power;
};

struct SVelocityParam {
	struct {
		f32 linear;
		f32 angular_path;
		f32 angular_real;
	} velocity;
	f32	min_factor;
	f32	max_factor;

	SVelocityParam() {
		velocity.linear			= 0.f;
		velocity.angular_real	= 0.f;
		velocity.angular_path	= 0.f;
		min_factor				= 1.0f;
		max_factor				= 1.0f;
	}

	void	Load (pcstr section, pcstr line) {
		string32 buffer;
		velocity.linear			= f32(atof(_GetItem(pSettings->r_string(section,line),0,buffer)));
		velocity.angular_real	= f32(atof(_GetItem(pSettings->r_string(section,line),1,buffer)));
		velocity.angular_path	= f32(atof(_GetItem(pSettings->r_string(section,line),2,buffer)));
		min_factor				= f32(atof(_GetItem(pSettings->r_string(section,line),3,buffer)));
		max_factor				= f32(atof(_GetItem(pSettings->r_string(section,line),4,buffer)));
	}
};

// Activities
enum EMotionAnim
{
	eAnimStandIdle			= u32(0),
	eAnimStandTurnLeft,
	eAnimStandTurnRight,

	eAnimSitIdle,
	eAnimLieIdle,

	eAnimSitToSleep,
	eAnimLieToSleep,
	eAnimStandSitDown,
	eAnimStandLieDown,
	eAnimLieStandUp,
	eAnimSitStandUp,
	eAnimStandLieDownEat,
	eAnimSitLieDown,
	eAnimLieSitUp,
	eAnimSleepStandUp,

	eAnimWalkFwd,
	eAnimWalkBkwd,
	eAnimWalkTurnLeft,
	eAnimWalkTurnRight,

	eAnimRun,
	eAnimRunTurnLeft,
	eAnimRunTurnRight,
	eAnimFastTurn,

	eAnimAttack,
	eAnimAttackFromBack,
	eAnimAttackRun,

	eAnimEat,
	eAnimSleep,
	eAnimDie,

	eAnimDragCorpse,
	eAnimCheckCorpse,
	eAnimScared,
	eAnimAttackJump,

	eAnimLookAround,

	eAnimJump,
	eAnimSteal,

	eAnimJumpStart,
	eAnimJumpGlide,
	eAnimJumpFinish,

	eAnimJumpLeft,
	eAnimJumpRight,

	eAnimStandDamaged,
	eAnimWalkDamaged,
	eAnimRunDamaged,

	eAnimSniff,
	eAnimHowling,
	eAnimThreaten,

	eAnimMiscAction_00,
	eAnimMiscAction_01,

	eAnimUpperStandIdle,
	eAnimUpperStandTurnLeft,
	eAnimUpperStandTurnRight,

	eAnimStandToUpperStand,
	eAnimUppperStandToStand,

	eAnimUpperWalkFwd,
	eAnimUpperThreaten,
	eAnimUpperAttack,
	
	eAnimAttackPsi,

	eAnimTeleRaise,
	eAnimTeleFire,
	eAnimGraviPrepare,
	eAnimGraviFire,

	eAnimCount,
	eAnimUndefined			= u32(-1)
};

// Generic actions
enum EAction
{
	ACT_STAND_IDLE	= u32(0),
	ACT_SIT_IDLE,
	ACT_LIE_IDLE,
	ACT_WALK_FWD,
	ACT_WALK_BKWD,
	ACT_RUN,
	ACT_EAT,
	ACT_SLEEP,
	ACT_REST,
	ACT_DRAG,
	ACT_ATTACK,
	ACT_STEAL,
	ACT_LOOK_AROUND,
	ACT_NONE		= u32(-1)
};

enum EPState
{
	PS_STAND,
	PS_SIT, 
	PS_LIE,
	PS_STAND_UPPER
};

typedef		shared_str			anim_string;
#define		DEFAULT_ANIM		eAnimStandIdle

// элемент анимации
struct SAnimItem
{
	anim_string		target_name;	// "stand_idle_"
	s32				spec_id;		// (-1) - any,  (0 - ...) - идентификатор 3
	u8				count;			// количество анимаций : "idle_0", "idle_1", "idle_2" 
	
	SVelocityParam	velocity;

	EPState			pos_state;

	struct
	{
		anim_string front;
		anim_string	back;
		anim_string	left;
		anim_string	right;
	} fxs;
};

#define SKIP_IF_AGGRESSIVE	true

// описание перехода
struct STransition
{

	struct
	{
		bool		state_used;
		EMotionAnim anim;
		EPState		state;
	} from, target;

	EMotionAnim		anim_transition;
	bool			chain;
	bool			skip_if_aggressive;
};

// элемент движения
struct SMotionItem
{
	EMotionAnim		anim;
	bool			is_turn_params;

	struct
	{
		EMotionAnim	anim_left;			// speed, r_speed got from turn_left member
		EMotionAnim	anim_right;
		f32		min_angle;
	} turn;
};

// подмена анимаций (если *flag == true, то необходимо заменить анимацию)
struct SReplacedAnim
{
	EMotionAnim cur_anim;
	EMotionAnim new_anim;
	bool		*flag;
};

// Определение времени аттаки по анимации
typedef struct
{
	EMotionAnim	anim;				// параметры конкретной анимации 
	u32			anim_i3;

	TTime		time_from;			// диапазон времени когда можно наносить hit (от)
	TTime		time_to;		    // диапазон времени когда можно наносить hit (до)

	fVector3		trace_from;			// направление трассировки (относительно центра)
	fVector3		trace_to;

	u32			flags;				// специальные флаги

	f32		damage;				// урон при данной атаке
	fVector3		hit_dir;			// угол направления приложения силы к объекту

	//-----------------------------------------
	// temp 
	f32	yaw_from;
	f32	yaw_to;
	f32	pitch_from;
	f32	pitch_to;
	f32	dist;

} SAttackAnimation;

struct SAAParam
{
	MotionID	motion;
	f32			time;
	f32			hit_power;		// damage
	f32			impulse;
	fVector3		impulse_dir;

	// field of hit
	struct
	{
		f32	from_yaw;
		f32	to_yaw;
		f32	from_pitch;
		f32	to_pitch;
	} foh;
	
	f32		dist;
};

DEFINE_VECTOR(SAAParam, AA_VECTOR, AA_VECTOR_IT);

struct SCurrentAnimationInfo
{
	shared_str		name;

	EMotionAnim	motion;
	u8			index;

	TTime		time_started;

	struct
	{
		IC void		_set_current	(f32 v)			{ current=v; VERIFY2(_abs(v)<1000,"_set_current(). monster speed is too big"); }
		IC void		_set_target		(f32 v)			{ target=v;	VERIFY2(_abs(v)<1000,"_set_target(). monster speed is too big");}
		IC f32	_get_current	()					{ return current; }
		IC f32	_get_target		()					{ return target; }

	private:
		f32		current;
		f32		target;
	} speed;

	f32			speed_change_vel;
	CBlend			*blend;
};

//////////////////////////////////////////////////////////////////////////

struct t_fx_index
{
	s8 front;	
	s8 back;	
};

enum EHitSide
{
	eSideFront	= u32(0),
	eSideBack,
	eSideLeft,
	eSideRight
};

DEFINE_VECTOR	(SAnimItem*,		ANIM_ITEM_VECTOR,		ANIM_ITEM_VECTOR_IT);
DEFINE_VECTOR	(STransition,		TRANSITION_ANIM_VECTOR, TRANSITION_ANIM_VECTOR_IT);
DEFINE_MAP		(EAction,			SMotionItem,			MOTION_ITEM_MAP,			MOTION_ITEM_MAP_IT);
DEFINE_VECTOR	(EMotionAnim,		SEQ_VECTOR,				SEQ_VECTOR_IT);
DEFINE_VECTOR	(SAttackAnimation,	ATTACK_ANIM,			ATTACK_ANIM_IT);
DEFINE_VECTOR	(SReplacedAnim,		REPLACED_ANIM,			REPLACED_ANIM_IT);

DEFINE_MAP		(u16,				t_fx_index,				FX_MAP_U16,					FX_MAP_U16_IT);
DEFINE_MAP		(shared_str,		t_fx_index,				FX_MAP_STRING,				FX_MAP_STRING_IT);

DEFINE_VECTOR	(SEQ_VECTOR, VELOCITY_CHAIN_VEC, VELOCITY_CHAIN_VEC_IT);

struct SVelocity
{
	f32	current;
	f32	target;
	
	void	set		(f32 c, f32 t) {current = c; target = t;}
}; 

struct SMotionVel
{
	f32 linear;
	f32 angular;
	void	set		(f32 l, f32 a) {linear = l; angular = a;}
};

enum EAccelType
{
	eAT_Calm,
	eAT_Aggressive
};

enum EAccelValue
{
	eAV_Accel,
	eAV_Braking
};

#define deg(x) (x * PI / 180)

///////////////////////////////////////////////////////////////////////////////
// State Management
#define DO_ONCE_BEGIN(flag)	if (!flag) {flag = true;  
#define DO_ONCE_END()		}

#define TIME_OUT(a,b) a+b<m_dwCurrentTime

#define DO_IN_TIME_INTERVAL_BEGIN(varLastTime, varTimeInterval)	if (TIME_OUT(varLastTime,varTimeInterval)) { varLastTime = m_dwCurrentTime;
#define DO_IN_TIME_INTERVAL_END()								}
///////////////////////////////////////////////////////////////////////////////

#define PATH_NEED_REBUILD() m_object->IsPathEnd(2,0.5f)

// тип монстра (по количеству ног)
#define QUADRUPEDAL		4
#define BIPEDAL			2

struct SMonsterEnemy
{
	fVector3 position;
	u32		vertex;
	TTime	time;
	f32	danger;
};

class CEntityAlive;

DEFINE_MAP(const CEntityAlive *,SMonsterEnemy,ENEMIES_MAP, ENEMIES_MAP_IT);

struct SMonsterCorpse
{
	fVector3 position;
	u32		vertex;
	TTime	time;
};

DEFINE_MAP(const CEntityAlive *,SMonsterCorpse,CORPSE_MAP, CORPSE_MAP_IT);

struct SMonsterHit
{
	CObject		*object;
	TTime		time;
	EHitSide	side;
	fVector3		position;

	bool	operator==(const CObject *obj) {
		return (object == obj);
	}
};

DEFINE_VECTOR(SMonsterHit,MONSTER_HIT_VECTOR, MONSTER_HIT_VECTOR_IT);

enum EDangerType
{
	eWeak,
	eNormal,
	eStrong,
	eVeryStrong,
	eNone
};

DEFINE_MAP(MotionID, shared_str, ANIM_TO_MOTION_MAP, ANIM_TO_MOTION_MAP_IT);

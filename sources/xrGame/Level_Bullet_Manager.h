// Level_Bullet_Manager.h:  для обеспечения полета пули по траектории
//							все пули и осколки передаются сюда
//////////////////////////////////////////////////////////////////////

#pragma once


#include "weaponammo.h"
#include "tracer.h"
//структура, описывающая пулю и ее свойства в полете
struct SBullet
{
	u32				frame_num			;			//номер кадра на котором была запущена пуля
	union			{
		struct			{
			u16			ricochet_was	: 1	;			//пуля срикошетила
			u16			explosive		: 1	;			//special explosive mode for particles
			u16			allow_tracer	: 1	;
			u16			allow_ricochet	: 1	;			//разрешить рикошет
			u16			allow_sendhit	: 1	;			//statistics
			u16			skipped_frame	: 1	;			//пропуск первой отрисовки
			u16			aim_bullet		: 1 ;			//прицеленная пуля( вылетевшая первой после длительного молчания оружия (1-3 сек.))
		};
		u16				_storage			;
	}				flags				;
	u16				bullet_material_idx	;

	fVector3			pos					;			//текущая позиция
	fVector3			dir					;
	f32			speed				;			//текущая скорость
	
	u16				parent_id			;			//ID персонажа который иницировал действие
	u16				weapon_id			;			//ID оружия из которого была выпущены пуля
	
	f32			fly_dist			;			//дистанция которую пуля пролетела

	//коэфициенты и параметры патрона
	f32			hit_power			;			// power*cartridge
	f32			hit_impulse			;			// impulse*cartridge
	//-------------------------------------------------------------------
	f32			ap					;
	f32			air_resistance		;
	//-------------------------------------------------------------------
	f32			max_speed			;			// maxspeed*cartridge
	f32			max_dist			;			// maxdist*cartridge
	f32			pierce				;
	f32			wallmark_size		;
	//-------------------------------------------------------------------
	u8				m_u8ColorID			;
	
	//тип наносимого хита
	ALife::EHitType hit_type			;
	//---------------------------------
	u32				m_dwID				;
	ref_sound		m_whine_snd			;
	ref_sound		m_mtl_snd			;
	//---------------------------------
	u16				targetID			;
	bool			operator	==		(u32 ID){return	ID == m_dwID;}

public:
					SBullet				();
					~SBullet			();

	void			Init				(const	fVector3& position,
										const	fVector3& direction,
										 f32	start_speed,
										 f32	power,
										 f32	impulse,
										u16		sender_id,
										u16		sendersweapon_id,
										ALife::EHitType e_hit_type,
										 f32	maximum_distance,
										const	CCartridge& cartridge,
										bool	SendHit);
};

class CLevel;

class CBulletManager
{
private:
	collide::rq_results		rq_storage;
	xr_vector<ISpatial*>	rq_spatial;
	collide::rq_results		m_rq_results;

private:
	DEFINE_VECTOR						(ref_sound,SoundVec,SoundVecIt);
	DEFINE_VECTOR						(SBullet,BulletVec,BulletVecIt);
	typedef std::pair<f32, f32>		_hit		;
	friend	CLevel;

	enum EventType {
		EVENT_HIT	= u8(0),
		EVENT_REMOVE,

		EVENT_DUMMY = u8(-1),
	};
	struct	_event			{
		EventType			Type;
		BOOL				dynamic		;
		BOOL				Repeated	;	// последовательное повторное попадание в динамический объект
		_hit				result		;
		SBullet				bullet		;
		fVector3				normal		;
		fVector3				point		;
		collide::rq_result	R			;
		u16					tgt_material;
	};

protected:
	SoundVec				m_WhineSounds		;
	RStringVec				m_ExplodeParticles	;

	//список пуль находящихся в данный момент на уровне
	xrCriticalSection		m_Lock				;
	BulletVec				m_Bullets			;	// working set, locked
	BulletVec				m_BulletsRendered	;	// copy for rendering
	xr_vector<_event>		m_Events			;	

	//остаток времени, который не был учтен на предыдущем кадре
	u32						m_dwTimeRemainder;

	//отрисовка трассеров от пуль
	CTracer					tracers;

	//фиксированное время шага просчета пули
	u32						m_dwStepTime;	
	//минимальная скорость, на которой пуля еще считается
	static f32			m_fMinBulletSpeed;

	f32					m_fHPMaxDist;

	//константа G
	f32					m_fGravityConst;
	//сопротивление воздуха, процент, который отнимается от скорости
	//полета пули
	f32					m_fAirResistanceK;
	//cколько процентов энергии потеряет пуля при столкновении с материалом (при падении под прямым углом)
	f32					m_fCollisionEnergyMin;
	//сколькол процентов энергии устанется у пули при любом столкновении
	f32					m_fCollisionEnergyMax;

	//параметры отрисовки трассеров
	f32					m_fTracerWidth;
	f32 					m_fTracerLengthMax;
	f32 					m_fTracerLengthMin;

protected:
	void					PlayWhineSound		(SBullet* bullet, CObject* object, const fVector3& pos);
	void					PlayExplodePS		(const fMatrix4x4& xf);
	//функция обработки хитов объектов
	static BOOL 			test_callback		(const collide::ray_defs& rd, CObject* object, LPVOID params);
	static BOOL				firetrace_callback	(collide::rq_result& result, LPVOID params);

	// Deffer event
	void					RegisterEvent		(EventType Type, BOOL _dynamic, SBullet* bullet, const fVector3& end_point, collide::rq_result& R, u16 target_material);
	
	//попадание по динамическому объекту
	void					DynamicObjectHit	(_event& E);
	
	//попадание по статическому объекту
	void					StaticObjectHit		(_event& E);

	//попадание по любому объекту, на выходе - импульс и сила переданные пулей объекту
	_hit					ObjectHit			(SBullet* bullet, const fVector3& end_point,
												collide::rq_result& R, u16 target_material, fVector3& hit_normal);
	//отметка на пораженном объекте
	void					FireShotmark		(SBullet* bullet, const fVector3& vDir,
												const fVector3&vEnd,    collide::rq_result& R,  u16 target_material,
												const fVector3& vNormal, bool ShowMark = true);
	//просчет полета пули за некоторый промежуток времени
	//принимается что на этом участке пуля движется прямолинейно
	//и равномерно, а после просчета также изменяется текущая
	//скорость и положение с учетом гравитации и ветра
	//возвращаем true если пуля продолжает полет
	bool					CalcBullet			(collide::rq_results & rq_storage, xr_vector<ISpatial*>& rq_spatial, SBullet* bullet, u32 delta_time);
	void 		__stdcall	UpdateWorkload		();

public:
							CBulletManager		();
	virtual					~CBulletManager		();

	void 					Load				();
	void 					Clear				();
	void 					AddBullet			(const fVector3& position, const fVector3& direction,
												 f32 starting_speed, f32 power, f32 impulse,
												u16	sender_id, u16 sendersweapon_id,
												ALife::EHitType e_hit_type, f32 maximum_distance,
												const CCartridge& cartridge, bool SendHit,bool AimBullet=false);

	void					CommitEvents		();	// @ the start of frame
	void					CommitRenderSet		();	// @ the end of frame
	void 					Render				();
};

struct bullet_test_callback_data
{
	SBullet*		pBullet;
	bool			bStopTracing;
};

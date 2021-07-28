// ParticlesPlayer.h
// интерфейс для проигрывания партиклов на объекте

#pragma once

#include "ParticlesObject.h"
#include "..\ENGINE\bone.h"

using PARTICLES_PTR_VECTOR = xr_vector<CParticlesObject*>;
using PARTICLES_PTR_VECTOR_IT = PARTICLES_PTR_VECTOR::iterator;

class CObject;

class CParticlesPlayer
{
public:
	//структура с внутренней информацией о партикле
	struct SParticlesInfo
	{
		CParticlesObject* ps;
		//Fvector3				dir;
		//Fmatrix				x_form;
		Fvector3				angles;
		U16					sender_id;	//id - объекта, который запустил партиклы
		u32					life_time;	//время жизни партикла (-1) - бесконечно

		//int					cur_time;	//текущее время существования партикла
		//bool				auto_stop;	//автоматическая остановка партиклов, когда закончится время
	};

	using ParticlesInfoList = xr_vector<SParticlesInfo>;
	using ParticlesInfoListIt = ParticlesInfoList::iterator;

	//структура для косточки с списком запущенных партиклов
	struct SBoneInfo
	{
		U16					index;
		Fvector3				offset;
		ParticlesInfoList	particles;
		SParticlesInfo* FindParticles(const shared_str& ps_name);

	public:
		SBoneInfo(U16 idx, const Fvector3& offs) :index(idx), offset(offs)
		{ }
		SParticlesInfo* AppendParticles(CObject* object, const shared_str& ps_name);
		void				StopParticles(const shared_str& ps_name, bool bDestroy);
		void				StopParticles(U16 sender_id, bool bDestroy);
	};

	using BoneInfoVec = xr_vector<SBoneInfo>;
	using BoneInfoVecIt = BoneInfoVec::iterator;

private:
	// список костей
	U64						bone_mask; // используемые кости
	BoneInfoVec				m_Bones;
	CObject* m_self_object;

protected:
	bool					m_bActiveBones;	//есть ли косточки на которых играются партиклы

public:
	inline SBoneInfo* get_bone_info(U16 bone_index)
	{
		if (BI_NONE == bone_index)
		{
			return nullptr;
		}

		for (BoneInfoVecIt it = m_Bones.begin( ); it != m_Bones.end( ); it++)
		{
			if (it->index == bone_index)
			{
				return &(*it);
			}
		}

		return nullptr;
	}
	SBoneInfo* get_nearest_bone_info(CKinematics* K, U16 bone_index);
	Fvector3					parent_vel;

	CParticlesPlayer( );
	virtual					~CParticlesPlayer( );
	void					LoadParticles(CKinematics* K);

	void					net_DestroyParticles( );
	void					net_SpawnParticles( );

	void					UpdateParticles( );

	void					StartParticles(const shared_str& ps_name, U16 bone_num, const Fvector3& dir, U16 sender_id, int life_time = -1, bool auto_stop = true);
	void					StartParticles(const shared_str& ps_name, const Fvector3& dir, U16 sender_id, int life_time = -1, bool auto_stop = true);

	void					StartParticles(const shared_str& ps_name, U16 bone_num, const Fmatrix& dir, U16 sender_id, int life_time = -1, bool auto_stop = true);
	void					StartParticles(const shared_str& ps_name, const Fmatrix& dir, U16 sender_id, int life_time = -1, bool auto_stop = true);


	void					StopParticles(U16 sender_ID, U16 bone_id, bool bDestroy);
	void					StopParticles(const shared_str& particles_name, U16 bone_id, bool bDestroy);
	void					AutoStopParticles(const shared_str& ps_name, U16 bone_id, u32 life_time);

	static void				MakeXFORM(CObject* pObject, U16 bone_id, const Fvector3& dir, const Fvector3& offset, Fmatrix& result);
	static void				GetBonePos(CObject* pObject, U16 bone_id, const Fvector3& offset, Fvector3& result);
	U16						GetNearestBone(CKinematics* K, U16 bone_id);
	inline U16					GetRandomBone( )
	{
		U16 l_PBCount = U16(m_Bones.size( ));
		if (l_PBCount)
		{
			return m_Bones[(U16) Random.randI(l_PBCount)].index;
		}
		else
		{
			return BI_NONE;
		}
	}

	void					SetParentVel(const Fvector3& vel)
	{
		parent_vel = vel;
	}

	bool					IsPlaying( )
	{
		return m_bActiveBones;
	}
	virtual CParticlesPlayer* cast_particles_player( )
	{
		return this;
	}
};

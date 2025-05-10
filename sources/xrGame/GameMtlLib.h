#pragma once

#define GAMEMTL_CURRENT_VERSION			0x0001
//----------------------------------------------------

#define GAMEMTLS_CHUNK_VERSION        	0x1000
#define GAMEMTLS_CHUNK_AUTOINC	        0x1001
#define GAMEMTLS_CHUNK_MTLS	        	0x1002
#define GAMEMTLS_CHUNK_MTLS_PAIR       	0x1003
//----------------------------------------------------
#define GAMEMTL_CHUNK_MAIN	        	0x1000
#define GAMEMTL_CHUNK_FLAGS	        	0x1001
#define GAMEMTL_CHUNK_PHYSICS        	0x1002
#define GAMEMTL_CHUNK_FACTORS        	0x1003
#define GAMEMTL_CHUNK_FLOTATION   		0x1004
#define GAMEMTL_CHUNK_DESC				0x1005
#define GAMEMTL_CHUNK_INJURIOUS			0x1006
//----------------------------------------------------
#define GAMEMTLPAIR_CHUNK_PAIR   		0x1000
#define GAMEMTLPAIR_CHUNK_BREAKING   	0x1002
#define GAMEMTLPAIR_CHUNK_STEP   		0x1003
#define GAMEMTLPAIR_CHUNK_COLLIDE	  	0x1005
//----------------------------------------------------

#define GAMEMTL_SUBITEM_COUNT			4

#define GAMEMTL_NONE_ID					u32(-1)
#define GAMEMTL_NONE_IDX				u16(-1)
#define GAMEMTL_FILENAME				"gamemtl.xr"

DEFINE_VECTOR(ref_sound, SoundVec, SoundIt);
DEFINE_VECTOR(shared_str, PSVec, PSIt);
DEFINE_VECTOR(ref_shader, ShaderVec, ShaderIt);

struct SGameMtl
{
	friend class CGameMtlLibrary;
protected:
	s32 				ID; 	// auto number

public:
	enum
	{
		flBreakable = (1ul << 0ul),
		flBounceable = (1ul << 2ul),
		flBloodmark = (1ul << 4ul),
		flClimable = (1ul << 5ul),
		flPassable = (1ul << 7ul),
		flDynamic = (1ul << 8ul),
		flLiquid = (1ul << 9ul),
		flSuppressShadows = (1ul << 10ul),
		flSuppressWallmarks = (1ul << 11ul),
		flActorObstacle = (1ul << 12ul),

		flInjurious = (1ul << 28ul),// flInjurious = fInjuriousSpeed > 0.f
		flSlowDown = (1ul << 31ul) // flSlowDown = (fFlotationFactor<1.f)
	};

public:
	shared_str			m_Name;
	shared_str			m_Desc;

	flags32				Flags;
	// physics part
	f32				fPHFriction;            // ?
	f32				fPHDamping;             // ?
	f32				fPHSpring;              // ?
	f32				fPHBounceStartVelocity;	// ?
	f32				fPHBouncing;            // ?
	// shoot&bounce&visibility&flotation
	f32		    	fFlotationFactor;		// 0.0f - 1.0f   	(1.0f-полностью проходимый)
	f32				fShootFactor;			// 0.0f - 1.0f	(1.0f-полностью простреливаемый)
	f32				fBounceDamageFactor;	// 0.0f - 100.0f
	f32				fInjuriousSpeed; 		// 0.0f - ...	(00.f-не отбирает здоровье (скорость уменьшения здоровья))
	f32				fVisTransparencyFactor;	// 0.0f - 1.0f	(1.0f-полностью прозрачный)
	f32				fSndOcclusionFactor;	// 0.0f - 1.0f    (1.0f-полностью слышен)

public:
	SGameMtl( )
	{
		ID = -1;
		m_Name = "unknown";
		Flags.zero( );
		// factors
		fFlotationFactor = 1.0f;
		fShootFactor = 0.0f;
		fBounceDamageFactor = 1.0f;
		fInjuriousSpeed = 0.0f;
		fVisTransparencyFactor = 0.0f;
		fSndOcclusionFactor = 0.0f;
		// physics
		fPHFriction = 1.0f;
		fPHDamping = 1.0f;
		fPHSpring = 1.0f;
		fPHBounceStartVelocity = 0.0f;
		fPHBouncing = 0.1f;
	}
	void 				Load(IReader& fs);
	void 				Save(IWriter& fs);
	IC s32				GetID( )
	{
		return ID;
	}
};
DEFINE_VECTOR(SGameMtl*, GameMtlVec, GameMtlIt);

struct SGameMtlPair
{
	friend class CGameMtlLibrary;
	CGameMtlLibrary* m_Owner;

private:
	s32					mtl0;
	s32					mtl1;

protected:
	s32 				ID; 	// auto number
	s32					ID_parent;

public:
	flags32				OwnProps;
	//	properties
	SoundVec			BreakingSounds;
	SoundVec			StepSounds;
	SoundVec			CollideSounds;
	PSVec				CollideParticles;
	ShaderVec			CollideMarks;

public:
	SGameMtlPair(CGameMtlLibrary* owner)
	{
		mtl0 = -1;
		mtl1 = -1;
		ID = -1;
		ID_parent = -1;
		m_Owner = owner;
		OwnProps.one( );
	}
	~SGameMtlPair( );
	IC s32				GetMtl0( )
	{
		return mtl0;
	}
	IC s32				GetMtl1( )
	{
		return mtl1;
	}
	IC s32				GetID( )
	{
		return ID;
	}
	IC void				SetPair(s32 m0, s32 m1)
	{
		mtl0 = m0;
		mtl1 = m1;
	}
	IC bool				IsPair(s32 m0, s32 m1)
	{
		return !!(((mtl0 == m0) && (mtl1 == m1)) || ((mtl0 == m1) && (mtl1 == m0)));
	}
	void				Save(IWriter& fs);
	void				Load(IReader& fs);
	IC s32				GetParent( )
	{
		return ID_parent;
	}
	BOOL				SetParent(s32 parent);

#ifdef DEBUG
	pcstr				dbg_Name( );
#endif // def DEBUG

};

DEFINE_VECTOR(SGameMtlPair*, GameMtlPairVec, GameMtlPairIt);

class CGameMtlLibrary
{
	s32					material_index;
	s32					material_pair_index;

	GameMtlVec			materials;
	GameMtlPairVec		material_pairs;

	// game part
	u32					material_count;
	GameMtlPairVec		material_pairs_rt;

public:
	CGameMtlLibrary( )
	{
		material_index = 0;
		material_pair_index = 0;
		material_count = 0;
	}
	~CGameMtlLibrary( )
	{
		/*
		R_ASSERT		(0==material_pairs_rt.size());
		R_ASSERT		(0==material_pairs.size());
		R_ASSERT		(0==materials.size());
		*/
	}
	IC void				Unload( )
	{
		material_count = 0;
		material_pairs_rt.clear( );

		for (GameMtlIt m_it = materials.begin( ); materials.end( ) != m_it; ++m_it)
		{
			xr_delete(*m_it);
		}

		materials.clear( );
		for (GameMtlPairIt p_it = material_pairs.begin( ); material_pairs.end( ) != p_it; ++p_it)
		{
			xr_delete(*p_it);
		}

		material_pairs.clear( );
	}
	// material routine
	IC GameMtlIt 		GetMaterialIt(pcstr name)
	{
		for (GameMtlIt it = materials.begin( ); materials.end( ) != it; ++it)
		{
			if (0 == strcmpi(*(*it)->m_Name, name))
			{
				return it;
			}
		}

		return materials.end( );
	}
	IC GameMtlIt 		GetMaterialIt(shared_str& name)
	{
		for (GameMtlIt it = materials.begin( ); materials.end( ) != it; ++it)
		{
			if (name.equal((*it)->m_Name))
			{
				return it;
			}
		}

		return materials.end( );
	}
	IC GameMtlIt 		GetMaterialItByID(s32 id)
	{
		for (GameMtlIt it = materials.begin( ); materials.end( ) != it; ++it)
		{
			if ((*it)->ID == id)
			{
				return it;
			}
		}

		return materials.end( );
	}
	IC u32				GetMaterialID(pcstr name)
	{
		GameMtlIt it = GetMaterialIt(name);
		return ((it == materials.end( )) ? GAMEMTL_NONE_ID : (*it)->ID);
	}

	IC u16				GetMaterialIdx(s32 ID)
	{
		GameMtlIt it = GetMaterialItByID(ID);
		VERIFY(materials.end( ) != it);
		return (u16) (it - materials.begin( ));
	}
	IC u16				GetMaterialIdx(pcstr name)
	{
		GameMtlIt it = GetMaterialIt(name);
		VERIFY(materials.end( ) != it);
		return (u16) (it - materials.begin( ));
	}
	IC SGameMtl* GetMaterialByIdx(u16 idx)
	{
		VERIFY(idx < materials.size( ));
		return materials[idx];
	}
	IC SGameMtl* GetMaterialByID(s32 id)
	{
		return GetMaterialByIdx(GetMaterialIdx(id));
	}

	IC GameMtlIt		FirstMaterial( )
	{
		return materials.begin( );
	}
	IC GameMtlIt		LastMaterial( )
	{
		return materials.end( );
	}
	IC u32				CountMaterial( )
	{
		return materials.size( );
	}

	// material pair routine
	IC SGameMtlPair* GetMaterialPair(u16 idx0, u16 idx1)
	{
		R_ASSERT((idx0 < material_count) && (idx1 < material_count));
		return material_pairs_rt[idx1 * material_count + idx0];
	}

	IC GameMtlPairIt	FirstMaterialPair( )
	{
		return material_pairs.begin( );
	}
	IC GameMtlPairIt	LastMaterialPair( )
	{
		return material_pairs.end( );
	}

	// IO routines
	void				Load( );
	bool				Save( );
};

#define GET_RANDOM(a_vector)			(a_vector[Random.randI(a_vector.size())])

#define CLONE_MTL_SOUND(_res_, _mtl_pair_, _a_vector_)\
	{ VERIFY2(!_mtl_pair_##->_a_vector_.empty(),_mtl_pair_->dbg_Name());\
	_res_.clone(GET_RANDOM(_mtl_pair_##->_a_vector_),st_Effect,sg_SourceType);\
	}

extern CGameMtlLibrary GMLib;

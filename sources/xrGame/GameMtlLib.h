#pragma once

#define GAMEMTL_CURRENT_VERSION			0x0001
//----------------------------------------------------
#define GAMEMTLS_CHUNK_VERSION			0x1000
#define GAMEMTLS_CHUNK_AUTOINC			0x1001
#define GAMEMTLS_CHUNK_MTLS				0x1002
#define GAMEMTLS_CHUNK_MTLS_PAIR		0x1003
//----------------------------------------------------
#define GAMEMTL_CHUNK_MAIN				0x1000
#define GAMEMTL_CHUNK_FLAGS				0x1001
#define GAMEMTL_CHUNK_PHYSICS			0x1002
#define GAMEMTL_CHUNK_FACTORS			0x1003
#define GAMEMTL_CHUNK_FLOTATION			0x1004
#define GAMEMTL_CHUNK_DESC				0x1005
#define GAMEMTL_CHUNK_INJURIOUS			0x1006
//----------------------------------------------------
#define GAMEMTLPAIR_CHUNK_PAIR			0x1000
//#define GAMEMTLPAIR_CHUNK_FLOTATION	0x1001 - obsolete
#define GAMEMTLPAIR_CHUNK_BREAKING		0x1002
#define GAMEMTLPAIR_CHUNK_STEP			0x1003
//#define GAMEMTLPAIR_CHUNK_COLLIDE		0x1004 - obsolete / rename HIT
#define GAMEMTLPAIR_CHUNK_COLLIDE		0x1005
//----------------------------------------------------
#define GAMEMTL_SUBITEM_COUNT			4

#define GAMEMTL_NONE_ID					u32(-1)
#define GAMEMTL_NONE_IDX				U16(-1)
#define GAMEMTL_FILENAME				"gamemtl.xr"

using SoundVec = xr_vector<ref_sound>;
using SoundIt = SoundVec::iterator;

using PSVec = xr_vector<CSharedString>;
using PSIt = PSVec::iterator;

using ShaderVec = xr_vector<ref_shader>;
using ShaderIt = ShaderVec::iterator;

struct SGameMtl
{
	friend class CGameMtlLibrary;
protected:
	int 				ID; 	// auto number

public:
	enum
	{
		flBreakable = (1ul << 0ul),
//		flShootable 	= (1ul<<1ul),
		flBounceable = (1ul << 2ul),
		flSkidmark = (1ul << 3ul),
		flBloodmark = (1ul << 4ul),
		flClimable = (1ul << 5ul),
//		flWalkOn		= (1ul<<6ul), // obsolette
		flPassable = (1ul << 7ul),
		flDynamic = (1ul << 8ul),
		flLiquid = (1ul << 9ul),
		flSuppressShadows = (1ul << 10ul),
		flSuppressWallmarks = (1ul << 11ul),
		flActorObstacle = (1ul << 12ul),

		flInjurious = (1ul << 28ul),// flInjurious = fInjuriousSpeed > 0.0f
		flShootable = (1ul << 29ul),
		flTransparent = (1ul << 30ul),
		flSlowDown = (1ul << 31ul) // flSlowDown = (fFlotationFactor<1.0f)
	};

	CSharedString			m_Name;
	CSharedString			m_Desc;

	Flags32				Flags;
	//physics part
	float				fPHFriction;			// ?
	float				fPHDamping;				// ?
	float				fPHSpring;				// ?
	float				fPHBounceStartVelocity;	// ?
	float				fPHBouncing;			// ?
	//shoot&bounce&visibility&flotation
	float				fFlotationFactor;		// 0.0f - 1.0f		(1.0f-полностью проходимый)
	float				fShootFactor;			// 0.0f - 1.0f		(1.0f-полностью простреливаемый)
	float				fBounceDamageFactor;	// 0.0f - 100.0f
	float				fInjuriousSpeed;		// 0.0f - ...		(0.0f-не отбирает здоровье (скорость уменьшения здоровья))
	float				fVisTransparencyFactor;	// 0.0f - 1.0f		(1.0f-полностью прозрачный)
	float				fSndOcclusionFactor;	// 0.0f - 1.0f		(1.0f-полностью слышен)

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
	inline int				GetID( )
	{
		return ID;
	}
};

using GameMtlVec = xr_vector<SGameMtl*>;
using GameMtlIt = GameMtlVec::iterator;

struct SGameMtlPair
{
	friend class CGameMtlLibrary;
	CGameMtlLibrary* m_Owner;
private:
	int					mtl0;
	int					mtl1;

protected:
	int 				ID; 	// auto number
	int					ID_parent;

public:
	enum
	{
//		flFlotation = (1 << 0),
		flBreakingSounds = (1 << 1),
		flStepSounds = (1 << 2),
//		flCollideSounds = (1 << 3),
		flCollideSounds = (1 << 4),
		flCollideParticles = (1 << 5),
		flCollideMarks = (1 << 6)
	};
	Flags32				OwnProps;
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
	inline int				GetMtl0( )
	{
		return mtl0;
	}
	inline int				GetMtl1( )
	{
		return mtl1;
	}
	inline int				GetID( )
	{
		return ID;
	}
	inline void				SetPair(int m0, int m1)
	{
		mtl0 = m0;
		mtl1 = m1;
	}
	inline bool				IsPair(int m0, int m1)
	{
		return !!(((mtl0 == m0) && (mtl1 == m1)) || ((mtl0 == m1) && (mtl1 == m0)));
	}
	void				Save(IWriter& fs);
	void				Load(IReader& fs);
	inline int 				GetParent( )
	{
		return ID_parent;
	}
	//BOOL				SetParent(int parent);

#ifdef DEBUG
	const char* dbg_Name( );
#endif

};

using GameMtlPairVec = xr_vector<SGameMtlPair*>;
using GameMtlPairIt = GameMtlPairVec::iterator;

class CGameMtlLibrary
{
	int					material_index;
	int					material_pair_index;

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
		R_ASSERT(0 == material_pairs_rt.size( ));
		R_ASSERT(0 == material_pairs.size( ));
		R_ASSERT(0 == materials.size( ));
		*/
	}
	inline void				Unload( )
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
	inline GameMtlIt 		GetMaterialIt(const char* name)
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
	inline GameMtlIt 		GetMaterialIt(CSharedString& name)
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
	inline GameMtlIt 		GetMaterialItByID(int id)
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
	inline u32				GetMaterialID(const char* name)
	{
		GameMtlIt it = GetMaterialIt(name);
		return (it == materials.end( )) ? GAMEMTL_NONE_ID : (*it)->ID;
	}

	inline U16				GetMaterialIdx(int ID)
	{
		GameMtlIt it = GetMaterialItByID(ID);
		VERIFY(materials.end( ) != it);
		return (U16) (it - materials.begin( ));
	}
	inline U16				GetMaterialIdx(const char* name)
	{
		GameMtlIt it = GetMaterialIt(name);
		VERIFY(materials.end( ) != it);
		return (U16) (it - materials.begin( ));
	}
	inline SGameMtl* GetMaterialByIdx(U16 idx)
	{
		VERIFY(idx < materials.size( ));
		return materials[idx];
	}
	inline SGameMtl* GetMaterialByID(int id)
	{
		return GetMaterialByIdx(GetMaterialIdx(id));
	}

	inline GameMtlIt		FirstMaterial( )
	{
		return materials.begin( );
	}
	inline GameMtlIt		LastMaterial( )
	{
		return materials.end( );
	}
	inline u32				CountMaterial( )
	{
		return materials.size( );
	}

	// material pair routine
	inline SGameMtlPair* GetMaterialPair(U16 idx0, U16 idx1)
	{
		R_ASSERT((idx0 < material_count) && (idx1 < material_count));
		return material_pairs_rt[idx1 * material_count + idx0];
	}

	inline GameMtlPairIt	FirstMaterialPair( )
	{
		return material_pairs.begin( );
	}
	inline GameMtlPairIt	LastMaterialPair( )
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

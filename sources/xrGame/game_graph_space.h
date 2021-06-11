////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_space.h
//	Created 	: 18.02.2003
//  Modified 	: 11.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Game graph namespace
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "associative_vector.h"
#include "..\ENGINE\xrLevel.h"

namespace GameGraph {
	typedef u16	_GRAPH_ID;
	typedef U8	_LEVEL_ID;
	typedef U8	_LOCATION_ID;

	enum {
		LOCATION_TYPE_COUNT = 4,
		LOCATION_COUNT		= (u32(1) << (8*sizeof(_LOCATION_ID))),
	};

	class

		SLevel {
		shared_str				m_name;
		Fvector3					m_offset;
		_LEVEL_ID				m_id;
		shared_str				m_section;
		xrGUID					m_guid;

	public:
		inline const shared_str &name		() const
		{
			return				(m_name);
		}

		inline const Fvector3& offset		() const
		{
			return				(m_offset);
		}

		inline const _LEVEL_ID &id			() const
		{
			return				(m_id);
		}

		inline const shared_str &section	() const
		{
			return				(m_section);
		}

		inline const xrGUID &guid			() const
		{
			return				(m_guid);
		}

		inline void load					(IReader *reader);
		inline void save					(IWriter *writer);

		friend class CGameGraph;
	};

	typedef associative_vector<_LEVEL_ID,SLevel>		LEVEL_MAP;

#pragma pack(push,1)

	class

		CEdge {
		_GRAPH_ID					m_vertex_id;
		float						m_path_distance;
	public:
		inline	const _GRAPH_ID			&vertex_id			() const;
		inline	const float				&distance			() const;
	};

	class

		CVertex {
		Fvector3						tLocalPoint;
		Fvector3						tGlobalPoint;
		u32							tLevelID:8;
		u32							tNodeID:24;
		U8							tVertexTypes[LOCATION_TYPE_COUNT];
		u32							dwEdgeOffset;
		u32							dwPointOffset;
		U8							tNeighbourCount;
		U8							tDeathPointCount;
	public:
		inline	const Fvector3&				level_point		() const;
		inline	const Fvector3&			game_point			() const;
		inline	_LEVEL_ID				level_id			() const;
		inline	u32						level_vertex_id		() const;
		inline	const U8* vertex_type( ) const;
		inline	const U8& edge_count( ) const;
		inline	const u32				&edge_offset		() const;
		inline	const U8& death_point_count( ) const;
		inline	const u32				&death_point_offset	() const;
		friend class CGameGraph;
	};

	class

		CHeader {
		U8							m_version;
		_GRAPH_ID					m_vertex_count;
		u32							m_edge_count;
		u32							m_death_point_count;
		xrGUID						m_guid;
		LEVEL_MAP					m_levels;

	public:
		inline	const U8& version( ) const;
		inline	_LEVEL_ID				level_count			() const;
		inline	const _GRAPH_ID			&vertex_count		() const;
		inline	const u32				&edge_count			() const;
		inline	const u32				&death_point_count	() const;
		inline	const xrGUID			&guid				() const;
		inline	const LEVEL_MAP			&levels				() const;
		inline	const SLevel			&level				(const _LEVEL_ID &id) const;
		inline	const SLevel			&level				(const char* level_name) const;
		inline	const SLevel			*level				(const char* level_name, bool) const;
		inline	void					load				(IReader *reader);
		inline	void					save				(IWriter *reader);
		friend class CGameGraph;
	};
#pragma pack(pop)

	class

		CLevelPoint  {
		Fvector3		tPoint;
		u32			tNodeID;
		float		fDistance;	
	public:
		inline const Fvector3&	level_point		() const
		{
			return				(tPoint);
		}

		inline u32						level_vertex_id		() const
		{
			return				(tNodeID);
		}

		inline float					distance			() const
		{
			return				(fDistance);
		}
	};

	struct STerrainPlace{
		svector<_LOCATION_ID,LOCATION_TYPE_COUNT>	tMask;
	};

//	DEFINE_VECTOR	(STerrainPlace,				TERRAIN_VECTOR,					TERRAIN_IT);
	using TERRAIN_VECTOR = xr_vector<STerrainPlace>;
	using TERRAIN_IT = TERRAIN_VECTOR::iterator;
};

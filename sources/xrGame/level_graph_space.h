////////////////////////////////////////////////////////////////////////////
//	Module 		: level_graph_space.h
//	Created 	: 02.10.2001
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Level graph space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace LevelGraph {
	class CHeader : private hdrNODES {
	//private:
	//	friend class CRenumbererConverter;

	public:
		__forceinline	u32				version					() const;
		__forceinline	u32				vertex_count			() const;
		__forceinline	float			cell_size				() const;
		__forceinline	float			factor_y				() const;
		__forceinline	const Fbox3&	box					() const;
		__forceinline const xrGUID	&guid					() const;
	};

	typedef NodePosition	CPosition;

	class CVertex : private NodeCompressed {
	//private:
	//	friend class CRenumbererConverter;

	public:
		__forceinline	u32				link					(int i) const;
		__forceinline	u8				light					() const;
		__forceinline	u16				cover					(u8 index) const;
		__forceinline	u16				plane					() const;
		__forceinline	const CPosition &position				() const;
		__forceinline	bool			operator<				(const LevelGraph::CVertex &vertex) const;
		__forceinline	bool			operator>				(const LevelGraph::CVertex &vertex) const;
		__forceinline	bool			operator==				(const LevelGraph::CVertex &vertex) const;
		friend class CLevelGraph;
	};

	struct SSegment {
		Fvector v1;
		Fvector v2;
	};

	struct SContour : public SSegment {
		Fvector v3;
		Fvector v4;
	};
};

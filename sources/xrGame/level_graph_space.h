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
	public:
		__forceinline	u32				link					(int i) const;
		__forceinline	unsigned char				light					() const;
		__forceinline	unsigned short				cover					(unsigned char index) const;
		__forceinline	unsigned short				plane					() const;
		__forceinline	const CPosition &position				() const;
		__forceinline	bool			operator<				(const LevelGraph::CVertex &vertex) const;
		__forceinline	bool			operator>				(const LevelGraph::CVertex &vertex) const;
		__forceinline	bool			operator==				(const LevelGraph::CVertex &vertex) const;
		friend class CLevelGraph;
	};

	struct SSegment {
		Fvector3 v1;
		Fvector3 v2;
	};

	struct SContour : public SSegment {
		Fvector3 v3;
		Fvector3 v4;
	};
};

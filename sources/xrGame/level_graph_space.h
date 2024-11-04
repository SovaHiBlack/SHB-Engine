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
	private:
		friend class CRenumbererConverter;

	public:
		ICF	u32				version					() const;
		ICF	u32				vertex_count			() const;
		ICF	f32			cell_size				() const;
		ICF	f32			factor_y				() const;
		ICF	const fBox3&	box					() const;
		ICF const xrGUID	&guid					() const;
	};

	typedef NodePosition	CPosition;

	class CVertex : private NodeCompressed {
	private:
		friend class CRenumbererConverter;

	public:
		ICF	u32				link					(int i) const;
		ICF	u8				light					() const;
		ICF	u16				cover					(u8 index) const;
		ICF	u16				plane					() const;
		ICF	const CPosition &position				() const;
		ICF	bool			operator<				(const LevelGraph::CVertex &vertex) const;
		ICF	bool			operator>				(const LevelGraph::CVertex &vertex) const;
		ICF	bool			operator==				(const LevelGraph::CVertex &vertex) const;
		friend class CLevelGraph;
	};

	struct SSegment {
		fVector3 v1;
		fVector3 v2;
	};

	struct SContour : public SSegment {
		fVector3 v3;
		fVector3 v4;
	};
};

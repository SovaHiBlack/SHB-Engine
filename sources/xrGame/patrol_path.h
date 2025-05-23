////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_abstract.h"
#include "patrol_point.h"

class CPatrolPath : public CGraphAbstractSerialize<CPatrolPoint, f32,u32> {
private:
	struct CAlwaysTrueEvaluator {
		IC	bool	operator()	(const fVector3& position) const
		{
			return	(true);
		}
	};

protected:
	typedef CGraphAbstractSerialize<CPatrolPoint, f32,u32> inherited;

public:
#ifdef DEBUG
	shared_str				m_name;
#endif

public:
							CPatrolPath		(shared_str name = "");
	virtual					~CPatrolPath	();
			CPatrolPath		&load_raw		(const CLevelGraph *level_graph, const CGameLevelCrossTable *cross, const CGameGraph *game_graph, IReader &stream);
	IC		const CVertex	*point			(shared_str name) const;
	template <typename T>
	IC		const CVertex* point(const fVector3& position, const T& evaluator) const;
	IC		const CVertex* point(const fVector3& position) const;

#ifdef DEBUG
public:
	virtual void			load			(IReader &stream);
	IC		void			name			(const shared_str &name);
#endif
};

#include "patrol_path_inline.h"
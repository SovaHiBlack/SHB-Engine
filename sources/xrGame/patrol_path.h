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

class CPatrolPath : public CGraphAbstractSerialize<CPatrolPoint, float, u32>
{
private:
	struct CAlwaysTrueEvaluator
	{
		inline	bool	operator()	(const Fvector3& position) const
		{
			return	(true);
		}
	};

protected:
	typedef CGraphAbstractSerialize<CPatrolPoint, float, u32> inherited;

public:

#ifdef DEBUG
	CSharedString				m_name;
#endif

	CPatrolPath(CSharedString name = "");
	virtual					~CPatrolPath( );
	CPatrolPath& load_raw(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph, IReader& stream);
	inline		const CVertex* point(CSharedString name) const;
	template <typename T>
	inline		const CVertex* point(const Fvector3& position, const T& evaluator) const;
	inline		const CVertex* point(const Fvector3& position) const;

#ifdef DEBUG
	virtual void			load(IReader& stream);
	inline void			name(const CSharedString& name);
#endif

};

#include "patrol_path_inline.h"

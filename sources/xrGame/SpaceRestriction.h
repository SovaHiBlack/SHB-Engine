//	Module 		: SpaceRestriction.h
//	Description : Space restriction

#pragma once

#include "restriction_space.h"
#include "SpaceRestrictionHolder.h"
#include "SpaceRestrictionBridge.h"
#include "SpaceRestrictionAbstract.h"

//#define USE_FREE_IN_RESTRICTIONS

class CSpaceRestrictionManager;

class CSpaceRestriction : public RestrictionSpace::CTimeIntrusiveBase, public CSpaceRestrictionAbstract
{
private:
	using inherited = CSpaceRestrictionAbstract;

	friend struct CRemoveMergedFreeInRestrictions;

#ifdef DEBUG
	friend class CLevelGraph;
#endif

	using CBaseRestrictionPtr = SpaceRestrictionHolder::CBaseRestrictionPtr;

	struct CFreeInRestriction
	{
		CBaseRestrictionPtr		m_restriction;
		bool					m_enabled;

		inline CFreeInRestriction(CBaseRestrictionPtr restriction, bool enabled)
		{
			m_restriction = restriction;
			m_enabled = enabled;
		}
	};

private:
	using RESTRICTIONS = xr_vector<CBaseRestrictionPtr>;
	using FREE_IN_RESTRICTIONS = xr_vector<CFreeInRestriction>;

protected:
	bool							m_applied;
	CSharedString					m_out_restrictions;
	CSharedString					m_in_restrictions;
	xr_vector<u32>					m_temp;
	CSpaceRestrictionManager*		m_space_restriction_manager;
	CBaseRestrictionPtr				m_out_space_restriction;
	CBaseRestrictionPtr				m_in_space_restriction;

#ifdef USE_FREE_IN_RESTRICTIONS
	FREE_IN_RESTRICTIONS			m_free_in_restrictions;
#endif

private:
	inline bool					intersects(CBaseRestrictionPtr bridge);
	inline bool					intersects(SpaceRestrictionHolder::CBaseRestrictionPtr bridge0, SpaceRestrictionHolder::CBaseRestrictionPtr bridge1);
	CBaseRestrictionPtr			merge(CBaseRestrictionPtr bridge, const RESTRICTIONS& temp_restrictions) const;
	void						merge_in_out_restrictions( );
	void						merge_free_in_retrictions( );

protected:
	inline bool					initialized( ) const;
	bool						affect(CBaseRestrictionPtr bridge, const Fsphere& sphere) const;
	bool						affect(CBaseRestrictionPtr bridge, unsigned int start_vertex_id, float radius) const;
	bool						affect(CBaseRestrictionPtr bridge, const Fvector3& start_position, const Fvector3& dest_position) const;
	bool						affect(CBaseRestrictionPtr bridge, unsigned int start_vertex_id, unsigned int dest_vertex_id) const;

public:
								CSpaceRestriction(CSpaceRestrictionManager* space_restriction_manager, CSharedString out_restrictions, CSharedString in_restrictions);
	void						initialize( );
	void						remove_border( );

	template <typename T1, typename T2>
	inline void					add_border(T1 p1, T2 p2);

	unsigned int				accessible_nearest(const Fvector3& position, Fvector3& result);
	bool						accessible(const Fsphere& sphere);
	bool						accessible(unsigned int level_vertex_id, float radius);
	inline CSharedString		out_restrictions( ) const;
	inline CSharedString		in_restrictions( ) const;
	inline bool					applied( ) const;
	inline bool					inside(const Fsphere& sphere);
	inline bool					inside(unsigned int level_vertex_id, bool partially_inside);
	virtual CSharedString		name( ) const;
};

#include "SpaceRestriction_inline.h"

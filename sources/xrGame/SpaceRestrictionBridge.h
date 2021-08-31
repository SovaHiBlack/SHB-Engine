//	Module 		: SpaceRestrictionBridge.h
//	Description : Space restriction bridge

#pragma once

#include "restriction_space.h"

class CSpaceRestrictionBase;

class CSpaceRestrictionBridge : public RestrictionSpace::CTimeIntrusiveBase
{
protected:
	CSpaceRestrictionBase* m_object;

public:
	inline CSpaceRestrictionBase&	object( ) const;

	inline							CSpaceRestrictionBridge(CSpaceRestrictionBase* object);
	virtual					~CSpaceRestrictionBridge( );
	void					change_implementation(CSpaceRestrictionBase* object);
	const xr_vector<u32>& border( ) const;
	bool					initialized( ) const;
	void					initialize( );
	bool					inside(const Fsphere& sphere);
	bool					inside(u32 level_vertex_id, bool partially_inside);
	bool					inside(u32 level_vertex_id, bool partially_inside, float radius);
	CSharedString				name( ) const;
	u32						accessible_nearest(const Fvector3& position, Fvector3& result, bool out_restriction);
	bool					shape( ) const;
	bool					default_restrictor( ) const;
	bool					on_border(const Fvector3& position) const;
	bool					out_of_border(const Fvector3& position);
	Fsphere					sphere( ) const;

	template <typename T>
	inline u32						accessible_nearest(T& restriction, const Fvector3& position, Fvector3& result, bool out_restriction);
	template <typename T>
	inline const xr_vector<u32>& accessible_neighbour_border(T& restriction, bool out_restriction);
};

#include "SpaceRestrictionBridge_inline.h"

//	Module 		: SpaceRestrictionAbstract.h
//	Description : Space restriction abstract

#pragma once

class CSpaceRestrictionAbstract
{
protected:
	xr_vector<unsigned int>			m_border;
	bool							m_initialized;
	xr_vector<unsigned int>			m_accessible_neighbour_border;
	bool							m_accessible_neighbour_border_actual;

private:
	template <typename T>
	inline bool					accessible_neighbours(T& restriction, unsigned int level_vertex_id, bool out_restriction);

	template <typename T>
	inline void					prepare_accessible_neighbour_border(T& restriction, bool out_restriction);

public:
	inline								CSpaceRestrictionAbstract( );
	virtual							~CSpaceRestrictionAbstract( )
	{ }
	virtual void					initialize( ) = 0;
	inline const xr_vector<unsigned int>& border( );
	inline bool					initialized( ) const;

	template <typename T>
	inline const xr_vector<unsigned int>& accessible_neighbour_border(T& restriction, bool out_restriction);

	virtual CSharedString				name( ) const = 0;
};

#include "SpaceRestrictionAbstract_inline.h"

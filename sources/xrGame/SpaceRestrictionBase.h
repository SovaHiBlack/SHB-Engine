//	Module 		: SpaceRestrictionBase.h
//	Description : Space restriction base

#pragma once

#include "SpaceRestrictionAbstract.h"

class CSpaceRestrictionBase : public CSpaceRestrictionAbstract
{
private:
	using inherited = CSpaceRestrictionAbstract;

public:

#ifdef DEBUG
	xr_vector<unsigned int>		m_test_storage;
	bool				m_correct;
#endif // def DEBUG

protected:
	void		process_borders( );

public:
	bool		inside(unsigned int level_vertex_id, bool partially_inside);
	bool		inside(unsigned int level_vertex_id, bool partially_inside, float radius);
	virtual	bool		inside(const Fsphere& sphere) = 0;
	virtual bool		shape( ) const = 0;
	virtual bool		default_restrictor( ) const = 0;
	virtual Fsphere		sphere( ) const = 0;

#ifdef DEBUG
	inline bool		correct( ) const;
#endif // def DEBUG

};

#include "SpaceRestrictionBase_inline.h"

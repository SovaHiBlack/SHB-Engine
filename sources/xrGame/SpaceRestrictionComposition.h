//	Module 		: SpaceRestrictionComposition.h
//	Description : Space restriction composition

#pragma once

#include "SpaceRestrictionBase.h"
#include "SpaceRestrictionHolder.h"

class CSpaceRestrictionBridge;
class CSpaceRestrictionHolder;

extern int g_restriction_checker;

class CSpaceRestrictionComposition : public CSpaceRestrictionBase
{
public:
	using CSpaceRestrictionBase::inside;

protected:
	using CBaseRestrictionPtr										= SpaceRestrictionHolder::CBaseRestrictionPtr;
	using RESTRICTIONS												= xr_vector<CBaseRestrictionPtr>;

	RESTRICTIONS													m_restrictions;
	CSharedString													m_space_restrictors;
	CSpaceRestrictionHolder*										m_space_restriction_holder;
	Fsphere															m_sphere;

#ifdef DEBUG
private:
	void							check_restrictor_type			( );
#endif // def DEBUG

protected:
	inline void						merge							(CBaseRestrictionPtr restriction);

public:
	inline							CSpaceRestrictionComposition	(CSpaceRestrictionHolder* space_restriction_holder, CSharedString space_restrictors);
	virtual							~CSpaceRestrictionComposition	( );
	virtual void					initialize						( );
	virtual bool					inside							(const Fsphere& sphere);
	inline virtual CSharedString	name							( ) const;
	inline virtual bool				shape							( ) const;
	inline virtual bool				default_restrictor				( ) const;
	virtual Fsphere					sphere							( ) const;

#ifdef DEBUG
	void							test_correctness				( );
#endif // def DEBUG

};

#include "SpaceRestrictionComposition_inline.h"

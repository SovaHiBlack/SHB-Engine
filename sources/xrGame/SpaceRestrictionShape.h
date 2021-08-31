//	Module 		: SpaceRestrictionShape.h
//	Description : Space restriction shape

#pragma once

#include "SpaceRestrictionBase.h"
#include "..\ENGINE\xr_collide_form.h"

class CSpaceRestrictor;

class CSpaceRestrictionShape : public CSpaceRestrictionBase
{
private:
	friend struct CBorderMergePredicate;

public:
	using CSpaceRestrictionBase::inside;

protected:
	CSpaceRestrictor* m_restrictor;
	bool					m_default;

	inline Fvector3			position(const CCF_Shape::shape_def& data) const;
	inline float			radius(const CCF_Shape::shape_def& data) const;
	void					build_border( );
	void					fill_shape(const CCF_Shape::shape_def& shape);

public:
	inline					CSpaceRestrictionShape(CSpaceRestrictor* space_restrictor, bool default_restrictor);
	inline virtual void		initialize( );
	virtual bool			inside(const Fsphere& sphere);
	virtual CSharedString	name( ) const;
	inline virtual bool		shape( ) const;
	inline virtual bool		default_restrictor( ) const;
	virtual Fsphere			sphere( ) const;

#ifdef DEBUG
	void					test_correctness( );
#endif // def DEBUG

};

#include "SpaceRestrictionShape_inline.h"

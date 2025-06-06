////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_shape.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction shape
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "space_restriction_base.h"
#include "..\XR_3DA\xr_collide_form.h"

class CSpaceRestrictor;

class CSpaceRestrictionShape : public CSpaceRestrictionBase {
private:
	friend struct CBorderMergePredicate;

public:
	using CSpaceRestrictionBase::inside;

protected:
	CSpaceRestrictor		*m_restrictor;
	bool					m_default;

protected:
	IC			fVector3		position				(const CCF_Shape::shape_def &data) const;
	IC			f32		radius					(const CCF_Shape::shape_def &data) const;
				void		build_border			();
				void		fill_shape				(const CCF_Shape::shape_def &shape);

public:
	IC						CSpaceRestrictionShape	(CSpaceRestrictor *space_restrictor, bool default_restrictor);
	IC	virtual void		initialize				();
		virtual bool		inside					(const fSphere& sphere);
		virtual shared_str	name					() const;
	IC	virtual bool		shape					() const;
	IC	virtual bool		default_restrictor		() const;
		virtual	fSphere		sphere					() const;
#ifdef DEBUG
				void		test_correctness		();
#endif
};

#include "space_restriction_shape_inline.h"
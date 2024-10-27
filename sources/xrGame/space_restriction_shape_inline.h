////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_shape_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction shape inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	fVector3	CSpaceRestrictionShape::position		(const CCF_Shape::shape_def &data) const
{
	switch (data.type) {
		case 0	: return(data.data.sphere.P);
		case 1	: return(data.data.box.c);
		default : NODEFAULT;
	}
#ifdef DEBUG
	return	(fVector3().set(0.0f,0.0f,0.0f));
#endif
}

IC	f32 CSpaceRestrictionShape::radius			(const CCF_Shape::shape_def &data) const
{
	switch (data.type) {
		case 0	: return(data.data.sphere.R);
		case 1	: return(fBox3().set(fVector3().set(-0.5f,-0.5f,-0.5f), fVector3().set(0.5f,0.5f,0.5f)).xform(data.data.box).getradius());
		default : NODEFAULT;
	}
#ifdef DEBUG
	return	(0.f);
#endif
}

IC	CSpaceRestrictionShape::CSpaceRestrictionShape	(CSpaceRestrictor *space_restrictor, bool default_restrictor)
{
	m_default					= default_restrictor;
	m_initialized				= true;
	

	VERIFY						(space_restrictor);
	m_restrictor				= space_restrictor;
	
	build_border				();
}

IC	void CSpaceRestrictionShape::initialize			()
{
	VERIFY						(m_initialized);
}

IC	bool CSpaceRestrictionShape::shape				() const
{
	return						(true);
}

IC	bool CSpaceRestrictionShape::default_restrictor	() const
{
	return						(m_default);
}

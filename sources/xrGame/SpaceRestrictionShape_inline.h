//	Module 		: SpaceRestrictionShape_inline.h
//	Description : Space restriction shape inline functions

#pragma once

inline Fvector3 CSpaceRestrictionShape::position(const CCF_Shape::shape_def& data) const
{
	switch (data.type)
	{
		case 0:
		{
			return(data.data.sphere.P);
		}
		case 1:
		{
			return(data.data.box.c);
		}
		default:
		{
			NODEFAULT;
		}
	}

#ifdef DEBUG
	return Fvector3( ).set(0.0f, 0.0f, 0.0f);
#endif // def DEBUG

}

inline float CSpaceRestrictionShape::radius(const CCF_Shape::shape_def& data) const
{
	switch (data.type)
	{
		case 0:
		{
			return data.data.sphere.R;
		}
		case 1:
		{
			return (Fbox3( ).set(Fvector3( ).set(-0.5f, -0.5f, -0.5f), Fvector3( ).set(0.5f, 0.5f, 0.5f)).xform(data.data.box).getradius( ));
		}
		default:
		{
			NODEFAULT;
		}
	}

#ifdef DEBUG
	return 0.0f;
#endif // def DEBUG

}

inline CSpaceRestrictionShape::CSpaceRestrictionShape(CSpaceRestrictor* space_restrictor, bool default_restrictor)
{
	m_default = default_restrictor;
	m_initialized = true;

	VERIFY(space_restrictor);
	m_restrictor = space_restrictor;

	build_border( );
}

inline void CSpaceRestrictionShape::initialize( )
{
	VERIFY(m_initialized);
}

inline bool CSpaceRestrictionShape::shape( ) const
{
	return true;
}

inline bool CSpaceRestrictionShape::default_restrictor( ) const
{
	return m_default;
}

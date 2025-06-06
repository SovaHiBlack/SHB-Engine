////////////////////////////////////////////////////////////////////////////
//	Module 		: SpaceRestrictor.h
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "restriction_space.h"
#include "script_export_space.h"

class CSpaceRestrictor : public CGameObject
{
private:
	typedef CGameObject			inherited;

private:
	enum
	{
		PLANE_COUNT = 6
	};

private:
	typedef fPlane3				CPlanesArray[PLANE_COUNT];

private:
	struct CPlanes
	{
		CPlanesArray			m_planes;
	};

private:
	typedef xr_vector<fSphere>	SPHERES;
	typedef xr_vector<CPlanes>	BOXES;

private:
	mutable SPHERES				m_spheres;
	mutable BOXES				m_boxes;
	mutable fSphere				m_selfbounds;
	mutable bool				m_actuality;

private:
	u8					m_space_restrictor_type;

private:
	IC		void							actual(bool value) const;
	void									prepare( ) const;
	bool									prepared_inside(const fSphere& sphere) const;

public:
	IC										CSpaceRestrictor( );
	virtual									~CSpaceRestrictor( );
	virtual BOOL							net_Spawn(CSE_Abstract* data);
	virtual void							net_Destroy( );
	bool									inside(const fSphere& sphere) const;
	virtual void							Center(fVector3& C) const;
	virtual f32								Radius( ) const;
	virtual BOOL							UsedAI_Locations( );
	virtual void							spatial_move( );
	IC bool									actual( ) const;
	virtual CSpaceRestrictor*				cast_restrictor( )
	{
		return this;
	}
	virtual bool							register_schedule( ) const
	{
		return false;
	}

	IC RestrictionSpace::ERestrictorTypes	restrictor_type( ) const;

#ifdef DEBUG
public:
	virtual void							OnRender( );
#endif // def DEBUG

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CSpaceRestrictor)
#undef script_type_list
#define script_type_list save_type_list(CSpaceRestrictor)

#include "SpaceRestrictor_inline.h"

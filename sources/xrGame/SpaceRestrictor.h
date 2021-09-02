//	Module 		: SpaceRestrictor.h
//	Description : Space restrictor

#pragma once

#include "GameObject.h"
#include "Restriction_space.h"
#include "script_export_space.h"

class CSpaceRestrictor : public CGameObject
{
private:
	using inherited = CGameObject;

private:
	enum
	{
		PLANE_COUNT = 6
	};

private:
	typedef Fplane				CPlanesArray[PLANE_COUNT];

private:
	struct CPlanes
	{
		CPlanesArray			m_planes;
	};

private:
	typedef xr_vector<Fsphere>	SPHERES;
	typedef xr_vector<CPlanes>	BOXES;

private:
	mutable SPHERES				m_spheres;
	mutable BOXES				m_boxes;
	mutable Fsphere				m_selfbounds;
	mutable bool				m_actuality;

private:
	U8					m_space_restrictor_type;

private:
	inline void				actual(bool value) const;
	void				prepare( ) const;
	bool				prepared_inside(const Fsphere& sphere) const;

public:
	inline						CSpaceRestrictor( );
	virtual						~CSpaceRestrictor( );
	virtual BOOL				net_Spawn(CSE_Abstract* data);
	virtual void				net_Destroy( );
	bool						inside(const Fsphere& sphere) const;
	virtual void				Center(Fvector3& C) const;
	virtual float				Radius( ) const;
	virtual BOOL				UsedAI_Locations( );
	virtual void				spatial_move( );
	inline bool					actual( ) const;
	virtual CSpaceRestrictor*	cast_restrictor( )
	{
		return this;
	}
	virtual bool				register_schedule( ) const
	{
		return false;
	}

	inline Restriction::ERestrictorTypes restrictor_type( ) const;

public:
#ifdef DEBUG
	virtual void				OnRender( );
#endif // def DEBUG

public:
	static void					script_register(lua_State*);
};

add_to_type_list(CSpaceRestrictor)
#undef script_type_list
#define script_type_list save_type_list(CSpaceRestrictor)

// Description : Space restrictor inline functions
inline CSpaceRestrictor::CSpaceRestrictor( )
{
	m_space_restrictor_type = Restriction::eRestrictorTypeNone;
}

inline bool CSpaceRestrictor::actual( ) const
{
	return m_actuality;
}

inline void CSpaceRestrictor::actual(bool value) const
{
	m_actuality = value;
}

inline Restriction::ERestrictorTypes CSpaceRestrictor::restrictor_type( ) const
{
	return Restriction::ERestrictorTypes(m_space_restrictor_type);
}

#pragma once

#include "MathUtils.h"
#include "PHValideValues.h"

class CPHActivationShape : public CPHObject
{
	dBodyID					m_body;
	dGeomID					m_geom;
	flags16					m_flags;
	CSafeFixedRotationState	m_safe_state;

public:
	enum					EType
	{
		etBox,
		etCylinder,
		etSphere
	};

	enum
	{
		flFixedRotation		= 1 << 0,
		flFixedPosition		= 1 << 1,
		flStaticEnvironment	= 1 << 2,
		flGravity			= 1 << 3
	};

							CPHActivationShape		( );
							~CPHActivationShape		( );
	void					Create					(const fVector3 start_pos, const fVector3 start_size, CPhysicsShellHolder* ref_obj, EType type = etBox, u16 flags = 0);
	void					Destroy					( );
	bool					Activate				(const fVector3 need_size, u16 steps, f32 max_displacement, f32 max_rotation, bool un_freeze_later = false);
	const fVector3&			Position				( );
	void					Size					(fVector3& size);
	dBodyID					ODEBody					( )
	{
		return m_body;
	}
	void					set_rotation			(const fMatrix4x4& rot);

private:
	virtual void			PhDataUpdate			(dReal step);
	virtual void			PhTune					(dReal step);
	virtual void			CutVelocity				(f32 l_limit, f32 a_limit);
	virtual void			InitContact				(dContact* c, bool& do_collide, u16, u16);
	virtual dGeomID			dSpacedGeom				( );
	virtual void			get_spatial_params		( );
	virtual u16				get_elements_number		( )
	{
		return 0;
	}
	virtual CPHSynchronize* get_element_sync		(u16 element)
	{
		return NULL;
	}
};

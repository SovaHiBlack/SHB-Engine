#pragma once

#include "PhysicsCommon.h"
#include "ExtendedGeom.h"

// this is equivalent dMULTIPLYOP0_333 whith consequent transposion of A
#define dMULTIPLYOP3_333(A, op, B, C)    \
	(A)[0]  op dDOT14((B),     (C));     \
	(A)[1]  op dDOT14((B + 4), (C));     \
	(A)[2]  op dDOT14((B + 8), (C));     \
	(A)[4]  op dDOT14((B),     (C + 1)); \
	(A)[5]  op dDOT14((B + 4), (C + 1)); \
	(A)[6]  op dDOT14((B + 8), (C + 1)); \
	(A)[8]  op dDOT14((B),     (C + 2)); \
	(A)[9]  op dDOT14((B + 4), (C + 2)); \
	(A)[10] op dDOT14((B + 8), (C + 2));

inline void dMULTIPLY3_333(dReal* A, const dReal* B, const dReal* C)
{
	dMULTIPLYOP3_333(A, =, B, C)
}

class CGameObject;
class CPHObject;

class CCodeGeom
{
public:
	//get
	virtual float		volume( ) = 0;
	virtual void		get_mass(dMass& m) = 0;		//unit dencity mass;
	void		get_mass(dMass& m, const Fvector3& ref_point, float density);
	void		get_mass(dMass& m, const Fvector3& ref_point);
	void		add_self_mass(dMass& m, const Fvector3& ref_point);
	void		add_self_mass(dMass& m, const Fvector3& ref_point, float density);
	void		get_local_center_bt(Fvector3& center);		//for built
	void		get_global_center_bt(Fvector3& center);		//for built
	void		get_local_form_bt(Fmatrix& form);	    //for built
	void		get_global_form_bt(Fmatrix& form);		//for built

	void		set_static_ref_form(const Fmatrix& form);		//for built
	virtual void		get_max_area_dir_bt(Fvector3& dir) = 0;
	virtual float		radius( ) = 0;
	virtual void		get_extensions_bt(const Fvector3& axis, float center_prg, float& lo_ext, float& hi_ext) = 0;
	void		clear_cashed_tries( );
	inline dGeomID		geom( )
	{
		return dGeomTransformGetGeom(m_geom_transform);
	}
	inline dGeomID		geometry_transform( )
	{
		return m_geom_transform;
	}
	inline dGeomID		geometry( )
	{
		return m_geom_transform ? (geom( ) ? geom( ) : m_geom_transform) : nullptr;
	}
	inline dGeomID		geometry_bt( )
	{
		if (is_transformed_bt( ))
		{
			return geom( );
		}
		else
		{
			return geometry_transform( );
		}

	}
	__forceinline static bool		is_transform(dGeomID g)
	{
		return dGeomGetClass(g) == dGeomTransformClass;
	}
	inline bool		is_transformed_bt( )
	{
		return is_transform(m_geom_transform);
	}
	inline unsigned short& element_position( )
	{
		return dGeomGetUserData(geometry( ))->element_position;
	}
	virtual const Fvector3& local_center( ) = 0;
	virtual void		get_local_form(Fmatrix& form) = 0;
	virtual void		set_local_form(const Fmatrix& form) = 0;
		//set
		//element part
	void		set_body(dBodyID body);
	void		set_bone_id(unsigned short id)
	{
		m_bone_id = id;
	}
	unsigned short			bone_id( )
	{
		return m_bone_id;
	}
	void		add_to_space(dSpaceID space);
	void		remove_from_space(dSpaceID space);
	void		set_material(unsigned short ul_material);
	void		set_contact_cb(ContactCallbackFun* ccb);
	void		set_obj_contact_cb(ObjectContactCallbackFun* occb);
	void		add_obj_contact_cb(ObjectContactCallbackFun* occb);
	void		remove_obj_contact_cb(ObjectContactCallbackFun* occb);
	void		set_callback_data(void* cd);
	void* get_callback_data( );
	void		set_ref_object(CPHShellHolder* ro);
	void		set_ph_object(CPHObject* o);

	static void		get_final_tx(dGeomID g, const dReal*& p, const dReal*& R, dReal* bufV, dReal* bufM);
	void		build(const Fvector3& ref_point);
	virtual void		set_position(const Fvector3& ref_point);//for build geom
	void		move_local_basis(const Fmatrix& inv_new_mul_old);
	void		destroy( );
	CCodeGeom( );
	virtual					~CCodeGeom( );

protected:
	dGeomID m_geom_transform;
	unsigned short		m_bone_id;

	void		init( );
	void		get_final_tx_bt(const dReal*& p, const dReal*& R, dReal* bufV, dReal* bufM);
	virtual dGeomID		create( ) = 0;
};

class CBoxGeom : public CCodeGeom
{
	typedef CCodeGeom inherited;
	Fobb	m_box;
public:
	CBoxGeom(const Fobb& box);
//	virtual					~CBoxGeom			(const Fobb& box)													;
	virtual		float		volume( );
	virtual		float		radius( );
	virtual		void		get_extensions_bt(const Fvector3& axis, float center_prg, float& lo_ext, float& hi_ext);
	virtual		void		get_max_area_dir_bt(Fvector3& dir);
	virtual		void		get_mass(dMass& m);//unit dencity mass;
	virtual const	Fvector3& local_center( );
	virtual		void		get_local_form(Fmatrix& form);
	virtual			void		set_local_form(const Fmatrix& form);
	virtual		dGeomID		create( );
	virtual		void		set_position(const Fvector3& ref_point);
};

class CSphereGeom : public CCodeGeom
{
	typedef CCodeGeom inherited;
	Fsphere	m_sphere;
public:
	CSphereGeom(const Fsphere& sphere);
	virtual		float		volume( );
	virtual		float		radius( );
	virtual		void		get_extensions_bt(const Fvector3& axis, float center_prg, float& lo_ext, float& hi_ext);
	virtual		void		get_max_area_dir_bt(Fvector3& dir)
	{ };
	virtual		void		get_mass(dMass& m);//unit dencity mass;
	virtual const	Fvector3& local_center( );
	virtual		void		get_local_form(Fmatrix& form);
	virtual		void		set_local_form(const Fmatrix& form);
	virtual		dGeomID		create( );
	virtual		void		set_position(const Fvector3& ref_point);
};
class CCylinderGeom : public CCodeGeom
{
	typedef CCodeGeom inherited;
	Fcylinder m_cylinder;
public:
	CCylinderGeom(const Fcylinder& cyl);
	virtual		float		volume( );
	virtual		float		radius( );
	virtual		void		get_extensions_bt(const Fvector3& axis, float center_prg, float& lo_ext, float& hi_ext);
	virtual		void		get_max_area_dir_bt(Fvector3& dir)
	{ };
	virtual const	Fvector3& local_center( );
	virtual		void		get_mass(dMass& m);//unit dencity mass;
	virtual		void		get_local_form(Fmatrix& form);
	virtual		void		set_local_form(const Fmatrix& form);
	virtual		dGeomID		create( );
	virtual		void		set_position(const Fvector3& ref_point);
};

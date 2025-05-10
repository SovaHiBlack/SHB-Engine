#pragma once

#include "PhysicsCommon.h"
#include "ExtendedGeom.h"

//this is equivalent dMULTIPLYOP0_333 whith consequent transposion of A
#define dMULTIPLYOP3_333(A,op,B,C)  \
	(A)[0]  op dDOT14((B),  (C));   \
	(A)[1]  op dDOT14((B+4),(C));   \
	(A)[2]  op dDOT14((B+8),(C));   \
	(A)[4]  op dDOT14((B),  (C+1)); \
	(A)[5]  op dDOT14((B+4),(C+1)); \
	(A)[6]  op dDOT14((B+8),(C+1)); \
	(A)[8]  op dDOT14((B),  (C+2)); \
	(A)[9]  op dDOT14((B+4),(C+2)); \
	(A)[10] op dDOT14((B+8),(C+2));

IC void dMULTIPLY3_333(dReal *A, const dReal *B, const dReal *C)
{ dMULTIPLYOP3_333(A,=,B,C) }

class CGameObject;
class CPHObject;

class CODEGeom
{
protected:
	dGeomID m_geom_transform;
	u16		m_bone_id;

public:
	//get
	virtual		f32			volume				()																	=0;
	virtual		void		get_mass			(dMass& m)															=0;		//unit dencity mass;
				void		get_mass			(dMass& m,const fVector3& ref_point, f32 density)					;
				void		get_mass			(dMass& m,const fVector3& ref_point)								;
				void		add_self_mass		(dMass& m,const fVector3& ref_point)								;
				void		add_self_mass		(dMass& m,const fVector3& ref_point, f32 density)					;
				void		get_local_center_bt	(fVector3& center)													;		//for built
				void		get_global_center_bt(fVector3& center)													;		//for built
				void		get_local_form_bt	(fMatrix4x4& form)													;		//for built
				void		get_global_form_bt	(fMatrix4x4& form)													;		//for built
				
				void		set_static_ref_form	(const fMatrix4x4& form)											;		//for built
	virtual		void		get_max_area_dir_bt	(fVector3& dir)														=0;
	virtual		f32			radius				()																	=0;
	virtual		void		get_extensions_bt	(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext) =0;
				void		clear_cashed_tries	()																	;
	IC			dGeomID		geom()
	{
								return dGeomTransformGetGeom(m_geom_transform);
	}
	IC			dGeomID		geometry_transform	()
							{
								return m_geom_transform;
							}
	IC			dGeomID		geometry()
							{
								return (m_geom_transform ? (geom() ? geom() : m_geom_transform) : NULL);
							}
	IC			dGeomID		geometry_bt()
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
	ICF	static	bool		is_transform(dGeomID g)
							{
								return dGeomGetClass(g)==dGeomTransformClass;
							}
	IC			bool		is_transformed_bt()
							{
								return is_transform(m_geom_transform);
							}
	IC			u16&		element_position()
							{
								return dGeomGetUserData(geometry())->element_position;
							}
virtual const	fVector3&	local_center		()																	=0;
virtual			void		get_local_form		(fMatrix4x4& form)													=0;
virtual			void		set_local_form		(const fMatrix4x4& form)											=0;
	//set
	//element part
				void		set_body			(dBodyID body)														;
				void		set_bone_id			(u16 id)											{m_bone_id=id;}
				u16			bone_id				()												{return m_bone_id;}
				void		add_to_space		(dSpaceID space)													;
				void		remove_from_space	(dSpaceID space)													;
				void		set_material		(u16 ul_material)													;
				void		set_contact_cb		(ContactCallbackFun* ccb)											;
				void		set_obj_contact_cb	(ObjectContactCallbackFun* occb)									;
				void		add_obj_contact_cb	(ObjectContactCallbackFun* occb)									;
				void		remove_obj_contact_cb(ObjectContactCallbackFun* occb)									;
				void		set_callback_data	(pvoid cd)															;
				pvoid		get_callback_data	()																	;
				void		set_ref_object		(CPhysicsShellHolder* ro)											;
				void		set_ph_object		(CPHObject* o)														;

	//build/destroy
protected:
				void		init				()																	;
				void		get_final_tx_bt		(const dReal*	&p,const dReal*	&R,dReal * bufV, dReal* bufM)		;
	virtual		dGeomID		create				()																	=0;

public:
	static		void		get_final_tx		(dGeomID g,const dReal*	&p,const dReal*	&R,dReal * bufV, dReal* bufM);
				void		build				(const fVector3& ref_point)											;
	virtual		void		set_position		(const fVector3& ref_point)											;//for build geom
				void		move_local_basis	(const fMatrix4x4& inv_new_mul_old)									;
				void		destroy				()																	;
							CODEGeom			()																	;
	virtual					~ CODEGeom			()																	;
};

class CBoxGeom : public CODEGeom
{
	typedef CODEGeom inherited																						;
	fObb	m_box;

public:
							CBoxGeom			(const fObb& box)													;
//	virtual					~CBoxGeom			(const fObb& box)													;
	virtual		f32			volume				()																	;
	virtual		f32			radius				()																	;
	virtual		void		get_extensions_bt	(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext)	;
	virtual		void		get_max_area_dir_bt	(fVector3& dir)														;
	virtual		void		get_mass			(dMass& m)															;//unit dencity mass;
virtual const	fVector3&	local_center		()																	;
	virtual		void		get_local_form		(fMatrix4x4& form)													;
	virtual		void		set_local_form		(const fMatrix4x4& form)											;
	virtual		dGeomID		create				()																	;
	virtual		void		set_position		(const fVector3& ref_point)											;
};

class CSphereGeom : public CODEGeom
{
	typedef CODEGeom inherited																						;
	fSphere	m_sphere;

public:
							CSphereGeom			(const fSphere& sphere)												;
	virtual		f32			volume				()																	;
	virtual		f32			radius				()																	;
	virtual		void		get_extensions_bt	(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext)	;
	virtual		void		get_max_area_dir_bt	(fVector3& dir)													  {}
	virtual		void		get_mass			(dMass& m)															;//unit dencity mass;
virtual const	fVector3&	local_center		()																	;
	virtual		void		get_local_form		(fMatrix4x4& form)													;
	virtual		void		set_local_form		(const fMatrix4x4& form)											;
	virtual		dGeomID		create				()																	;
	virtual		void		set_position		(const fVector3& ref_point)											;
};

class CCylinderGeom : public CODEGeom
{
	typedef CODEGeom inherited																						;
	fCylinder m_cylinder;

public:
							CCylinderGeom		(const fCylinder& cyl)												;
	virtual		f32			volume				()																	;
	virtual		f32			radius				()																	;
	virtual		void		get_extensions_bt	(const fVector3& axis, f32 center_prg, f32& lo_ext, f32& hi_ext)	;
	virtual		void		get_max_area_dir_bt	(fVector3& dir)													  {}
virtual const	fVector3&	local_center		()																	;
	virtual		void		get_mass			(dMass& m)															;//unit dencity mass;
	virtual		void		get_local_form		(fMatrix4x4& form)													;
	virtual		void		set_local_form		(const fMatrix4x4& form)											;
	virtual		dGeomID		create				()																	;
	virtual		void		set_position		(const fVector3& ref_point)											;
};

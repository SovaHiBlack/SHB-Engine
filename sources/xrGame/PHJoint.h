/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#pragma once

#include "PhysicsShell.h"

class CPHJointDestroyInfo;

class CPHJoint : public CPhysicsJoint
{
	///////////////////////////////////////////////////////
	u16									m_bone_id;
	CPHElement*							pFirst_element;
	CPHElement*							pSecond_element;
	CODEGeom*							pFirstGeom;
	/////////////////////////////////////////////////////////
	CPHShell*							pShell;
	dJointID							m_joint;
	dJointID							m_joint1;
	CPhysicsJoint**						m_back_ref;
	CPHJointDestroyInfo*				m_destroy_info;
	f32									m_erp;			//joint erp
	f32									m_cfm;			//joint cfm
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SPHAxis
	{
		f32					high;						//high limit
		f32					low;						//law limit
		f32					zero;						//zero angle position
		f32					erp;						//limit erp
		f32					cfm;						//limit cfm
		eVs  				vs;							//coordinate system
		f32					force;						//max force
		f32					velocity;					//velocity to achieve
		fVector3			direction;					//axis direction
		IC 	void 					set_limits(f32 h, f32 l)
		{
			high = h;
			low = l;
		}
		IC 	void 					set_direction(const fVector3& v)
		{
			direction.set(v);
		}
		IC 	void 					set_direction(const f32 x, const f32 y, const f32 z)
		{
			direction.set(x, y, z);
		}
		IC 	void 					set_param(const f32 e, const f32 c)
		{
			erp = e;
			cfm = c;
		}
		void					set_sd_factors(f32 sf, f32 df, enumType jt);
		SPHAxis( );
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	xr_vector<SPHAxis>		axes;
	fVector3				anchor;
	eVs						vs_anchor;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////																						
	void 					CreateBall( );
	void 					CreateHinge( );
	void 					CreateHinge2( );
	void 					CreateFullControl( );
	void					CreateSlider( );
	void 					LimitAxisNum(s32& axis_num);
	void 					SetForceActive(const s32 axis_num);
	void 					SetVelocityActive(const s32 axis_num);
	void					SetLimitsActive(s32 axis_num);
	void 					CalcAxis(s32 ax_num, fVector3& axis, f32& lo, f32& hi, const fMatrix4x4& first_matrix, const fMatrix4x4& second_matrix);
	void 					CalcAxis(s32 ax_num, fVector3& axis, f32& lo, f32& hi, const fMatrix4x4& first_matrix, const fMatrix4x4& second_matrix, const fMatrix4x4& rotate);
	virtual u16						GetAxesNumber( );
	virtual void 					SetAxisSDfactors(f32 spring_factor, f32 damping_factor, s32 axis_num);
	virtual void 					SetJointSDfactors(f32 spring_factor, f32 damping_factor);
	virtual void					SetJointSDfactorsActive( );
	virtual void					SetLimitsSDfactorsActive( );
	virtual void					SetAxisSDfactorsActive(int axis_num);
	virtual void 					SetAxis(const SPHAxis& axis, const int axis_num);
	virtual void 					SetAnchor(const fVector3& position)
	{
		SetAnchor(position.x, position.y, position.z);
	}
	virtual void 					SetAnchorVsFirstElement(const fVector3& position)
	{
		SetAnchorVsFirstElement(position.x, position.y, position.z);
	}
	virtual void 					SetAnchorVsSecondElement(const fVector3& position)
	{
		SetAnchorVsSecondElement(position.x, position.y, position.z);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void 					SetAxisDir(const fVector3& orientation, const s32 axis_num)
	{
		SetAxisDir(orientation.x, orientation.y, orientation.z, axis_num);
	}
	virtual void 					SetAxisDirVsFirstElement(const fVector3& orientation, const s32 axis_num)
	{
		SetAxisDirVsFirstElement(orientation.x, orientation.y, orientation.z, axis_num);
	}
	virtual void 					SetAxisDirVsSecondElement(const fVector3& orientation, const s32 axis_num)
	{
		SetAxisDirVsSecondElement(orientation.x, orientation.y, orientation.z, axis_num);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void 					SetLimits(const f32 low, const f32 high, const s32 axis_num);
	virtual void 					SetLimitsVsFirstElement(const f32 low, const f32 high, const s32 axis_num);
	virtual void 					SetLimitsVsSecondElement(const f32 low, const f32 high, const s32 axis_num);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void 					SetAnchor(const f32 x, const f32 y, const f32 z);
	virtual void 					SetAnchorVsFirstElement(const f32 x, const f32 y, const f32 z);
	virtual void 					SetAnchorVsSecondElement(const f32 x, const f32 y, const f32 z);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void 					SetAxisDir(const f32 x, const f32 y, const f32 z, const s32 axis_num);
	virtual void 					SetAxisDirVsFirstElement(const f32 x, const f32 y, const f32 z, const s32 axis_num);
	virtual void 					SetAxisDirVsSecondElement(const f32 x, const f32 y, const f32 z, const s32 axis_num);

public:
	virtual CPhysicsElement*		PFirst_element( );
	virtual CPhysicsElement*		PSecond_element( );
	virtual u16						BoneID( )
	{
		return m_bone_id;
	}
	virtual void					SetBoneID(u16 bone_id)
	{
		m_bone_id = bone_id;
	}
	IC CPHElement*					PFirstElement( )
	{
		return pFirst_element;
	}
	IC CPHElement*					PSecondElement( )
	{
		return pSecond_element;
	}
	virtual void 					Activate( );
	virtual void 					Create( );
	virtual void 					RunSimulation( );
	virtual void 					SetBackRef(CPhysicsJoint** j);
	virtual void 					SetForceAndVelocity(const f32 force, const f32 velocity = 0.0f, const s32 axis_num = -1);
	virtual void 					SetForce(const f32 force, const s32 axis_num = -1);
	virtual void 					SetVelocity(const f32 velocity = 0.0f, const s32 axis_num = -1);
	virtual void 					SetBreakable(f32 force, f32 torque);
	virtual bool					isBreakable( )
	{
		return !!m_destroy_info;
	}
	virtual dJointID 				GetDJoint( )
	{
		return m_joint;
	}
	virtual dJointID 				GetDJoint1( )
	{
		return m_joint1;
	}
	virtual void 					GetLimits(f32& lo_limit, f32& hi_limit, s32 axis_num);
	virtual void 					GetAxisDir(s32 num, fVector3& axis, eVs& vs);
	virtual void 					GetAxisDirDynamic(s32 num, fVector3& axis);
	virtual void 					GetAnchorDynamic(fVector3& anchor);
	virtual void 					GetAxisSDfactors(f32& spring_factor, f32& damping_factor, s32 axis_num);
	virtual void 					GetJointSDfactors(f32& spring_factor, f32& damping_factor);
	virtual void					GetMaxForceAndVelocity(f32& force, f32& velocity, s32 axis_num);
	virtual f32						GetAxisAngle(s32 axis_num);
	virtual void 					Deactivate( );
	void 							ReattachFirstElement(CPHElement* new_element);
	CODEGeom*& RootGeom( )
	{
		return pFirstGeom;
	}
	virtual CPHJointDestroyInfo* JointDestroyInfo( )
	{
		return m_destroy_info;
	}
	CPHJoint(CPhysicsJoint::enumType type, CPhysicsElement* first, CPhysicsElement* second);
	virtual								~CPHJoint( );
	void					SetShell(CPHShell* p);

	void					ClearDestroyInfo( );
};

///////////////////////////////////////////////////////////////////////////////////////////////////
IC void own_axis(const fMatrix4x4& m, fVector3& axis)
{
	if (m._11 == 1.0f)
	{
		axis.set(1.0f, 0.0f, 0.0f);
		return;
	}

	f32 k = m._13 * m._21 - m._11 * m._23 + m._23;

	if (k == 0.0f)
	{
		if (m._13 == 0.0f)
		{
			axis.set(0.0f, 0.0f, 1.0f);
			return;
		}

		f32 k1 = m._13 / (1.0f - m._11);
		axis.z = _sqrt(1.0f / (1.0f + k1 * k1));
		axis.x = axis.z * k1;
		axis.y = 0.0f;
		return;
	}

	f32 k_zy = -(m._12 * m._21 - m._11 * m._22 + m._11 + m._22 - 1.f) / k;
	f32 k_xy = (m._12 + m._13 * k_zy) / (1.f - m._11);
	axis.y = _sqrt(1.f / (k_zy * k_zy + k_xy * k_xy + 1.f));
	axis.x = axis.y * k_xy;
	axis.z = axis.y * k_zy;
	return;
}

IC void own_axis_angle(const fMatrix4x4& m, fVector3& axis, f32& angle)
{
	own_axis(m, axis);
	fVector3 ort1;
	fVector3 ort2;
	if (!(axis.z == 0.0f && axis.y == 0.0f))
	{
		ort1.set(0.0f, -axis.z, axis.y);
		ort2.crossproduct(axis, ort1);
	}
	else
	{
		ort1.set(0.0f, 1.0f, 0.0f);
		ort2.crossproduct(axis, ort1);
	}

	ort1.normalize( );
	ort2.normalize( );

	fVector3 ort1_t;
	m.transform_dir(ort1_t, ort1);

	f32 cosinus = ort1.dotproduct(ort1_t);
	f32 sinus = ort2.dotproduct(ort1_t);
	angle = acosf(cosinus);
	if (sinus < 0.0f)
	{
		angle = -angle;
	}
}

IC void axis_angleB(const fMatrix4x4& m, const fVector3& axis, f32& angle)
{
	fVector3 ort1;
	fVector3 ort2;
	if (!(fis_zero(axis.z) && fis_zero(axis.y)))
	{
		ort1.set(0.0f, -axis.z, axis.y);
		ort2.crossproduct(axis, ort1);
	}
	else
	{
		ort1.set(0.0f, 1.0f, 0.0f);
		ort2.crossproduct(axis, ort1);
	}

	ort1.normalize( );
	ort2.normalize( );
	fVector3 ort1_t;
	m.transform_dir(ort1_t, ort1);
	fVector3 ort_r;
	f32 pr1;
	f32 pr2;
	pr1 = ort1.dotproduct(ort1_t);
	pr2 = ort2.dotproduct(ort1_t);
	if (pr1 == 0.0f && pr2 == 0.0f)
	{
		angle = 0.0f;
		return;
	}

	ort_r.set(pr1 * ort1.x + pr2 * ort2.x, pr1 * ort1.y + pr2 * ort2.y, pr1 * ort1.z + pr2 * ort2.z);

	ort_r.normalize( );
	f32 cosinus = ort1.dotproduct(ort_r);
	f32 sinus = ort2.dotproduct(ort_r);
	angle = acosf(cosinus);
	if (sinus < 0.0f)
	{
		angle = -angle;
	}
}

IC void axis_angleA(const fMatrix4x4& m, const fVector3& axis, f32& angle)
{
	fVector3 ort1;
	fVector3 ort2;
	fVector3 axis_t;
	m.transform_dir(axis_t, axis);
	if (!(fis_zero(axis_t.z) && fis_zero(axis_t.y)))
	{
		ort1.set(0.0f, -axis_t.z, axis_t.y);
		ort2.crossproduct(axis_t, ort1);
	}
	else
	{
		ort1.set(0.0f, 1.0f, 0.0f);
		ort2.crossproduct(axis_t, ort1);
	}

	ort1.normalize( );
	ort2.normalize( );
	fVector3 ort1_t;
	m.transform_dir(ort1_t, ort1);
	fVector3 ort_r;
	f32 pr1;
	f32 pr2;
	pr1 = ort1.dotproduct(ort1_t);
	pr2 = ort2.dotproduct(ort1_t);
	if (pr1 == 0.0f && pr2 == 0.0f)
	{
		angle = 0.0f;
		return;
	}

	ort_r.set(pr1 * ort1.x + pr2 * ort2.x, pr1 * ort1.y + pr2 * ort2.y, pr1 * ort1.z + pr2 * ort2.z);

	ort_r.normalize( );
	f32 cosinus = ort1.dotproduct(ort_r);
	f32 sinus = ort2.dotproduct(ort_r);
	angle = acosf(cosinus);
	if (sinus < 0.0f)
	{
		angle = -angle;
	}
	//if(angle>M_PI) angle=angle-2.f*M_PI;
	//if(angle<-M_PI) angle=angle+2.f*M_PI;
}

#pragma once

class CPHElement;
class CPHJoint;
class CPhysicsShell;
//class CPHFracture;
//class CShellSplitInfo;

#include "ode_include.h"

inline void sub_diapasones(U16& from1, U16& to1, const U16& from0, const U16& to0)
{
	if (from0 == to0 || from1 == to1 || to1 <= from0 || to1 == U16(-1))
	{
		return;
	}

	R_ASSERT(from0 >= from1 && to0 <= to1);
	U16 dip = to0 - from0;
	to1 = to1 - dip;
}

class CShellSplitInfo
{
	friend class  CPHFracturesHolder;
	friend class  CPHShellSplitterHolder;
	friend class CPHElement;
	inline bool HaveElements( )
	{
		return m_end_el_num != m_start_el_num;
	}
	inline bool HaveJoints( )
	{
		return m_start_jt_num != m_end_jt_num;
	}

public:
	inline void sub_diapasone(const CShellSplitInfo& sub)
	{
		sub_diapasones(m_start_el_num, m_end_el_num, sub.m_start_el_num, sub.m_end_el_num);
		sub_diapasones(m_start_jt_num, m_end_jt_num, sub.m_start_jt_num, sub.m_end_jt_num);
	}

protected:
	U16				m_start_el_num;
	U16				m_end_el_num;
	U16				m_start_jt_num;
	U16				m_end_jt_num;
	U16				m_start_geom_num;
	U16				m_end_geom_num;
	U16				m_bone_id;
};

class CPHFracture : public CShellSplitInfo
{
	friend class  CPHFracturesHolder;
	friend class CPHElement;
	friend class CPHShell;
	bool			m_breaked;
	dMass			m_firstM;
	dMass			m_secondM;
	//when breaked m_pos_in_element-additional force m_break_force-additional torque -x additional torque-y add_torque_z - additional torque z
	float			m_break_force;
	float			m_break_torque;
	Fvector3			m_pos_in_element;
	float			m_add_torque_z;
	CPHFracture( );

public:
	bool			Update(CPHElement* element);
	inline bool			Breaked( )
	{
		return m_breaked;
	}
	void			SetMassParts(const dMass& first, const dMass& second);
	void			MassSetZerro( );
	void			MassAddToFirst(const dMass& m);
	void			MassAddToSecond(const dMass& m);
	void			MassSubFromFirst(const dMass& m);
	void			MassSubFromSecond(const dMass& m);
	void			MassSetFirst(const dMass& m);
	void			MassSetSecond(const dMass& m);
	const dMass& MassFirst( )
	{
		return m_firstM;
	}
	const dMass& MassSecond( )
	{
		return m_secondM;
	}
	void			MassUnsplitFromFirstToSecond(const dMass& m);
};


using shell_root = std::pair<CPhysicsShell*, U16>;
using element_fracture = std::pair<CPHElement*, CShellSplitInfo>;

//DEFINE_VECTOR(CPHElement*, ELEMENT_STORAGE, ELEMENT_I)
using ELEMENT_STORAGE = xr_vector<CPHElement*>;
using ELEMENT_I = ELEMENT_STORAGE::iterator;
using ELEMENT_RI = ELEMENT_STORAGE::reverse_iterator;

//DEFINE_VECTOR(CPHJoint*, JOINT_STORAGE, JOINT_I)
using JOINT_STORAGE = xr_vector<CPHJoint*>;
using JOINT_I = JOINT_STORAGE::iterator;

//DEFINE_VECTOR(shell_root, PHSHELL_PAIR_VECTOR, SHELL_PAIR_I)
using PHSHELL_PAIR_VECTOR = xr_vector<shell_root>;
using SHELL_PAIR_I = PHSHELL_PAIR_VECTOR::iterator;
using SHELL_PAIR_RI = PHSHELL_PAIR_VECTOR::reverse_iterator;

//DEFINE_VECTOR(element_fracture, ELEMENT_PAIR_VECTOR, ELEMENT_PAIR_I)
using ELEMENT_PAIR_VECTOR = xr_vector<element_fracture>;
using ELEMENT_PAIR_I = ELEMENT_PAIR_VECTOR::iterator;
using ELEMENT_PAIR_RI = ELEMENT_PAIR_VECTOR::reverse_iterator;

//DEFINE_VECTOR(CPHFracture, FRACTURE_STORAGE, FRACTURE_I)
using FRACTURE_STORAGE = xr_vector<CPHFracture>;
using FRACTURE_I = FRACTURE_STORAGE::iterator;
using FRACTURE_RI = FRACTURE_STORAGE::reverse_iterator;

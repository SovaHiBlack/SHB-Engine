#pragma once

#include "PHDefs.h"
#include "PHImpact.h"

class CPHFracture;
class CPHElement;

//DEFINE_VECTOR(dJointFeedback,CFEEDBACK_STORAGE,CFEEDBACK_I)
using CFEEDBACK_STORAGE = xr_vector<dJointFeedback>;
using CFEEDBACK_I = CFEEDBACK_STORAGE::iterator;

class CPHFracturesHolder 			//stored in CPHElement
{
friend class CPHElement;
friend class CPHShellSplitterHolder;
bool			 m_has_breaks;

FRACTURE_STORAGE m_fractures;
PH_IMPACT_STORAGE m_impacts;		//filled in anytime from CPHElement applyImpulseTrace cleared in PhDataUpdate
CFEEDBACK_STORAGE m_feedbacks;		//this store feedbacks for non contact joints 
public:
CPHFracturesHolder			();

~CPHFracturesHolder			();
void				DistributeAdditionalMass	(u16 geom_num,const dMass& m);//
void				SubFractureMass				(u16 fracture_num);
void				AddImpact		(const Fvector3& force,const Fvector3& point,u16 id);
PH_IMPACT_STORAGE&	Impacts			(){return m_impacts;}

CPHFracture&		LastFracture	(){return m_fractures.back();}
protected:
private:

u16 				CheckFractured	();										//returns first breaked fracture

element_fracture	SplitFromEnd	(CPHElement* element,u16 geom_num);
void				InitNewElement	(CPHElement* element,const Fmatrix &shift_pivot,float density);
void				PassEndFractures(u16 from,CPHElement* dest);
public:
void				SplitProcess	(CPHElement* element,ELEMENT_PAIR_VECTOR &new_elements);
u16					AddFracture		(const CPHFracture& fracture);
CPHFracture&		Fracture		(u16 num);
void				PhTune			(dBodyID body);										//set feedback for joints called from PhTune of ShellSplitterHolder
bool				PhDataUpdate	(CPHElement* element);										//collect joints and external impacts in fractures Update which set m_fractured; called from PhDataUpdate of ShellSplitterHolder returns true if has breaks
void				ApplyImpactsToElement(CPHElement* element);
};

using shell_root = std::pair<CPhysicsShell*, u16>;
using element_fracture = std::pair<CPHElement*, CShellSplitInfo>;

//DEFINE_VECTOR(CPHElement*,ELEMENT_STORAGE,ELEMENT_I)
using ELEMENT_STORAGE = xr_vector<CPHElement*>;
using ELEMENT_I = ELEMENT_STORAGE::iterator;
using ELEMENT_RI = ELEMENT_STORAGE::reverse_iterator;

//DEFINE_VECTOR(CPHJoint*,JOINT_STORAGE,JOINT_I)
using JOINT_STORAGE = xr_vector<CPHJoint*>;
using JOINT_I = JOINT_STORAGE::iterator;

//DEFINE_VECTOR(shell_root,PHSHELL_PAIR_VECTOR,SHELL_PAIR_I)
using PHSHELL_PAIR_VECTOR = xr_vector<shell_root>;
using SHELL_PAIR_I = PHSHELL_PAIR_VECTOR::iterator;
using SHELL_PAIR_RI = PHSHELL_PAIR_VECTOR::reverse_iterator;

//DEFINE_VECTOR(element_fracture,ELEMENT_PAIR_VECTOR,ELEMENT_PAIR_I)
using ELEMENT_PAIR_VECTOR = xr_vector<element_fracture>;
using ELEMENT_PAIR_I = ELEMENT_PAIR_VECTOR::iterator;
using ELEMENT_PAIR_RI = ELEMENT_PAIR_VECTOR::reverse_iterator;

//DEFINE_VECTOR(CPHFracture,FRACTURE_STORAGE,FRACTURE_I)
using FRACTURE_STORAGE = xr_vector<CPHFracture>;
using FRACTURE_I = FRACTURE_STORAGE::iterator;
using FRACTURE_RI = FRACTURE_STORAGE::reverse_iterator;

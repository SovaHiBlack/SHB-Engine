#pragma once

#include "PHDefs.h"
#include "PHObject.h"

class CPHShellSplitter 
{
	friend	class CPHShellSplitterHolder;
	friend	class CPHShell;

public:
	enum EType				  {splElement,splJoint}										;

private:
	bool						m_breaked												;
	EType						m_type													;
	U16							m_element												;
	U16							m_joint													;
	CPHShellSplitter			(CPHShellSplitter::EType type, U16 element, U16 joint)	;
	CPHShellSplitter			()														;
};

//class CPHShellSplitter;
class CPHShell;
class CCodeGeom;

using id_geom = std::pair<U16, CCodeGeom*>;

//DEFINE_MAP(U16,CCodeGeom*,GEOM_MAP,GEOM_MAP_I)
using GEOM_MAP = xr_map<U16, CCodeGeom*>;
using GEOM_MAP_I = GEOM_MAP::iterator;

//DEFINE_VECTOR(CPHShellSplitter,SPLITTER_STORAGE,SPLITTER_I)
using SPLITTER_STORAGE = xr_vector<CPHShellSplitter>;
using SPLITTER_I = SPLITTER_STORAGE::iterator;
using SPLITTER_RI = SPLITTER_STORAGE::reverse_iterator;

class CPHShellSplitterHolder : public CPHUpdateObject			//call all Fractures and Breakable Joints Updates
{
friend class CPHShell;
bool							m_has_breaks																																;
bool							m_unbreakable																																;
CPHShell*						m_pShell																																	;			//purpose: to extract elements and joints corresponded splitters
SPLITTER_STORAGE				m_splitters																																	;			//
GEOM_MAP						m_geom_root_map																																;			//to find geom pointer by bone id
virtual	void					PhTune						(dReal step)																									;						//call fractures PhTune for element splitters m_pShell->m_elements[m_splitters[i]->m_element]->m_pFracturesHolder->PhTune()
virtual void					PhDataUpdate				(dReal step)																									;				//call fractures PhDataUpdate for element splitters m_pShell->m_elements[m_splitters[i]->m_element]->m_pFracturesHolder->PhDataUpdate()
		bool					CheckSplitter				(U16 aspl)																										;		//
		shell_root				SplitJoint					(U16 aspl)																										;		//create new shell moving into it departed elements and joints 
		shell_root				ElementSingleSplit			(const element_fracture &split_elem,const CPHElement* source_element)											;
		void					SplitElement				(U16 aspl,PHSHELL_PAIR_VECTOR &out_shels)																		;//
		void					PassEndSplitters			(const CShellSplitInfo& spl_inf,CPHShell* dest, U16 jt_add_shift, U16 el_add_shift)								;
		void					InitNewShell				(CPHShell* shell)																								;			//inits new active shell
public:
								CPHShellSplitterHolder		(CPHShell* shell)																								;
virtual							~CPHShellSplitterHolder		()																												;
		void					Activate					()																												;
		void					Deactivate					()																												;
		void					AddSplitter					(CPHShellSplitter::EType type, U16 element, U16 joint)															;
		void					AddSplitter					(CPHShellSplitter::EType type, U16 element, U16 joint, U16 position)												;
		void					SplitProcess				(PHSHELL_PAIR_VECTOR &out_shels)																				;
		void					AddToGeomMap				(const id_geom& id_rootgeom)																					;
		U16						FindRootGeom				(U16 bone_id)																									;
inline      bool					Breaked						()																							{return m_has_breaks;}
inline		bool					isEmpty						()																					{return m_splitters.empty()	;}
		void					SetUnbreakable				()																												;
		void					SetBreakable				()																												;
		bool					IsUnbreakable				()																					{return m_unbreakable		;}					
};

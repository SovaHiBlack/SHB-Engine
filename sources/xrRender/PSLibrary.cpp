//----------------------------------------------------
// file: PSLibrary.cpp
//----------------------------------------------------
#include "stdafx.h"

#include "PSLibrary.h"
#include "ParticleEffect.h"

bool ped_sort_pred	(const PS::CPEDef* a, 	const PS::CPEDef* b)	{	return xr_strcmp(a->Name(),b->Name())<0;}
bool pgd_sort_pred	(const PS::CPGDef* a, 	const PS::CPGDef* b)	{	return xr_strcmp(a->m_Name,b->m_Name)<0;}

bool ped_find_pred	(const PS::CPEDef* a, pcstr b)				{	return xr_strcmp(a->Name(),b)<0;}
bool pgd_find_pred	(const PS::CPGDef* a, pcstr b)				{	return xr_strcmp(a->m_Name,b)<0;}
//----------------------------------------------------
void CPSLibrary::OnCreate()
{
	string_path fn;
    FS.update_path(fn, "$game_data$",PSLIB_FILENAME);
	if (FS.exist(fn)){
    	if (!Load(fn)) Msg("PS Library: Unsupported version.");
    }else{
    	Msg("Can't find file: '%s'",fn);
    }
	for (PS::PEDIt e_it = m_PEDs.begin(); e_it!=m_PEDs.end(); e_it++)
    	(*e_it)->CreateShader();
}
 
void CPSLibrary::OnDestroy()
{
	for (PS::PEDIt e_it = m_PEDs.begin(); e_it!=m_PEDs.end(); e_it++)
    	(*e_it)->DestroyShader();

	for (e_it = m_PEDs.begin(); e_it!=m_PEDs.end(); e_it++)
		xr_delete	(*e_it);
	m_PEDs.clear	();

	for (PS::PGDIt g_it = m_PGDs.begin(); g_it!=m_PGDs.end(); g_it++)
		xr_delete	(*g_it);
	m_PGDs.clear	();
}
//----------------------------------------------------
PS::PEDIt CPSLibrary::FindPEDIt(pcstr Name)
{
	if (!Name) return m_PEDs.end();

	PS::PEDIt I = std::lower_bound(m_PEDs.begin(),m_PEDs.end(),Name,ped_find_pred);
	if (I==m_PEDs.end() || (0!=xr_strcmp((*I)->m_Name,Name)))	return m_PEDs.end();
	else														return I;
}

PS::CPEDef* CPSLibrary::FindPED(pcstr Name)
{
	PS::PEDIt it = FindPEDIt(Name);
    return (it==m_PEDs.end())?0:*it;
}

PS::PGDIt CPSLibrary::FindPGDIt(pcstr Name)
{
	if (!Name) return m_PGDs.end();

	PS::PGDIt I = std::lower_bound(m_PGDs.begin(),m_PGDs.end(),Name,pgd_find_pred);
	if (I==m_PGDs.end() || (0!=xr_strcmp((*I)->m_Name,Name)))	return m_PGDs.end();
	else														return I;
}

PS::CPGDef* CPSLibrary::FindPGD(pcstr Name)
{
	PS::PGDIt it = FindPGDIt(Name);
    return (it==m_PGDs.end())?0:*it;
}

void CPSLibrary::RenamePED(PS::CPEDef* src, pcstr new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::RenamePGD(PS::CPGDef* src, pcstr new_name)
{
	R_ASSERT(src&&new_name&&new_name[0]);
	src->SetName(new_name);
}

void CPSLibrary::Remove(pcstr nm)
{
	PS::PEDIt it = FindPEDIt(nm);
	if (it!=m_PEDs.end()){
		(*it)->DestroyShader();
		xr_delete		(*it);
		m_PEDs.erase	(it);
	}else{
		PS::PGDIt it = FindPGDIt(nm);
		if (it!=m_PGDs.end()){
			xr_delete	(*it);
			m_PGDs.erase(it);
		}
	}
}
//----------------------------------------------------

bool CPSLibrary::Load(pcstr nm)
{
	IReader*	F			= FS.r_open(nm);
	bool bRes 				= true;
    R_ASSERT(F->find_chunk(PS_CHUNK_VERSION));
    u16 ver					= F->r_u16();
    if (ver!=PS_VERSION) return false;
    // second generation
    IReader* OBJ;
    OBJ			 			= F->open_chunk(PS_CHUNK_SECONDGEN);
    if (OBJ){
        IReader* O   		= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            PS::CPEDef*	def	= xr_new<PS::CPEDef>();
            if (def->Load(*O)) m_PEDs.push_back(def);
            else{ bRes = false; xr_delete(def); }
            O->close();
            if (!bRes)	break;
            O 			= OBJ->open_chunk(count);
        }
        OBJ->close();
    }
    // second generation
    OBJ 					= F->open_chunk(PS_CHUNK_THIRDGEN);
    if (OBJ){
        IReader* O   		= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            PS::CPGDef*	def	= xr_new<PS::CPGDef>();
            if (def->Load(*O)) m_PGDs.push_back(def);
            else{ bRes = false; xr_delete(def); }
            O->close();
            if (!bRes) break;
            O 			= OBJ->open_chunk(count);
        }
        OBJ->close();
    }

    // final
	FS.r_close			(F);

	std::sort			(m_PEDs.begin(),m_PEDs.end(),ped_sort_pred);
	std::sort			(m_PGDs.begin(),m_PGDs.end(),pgd_sort_pred);
   
    return bRes;
}
//----------------------------------------------------
void CPSLibrary::Reload()
{
	OnDestroy();
    OnCreate();
	Msg( "PS Library was succesfully reloaded." );
}
//----------------------------------------------------


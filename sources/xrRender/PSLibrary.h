//----------------------------------------------------
// file: PSLibrary.h
//----------------------------------------------------
#ifndef PSLibraryH
#define PSLibraryH

#include "ParticleEffect.h"
#include "ParticleGroup.h"

class CPSLibrary	{
    PS::PEDVec			m_PEDs;
    PS::PGDVec			m_PGDs;

public:
    bool 				Load			(pcstr nm);
    bool				Save			(pcstr nm);

public:
						CPSLibrary		(){;}
    		 			~CPSLibrary		(){;}

    void				OnCreate		();
    void				OnDestroy		();

    PS::CPEDef*			FindPED			(pcstr name);
    PS::PEDIt			FindPEDIt		(pcstr name);
    PS::CPGDef*			FindPGD			(pcstr name);
    PS::PGDIt			FindPGDIt		(pcstr name);

    // get object properties methods
    IC PS::PEDIt		FirstPED		()	{return m_PEDs.begin();}
    IC PS::PEDIt		LastPED			()	{return m_PEDs.end();}
    IC PS::PGDIt		FirstPGD		()	{return m_PGDs.begin();}
    IC PS::PGDIt		LastPGD			()	{return m_PGDs.end();}

    PS::CPEDef*			AppendPED		(PS::CPEDef* src=0);
    PS::CPGDef*			AppendPGD		(PS::CPGDef* src=0);
    void				Remove			(pcstr name);
    void				RenamePED		(PS::CPEDef* src, pcstr new_name);
    void				RenamePGD		(PS::CPGDef* src, pcstr new_name);

    void				Reload			();
    bool				Save			();
};

#define PSLIB_FILENAME 			"particles.xr"

#define PS_LIB_SIGN 			"PS_LIB"

#define PS_VERSION				0x0001
//----------------------------------------------------
#define PS_CHUNK_VERSION		0x0001
#define PS_CHUNK_FIRSTGEN		0x0002
#define PS_CHUNK_SECONDGEN		0x0003
#define PS_CHUNK_THIRDGEN		0x0004

#endif /*_INCDEF_PSLibrary_H_*/


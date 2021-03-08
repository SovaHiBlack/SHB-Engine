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
	bool 				Load			(const char* nm);
	bool				Save			(const char* nm);

public:
						CPSLibrary		(){;}
						~CPSLibrary		(){;}

	void				OnCreate		();
	void				OnDestroy		();

	PS::CPEDef*			FindPED			(const char* name);
	PS::PEDIt			FindPEDIt		(const char* name);
	PS::CPGDef*			FindPGD			(const char* name);
	PS::PGDIt			FindPGDIt		(const char* name);

	// get object properties methods
	inline PS::PEDIt		FirstPED		()	{return m_PEDs.begin();}
	inline PS::PEDIt		LastPED			()	{return m_PEDs.end();}
	inline PS::PGDIt		FirstPGD		()	{return m_PGDs.begin();}
	inline PS::PGDIt		LastPGD			()	{return m_PGDs.end();}

	PS::CPEDef*			AppendPED		(PS::CPEDef* src=0);
	PS::CPGDef*			AppendPGD		(PS::CPGDef* src=0);
	void				Remove			(const char* name);
	void				RenamePED		(PS::CPEDef* src, const char* new_name);
	void				RenamePGD		(PS::CPGDef* src, const char* new_name);

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


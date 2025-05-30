#include "stdafx.h"
//#include "igame_level.h"
#include "IGame_Persistent.h"
#include "igame_objectpool.h"
#include "Object.h"

IGame_ObjectPool::IGame_ObjectPool( )
{ }

IGame_ObjectPool::~IGame_ObjectPool( )
{
	R_ASSERT(m_PrefetchObjects.empty( ));
}

void IGame_ObjectPool::prefetch( )
{
	R_ASSERT(m_PrefetchObjects.empty( ));

	s32 p_count = 0;
	::Render->model_Logging(FALSE);

	string256 section;
	// prefetch objects
	strconcat(sizeof(section), section, "prefetch_objects_", g_pGamePersistent->m_game_params.m_game_type);
	CIniFile::Sect& sect = pSettings->r_section(section);
	for (CIniFile::SectCIt I = sect.Data.begin( ); I != sect.Data.end( ); I++)
	{
		const CIniFile::Item& item = *I;
		CLASS_ID CLS = pSettings->r_clsid(item.first.c_str( ), "class");
		p_count++;
		CObject* pObject = (CObject*)NEW_INSTANCE(CLS);
		pObject->Load(item.first.c_str( ));
		VERIFY2(pObject->cNameSect( ).c_str( ), item.first.c_str( ));
		m_PrefetchObjects.push_back(pObject);
	}

	// out statistic
	::Render->model_Logging(TRUE);
}

void IGame_ObjectPool::clear( )
{
	// Clear POOL
	ObjectVecIt it = m_PrefetchObjects.begin( );
	ObjectVecIt itE = m_PrefetchObjects.end( );
	for (; it != itE; it++)
	{
		xr_delete(*it);
	}

	m_PrefetchObjects.clear( );
}

CObject* IGame_ObjectPool::create(pcstr	name)
{
	CLASS_ID CLS = pSettings->r_clsid(name, "class");
	CObject* O = (CObject*)NEW_INSTANCE(CLS);
	O->Load(name);
	return				O;
}

void		IGame_ObjectPool::destroy(CObject* O)
{
	xr_delete(O);
}

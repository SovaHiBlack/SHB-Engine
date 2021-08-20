#include "stdafx.h"

//#include "IGameLevel.h"
#include "IGamePersistent.h"//==>
#include "IGameObjectPool.h"
#include "Object.h"

IGameObjectPool::IGameObjectPool( )
{ }

IGameObjectPool::~IGameObjectPool( )
{
	R_ASSERT(m_PrefetchObjects.empty( ));
}

void IGameObjectPool::prefetch( )
{
	R_ASSERT(m_PrefetchObjects.empty( ));

	int p_count = 0;
	::Render->model_Logging(FALSE);

	char section[256];
	// prefetch objects
	strconcat(sizeof(section), section, "prefetch_objects_", g_pGamePersistent->m_game_params.m_game_type);
	CIniFile::Sect& sect = pSettings->r_section(section);
	for (CIniFile::SectCIt I = sect.Data.begin( ); I != sect.Data.end( ); I++)
	{
		const CIniFile::Item& item = *I;
		CLASS_ID CLS = pSettings->r_clsid(item.first.c_str( ), "class");
		p_count++;
		CObject* pObject = (CObject*) NEW_INSTANCE(CLS);
		pObject->Load(item.first.c_str( ));
		VERIFY2(pObject->cNameSect( ).c_str( ), item.first.c_str( ));
		m_PrefetchObjects.push_back(pObject);
	}

	// out statistic
	::Render->model_Logging(TRUE);
}

void IGameObjectPool::clear( )
{
	// Clear POOL
	ObjectVec_it it = m_PrefetchObjects.begin( );
	ObjectVec_it itE = m_PrefetchObjects.end( );
	for (; it != itE; it++)
	{
		xr_delete(*it);
	}

	m_PrefetchObjects.clear( );
}

CObject* IGameObjectPool::create(const char* name)
{
	CLASS_ID CLS = pSettings->r_clsid(name, "class");
	CObject* O = (CObject*) NEW_INSTANCE(CLS);
	O->Load(name);
	return O;
}

void IGameObjectPool::destroy(CObject* O)
{
	xr_delete(O);
}

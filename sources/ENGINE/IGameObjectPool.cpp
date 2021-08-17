#include "stdafx.h"

//#include "IGameLevel.h"
#include "IGamePersistent.h"//==>
#include "IGameObjectPool.h"
#include "Object.h"

IGameObjectPool::IGameObjectPool(void)
{
}

IGameObjectPool::~IGameObjectPool(void)
{
	R_ASSERT			(m_PrefetchObjects.empty());
}

void IGameObjectPool::prefetch	()
{
	R_ASSERT			(m_PrefetchObjects.empty());

	int	p_count			= 0;
	::Render->model_Logging	(FALSE);

	string256				section;
	// prefetch objects
	strconcat				(sizeof(section),section,"prefetch_objects_",g_pGamePersistent->m_game_params.m_game_type);
	CIniFile::Sect& sect	= pSettings->r_section(section);
	for (CIniFile::SectCIt I=sect.Data.begin(); I!=sect.Data.end(); I++)	{
		const CIniFile::Item& item= *I;
		CLASS_ID CLS		= pSettings->r_clsid(item.first.c_str(),"class");
		p_count				++;
		CObject* pObject	= (CObject*) NEW_INSTANCE(CLS);
		pObject->Load		(item.first.c_str());
		VERIFY2				(pObject->cNameSect().c_str(),item.first.c_str());
		m_PrefetchObjects.push_back	(pObject);
	}

	// out statistic
	::Render->model_Logging	(TRUE);
}

void IGameObjectPool::clear()
{
	// Clear POOL
	ObjectVecIt it			= m_PrefetchObjects.begin();
	ObjectVecIt itE			= m_PrefetchObjects.end();
	for (; it!=itE; it++)	
		xr_delete			(*it);
	m_PrefetchObjects.clear	(); 
}

CObject* IGameObjectPool::create			(const char* name	)
{
	CLASS_ID CLS		=	pSettings->r_clsid		(name,"class");
	CObject* O			=	(CObject*) NEW_INSTANCE	(CLS);
	O->Load				(name);
	return				O;
}

void		IGameObjectPool::destroy			( CObject*	O		)
{
	xr_delete				(O);
}

/*
void IGameObjectPool::prefetch	()
{
	R_ASSERT			(map_POOL.empty());

	u32	mem_0			= Memory.mem_usage();
	float	p_time		= 1000.f*Device.GetTimerGlobal()->GetElapsed_sec();
	int	p_count			= 0;
	::Render->model_Logging	(FALSE);

	string256 section;
	// prefetch objects
	strconcat				(section,"prefetch_objects_",g_pGamePersistent->m_game_params.m_game_type);
	CIniFile::Sect& sect	= pSettings->r_section(section);
	for (CIniFile::SectIt I=sect.begin(); I!=sect.end(); I++)	{
		CIniFile::Item& item= *I;
		CLASS_ID CLS		= pSettings->r_clsid(item.first.c_str(),"class");
		int count			= atoi(item.second.c_str());
		count				+= (count==0)?1:0;
		R_ASSERT2			((count>0) && (count<=128), "Too many objects for prefetching");
		p_count				+=	count;
		for (int c=0; c<count; c++){
			CObject* pObject= (CObject*) NEW_INSTANCE(CLS);
			pObject->Load	(item.first.c_str());
			VERIFY2			(pObject->cNameSect().c_str(),item.first.c_str());
			map_POOL.insert	(mk_pair(pObject->cNameSect(),pObject));
		}
	}

	// out statistic
	::Render->model_Logging	(TRUE);
	p_time = 1000.f*Device.GetTimerGlobal()->GetElapsed_sec() - p_time;
	u32		p_mem		= Memory.mem_usage() - mem_0;
	if (p_count){
		float 	a_time		= float(p_time)/float(p_count);
		Msg					("* [Object-prefetch] objects: %d",		p_count);
		Msg					("* [Object-prefetch] time:    %d ms",	iFloor(p_time));
		Msg					("* [Object-prefetch] memory:  %dKb",	p_mem/1024);
		Msg					("* [Object-prefetch] average: %2.2f ms, %d bytes", a_time, p_mem/p_count);
	}
}

void IGameObjectPool::clear	( )
{
	// Clear POOL
	for (POOL_IT it=map_POOL.begin(); it!=map_POOL.end(); it++)	
		xr_delete	(it->second);
	map_POOL.clear(); 
}

CObject*	IGameObjectPool::create			( const char*	name	)
{
	string256			l_name;
	POOL_IT	it			=	map_POOL.find	(CSharedString(strlwr(strcpy_s(l_name,name))));
	if (it!=map_POOL.end())
	{
		// Instance found
		CObject* O			=	it->second;
		map_POOL.erase		(it);
		return				O;
	} else {
		// Create and load _new instance
		// Msg				("! Uncached loading '%s'...",name);
		CLASS_ID CLS		=	pSettings->r_clsid		(name,"class");
		CObject* O			=	(CObject*) NEW_INSTANCE	(CLS);
		O->Load				(name);
		return				O;
	}
}

void		IGameObjectPool::destroy			( CObject*	O		)
{
	map_POOL.insert			(mk_pair(O->cNameSect(),O));
}
*/
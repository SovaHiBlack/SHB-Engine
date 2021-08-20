#pragma once

class ENGINE_API CObject;

class ENGINE_API IGameObjectPool
{
	using ObjectVec					= xr_vector<CObject*>;
	using ObjectVec_it				= ObjectVec::iterator;

	ObjectVec						m_PrefetchObjects;

public:
	void		prefetch			( );
	void		clear				( );

	CObject*	create				(const char* name);
	void		destroy				(CObject* O);

				IGameObjectPool		( );
	virtual		~IGameObjectPool	( );
};

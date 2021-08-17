#pragma once

// refs
class ENGINE_API				CObject;

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API 				IGameObjectPool
{
/*
private:
	struct str_pred : public std::binary_function<CSharedString&, CSharedString&, bool> 
	{	
		inline bool operator()(const CSharedString& x, const CSharedString& y) const
		{	return xr_strcmp(x,y)<0;	}
	};
	typedef xr_multimap<CSharedString,CObject*,str_pred>	POOL;
	typedef POOL::iterator							POOL_IT;
private:
	POOL						map_POOL;
*/
	typedef xr_vector<CObject*>	ObjectVec;
	typedef ObjectVec::iterator	ObjectVecIt;
	ObjectVec					m_PrefetchObjects;
public:
	void						prefetch			( );
	void						clear				( );

	CObject*					create				(const char* name	);
	void						destroy				( CObject*	O		);

	IGameObjectPool();
	virtual ~IGameObjectPool();
};
//-----------------------------------------------------------------------------------------------------------

#pragma once

// refs
class ENGINE_API CObject;
class CNetPacket;

class ENGINE_API CObjectList
{
private:
	// data
	xr_map<u32, CObject*>		map_NETID;
	xr_vector<CObject*>			destroy_queue;
	xr_vector<CObject*>			objects_active;
	xr_vector<CObject*>			objects_sleeping;

	xr_vector<CObject*>			crows_0;
	xr_vector<CObject*>			crows_1;
	xr_vector<CObject*>* crows;

	CObject** objects_dup;
	u32							objects_dup_memsz;

public:
	typedef fastdelegate::FastDelegate1<CObject*>	RELCASE_CALLBACK;
	struct SRelcasePair
	{
		s32* m_ID;
		RELCASE_CALLBACK		m_Callback;
		SRelcasePair(s32* id, RELCASE_CALLBACK cb) :m_ID(id), m_Callback(cb)
		{ }
		bool	operator==			(RELCASE_CALLBACK cb)
		{
			return m_Callback == cb;
		}
	};
	typedef xr_vector<SRelcasePair>					RELCASE_CALLBACK_VEC;
	RELCASE_CALLBACK_VEC							m_relcase_callbacks;

	void						relcase_register(RELCASE_CALLBACK, s32*);
	void						relcase_unregister(s32*);

public:
	// methods
	CObjectList( );
	~CObjectList( );

	CObject* FindObjectByName(shared_str name);
	CObject* FindObjectByName(pcstr name);
	CObject* FindObjectByCLS_ID(CLASS_ID cls);

	void						Load( );
	void						Unload( );

	CObject* Create(pcstr		name);
	void						Destroy(CObject* O);

	void						SingleUpdate(CObject* O);
	void						Update(bool bForce);

	void						net_Register(CObject* O);
	void						net_Unregister(CObject* O);

	u32							net_Export(CNetPacket* P, u32 _start, u32 _count);	// return next start
	void						net_Import(CNetPacket* P);
	CObject* net_Find(u32 ID);

	void						o_crow(CObject* O)
	{
		crows->push_back(O);
	}

	void						o_remove(xr_vector<CObject*>& v, CObject* O);
	void						o_activate(CObject* O);
	void						o_sleep(CObject* O);
	IC u32						o_count( )
	{
		return objects_active.size( ) + objects_sleeping.size( );
	}
	IC CObject* o_get_by_iterator(u32 _it)
	{
		if (_it < objects_active.size( ))
		{
			return objects_active[_it];
		}
		else
		{
			return objects_sleeping[_it - objects_active.size( )];
		}
	}
	bool						dump_all_objects( );

public:
	void				register_object_to_destroy(CObject* object_to_destroy);

#ifdef DEBUG
	bool				registered_object_to_destroy(const CObject* object_to_destroy) const;
#endif // DEBUG

};

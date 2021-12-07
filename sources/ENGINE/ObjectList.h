#pragma once

// refs
class ENGINE_API CObject;
class CNetPacket;

class ENGINE_API CObjectList
{
public:
	using RELCASE_CALLBACK											= fastdelegate::FastDelegate1<CObject*>;

	struct SRelcasePair
	{
		int* m_ID;
		RELCASE_CALLBACK												m_Callback;
										SRelcasePair					(int* id, RELCASE_CALLBACK cb) : m_ID(id), m_Callback(cb)
		{ }
		bool							operator ==						(RELCASE_CALLBACK cb)
		{
			return m_Callback == cb;
		}
	};

	using RELCASE_CALLBACK_VEC										= xr_vector<SRelcasePair>;
	RELCASE_CALLBACK_VEC											m_relcase_callbacks;

	void							relcase_register				(RELCASE_CALLBACK, int*);
	void							relcase_unregister				(int*);

	// methods
									CObjectList						( );
									~CObjectList					( );

	CObject*						FindObjectByName				(CSharedString name);
	CObject*						FindObjectByName				(const char* name);
	CObject*						FindObjectByCLS_ID				(CLASS_ID cls);

	void							Load							( );
	void							Unload							( );

	CObject*						Create							(const char* name);
	void							Destroy							(CObject* O);

	void							SingleUpdate					(CObject* O);
	void							Update							(bool bForce);

	void							net_Register					(CObject* O);
	void							net_Unregister					(CObject* O);

	unsigned int								net_Export						(CNetPacket* P, unsigned int _start, unsigned int _count);	// return next start
	void							net_Import						(CNetPacket* P);
	CObject*						net_Find						(unsigned int ID);

	void							o_crow							(CObject* O)
	{
		crows->push_back(O);
	}

	void							o_remove						(xr_vector<CObject*>& v, CObject* O);
	void							o_activate						(CObject* O);
	void							o_sleep							(CObject* O);
	inline unsigned int						o_count							( )
	{
		return objects_active.size( ) + objects_sleeping.size( );
	}
	inline CObject*					o_get_by_iterator				(unsigned int _it)
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
	bool							dump_all_objects				( );

	void							register_object_to_destroy		(CObject* object_to_destroy);

#ifdef DEBUG
	bool							registered_object_to_destroy	(const CObject* object_to_destroy) const;
#endif // def DEBUG

private:
	// data
	xr_map<unsigned int, CObject*>											map_NETID;
	xr_vector<CObject*>												destroy_queue;
	xr_vector<CObject*>												objects_active;
	xr_vector<CObject*>												objects_sleeping;

	xr_vector<CObject*>												crows_0;
	xr_vector<CObject*>												crows_1;
	xr_vector<CObject*>*											crows;

	CObject**														objects_dup;
	unsigned int																objects_dup_memsz;
};

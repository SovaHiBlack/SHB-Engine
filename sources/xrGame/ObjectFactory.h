////////////////////////////////////////////////////////////////////////////
//	Module 		: ObjectFactory.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "script_export_space.h"
#include "ObjectItemAbstract.h"
#include "xrServer_Objects.h"

class CObjectFactory {
public:
	typedef ObjectFactory::CLIENT_BASE_CLASS			CLIENT_BASE_CLASS;
	typedef ObjectFactory::SERVER_BASE_CLASS			SERVER_BASE_CLASS;

	typedef ObjectFactory::CLIENT_SCRIPT_BASE_CLASS	CLIENT_SCRIPT_BASE_CLASS;
	typedef ObjectFactory::SERVER_SCRIPT_BASE_CLASS	SERVER_SCRIPT_BASE_CLASS;

protected:
	struct CObjectItemPredicate {
		inline	bool					operator()							(const CObjectItemAbstract *item1, const CObjectItemAbstract *item2) const;
		inline	bool					operator()							(const CObjectItemAbstract *item, const CLASS_ID &clsid) const;
	};

	struct CObjectItemPredicateCLSID {
		CLASS_ID					m_clsid;

		inline							CObjectItemPredicateCLSID			(const CLASS_ID &clsid);
		inline	bool					operator()							(const CObjectItemAbstract *item) const;
	};

	struct CObjectItemPredicateScript {
		CSharedString					m_script_clsid_name;

		inline							CObjectItemPredicateScript			(const CSharedString& script_clsid_name);
		inline	bool					operator()							(const CObjectItemAbstract *item) const;
	};

public:
	typedef xr_vector<CObjectItemAbstract*>		OBJECT_ITEM_STORAGE;
	typedef OBJECT_ITEM_STORAGE::iterator		iterator;
	typedef OBJECT_ITEM_STORAGE::const_iterator	const_iterator;

protected:
	mutable OBJECT_ITEM_STORAGE			m_clsids;
	mutable bool						m_actual;

protected:
			void						register_classes				();
	inline		void						add								(CObjectItemAbstract *item);
	inline		const OBJECT_ITEM_STORAGE	&clsids							() const;
	inline		void						actualize						() const;
	template <typename _unknown_type>
	inline		void						add								(const CLASS_ID &clsid, const char* script_clsid);

	template <typename _client_type, typename _server_type>
	inline		void						add								(const CLASS_ID &clsid, const char* script_clsid);
	inline		const CObjectItemAbstract	&item							(const CLASS_ID &clsid) const;

public:
										CObjectFactory					();
	virtual								~CObjectFactory					();
			void						init							();

	inline		CLIENT_BASE_CLASS			*client_object					(const CLASS_ID &clsid) const;
	inline		SERVER_BASE_CLASS			*server_object					(const CLASS_ID &clsid, const char* section) const;

	inline		int							script_clsid					(const CLASS_ID &clsid) const;
			void						register_script					() const;
			void						register_script_class			(const char* client_class, const char* server_class, const char* clsid, const char* script_clsid);
			void						register_script_class			(const char* unknown_class, const char* clsid, const char* script_clsid);
			void						register_script_classes			();
public:
	static void script_register(lua_State*);
};

add_to_type_list(CObjectFactory)
#undef script_type_list
#define script_type_list save_type_list(CObjectFactory)

extern CObjectFactory *g_object_factory;

inline	const CObjectFactory &object_factory();

#include "ObjectFactory_inline.h"

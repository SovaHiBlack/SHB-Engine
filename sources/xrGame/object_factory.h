////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "script_export_space.h"
#include "object_item_abstract.h"
#include "xrServer_Objects.h"

class CObjectFactory {
public:
	typedef ObjectFactory::CLIENT_BASE_CLASS			CLIENT_BASE_CLASS;
	typedef ObjectFactory::SERVER_BASE_CLASS			SERVER_BASE_CLASS;

	typedef ObjectFactory::CLIENT_SCRIPT_BASE_CLASS	CLIENT_SCRIPT_BASE_CLASS;
	typedef ObjectFactory::SERVER_SCRIPT_BASE_CLASS	SERVER_SCRIPT_BASE_CLASS;

protected:
	struct CObjectItemPredicate {
		IC	bool					operator()							(const CObjectItemAbstract *item1, const CObjectItemAbstract *item2) const;
		IC	bool					operator()							(const CObjectItemAbstract *item, const CLASS_ID &clsid) const;
	};

	struct CObjectItemPredicateCLSID {
		CLASS_ID					m_clsid;

		IC							CObjectItemPredicateCLSID			(const CLASS_ID &clsid);
		IC	bool					operator()							(const CObjectItemAbstract *item) const;
	};

	struct CObjectItemPredicateScript {
		shared_str					m_script_clsid_name;

		IC							CObjectItemPredicateScript			(const shared_str &script_clsid_name);
		IC	bool					operator()							(const CObjectItemAbstract *item) const;
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
	IC		void						add								(CObjectItemAbstract *item);
	IC		const OBJECT_ITEM_STORAGE	&clsids							() const;
	IC		void						actualize						() const;
	template <typename _unknown_type>
	IC		void						add								(const CLASS_ID &clsid, pcstr script_clsid);

	template <typename _client_type, typename _server_type>
	IC		void						add								(const CLASS_ID &clsid, pcstr script_clsid);
	IC		const CObjectItemAbstract	&item							(const CLASS_ID &clsid) const;

public:
										CObjectFactory					();
	virtual								~CObjectFactory					();
			void						init							();

	IC		CLIENT_BASE_CLASS			*client_object					(const CLASS_ID &clsid) const;
	IC		SERVER_BASE_CLASS			*server_object					(const CLASS_ID &clsid, pcstr section) const;

	IC		int							script_clsid					(const CLASS_ID &clsid) const;
			void						register_script					() const;
			void						register_script_class			(pcstr client_class, pcstr server_class, pcstr clsid, pcstr script_clsid);
			void						register_script_class			(pcstr unknown_class, pcstr clsid, pcstr script_clsid);
			void						register_script_classes			();
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CObjectFactory)
#undef script_type_list
#define script_type_list save_type_list(CObjectFactory)

extern CObjectFactory *g_object_factory;

IC	const CObjectFactory &object_factory();

#include "object_factory_inline.h"
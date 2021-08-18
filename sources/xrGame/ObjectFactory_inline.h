////////////////////////////////////////////////////////////////////////////
//	Module 		: ObjectFactory_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory inline functions
////////////////////////////////////////////////////////////////////////////
#pragma once

inline	const CObjectFactory &object_factory()
{
	if (!g_object_factory) {
		g_object_factory		= xr_new<CObjectFactory>();
		g_object_factory->init	();
	}
	return						(*g_object_factory);
}

inline	bool CObjectFactory::CObjectItemPredicate::operator()	(const CObjectItemAbstract *item1, const CObjectItemAbstract *item2) const
{
	return				(item1->clsid() < item2->clsid());
}

inline	bool CObjectFactory::CObjectItemPredicate::operator()	(const CObjectItemAbstract *item, const CLASS_ID &clsid) const
{
	return				(item->clsid() < clsid);
}

inline	CObjectFactory::CObjectItemPredicateCLSID::CObjectItemPredicateCLSID	(const CLASS_ID &clsid) :
	m_clsid				(clsid)
{
}

inline	bool CObjectFactory::CObjectItemPredicateCLSID::operator()	(const CObjectItemAbstract *item) const
{
	return				(m_clsid == item->clsid());
}

inline	CObjectFactory::CObjectItemPredicateScript::CObjectItemPredicateScript	(const CSharedString& script_clsid_name) :
	m_script_clsid_name	(script_clsid_name)
{
}

inline	bool CObjectFactory::CObjectItemPredicateScript::operator()	(const CObjectItemAbstract *item) const
{
	return				(m_script_clsid_name == item->script_clsid());
}

inline	const CObjectFactory::OBJECT_ITEM_STORAGE &CObjectFactory::clsids	() const
{
	return				(m_clsids);
}

inline	const CObjectItemAbstract &CObjectFactory::item	(const CLASS_ID &clsid) const
{
	actualize			();
	const_iterator		I = std::lower_bound(clsids().begin(),clsids().end(),clsid,CObjectItemPredicate());
	VERIFY				((I != clsids().end()) && ((*I)->clsid() == clsid));
	return				(**I);
}

inline	void CObjectFactory::add	(CObjectItemAbstract *item)
{
	const_iterator		I;

	I					= std::find_if(clsids().begin(),clsids().end(),CObjectItemPredicateCLSID(item->clsid()));
	VERIFY				(I == clsids().end());
	
	I					= std::find_if(clsids().begin(),clsids().end(),CObjectItemPredicateScript(item->script_clsid()));
	VERIFY				(I == clsids().end());
	
	m_actual			= false;
	m_clsids.push_back	(item);
}

inline	int	CObjectFactory::script_clsid	(const CLASS_ID &clsid) const
{
	actualize			();
	const_iterator		I = std::lower_bound(clsids().begin(),clsids().end(),clsid,CObjectItemPredicate());
	VERIFY				((I != clsids().end()) && ((*I)->clsid() == clsid));
	return				(int(I - clsids().begin()));
}

inline	CObjectFactory::CLIENT_BASE_CLASS *CObjectFactory::client_object	(const CLASS_ID &clsid) const
{
	return				(item(clsid).client_object());
}

inline	CObjectFactory::SERVER_BASE_CLASS *CObjectFactory::server_object	(const CLASS_ID &clsid, const char* section) const
{
	return				(item(clsid).server_object(section));
}

inline	void CObjectFactory::actualize										() const
{
	if (m_actual)
		return;

	m_actual			= true;
	std::sort			(m_clsids.begin(),m_clsids.end(),CObjectItemPredicate());
}

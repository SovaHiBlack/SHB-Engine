////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"

class CSE_Abstract;
class CCustomMonster;
class CGameObject;

namespace Restriction
{
	enum ERestrictorTypes;
};

template <bool add> struct CRestrictionPredicate;

class CRestrictedObject {
	friend struct CRestrictionPredicate<true>;
	friend struct CRestrictionPredicate<false>;
private:
	typedef CGameObject inherited;

private:
	CCustomMonster			*m_object;
	mutable bool			m_applied;
	mutable bool			m_removed;
	bool					m_actual;

protected:
	template <typename P, bool value>
	inline		void			construct_restriction_string	(char* temp_restrictions, const xr_vector<ALife::_OBJECT_ID> &restrictions, CSharedString current_restrictions, const P &p);
	inline		void			add_object_restriction			(ALife::_OBJECT_ID id, const Restriction::ERestrictorTypes &restrictor_type);
	inline		void			remove_object_restriction		(ALife::_OBJECT_ID id, const Restriction::ERestrictorTypes &restrictor_type);

public:
	inline						CRestrictedObject				(CCustomMonster *object);
	virtual					~CRestrictedObject				();
	virtual BOOL			net_Spawn						(CSE_Abstract* data);
	virtual void			net_Destroy						();
			void			add_border						(u32 start_vertex_id, float radius) const;
			void			add_border						(const Fvector3& start_position, const Fvector3& dest_position) const;
			void			add_border						(u32 start_vertex_id, u32 dest_vertex_id) const;
			void			remove_border					() const;
			u32				accessible_nearest				(const Fvector3& position, Fvector3& result) const;
			bool			accessible						(const Fvector3& position) const;
			bool			accessible						(const Fvector3& position, float radius) const;
			bool			accessible						(u32 level_vertex_id) const;
			bool			accessible						(u32 level_vertex_id, float radius) const;
			void			add_restrictions				(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions);
			void			remove_restrictions				(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions);
			void			add_restrictions				(const CSharedString& out_restrictions, const CSharedString& in_restrictions);
			void			remove_restrictions				(const CSharedString& out_restrictions, const CSharedString& in_restrictions);
			void			remove_all_restrictions			(const Restriction::ERestrictorTypes &restrictor_type);
			void			remove_all_restrictions			();
			CSharedString		in_restrictions					() const;
			CSharedString		out_restrictions				() const;
			CSharedString		base_in_restrictions			() const;
			CSharedString		base_out_restrictions			() const;
	inline		bool			applied							() const;
	inline		CCustomMonster	&object							() const;
	inline		bool			actual							() const;
			void			actual							(bool value);

public:
#ifdef DEBUG
	inline		void			initialize						();
#endif

};

#include "restricted_object_inline.h"

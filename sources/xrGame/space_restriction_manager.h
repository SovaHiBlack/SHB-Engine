////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_manager.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "space_restriction_holder.h"
#include "alife_space.h"

namespace RestrictionSpace {
	struct CTimeIntrusiveBase;
};

template <typename _1, typename _2> class intrusive_ptr;

class CSpaceRestriction;

class CSpaceRestrictionManager : public CSpaceRestrictionHolder {
#ifdef DEBUG
	friend class CLevelGraph;
#endif
	struct CClientRestriction;

protected:
	typedef intrusive_ptr<CSpaceRestriction,RestrictionSpace::CTimeIntrusiveBase> CRestrictionPtr;
	typedef xr_map<CSharedString,CSpaceRestriction*>				SPACE_RESTRICTIONS;
	typedef xr_map<ALife::_OBJECT_ID,CClientRestriction>		CLIENT_RESTRICTIONS;

private:

protected:
	SPACE_RESTRICTIONS			m_space_restrictions;
	CLIENT_RESTRICTIONS			*m_clients;

public:
	using CSpaceRestrictionHolder::restriction;

protected:
	inline		void				join_restrictions				(CSharedString& restrictions, CSharedString update);
	inline		void				difference_restrictions			(CSharedString& restrictions, CSharedString update);
	inline		CRestrictionPtr		restriction						(ALife::_OBJECT_ID id);
			CRestrictionPtr		restriction						(CSharedString out_restrictors, CSharedString in_restrictors);
	inline		void				collect_garbage					();
	virtual void				on_default_restrictions_changed	();

public:
								CSpaceRestrictionManager		();
	virtual						~CSpaceRestrictionManager		();
			void				restrict						(ALife::_OBJECT_ID id, CSharedString out_restrictors, CSharedString in_restrictors);
			void				unrestrict						(ALife::_OBJECT_ID id);
			void				add_restrictions				(ALife::_OBJECT_ID id, CSharedString out_restrictions, CSharedString in_restrictions);
			void				remove_restrictions				(ALife::_OBJECT_ID id, CSharedString out_restrictions, CSharedString in_restrictions);
			void				change_restrictions				(ALife::_OBJECT_ID id, CSharedString add_out_restrictions, CSharedString add_in_restrictions, CSharedString remove_out_restrictions, CSharedString remove_in_restrictions);
			void				clear							();

	template <typename T1, typename T2>
	inline		void				add_border						(ALife::_OBJECT_ID id, T1 p1, T2 p2);
			void				remove_border					(ALife::_OBJECT_ID id);
			
			CSharedString			in_restrictions					(ALife::_OBJECT_ID id);
			CSharedString			out_restrictions				(ALife::_OBJECT_ID id);

			CSharedString			base_in_restrictions			(ALife::_OBJECT_ID id);
			CSharedString			base_out_restrictions			(ALife::_OBJECT_ID id);

			bool				accessible						(ALife::_OBJECT_ID id, const Fsphere &sphere);
			bool				accessible						(ALife::_OBJECT_ID id, u32 level_vertex_id, float radius);
			u32					accessible_nearest				(ALife::_OBJECT_ID id, const Fvector3& position, Fvector3& result);

	inline		bool				restriction_presented			(CSharedString restrictions, CSharedString restriction) const;

#ifdef DEBUG
	inline		const SPACE_RESTRICTIONS &restrictions				() const;
#endif
};

#include "space_restriction_manager_inline.h"

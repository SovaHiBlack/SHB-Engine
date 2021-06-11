////////////////////////////////////////////////////////////////////////////
//	Module 		: restriction_space.h
//	Created 	: 30.08.2004
//  Modified 	: 30.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restriction space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace RestrictionSpace {
	struct CTimeIntrusiveBase : public intrusive_base {
		u32			m_last_time_dec;

		inline			CTimeIntrusiveBase	() : m_last_time_dec(0)
		{
		}

		template <typename T>
		inline	void	_release		(T*object)
		{
			m_last_time_dec = Device.dwTimeGlobal;
		}
	};

	enum ERestrictorTypes {
		eDefaultRestrictorTypeNone = U8(0),
		eDefaultRestrictorTypeOut  = U8(1),
		eDefaultRestrictorTypeIn   = U8(2),
		eRestrictorTypeNone		   = U8(3),
		eRestrictorTypeIn		   = U8(4),
		eRestrictorTypeOut		   = U8(5)
	};
};


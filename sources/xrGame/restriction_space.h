//	Module 		: Restriction_space.h
//	Description : Restriction space

#pragma once

namespace Restriction
{
	struct CTimeIntrusiveBase : public intrusive_base
	{
		u32			m_last_time_dec;

		inline			CTimeIntrusiveBase( ) : m_last_time_dec(0)
		{ }

		template <typename T>
		inline void	_release(T* object)
		{
			m_last_time_dec = Device.dwTimeGlobal;
		}
	};

	enum ERestrictorTypes
	{
		eDefaultRestrictorTypeNone = unsigned char(0),
		eDefaultRestrictorTypeOut = unsigned char(1),
		eDefaultRestrictorTypeIn = unsigned char(2),
		eRestrictorTypeNone = unsigned char(3),
		eRestrictorTypeIn = unsigned char(4),
		eRestrictorTypeOut = unsigned char(5)
	};
};


#pragma once

template <class T>
class _flags
{
public:
	using TYPE						= T;
	using Self						= _flags<TYPE>;
	using SelfRef					= Self&;
	using SelfCRef					= const Self&;

	TYPE							flags;

	inline TYPE			get			( )
	{
		return flags;
	}
	inline SelfRef		zero		( )
	{
		flags = TYPE(0);
		return *this;
	}
	inline SelfRef		one			( )
	{
		flags = TYPE(-1);
		return *this;
	}
	inline SelfRef		invert		( )
	{
		flags = ~flags;
		return *this;
	}
	inline SelfRef		invert		(SelfCRef f)
	{
		flags = ~f.flags;
		return *this;
	}
	inline SelfRef		invert		(const TYPE mask)
	{
		flags ^= mask;
		return *this;
	}
	inline SelfRef		assign		(SelfCRef f)
	{
		flags = f.flags;
		return *this;
	}
	inline SelfRef		assign		(const TYPE mask)
	{
		flags = mask;
		return *this;
	}
	inline SelfRef		set			(const TYPE mask, BOOL value)
	{
		if (value)
		{
			flags |= mask;
		}
		else
		{
			flags &= ~mask;
		}

		return *this;
	}
	inline BOOL			is			(const TYPE mask) const
	{
		return mask == (flags & mask);
	}
	inline BOOL			is_any		(const TYPE mask) const
	{
		return BOOL(flags & mask);
	}
	inline BOOL			test		(const TYPE mask) const
	{
		return BOOL(flags & mask);
	}
	inline SelfRef		or			(const TYPE mask)
	{
		flags |= mask;
		return *this;
	}
	inline SelfRef		or			(SelfCRef f, const T mask)
	{
		flags = f.flags | mask;	return *this;
	}
	inline SelfRef		and			(const TYPE mask)
	{
		flags &= mask;
		return *this;
	}
	inline SelfRef		and			(SelfCRef f, const TYPE mask)
	{
		flags = f.flags & mask;
		return *this;
	}
	inline BOOL			equal		(SelfCRef f) const
	{
		return flags == f.flags;
	}
	inline BOOL			equal		(SelfCRef f, const TYPE mask) const
	{
		return (flags & mask) == (f.flags & mask);
	}
};

typedef _flags<U8>	Flags8;	//	typedef _flags<U8>	flags8;
typedef _flags<u16>	Flags16;//	typedef _flags<u16>	flags16;
typedef _flags<u32>	Flags32;//	typedef _flags<u32>	flags32;
typedef _flags<u64>	Flags64;//	typedef _flags<u64>	flags64;

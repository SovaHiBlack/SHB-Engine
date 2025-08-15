#pragma once

template <class T>
class _flags
{
public:
	using TYPE							= T;
	using Self							= _flags<TYPE>;
	using SelfRef						= Self&;
	using SelfCRef						= const Self&;

	TYPE								flags;

	IC TYPE			get					( )
	{
		return flags;
	}
	IC SelfRef		zero				( )
	{
		flags							= TYPE(0);
		return *this;
	}
	IC SelfRef		one					( )
	{
		flags							= TYPE(-1);
		return *this;
	}

	IC SelfRef		invert				( )
	{
		flags							= ~flags;
		return *this;
	}
	IC SelfRef		invert				(SelfCRef f)
	{
		flags							= ~f.flags;
		return *this;
	}
	IC SelfRef		invert				(const TYPE mask)
	{
		flags							^= mask;
		return *this;
	}

	IC SelfRef		assign				(SelfCRef f)
	{
		flags							= f.flags;
		return *this;
	}
	IC SelfRef		assign				(const TYPE mask)
	{
		flags							= mask;
		return *this;
	}

	IC SelfRef		set					(const TYPE mask, BOOL value)
	{
		if (value)
		{
			flags						|= mask;
		}
		else
		{
			flags						&= ~mask;
		}

		return *this;
	}

	IC BOOL			is					(const TYPE mask) const
	{
		return (mask == (flags & mask));
	}
	IC BOOL			is_any				(const TYPE mask) const
	{
		return BOOL(flags & mask);
	}
	IC BOOL			test				(const TYPE mask) const
	{
		return BOOL(flags & mask);
	}

	IC SelfRef		or					(const TYPE mask)
	{
		flags							|= mask;
		return *this;
	}
	IC SelfRef		or					(SelfCRef f, const TYPE mask)
	{
		flags							= f.flags | mask;
		return *this;
	}

	IC SelfRef		and					(const TYPE mask)
	{
		flags							&= mask;
		return *this;
	}
	IC SelfRef		and					(SelfCRef f, const TYPE mask)
	{
		flags							= f.flags & mask;
		return *this;
	}

	IC BOOL			equal				(SelfCRef f) const
	{
		return (flags == f.flags);
	}
	IC BOOL			equal				(SelfCRef f, const TYPE mask) const
	{
		return ((flags & mask) == (f.flags & mask));
	}
};

using flags8							= _flags<u8>;
using flags16							= _flags<u16>;
using flags32							= _flags<u32>;
using flags64							= _flags<u64>;

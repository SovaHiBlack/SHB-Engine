#pragma once

class CClientID
{
	u32						id;

public:
			CClientID		( ) : id(0)
	{ }
			CClientID		(u32 val) : id(val)
	{ }

	u32		value			( ) const
	{
		return id;
	}
	void	set				(u32 v)
	{
		id					= v;
	}

	bool	compare			(u32 v) const
	{
		return (id == v);
	}
	bool	operator ==		(const CClientID& other) const
	{
		return (value( ) == other.value( ));
	}
	bool	operator !=		(const CClientID& other) const
	{
		return (value( ) != other.value( ));
	}
	bool	operator <		(const CClientID& other) const
	{
		return (value( ) < other.value( ));
	}
};

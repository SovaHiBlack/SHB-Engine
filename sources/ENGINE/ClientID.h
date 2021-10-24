#pragma once

class CClientID
{
	U32					id;

public:
	CClientID( ) : id(0)
	{ }
	CClientID(U32 val) : id(val)
	{ }

	U32		value		( ) const
	{
		return id;
	}
	void	set			(U32 v)
	{
		id = v;
	}

	bool	compare		(U32 v) const
	{
		return id == v;
	}
	bool	operator ==	(const CClientID& other) const
	{
		return value( ) == other.value( );
	}
	bool	operator !=	(const CClientID& other) const
	{
		return value( ) != other.value( );
	}
	bool	operator <	(const CClientID& other) const
	{
		return value( ) < other.value( );
	}
};

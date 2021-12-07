#pragma once

class CClientID
{
	unsigned int					id;

public:
	CClientID( ) : id(0)
	{ }
	CClientID(unsigned int val) : id(val)
	{ }

	unsigned int		value		( ) const
	{
		return id;
	}
	void	set			(unsigned int v)
	{
		id = v;
	}

	bool	compare		(unsigned int v) const
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

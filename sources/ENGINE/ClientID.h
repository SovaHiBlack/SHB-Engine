#pragma once

class ClientID
{
	U32					id;

public:
			ClientID	( ) : id(0)
	{ }
			ClientID	(U32 val) : id(val)
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
	bool	operator ==	(const ClientID& other) const
	{
		return value( ) == other.value( );
	}
	bool	operator !=	(const ClientID& other) const
	{
		return value( ) != other.value( );
	}
	bool	operator <	(const ClientID& other) const
	{
		return value( ) < other.value( );
	}
};

//	Module 		: script_value_container.h
//	Description : Script value container

#pragma once

class CScriptValue
{
public:
	virtual ~CScriptValue( )
	{ }
};

class CScriptValueContainer
{
protected:
	xr_vector<CScriptValue*>				m_values;

public:
	virtual			~CScriptValueContainer( );
	inline void	assign( );
	inline void	clear( );
	inline void	add(CScriptValue* value);
};

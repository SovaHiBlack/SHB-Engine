////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_base.h
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Base evaluation function class
////////////////////////////////////////////////////////////////////////////

#pragma once

#define OUT_MESSAGE(s1, s2)

class CEntityAlive;
class CSE_ALifeObject;
class CEF_Storage;

class CBaseFunction
{
protected:
	f32					m_fMinResultValue;
	f32					m_fMaxResultValue;
	char					m_caName[260];
	CEF_Storage* m_storage;

public:
	CBaseFunction(CEF_Storage* storage)
	{
		m_storage = storage;
		VERIFY(m_storage);
		m_caName[0] = 0;
	}

	virtual			~CBaseFunction( )
	{ }

	IC CEF_Storage& ef_storage( ) const
	{
		VERIFY(m_storage);
		return (*m_storage);
	}

	virtual f32	ffGetValue( ) = 0;

	virtual u32		dwfGetDiscreteValue(u32 dwDiscretizationValue = 2)
	{
		f32 fTemp = ffGetValue( );
		if (fTemp <= m_fMinResultValue)
		{
			return 0;
		}
		else if (fTemp >= m_fMaxResultValue)
		{
			return (dwDiscretizationValue - 1);
		}
		else
		{
			return iFloor((fTemp - m_fMinResultValue) / (m_fMaxResultValue - m_fMinResultValue) * f32(dwDiscretizationValue - 1) + 0.5f);
		}
	}

	IC		f32	ffGetMaxResultValue( )
	{
		return m_fMaxResultValue;
	}

	IC		f32	ffGetMinResultValue( )
	{
		return m_fMinResultValue;
	}

	IC		pcstr	Name( )
	{
		return m_caName;
	}

	IC	CLASS_ID clsid_member( ) const;
	IC	CLASS_ID clsid_enemy( ) const;
	IC	CLASS_ID clsid_member_item( ) const;
	IC	CLASS_ID clsid_enemy_item( ) const;
};
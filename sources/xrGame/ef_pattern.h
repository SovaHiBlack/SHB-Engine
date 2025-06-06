////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_pattern.h
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern based evaluation functions trained by supervised learning
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ef_base.h"

class CEF_Storage;

class CPatternFunction : public CBaseFunction
{
private:
	typedef CBaseFunction inherited;
	enum
	{
		EFC_VERSION = 1
	};
	typedef struct tagSEFHeader
	{
		u32			dwBuilderVersion;
		u32			dwDataFormat;
	} SEFHeader;

	typedef struct tagSPattern
	{
		u32			dwCardinality;
		u32* dwaVariableIndexes;
	} SPattern;

	u32* m_dwaAtomicFeatureRange;
	u32* m_dwaPatternIndexes;
	SPattern* m_tpPatterns;
	f32* m_faParameters;
	u32				m_dwPatternCount;
	u32				m_dwParameterCount;
	SEFHeader		m_tEFHeader;

	IC u32 dwfGetPatternIndex(u32* dwpTest, s32 iPatternIndex)
	{
		SPattern& tPattern = m_tpPatterns[iPatternIndex];
		for (u32 i = 1, dwIndex = dwpTest[tPattern.dwaVariableIndexes[0]]; i < (s32)tPattern.dwCardinality; ++i)
			dwIndex = dwIndex * m_dwaAtomicFeatureRange[tPattern.dwaVariableIndexes[i]] + dwpTest[tPattern.dwaVariableIndexes[i]];
		return(dwIndex + m_dwaPatternIndexes[iPatternIndex]);
	}

	f32			ffEvaluate( );

public:
	u32				m_dwVariableCount;
	u32				m_dwFunctionType;
	u32* m_dwaVariableTypes;
	u32* m_dwaVariableValues;

	CPatternFunction(pcstr caEFFileName, CEF_Storage* storage);
	virtual			~CPatternFunction( );

	virtual	void	vfLoadEF(pcstr caEFFileName);
	virtual f32	ffGetValue( );
};

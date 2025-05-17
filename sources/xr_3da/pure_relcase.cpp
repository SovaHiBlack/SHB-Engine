#include "stdafx.h"
#include "pure_relcase.h"
#include "ObjectList.h"
#include "IGameLevel.h"

pure_relcase::~pure_relcase( )
{
	if (g_pGameLevel)
	{
		g_pGameLevel->Objects.relcase_unregister(&m_ID);
	}
}

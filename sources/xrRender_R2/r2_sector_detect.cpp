#include "stdafx.h"
#include "r2.h"

IRender_Sector* CRender::detectSector(const fVector3& P)
{
	IRender_Sector*	S	= NULL;	
	fVector3			dir;
	Sectors_xrc.ray_options		(CDB::OPT_ONLYNEAREST);

	dir.set				(0,-1,0);
	S					= detectSector(P,dir);
	if (NULL==S)		
	{
		dir.set				(0,1,0);
		S					= detectSector(P,dir);
	}
	return S;
}

IRender_Sector* CRender::detectSector(const fVector3& P, fVector3& dir)
{
	// Portals model
	int		id1		= -1;
	f32	range1	= 500.f;
	if (rmPortals)	
	{
		Sectors_xrc.ray_query	(rmPortals,P,dir,range1);
		if (Sectors_xrc.r_count()) {
			CDB::RESULT *RP1 = Sectors_xrc.r_begin();
			id1 = RP1->id; range1 = RP1->range; 
		}
	}

	// Geometry model
	int		id2		= -1;
	f32	range2	= range1;
	Sectors_xrc.ray_query	(g_pGameLevel->ObjectSpace.GetStaticModel(),P,dir,range2);
	if (Sectors_xrc.r_count()) {
		CDB::RESULT *RP2 = Sectors_xrc.r_begin();
		id2 = RP2->id; range2 = RP2->range;
	}

	// Select ID
	int ID;
	if (id1>=0) {
		if (id2>=0) ID = (range1<=range2+ EPSILON_5)?id1:id2;	// both was found
		else ID = id1;									// only id1 found
	} else if (id2>=0) ID = id2;						// only id2 found
	else return 0;

	if (ID==id1) {
		// Take sector, facing to our point from portal
		CDB::TRI*	pTri	= rmPortals->get_tris() + ID;
		CPortal*	pPortal	= (CPortal*) Portals[pTri->dummy];
		return pPortal->getSectorFacing(P);
	} else {
		// Take triangle at ID and use it's Sector
		CDB::TRI*	pTri	= g_pGameLevel->ObjectSpace.GetStaticTris()+ID;
		return getSector(pTri->sector);
	}
}


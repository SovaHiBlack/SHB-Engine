// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Engine.h"

CEngine				Engine;
xrDispatchTable		PSGP;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine( )
{ }

CEngine::~CEngine( )
{ }

extern void msCreate(pcstr name);

void CEngine::Initialize( )
{
	// Bind PSGP
	hPSGP = LoadLibrary("xrCPU_Pipe.dll");
	R_ASSERT(hPSGP);
	xrBinder* bindCPU = (xrBinder*) GetProcAddress(hPSGP, "xrBind_PSGP");
	R_ASSERT(bindCPU);
	bindCPU(&PSGP, CPU::ID.feature & CPU::ID.os_support);

	// Other stuff
	Engine.Sheduler.Initialize( );

#ifdef DEBUG
	msCreate("game");
#endif // DEBUG

}

void CEngine::Destroy( )
{
	Engine.Sheduler.Destroy( );
	Engine.External.Destroy( );

	if (hPSGP)
	{
		FreeLibrary(hPSGP);
		hPSGP = 0;
		ZeroMemory(&PSGP, sizeof(PSGP));
	}
}

#include "stdafx.h"

CEngineAPI::CEngineAPI( )
{
	hGame = 0;
	hRender = 0;
	pCreate = 0;
	pDestroy = 0;
}

CEngineAPI::~CEngineAPI( )
{ }

extern U32 renderer_value; //con cmd

void CEngineAPI::Initialize(void)
{
	// render
	const char* r1_name = "xrRender_R1.dll";
	const char* r2_name = "xrRender_R2.dll";

	if (psDeviceFlags.test(rsR2))
	{
		// try to initialize R2
		Log("Loading DLL:", r2_name);
		hRender = LoadLibrary(r2_name);
		if (0 == hRender)
		{
			// try to load R1
			Msg("...Failed - incompatible hardware.");
		}
	}

	if (0 == hRender)
	{
		// try to load R1
		psDeviceFlags.set(rsR2, FALSE);
		renderer_value = 0; //con cmd

		Log("Loading DLL:", r1_name);
		hRender = LoadLibrary(r1_name);
		if (0 == hRender)
		{
			R_CHK(GetLastError( ));
		}

		R_ASSERT(hRender);
	}

	// game
	{
		const char* g_name = "xrGame.dll";
		Log("Loading DLL:", g_name);
		hGame = LoadLibrary(g_name);
		if (0 == hGame)
		{
			R_CHK(GetLastError( ));
		}

		R_ASSERT2(hGame, "Game DLL raised exception during loading or there is no game DLL at all");
		pCreate = (Factory_Create*) GetProcAddress(hGame, "xrFactory_Create");
		R_ASSERT(pCreate);
		pDestroy = (Factory_Destroy*) GetProcAddress(hGame, "xrFactory_Destroy");
		R_ASSERT(pDestroy);
	}
}

void CEngineAPI::Destroy(void)
{
	if (hGame)
	{
		FreeLibrary(hGame);
		hGame = 0;
	}

	if (hRender)
	{
		FreeLibrary(hRender);
		hRender = 0;
	}

	pCreate = 0;
	pDestroy = 0;
	Engine.Event._destroy( );
	XRC.r_clear_compact( );
}

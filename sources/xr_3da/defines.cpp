#include "stdafx.h"

#ifdef DEBUG
	BOOL bDebug	= FALSE;
#endif // DEBUG

// Video
u32			psCurrentVidMode[2] = {1024,768};
u32			psCurrentBPP		= 32;
// release version always has "mt_*" enabled
Flags32		psDeviceFlags		= {rsFullscreen|rsDetails|mtPhysics|mtSound|rsDrawStatic|rsDrawDynamic};

// textures 
int			psTextureLOD		= 0;

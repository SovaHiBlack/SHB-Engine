#include "stdafx.h"

#ifdef DEBUG
bool bDebug = false;
#endif // DEBUG

// Video
unsigned int			psCurrentVidMode[2] = { 1024, 768 };
unsigned int			psCurrentBPP = 32;
// release version always has "mt_*" enabled
Flags32		psDeviceFlags = { rsFullscreen | rsDetails | mtPhysics | mtSound | rsDrawStatic | rsDrawDynamic };

// textures 
int			psTextureLOD = 0;

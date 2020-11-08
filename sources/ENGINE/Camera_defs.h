#pragma once

enum ECameraStyle
{
	csCamDebug,
	csFirstEye,
	csLookAt,
	csMax,
	csFixed,
	cs_forcedword = u32(-1)
};

enum ECameraEffectorType
{
	cetDemo = 0,
	cetNext
};

enum EEffectorPostProcessType
{
	epptNext = 0
};

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CEffectorCam;
class ENGINE_API CEffectorPP;

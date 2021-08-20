#pragma once

enum ECameraStyle
{
	csFirstEye,
	csLookAt,
	cs_forcedword = unsigned int(-1)
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

class ENGINE_API CCameraBase;
class ENGINE_API CEffectorCam;
class ENGINE_API CEffectorPostProcess;

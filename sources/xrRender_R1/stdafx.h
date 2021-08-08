#pragma once

#pragma warning(disable:4995)
#include "..\ENGINE\stdafx.h"
//#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#pragma warning(disable:4714)
#pragma warning(4:4018)
#pragma warning(4:4244)

#define R_R1		1
#define R_R2		2
#define RENDER		R_R1

#include "..\ENGINE\ResourceManager.h"
#include "..\ENGINE\vis_common.h"
#include "..\ENGINE\Render.h"
#include "..\ENGINE\IGameLevel.h"
#include "..\ENGINE\blenders\blender.h"
#include "..\ENGINE\blenders\blender_clsid.h"
#include "..\Particles\psystem.h"
#include "..\xrRender\xrRender_console.h"
#include "FStaticRender.h"

#define TEX_POINT_ATT		"internal\\internal_light_attpoint"
#define TEX_SPOT_ATT		"internal\\internal_light_attclip"

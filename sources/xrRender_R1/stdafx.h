// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#pragma warning(disable:4995)
#include "..\XR_3DA\stdafx.h"
#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)
#pragma warning(disable:4714)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#define		R_R1	1
#define		R_R2	2
#define		RENDER	R_R1

#include "..\XR_3DA\resourcemanager.h"
#include "..\XR_3DA\vis_common.h"
#include "..\XR_3DA\Render.h"
#include "..\XR_3DA\igame_level.h"
#include "..\XR_3DA\blenders\blender.h"
#include "..\XR_3DA\blenders\blender_clsid.h"
#include "..\xrParticles\psystem.h"
#include "..\xrRender\xrRender_console.h"
#include "FStaticRender.h"

#define		TEX_POINT_ATT	"internal\\internal_light_attpoint"
#define		TEX_SPOT_ATT	"internal\\internal_light_attclip"

#pragma once

#define	CAP_VERSION(a,b)	(unsigned int(a)*10 + unsigned int(b))

class ENGINE_API CHWCaps {
public:
	struct		caps_Geometry
	{
		unsigned int	dwRegisters		: 16;
		unsigned int dwInstructions	: 16;
		unsigned int	bSoftware		: 1;
		unsigned int	bPointSprites	: 1;
		unsigned int	bVTF			: 1;		// vertex-texture-fetch
		unsigned int	bNPatches		: 1;
		unsigned int dwClipPlanes	: 4;
		unsigned int dwVertexCache	: 8;
	};
	struct		caps_Raster
	{
		unsigned int	dwRegisters		: 16;
		unsigned int dwInstructions	: 16;
		unsigned int	dwStages		: 4;		// number of tex-stages
		unsigned int	dwMRT_count		: 4;
		unsigned int b_MRT_mixdepth	: 1;
		unsigned int	bNonPow2		: 1;
		unsigned int	bCubemap		: 1;
	};

public:
	// force flags
	BOOL			bForceGPU_REF;
	BOOL			bForceGPU_SW;
	BOOL			bForceGPU_NonPure;
	BOOL			SceneMode;

	// device format
	D3DFORMAT		fTarget;
	D3DFORMAT		fDepth;
	unsigned int				dwRefreshRate;

	// caps itself
	unsigned short				geometry_major	;
	unsigned short				geometry_minor	;
	caps_Geometry	geometry		;
	unsigned short				raster_major	;
	unsigned short				raster_minor	;
	caps_Raster		raster			;

	unsigned int				id_vendor		;
	unsigned int				id_device		;

	BOOL			bStencil;			// stencil buffer present
	BOOL			bScissor;			// scissor rect supported
	BOOL			bTableFog;			//

	// some precalculated values
	D3DSTENCILOP	soDec, soInc;		// best stencil OPs for shadows
	unsigned int				dwMaxStencilValue;  // maximum value the stencil buffer can hold

	void			Update(void);
};

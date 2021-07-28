// Blender_Recorder.h: interface for the CBlender_Recorder class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\tss.h"

#pragma pack(push,4)

class ENGINE_API CBlender_Compile  
{
public:
	sh_list				L_textures;
	sh_list				L_constants;
	sh_list				L_matrices;

	const char* detail_texture;
	R_constant_setup*	detail_scaler;

	BOOL				bEditor;
	BOOL				bDetail;
	BOOL				bDetail_Diffuse;
	BOOL				bDetail_Bump;
	int					iElement;

public:
	CSimulator			RS;
	IBlender*			BT;
	ShaderElement*		SH;

private:
	SPass				dest;
	R_constant_table	ctable;

	STextureList		passTextures;
	SMatrixList			passMatrices;
	SConstantList		passConstants;
	U32					dwStage;

	string128			pass_vs;
	string128			pass_ps;

	U32					BC					(BOOL v)	{ return v?0xff:0; }

public:
	CSimulator&			R()					{ return RS; }
	
	void				SetParams			(int iPriority, bool bStrictB2F);
	void				SetMapping			();

	// R1-compiler
	void				PassBegin			();
	U32					Pass				()  { return SH->passes.size(); }
	void				PassSET_ZB			(BOOL bZTest,	BOOL bZWrite, BOOL bInvertZTest=FALSE);
	void				PassSET_ablend_mode	(BOOL bABlend, U32 abSRC, U32 abDST);
	void				PassSET_ablend_aref	(BOOL aTest, U32 aRef);
	void				PassSET_Blend		(BOOL bABlend, U32 abSRC, U32 abDST, BOOL aTest, U32 aRef);
	void				PassSET_Blend_BLEND	(BOOL bAref=FALSE, U32 ref=0)	{ PassSET_Blend	(TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,bAref,ref);	}
	void				PassSET_Blend_SET	(BOOL bAref=FALSE, U32 ref=0)	{ PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,bAref,ref);				}
	void				PassSET_Blend_ADD	(BOOL bAref=FALSE, U32 ref=0)	{ PassSET_Blend	(TRUE, D3DBLEND_ONE,D3DBLEND_ONE, bAref,ref);				}
	void				PassSET_Blend_MUL	(BOOL bAref=FALSE, U32 ref=0)	{ PassSET_Blend	(TRUE, D3DBLEND_DESTCOLOR,D3DBLEND_ZERO,bAref,ref);			}
	void				PassSET_Blend_MUL2X	(BOOL bAref=FALSE, U32 ref=0)	{ PassSET_Blend	(TRUE, D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,bAref,ref);		}
	void				PassSET_LightFog	(BOOL bLight, BOOL bFog);
	void				PassSET_PS			(const char* name);
	void				PassSET_VS			(const char* name);
	void				PassEnd				();

	void				StageBegin			();
	U32					Stage				()	{ return dwStage; }
	void				StageTemplate_LMAP0	();
	void				StageSET_Address	(U32 adr);
	void				StageSET_XForm		(U32 tf, U32 tc);
	void				StageSET_Color		(U32 a1, U32 op, U32 a2);
	void				StageSET_Color3		(U32 a1, U32 op, U32 a2, U32 a3);
	void				StageSET_Alpha		(U32 a1, U32 op, U32 a2);
	void				StageSET_TMC		(const char* T, const char* M, const char* C, int UVW_channel);
	void				Stage_Texture		(const char* name, U32 address=D3DTADDRESS_WRAP, U32	 fmin=D3DTEXF_LINEAR, U32 fmip=D3DTEXF_LINEAR, U32 fmag=D3DTEXF_LINEAR);
	void				Stage_Matrix		(const char* name, int UVW_channel);
	void				Stage_Constant		(const char* name);
	void				StageEnd			();

	// R1/R2-compiler	[programmable]
	U32					i_Sampler			(const char* name);
	void				i_Texture			(U32 s, const char* name);
	void				i_Projective		(U32 s, bool	b);
	void				i_Address			(U32 s, U32		address);
	void				i_Filter_Min		(U32 s, U32		f);
	void				i_Filter_Mip		(U32 s, U32		f);
	void				i_Filter_Mag		(U32 s, U32		f);
	void				i_Filter			(U32 s, U32 _min, U32 _mip, U32 _mag);

	// R1/R2-compiler	[programmable]		- templates
	void				r_Pass				(const char* vs, const char* ps,		bool bFog,	BOOL	bZtest=TRUE,				BOOL	bZwrite=TRUE,			BOOL	bABlend=FALSE,			D3DBLEND	abSRC=D3DBLEND_ONE,		D3DBLEND abDST=D3DBLEND_ZERO,	BOOL aTest=FALSE, U32 aRef=0);
	void				r_Constant			(const char* name,	R_constant_setup* s);
	U32					r_Sampler			(const char* name, const char* texture,		bool b_ps1x_ProjectiveDivide=false, U32	address=D3DTADDRESS_WRAP, U32		fmin=D3DTEXF_LINEAR, U32		fmip=D3DTEXF_LINEAR, U32 fmag=D3DTEXF_LINEAR);
	U32					r_Sampler			(const char* name,	shared_str texture, bool b_ps1x_ProjectiveDivide=false, U32	address=D3DTADDRESS_WRAP, U32		fmin=D3DTEXF_LINEAR, U32		fmip=D3DTEXF_LINEAR, U32 fmag=D3DTEXF_LINEAR)	{
		return r_Sampler	(name,texture.c_str(),b_ps1x_ProjectiveDivide,address,fmin,fmip,fmag);
	}
	void				r_Sampler_rtf		(const char* name, const char* texture,		bool b_ps1x_ProjectiveDivide=false);
	void				r_Sampler_clf		(const char* name, const char* texture,		bool b_ps1x_ProjectiveDivide=false);
	void				r_Sampler_clw		(const char* name, const char* texture,		bool b_ps1x_ProjectiveDivide=false);
	void				r_End				();

	CBlender_Compile	();
	~CBlender_Compile	();
	
	void				_cpp_Compile		(ShaderElement* _SH);
	ShaderElement* 		_lua_Compile		(const char* namesp, const char* name);
};
#pragma pack(pop)

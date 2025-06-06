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

	pcstr				detail_texture;
	R_constant_setup* detail_scaler;

	BOOL				bEditor;
	BOOL				bDetail;
	BOOL				bDetail_Diffuse;
	BOOL				bDetail_Bump;
	s32					iElement;

public:
	CSimulator			RS;
	IBlender* BT;
	ShaderElement* SH;

private:
	SPass				dest;
	R_constant_table	ctable;

	STextureList		passTextures;
	SMatrixList			passMatrices;
	SConstantList		passConstants;
	u32					dwStage;

	string128			pass_vs;
	string128			pass_ps;

	u32					BC(BOOL v)
	{
		return (v ? 0xff : 0);
	}

public:
	CSimulator& R( )
	{
		return RS;
	}

	void				SetParams(s32 iPriority, bool bStrictB2F);
	void				SetMapping( );

	// R1-compiler
	void				PassBegin( );
	u32					Pass( )
	{
		return SH->passes.size( );
	}
	void				PassSET_ZB(BOOL bZTest, BOOL bZWrite, BOOL bInvertZTest = FALSE);
	void				PassSET_ablend_mode(BOOL bABlend, u32 abSRC, u32 abDST);
	void				PassSET_ablend_aref(BOOL aTest, u32 aRef);
	void				PassSET_Blend(BOOL bABlend, u32 abSRC, u32 abDST, BOOL aTest, u32 aRef);
	void				PassSET_Blend_BLEND(BOOL bAref = FALSE, u32 ref = 0)
	{
		PassSET_Blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, bAref, ref);
	}
	void				PassSET_Blend_SET(BOOL bAref = FALSE, u32 ref = 0)
	{
		PassSET_Blend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, bAref, ref);
	}
	void				PassSET_Blend_ADD(BOOL bAref = FALSE, u32 ref = 0)
	{
		PassSET_Blend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE, bAref, ref);
	}
	void				PassSET_Blend_MUL(BOOL bAref = FALSE, u32 ref = 0)
	{
		PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_ZERO, bAref, ref);
	}
	void				PassSET_Blend_MUL2X(BOOL bAref = FALSE, u32 ref = 0)
	{
		PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_SRCCOLOR, bAref, ref);
	}
	void				PassSET_LightFog(BOOL bLight, BOOL bFog);
	void				PassSET_PS(pcstr name);
	void				PassSET_VS(pcstr name);
	void				PassEnd( );

	void				StageBegin( );
	u32					Stage( )
	{
		return dwStage;
	}
	void				StageTemplate_LMAP0( );
	void				StageSET_Address(u32 adr);
	void				StageSET_XForm(u32 tf, u32 tc);
	void				StageSET_Color(u32 a1, u32 op, u32 a2);
	void				StageSET_Color3(u32 a1, u32 op, u32 a2, u32 a3);
	void				StageSET_Alpha(u32 a1, u32 op, u32 a2);
	void				StageSET_TMC(pcstr T, pcstr M, pcstr C, s32 UVW_channel);
	void				Stage_Texture(pcstr name, u32 address = D3DTADDRESS_WRAP, u32	 fmin = D3DTEXF_LINEAR, u32 fmip = D3DTEXF_LINEAR, u32 fmag = D3DTEXF_LINEAR);
	void				Stage_Matrix(pcstr name, s32 UVW_channel);
	void				Stage_Constant(pcstr name);
	void				StageEnd( );

	// R1/R2-compiler	[programmable]
	u32					i_Sampler(pcstr name);
	void				i_Texture(u32 s, pcstr	name);
	void				i_Projective(u32 s, bool	b);
	void				i_Address(u32 s, u32		address);
	void				i_Filter_Min(u32 s, u32		f);
	void				i_Filter_Mip(u32 s, u32		f);
	void				i_Filter_Mag(u32 s, u32		f);
	void				i_Filter(u32 s, u32 _min, u32 _mip, u32 _mag);

	// R1/R2-compiler	[programmable]		- templates
	void				r_Pass(pcstr vs, pcstr ps, bool bFog, BOOL	bZtest = TRUE, BOOL	bZwrite = TRUE, BOOL	bABlend = FALSE, D3DBLEND	abSRC = D3DBLEND_ONE, D3DBLEND abDST = D3DBLEND_ZERO, BOOL aTest = FALSE, u32 aRef = 0);
	void				r_Constant(pcstr name, R_constant_setup* s);
	u32					r_Sampler(pcstr name, pcstr texture, bool b_ps1x_ProjectiveDivide = false, u32	address = D3DTADDRESS_WRAP, u32		fmin = D3DTEXF_LINEAR, u32		fmip = D3DTEXF_LINEAR, u32 fmag = D3DTEXF_LINEAR);
	u32					r_Sampler(pcstr name, shared_str texture, bool b_ps1x_ProjectiveDivide = false, u32	address = D3DTADDRESS_WRAP, u32		fmin = D3DTEXF_LINEAR, u32		fmip = D3DTEXF_LINEAR, u32 fmag = D3DTEXF_LINEAR)
	{
		return r_Sampler(name, texture.c_str( ), b_ps1x_ProjectiveDivide, address, fmin, fmip, fmag);
	}
	void				r_Sampler_rtf(pcstr name, pcstr texture, bool b_ps1x_ProjectiveDivide = false);
	void				r_Sampler_clf(pcstr name, pcstr texture, bool b_ps1x_ProjectiveDivide = false);
	void				r_Sampler_clw(pcstr name, pcstr texture, bool b_ps1x_ProjectiveDivide = false);
	void				r_End( );

	CBlender_Compile( );
	~CBlender_Compile( );

	void				_cpp_Compile(ShaderElement* _SH);
	ShaderElement* _lua_Compile(pcstr namesp, pcstr name);
};
#pragma pack(pop)

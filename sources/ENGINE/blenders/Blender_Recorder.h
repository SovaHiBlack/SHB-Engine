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
	R_constant_setup* detail_scaler;

	BOOL				bEditor;
	BOOL				bDetail;
	BOOL				bDetail_Diffuse;
	BOOL				bDetail_Bump;
	int					iElement;

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
	unsigned int					dwStage;

	string128			pass_vs;
	string128			pass_ps;

	unsigned int					BC(BOOL v)
	{
		return v ? 0xff : 0;
	}

public:
	CSimulator& R( )
	{
		return RS;
	}

	void				SetParams(int iPriority, bool bStrictB2F);
	void				SetMapping( );

	// R1-compiler
	void				PassBegin( );
	unsigned int					Pass( )
	{
		return SH->passes.size( );
	}
	void				PassSET_ZB(BOOL bZTest, BOOL bZWrite, BOOL bInvertZTest = FALSE);
	void				PassSET_ablend_mode(BOOL bABlend, unsigned int abSRC, unsigned int abDST);
	void				PassSET_ablend_aref(BOOL aTest, unsigned int aRef);
	void				PassSET_Blend(BOOL bABlend, unsigned int abSRC, unsigned int abDST, BOOL aTest, unsigned int aRef);
	void				PassSET_Blend_BLEND(BOOL bAref = FALSE, unsigned int ref = 0)
	{
		PassSET_Blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, bAref, ref);
	}
	void				PassSET_Blend_SET(BOOL bAref = FALSE, unsigned int ref = 0)
	{
		PassSET_Blend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, bAref, ref);
	}
	void				PassSET_Blend_ADD(BOOL bAref = FALSE, unsigned int ref = 0)
	{
		PassSET_Blend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE, bAref, ref);
	}
	void				PassSET_Blend_MUL(BOOL bAref = FALSE, unsigned int ref = 0)
	{
		PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_ZERO, bAref, ref);
	}
	void				PassSET_Blend_MUL2X(BOOL bAref = FALSE, unsigned int ref = 0)
	{
		PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_SRCCOLOR, bAref, ref);
	}
	void				PassSET_LightFog(BOOL bLight, BOOL bFog);
	void				PassSET_PS(const char* name);
	void				PassSET_VS(const char* name);
	void				PassEnd( );

	void				StageBegin( );
	unsigned int					Stage( )
	{
		return dwStage;
	}
	void				StageTemplate_LMAP0( );
	void				StageSET_Address(unsigned int adr);
	void				StageSET_XForm(unsigned int tf, unsigned int tc);
	void				StageSET_Color(unsigned int a1, unsigned int op, unsigned int a2);
	void				StageSET_Color3(unsigned int a1, unsigned int op, unsigned int a2, unsigned int a3);
	void				StageSET_Alpha(unsigned int a1, unsigned int op, unsigned int a2);
	void				StageSET_TMC(const char* T, const char* M, const char* C, int UVW_channel);
	void				Stage_Texture(const char* name, unsigned int address = D3DTADDRESS_WRAP, unsigned int fmin = D3DTEXF_LINEAR, unsigned int fmip = D3DTEXF_LINEAR, unsigned int fmag = D3DTEXF_LINEAR);
	void				Stage_Matrix(const char* name, int UVW_channel);
	void				Stage_Constant(const char* name);
	void				StageEnd( );

	// R1/R2-compiler	[programmable]
	unsigned int					i_Sampler(const char* name);
	void				i_Texture(unsigned int s, const char* name);
	void				i_Projective(unsigned int s, bool	b);
	void				i_Address(unsigned int s, unsigned int		address);
	void				i_Filter_Min(unsigned int s, unsigned int		f);
	void				i_Filter_Mip(unsigned int s, unsigned int		f);
	void				i_Filter_Mag(unsigned int s, unsigned int		f);
	void				i_Filter(unsigned int s, unsigned int _min, unsigned int _mip, unsigned int _mag);

	// R1/R2-compiler	[programmable]		- templates
	void				r_Pass(const char* vs, const char* ps, bool bFog, BOOL	bZtest = TRUE, BOOL	bZwrite = TRUE, BOOL	bABlend = FALSE, D3DBLEND	abSRC = D3DBLEND_ONE, D3DBLEND abDST = D3DBLEND_ZERO, BOOL aTest = FALSE, unsigned int aRef = 0);
	void				r_Constant(const char* name, R_constant_setup* s);
	unsigned int					r_Sampler(const char* name, const char* texture, bool b_ps1x_ProjectiveDivide = false, unsigned int	address = D3DTADDRESS_WRAP, unsigned int fmin = D3DTEXF_LINEAR, unsigned int fmip = D3DTEXF_LINEAR, unsigned int fmag = D3DTEXF_LINEAR);
	unsigned int					r_Sampler(const char* name, CSharedString texture, bool b_ps1x_ProjectiveDivide = false, unsigned int	address = D3DTADDRESS_WRAP, unsigned int fmin = D3DTEXF_LINEAR, unsigned int fmip = D3DTEXF_LINEAR, unsigned int fmag = D3DTEXF_LINEAR)
	{
		return r_Sampler(name, texture.c_str( ), b_ps1x_ProjectiveDivide, address, fmin, fmip, fmag);
	}
	void				r_Sampler_rtf(const char* name, const char* texture, bool b_ps1x_ProjectiveDivide = false);
	void				r_Sampler_clf(const char* name, const char* texture, bool b_ps1x_ProjectiveDivide = false);
	void				r_Sampler_clw(const char* name, const char* texture, bool b_ps1x_ProjectiveDivide = false);
	void				r_End( );

	CBlender_Compile( );
	~CBlender_Compile( );

	void				_cpp_Compile(ShaderElement* _SH);
	ShaderElement* _lua_Compile(const char* namesp, const char* name);
};
#pragma pack(pop)

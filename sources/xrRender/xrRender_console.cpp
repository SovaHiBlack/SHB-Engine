#include "stdafx.h"

#include "xrRender_console.h"

#include	"..\XR_3DA\Console.h"
#include	"..\XR_3DA\ConsoleCommand.h"

u32 ps_Preset = 2;
xr_token qpreset_token[ ] =
{
	{ "Minimum",		0	},
	{ "Low",			1	},
	{ "Default",		2	},
	{ "High",			3	},
	{ "Extreme",		4	},
	{ 0,				0	}
};

// Common
s32 ps_r__LightSleepFrames = 10;

f32 ps_r__Detail_l_ambient = 0.9f;
f32 ps_r__Detail_l_aniso = 0.25f;
f32 ps_r__Detail_density = 0.3f;
f32 ps_r__Detail_rainbow_hemi = 0.75f;

f32 ps_r__Tree_w_rot = 10.0f;
f32 ps_r__Tree_w_speed = 1.00f;
f32 ps_r__Tree_w_amp = 0.005f;
fVector3 ps_r__Tree_Wave = { 0.1f, 0.01f, 0.11f };
f32 ps_r__Tree_SBC = 1.5f;	// scale bias correct

f32 ps_r__WallmarkTTL = 300.0f;
f32 ps_r__WallmarkSHIFT = 0.0001f;
f32 ps_r__WallmarkSHIFT_V = 0.0001f;

f32 ps_r__GLOD_ssa_start = 256.0f;
f32 ps_r__GLOD_ssa_end = 64.0f;
f32 ps_r__LOD = 1.0f;
f32 ps_r__ssaDISCARD = 3.5f;					//RO
f32 ps_r__ssaDONTSORT = 32.0f;					//RO
f32 ps_r__ssaHZBvsTEX = 96.0f;					//RO

s32 ps_r__tf_Anisotropic = 4;

// R1
f32 ps_r1_ssaLOD_A = 64.0f;
f32 ps_r1_ssaLOD_B = 48.0f;
f32 ps_r1_tf_Mipbias = 0.0f;
flags32 ps_r1_flags = { R1FLAG_DLIGHTS };		// r1-only
f32 ps_r1_lmodel_lerp = 0.1f;
f32 ps_r1_dlights_clip = 30.0f;
f32 ps_r1_pps_u = 0.0f;
f32 ps_r1_pps_v = 0.0f;

// R1-specific
s32 ps_r1_GlowsPerFrame = 16;					// r1-only

// R2
f32 ps_r2_ssaLOD_A = 48.0f;
f32 ps_r2_ssaLOD_B = 32.0f;
f32 ps_r2_tf_Mipbias = 0.0f;

// R2-specific
flags32 ps_r2_ls_flags = { R2FLAG_SUN | R2FLAG_SUN_IGNORE_PORTALS | R2FLAG_EXP_DONT_TEST_UNSHADOWED | R2FLAG_USE_NVSTENCIL | R2FLAG_EXP_SPLIT_SCENE | R2FLAG_EXP_MT_CALC };	// r2-only
f32 ps_r2_df_parallax_h = 0.02f;
f32 ps_r2_df_parallax_range = 75.0f;
f32 ps_r2_tonemap_middlegray = 0.25f;			// r2-only
f32 ps_r2_tonemap_adaptation = 5.0f;				// r2-only
f32 ps_r2_tonemap_low_lum = 0.001f;			// r2-only
f32 ps_r2_tonemap_amount = 0.5f;				// r2-only
f32 ps_r2_ls_bloom_kernel_g = 3.3f;				// r2-only
f32 ps_r2_ls_bloom_kernel_b = 0.7f;				// r2-only
f32 ps_r2_ls_bloom_speed = 10.0f;				// r2-only
f32 ps_r2_ls_bloom_kernel_scale = 1.0f;				// r2-only	// gauss
f32 ps_r2_ls_dsm_kernel = 0.7f;				// r2-only
f32 ps_r2_ls_psm_kernel = 0.7f;				// r2-only
f32 ps_r2_ls_ssm_kernel = 0.7f;				// r2-only
f32 ps_r2_ls_bloom_threshold = 0.3f;				// r2-only
fVector3 ps_r2_aa_barier = { 0.8f, 0.1f, 0.0f };	// r2-only
fVector3 ps_r2_aa_weight = { 0.25f,0.25f,0.0f };	// r2-only
f32 ps_r2_aa_kernel = 0.5f;				// r2-only
f32 ps_r2_mblur = 0.5f;				// .5f
s32 ps_r2_GI_depth = 1;				// 1..5
s32 ps_r2_GI_photons = 16;				// 8..64
f32 ps_r2_GI_clip = EPSILON_3;			// EPS
f32 ps_r2_GI_refl = 0.9f;				// .9f
f32 ps_r2_ls_depth_scale = 1.00001f;			// 1.00001f
f32 ps_r2_ls_depth_bias = -0.0001f;			// -0.0001f
f32 ps_r2_ls_squality = 1.0f;				// 1.00f
f32 ps_r2_sun_tsm_projection = 0.18f;			// 0.18f
f32 ps_r2_sun_tsm_bias = -0.05f;			// 
f32 ps_r2_sun_near = 12.0f;				// 12.0f
f32 ps_r2_sun_near_border = 0.66f;			// 1.0f
f32 ps_r2_sun_depth_far_scale = 1.00000f;			// 1.00001f
f32 ps_r2_sun_depth_far_bias = 0.00000f;			// -0.0000f
f32 ps_r2_sun_depth_near_scale = 1.00001f;			// 1.00001f
f32 ps_r2_sun_depth_near_bias = -0.00004f;		// -0.00005f
f32 ps_r2_sun_lumscale = 1.0f;				// 1.0f
f32 ps_r2_sun_lumscale_hemi = 1.0f;				// 1.0f
f32 ps_r2_sun_lumscale_amb = 1.0f;
f32 ps_r2_gmaterial = 0.0f;				// 
f32 ps_r2_zfill = 0.1f;				// .1f

f32 ps_r2_dhemi_scale = 1.0f;				// 1.5f
s32 ps_r2_dhemi_count = 5;				// 5
s32 ps_r2_wait_sleep = 0;

f32 ps_r2_lt_smooth = 1.0f;				// 1.f
f32 ps_r2_slight_fade = 1.0f;				// 1.f

f32 ps_r2_gloss_factor = 1.0f;

//-----------------------------------------------------------------------
class CCC_tf_Aniso : public CCC_Integer
{
public:
	void apply( )
	{
		if (0 == HW.pDevice)
		{
			return;
		}

		s32 val = *value;
		clamp(val, 1, 16);
		for (u32 i = 0; i < HW.Caps.raster.dwStages; i++)
		{
			CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, val));
		}
	}

	CCC_tf_Aniso(pcstr N, s32* v) : CCC_Integer(N, v, 1, 16)
	{ }

	virtual void Execute(pcstr args)
	{
		CCC_Integer::Execute(args);
		apply( );
	}

	virtual void Status(TStatus& S)
	{
		CCC_Integer::Status(S);
		apply( );
	}
};

class CCC_tf_MipBias : public CCC_Float
{
public:
	void apply( )
	{
		if (0 == HW.pDevice)
		{
			return;
		}

		for (u32 i = 0; i < HW.Caps.raster.dwStages; i++)
		{
			CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)value)));
		}
	}

	CCC_tf_MipBias(pcstr N, f32* v) : CCC_Float(N, v, -0.5f, +0.5f)
	{ }

	virtual void Execute(pcstr args)
	{
		CCC_Float::Execute(args);
		apply( );
	}

	virtual void Status(TStatus& S)
	{
		CCC_Float::Status(S);
		apply( );
	}
};

class CCC_R2GM : public CCC_Float
{
public:
	CCC_R2GM(pcstr N, f32* v) : CCC_Float(N, v, 0.0f, 4.0f)
	{
		*v = 0;
	}

	virtual void Execute(pcstr args)
	{
		if (0 == xr_strcmp(args, "on"))
		{
			ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, TRUE);
		}
		else if (0 == xr_strcmp(args, "off"))
		{
			ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, FALSE);
		}
		else
		{
			CCC_Float::Execute(args);
			if (ps_r2_ls_flags.test(R2FLAG_GLOBALMATERIAL))
			{
				static pcstr name[4] = { "oren", "blin", "phong", "metal" };
				f32 mid = *value;
				s32 m0 = iFloor(mid) % 4;
				s32 m1 = (m0 + 1) % 4;
				f32	frc = mid - f32(iFloor(mid));
				Msg("* material set to [%s]-[%s], with lerp of [%f]", name[m0], name[m1], frc);
			}
		}
	}
};

class CCC_Screenshot : public IConsoleCommand
{
public:
	CCC_Screenshot(pcstr N) : IConsoleCommand(N)
	{ }

	virtual void Execute(pcstr args)
	{
		string_path name;
		name[0] = 0;
		sscanf(args, "%s", name);
		pcstr image = xr_strlen(name) ? name : 0;
		::Render->Screenshot(IRender_interface::SM_NORMAL, image);
	}
};

class CCC_ModelPoolStat : public IConsoleCommand
{
public:
	CCC_ModelPoolStat(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		RImplementation.Models->dump( );
	}
};
//-----------------------------------------------------------------------
class CCC_Preset : public CCC_Token
{
public:
	CCC_Preset(pcstr N, u32* V, xr_token* T) : CCC_Token(N, V, T)
	{ }

	virtual void Execute(pcstr args)
	{
		CCC_Token::Execute(args);
		string_path _cfg;
		string_path cmd;

		switch (*value)
		{
			case 0:
			{
				strcpy(_cfg, "rspec_minimum.ltx");
			}
			break;
			case 1:
			{
				strcpy(_cfg, "rspec_low.ltx");
			}
			break;
			case 2:
			{
				strcpy(_cfg, "rspec_default.ltx");
			}
			break;
			case 3:
			{
				strcpy(_cfg, "rspec_high.ltx");
			}
			break;
			case 4:
			{
				strcpy(_cfg, "rspec_extreme.ltx");
			}
			break;
		}

		FS.update_path(_cfg, "$game_config$", _cfg);
		strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
		Console->Execute(cmd);
	}
};

#if RENDER==R_R2
#include "r__pixel_calculator.h"
class CCC_BuildSSA : public IConsoleCommand
{
public:
	CCC_BuildSSA(pcstr N) : IConsoleCommand(N)
	{
		bEmptyArgsHandled = true;
	}

	virtual void Execute(pcstr args)
	{
		r_pixel_calculator c;
		c.run( );
	}
};
#endif
//-----------------------------------------------------------------------
void xrRender_initconsole( )
{
	CMD3(CCC_Preset, "_preset", &ps_Preset, qpreset_token);

	// Common
	CMD1(CCC_Screenshot, "screenshot");

#ifdef DEBUG
#if RENDER==R_R2
	CMD1(CCC_BuildSSA, "build_ssa");
#endif
	CMD4(CCC_Integer, "r__lsleep_frames", &ps_r__LightSleepFrames, 4, 30);
	CMD4(CCC_Float, "r__ssa_glod_start", &ps_r__GLOD_ssa_start, 128.0f, 512.0f);
	CMD4(CCC_Float, "r__ssa_glod_end", &ps_r__GLOD_ssa_end, 16.0f, 96.0f);
	CMD4(CCC_Float, "r__wallmark_shift_pp", &ps_r__WallmarkSHIFT, 0.0f, 1.0f);
	CMD4(CCC_Float, "r__wallmark_shift_v", &ps_r__WallmarkSHIFT_V, 0.0f, 1.0f);
	CMD4(CCC_Float, "r__wallmark_ttl", &ps_r__WallmarkTTL, 1.0f, 5.0f * 60.0f);
	CMD1(CCC_ModelPoolStat, "stat_models");
#endif // def DEBUG

	fVector3 tw_min;
	fVector3 tw_max;

	CMD4(CCC_Float, "r__geometry_lod", &ps_r__LOD, 0.1f, 1.2f);

	CMD4(CCC_Float, "r__detail_density", &ps_r__Detail_density, 0.2f, 0.6f);

#ifdef DEBUG
	CMD4(CCC_Float, "r__detail_l_ambient", &ps_r__Detail_l_ambient, 0.5f, 0.95f);
	CMD4(CCC_Float, "r__detail_l_aniso", &ps_r__Detail_l_aniso, 0.1f, 0.5f);

	CMD4(CCC_Float, "r__d_tree_w_amp", &ps_r__Tree_w_amp, 0.001f, 1.0f);
	CMD4(CCC_Float, "r__d_tree_w_rot", &ps_r__Tree_w_rot, 0.01f, 100.0f);
	CMD4(CCC_Float, "r__d_tree_w_speed", &ps_r__Tree_w_speed, 1.0f, 10.0f);

	tw_min.set(EPSILON_5, EPSILON_5, EPSILON_5);
	tw_max.set(2.0f, 2.0f, 2.0f);
	CMD4(CCC_Vector3, "r__d_tree_wave", &ps_r__Tree_Wave, tw_min, tw_max);
#endif // DEBUG

	CMD2(CCC_tf_Aniso, "r__tf_aniso", &ps_r__tf_Anisotropic); //	{1..16}

	// R1
	CMD4(CCC_Float, "r1_ssa_lod_a", &ps_r1_ssaLOD_A, 16.0f, 96.0f);
	CMD4(CCC_Float, "r1_ssa_lod_b", &ps_r1_ssaLOD_B, 16.0f, 64.0f);
	CMD4(CCC_Float, "r1_lmodel_lerp", &ps_r1_lmodel_lerp, 0.0f, 0.333f);
	CMD2(CCC_tf_MipBias, "r1_tf_mipbias", &ps_r1_tf_Mipbias);//	{-3 +3}
	CMD3(CCC_Mask, "r1_dlights", &ps_r1_flags, R1FLAG_DLIGHTS);
	CMD4(CCC_Float, "r1_dlights_clip", &ps_r1_dlights_clip, 10.0f, 150.0f);
	CMD4(CCC_Float, "r1_pps_u", &ps_r1_pps_u, -1.0f, +1.0f);
	CMD4(CCC_Float, "r1_pps_v", &ps_r1_pps_v, -1.0f, +1.0f);
	CMD4(CCC_Float, "r1_dlights_clip", &ps_r1_dlights_clip, 10.0f, 150.0f);

	// R1-specific
	CMD4(CCC_Integer, "r1_glows_per_frame", &ps_r1_GlowsPerFrame, 2, 32);

	// R2
	CMD4(CCC_Float, "r2_ssa_lod_a", &ps_r2_ssaLOD_A, 16.0f, 96.0f);
	CMD4(CCC_Float, "r2_ssa_lod_b", &ps_r2_ssaLOD_B, 32.0f, 64.0f);
	CMD2(CCC_tf_MipBias, "r2_tf_mipbias", &ps_r2_tf_Mipbias);

	// R2-specific
	CMD2(CCC_R2GM, "r2em", &ps_r2_gmaterial);
	CMD3(CCC_Mask, "r2_tonemap", &ps_r2_ls_flags, R2FLAG_TONEMAP);
	CMD4(CCC_Float, "r2_tonemap_middlegray", &ps_r2_tonemap_middlegray, 0.0f, 2.0f);
	CMD4(CCC_Float, "r2_tonemap_adaptation", &ps_r2_tonemap_adaptation, 0.01f, 10.0f);
	CMD4(CCC_Float, "r2_tonemap_lowlum", &ps_r2_tonemap_low_lum, 0.0001f, 1.0f);
	CMD4(CCC_Float, "r2_tonemap_amount", &ps_r2_tonemap_amount, 0.0000f, 1.0f);
	CMD4(CCC_Float, "r2_ls_bloom_kernel_scale", &ps_r2_ls_bloom_kernel_scale, 0.5f, 2.0f);
	CMD4(CCC_Float, "r2_ls_bloom_kernel_g", &ps_r2_ls_bloom_kernel_g, 1.0f, 7.0f);
	CMD4(CCC_Float, "r2_ls_bloom_kernel_b", &ps_r2_ls_bloom_kernel_b, 0.01f, 1.0f);
	CMD4(CCC_Float, "r2_ls_bloom_threshold", &ps_r2_ls_bloom_threshold, 0.0f, 1.0f);
	CMD4(CCC_Float, "r2_ls_bloom_speed", &ps_r2_ls_bloom_speed, 0.0f, 100.0f);
	CMD3(CCC_Mask, "r2_ls_bloom_fast", &ps_r2_ls_flags, R2FLAG_FASTBLOOM);
	CMD4(CCC_Float, "r2_ls_dsm_kernel", &ps_r2_ls_dsm_kernel, 0.1f, 3.0f);
	CMD4(CCC_Float, "r2_ls_psm_kernel", &ps_r2_ls_psm_kernel, 0.1f, 3.0f);
	CMD4(CCC_Float, "r2_ls_ssm_kernel", &ps_r2_ls_ssm_kernel, 0.1f, 3.0f);
	CMD4(CCC_Float, "r2_ls_squality", &ps_r2_ls_squality, 0.5f, 1.0f);

	CMD3(CCC_Mask, "r2_zfill", &ps_r2_ls_flags, R2FLAG_ZFILL);
	CMD4(CCC_Float, "r2_zfill_depth", &ps_r2_zfill, 0.001f, 0.5f);
	CMD3(CCC_Mask, "r2_allow_r1_lights", &ps_r2_ls_flags, R2FLAG_R1LIGHTS);

	CMD4(CCC_Float, "r2_gloss_factor", &ps_r2_gloss_factor, 0.0f, 10.0f);

#ifdef DEBUG
	CMD3(CCC_Mask, "r2_use_nvdbt", &ps_r2_ls_flags, R2FLAG_USE_NVDBT);
	CMD3(CCC_Mask, "r2_mt", &ps_r2_ls_flags, R2FLAG_EXP_MT_CALC);
#endif // DEBUG

	CMD3(CCC_Mask, "r2_sun", &ps_r2_ls_flags, R2FLAG_SUN);
	CMD3(CCC_Mask, "r2_sun_details", &ps_r2_ls_flags, R2FLAG_SUN_DETAILS);
	CMD3(CCC_Mask, "r2_sun_focus", &ps_r2_ls_flags, R2FLAG_SUN_FOCUS);
	CMD3(CCC_Mask, "r2_sun_tsm", &ps_r2_ls_flags, R2FLAG_SUN_TSM);
	CMD4(CCC_Float, "r2_sun_tsm_proj", &ps_r2_sun_tsm_projection, 0.001f, 0.8f);
	CMD4(CCC_Float, "r2_sun_tsm_bias", &ps_r2_sun_tsm_bias, -0.5f, +0.5f);
	CMD4(CCC_Float, "r2_sun_near", &ps_r2_sun_near, 1.0f, 50.0f);
	CMD4(CCC_Float, "r2_sun_near_border", &ps_r2_sun_near_border, 0.5f, 1.0f);
	CMD4(CCC_Float, "r2_sun_depth_far_scale", &ps_r2_sun_depth_far_scale, 0.5f, 1.5f);
	CMD4(CCC_Float, "r2_sun_depth_far_bias", &ps_r2_sun_depth_far_bias, -0.5f, +0.5f);
	CMD4(CCC_Float, "r2_sun_depth_near_scale", &ps_r2_sun_depth_near_scale, 0.5f, 1.5f);
	CMD4(CCC_Float, "r2_sun_depth_near_bias", &ps_r2_sun_depth_near_bias, -0.5f, +0.5f);
	CMD4(CCC_Float, "r2_sun_lumscale", &ps_r2_sun_lumscale, -1.0f, +3.0f);
	CMD4(CCC_Float, "r2_sun_lumscale_hemi", &ps_r2_sun_lumscale_hemi, 0.0f, +3.0f);
	CMD4(CCC_Float, "r2_sun_lumscale_amb", &ps_r2_sun_lumscale_amb, 0.0f, +3.0f);

	CMD3(CCC_Mask, "r2_aa", &ps_r2_ls_flags, R2FLAG_AA);
	CMD4(CCC_Float, "r2_aa_kernel", &ps_r2_aa_kernel, 0.3f, 0.7f);
	CMD4(CCC_Float, "r2_mblur", &ps_r2_mblur, 0.0f, 1.0f);

	CMD3(CCC_Mask, "r2_gi", &ps_r2_ls_flags, R2FLAG_GI);
	CMD4(CCC_Float, "r2_gi_clip", &ps_r2_GI_clip, 0.00001f, 0.1f);
	CMD4(CCC_Integer, "r2_gi_depth", &ps_r2_GI_depth, 1, 5);
	CMD4(CCC_Integer, "r2_gi_photons", &ps_r2_GI_photons, 8, 256);
	CMD4(CCC_Float, "r2_gi_refl", &ps_r2_GI_refl, EPSILON_3, 0.99f);

	CMD4(CCC_Integer, "r2_wait_sleep", &ps_r2_wait_sleep, 0, 1);

#ifdef DEBUG
	CMD4(CCC_Integer, "r2_dhemi_count", &ps_r2_dhemi_count, 4, 25);
	CMD4(CCC_Float, "r2_dhemi_scale", &ps_r2_dhemi_scale, 0.5f, 3.0f);
	CMD4(CCC_Float, "r2_dhemi_smooth", &ps_r2_lt_smooth, 0.0f, 10.0f);
#endif // DEBUG

	CMD4(CCC_Float, "r2_ls_depth_scale", &ps_r2_ls_depth_scale, 0.5f, 1.5f);
	CMD4(CCC_Float, "r2_ls_depth_bias", &ps_r2_ls_depth_bias, -0.5f, +0.5f);

	CMD4(CCC_Float, "r2_parallax_h", &ps_r2_df_parallax_h, 0.0f, 0.5f);

	CMD4(CCC_Float, "r2_slight_fade", &ps_r2_slight_fade, 0.02f, 2.0f);

	tw_min.set(0.0f, 0.0f, 0.0f);
	tw_max.set(1.0f, 1.0f, 1.0f);
	CMD4(CCC_Vector3, "r2_aa_break", &ps_r2_aa_barier, tw_min, tw_max);

	tw_min.set(0.0f, 0.0f, 0.0f);
	tw_max.set(1.0f, 1.0f, 1.0f);
	CMD4(CCC_Vector3, "r2_aa_weight", &ps_r2_aa_weight, tw_min, tw_max);
}

void xrRender_apply_tf( )
{
	Console->Execute("r__tf_aniso");

#if RENDER==R_R1
	Console->Execute("r1_tf_mipbias");
#else
	Console->Execute("r2_tf_mipbias");
#endif

}

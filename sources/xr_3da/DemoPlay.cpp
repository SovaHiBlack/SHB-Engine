// CDemoPlay.cpp: implementation of the CDemoPlay class.

#include "stdafx.h"
#include "igame_level.h"
#include "DemoPlay.h"
#include "Console.h"
#include "motion.h"
#include "Render.h"
#include "CameraManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDemoPlay::CDemoPlay(pcstr name, f32 ms, u32 cycles, f32 life_time) : CEffectorCam(cefDemo, life_time)
{
	Msg("*** Playing demo: %s", name);
	Console->Execute("hud_weapon 0");
	if (g_bBenchmark)
	{
		Console->Execute("hud_draw 0");
	}

	fSpeed = ms;
	dwCyclesLeft = cycles ? cycles : 1;

	m_pMotion = 0;
	m_MParam = 0;
	string_path nm;
	string_path fn;
	strcpy_s(nm, sizeof(nm), name);

	if (strext(nm))
	{
		strcpy(strext(nm), ".anm");
	}

	if (FS.exist(fn, "$level$", nm) || FS.exist(fn, "$game_anims$", nm))
	{
		m_pMotion = xr_new<COMotion>( );
		m_pMotion->LoadMotion(fn);
		m_MParam = xr_new<SAnimParams>( );
		m_MParam->Set(m_pMotion);
		m_MParam->Play( );
	}
	else
	{
		if (!FS.exist(name))
		{
			g_pGameLevel->Cameras( ).RemoveCamEffector(cefDemo);
			return;
		}

		IReader* fs = FS.r_open(name);
		u32 sz = fs->length( );
		if (sz % sizeof(fMatrix4x4) != 0)
		{
			FS.r_close(fs);
			g_pGameLevel->Cameras( ).RemoveCamEffector(cefDemo);
			return;
		}

		seq.resize(sz / sizeof(fMatrix4x4));
		m_count = seq.size( );
		CopyMemory(&*seq.begin( ), fs->pointer( ), sz);
		FS.r_close(fs);
		Log("~ Total key-frames: ", m_count);
	}

	stat_started = FALSE;
	Device.PreCache(50);
}

CDemoPlay::~CDemoPlay( )
{
	stat_Stop( );
	xr_delete(m_pMotion);
	xr_delete(m_MParam);
	Console->Execute("hud_weapon 1");
	if (g_bBenchmark)
	{
		Console->Execute("hud_draw 1");
	}
}

void CDemoPlay::stat_Start( )
{
	if (stat_started)
	{
		return;
	}

	stat_started = TRUE;
	Sleep(1);
	stat_StartFrame = Device.dwFrame;
	stat_Timer_frame.Start( );
	stat_Timer_total.Start( );
	stat_table.clear( );
	stat_table.reserve(1024);
	fStartTime = 0.0f;
}

extern string512		g_sBenchmarkName;

void CDemoPlay::stat_Stop( )
{
	if (!stat_started)
	{
		return;
	}

	stat_started = FALSE;
	f32 stat_total = stat_Timer_total.GetElapsed_sec( );

	f32 rfps_min;
	f32 rfps_max;
	f32 rfps_middlepoint;
	f32 rfps_average;

	// total
	u32	dwFramesTotal = Device.dwFrame - stat_StartFrame;
	rfps_average = f32(dwFramesTotal) / stat_total;

	// min/max/average
	rfps_min = flt_max;
	rfps_max = flt_min;
	rfps_middlepoint = 0.0f;
	for (u32 it = 1; it < stat_table.size( ); it++)
	{
		f32	fps = 1.0f / stat_table[it];
		if (fps < rfps_min)
		{
			rfps_min = fps;
		}

		if (fps > rfps_max)
		{
			rfps_max = fps;
		}

		rfps_middlepoint += fps;
	}

	rfps_middlepoint /= f32(stat_table.size( ) - 1);

	Msg("* [DEMO] FPS: average[%f], min[%f], max[%f], middle[%f]", rfps_average, rfps_min, rfps_max, rfps_middlepoint);

	if (g_bBenchmark)
	{
		string_path fname;
		if (xr_strlen(g_sBenchmarkName))
		{
			sprintf_s(fname, sizeof(fname), "%s.result", g_sBenchmarkName);
		}
		else
		{
			strcpy_s(fname, sizeof(fname), "benchmark.result");
		}

		FS.update_path(fname, "$app_data_root$", fname);
		CIniFile res(fname, FALSE, FALSE, TRUE);
		res.w_float("general", "renderer", f32(::Render->get_generation( )) / 10.0f, "dx-level required");
		res.w_float("general", "min", rfps_min, "absolute minimum");
		res.w_float("general", "max", rfps_max, "absolute maximum");
		res.w_float("general", "average", rfps_average, "average for this run");
		res.w_float("general", "middle", rfps_middlepoint, "per-frame middle-point");
		for (u32 it = 1; it < stat_table.size( ); it++)
		{
			string32 id;
			sprintf_s(id, sizeof(id), "%7d", it);
			for (u32 c = 0; id[c]; c++)
			{
				if (' ' == id[c])
				{
					id[c] = '0';
				}
			}

			res.w_float("per_frame_stats", id, 1.0f / stat_table[it]);
		}

		Console->Execute("quit");
	}
}

#define FIX(a) while (a>=m_count) a-=m_count
void spline1(f32 t, fVector3* p, fVector3* ret)
{
	f32 t2 = t * t;
	f32 t3 = t2 * t;
	f32 m[4];

	ret->x = 0.0f;
	ret->y = 0.0f;
	ret->z = 0.0f;
	m[0] = (0.5f * ((-1.0f * t3) + (2.0f * t2) + (-1.0f * t)));
	m[1] = (0.5f * ((3.0f * t3) + (-5.0f * t2) + (0.0f * t) + 2.0f));
	m[2] = (0.5f * ((-3.0f * t3) + (4.0f * t2) + (1.0f * t)));
	m[3] = (0.5f * ((1.0f * t3) + (-1.0f * t2) + (0.0f * t)));

	for (s32 i = 0; i < 4; i++)
	{
		ret->x += p[i].x * m[i];
		ret->y += p[i].y * m[i];
		ret->z += p[i].z * m[i];
	}
}

BOOL CDemoPlay::Process(fVector3& P, fVector3& D, fVector3& N, f32& fFov, f32& fFar, f32& fAspect)
{
	// skeep a few frames before counting
	if (Device.dwPrecacheFrame)
	{
		return TRUE;
	}

	stat_Start( );

	// Per-frame statistics
	{
		stat_table.push_back(stat_Timer_frame.GetElapsed_sec( ));
		stat_Timer_frame.Start( );
	}

	// Process motion
	if (m_pMotion)
	{
		fVector3 R;
		fMatrix4x4 mRotate;
		m_pMotion->_Evaluate(m_MParam->Frame( ), P, R);
		m_MParam->Update(Device.fTimeDelta, 1.0f, true);
		fLifeTime -= Device.fTimeDelta;
		if (m_MParam->bWrapped)
		{
			stat_Stop( );
			stat_Start( );
		}

		mRotate.setXYZi(R.x, R.y, R.z);
		D.set(mRotate.k);
		N.set(mRotate.j);
	}
	else
	{
		if (seq.empty( ))
		{
			g_pGameLevel->Cameras( ).RemoveCamEffector(cefDemo);
			return TRUE;
		}

		fStartTime += Device.fTimeDelta;

		f32 ip;
		f32 p = fStartTime / fSpeed;
		f32 t = modff(p, &ip);
		s32 frame = iFloor(ip);
		VERIFY(t >= 0.0f);

		if (frame >= m_count)
		{
			dwCyclesLeft--;
			if (0 == dwCyclesLeft)
			{
				return FALSE;
			}

			fStartTime = 0.0f;
		}

		s32 f1 = frame;
		FIX(f1);
		s32 f2 = f1 + 1;
		FIX(f2);
		s32 f3 = f2 + 1;
		FIX(f3);
		s32 f4 = f3 + 1;
		FIX(f4);

		fMatrix4x4* m1;
		fMatrix4x4* m2;
		fMatrix4x4* m3;
		fMatrix4x4* m4;
		fVector3 v[4];
		m1 = (fMatrix4x4*)&seq[f1];
		m2 = (fMatrix4x4*)&seq[f2];
		m3 = (fMatrix4x4*)&seq[f3];
		m4 = (fMatrix4x4*)&seq[f4];

		for (s32 i = 0; i < 4; i++)
		{
			v[0].x = m1->m[i][0]; v[0].y = m1->m[i][1];  v[0].z = m1->m[i][2];
			v[1].x = m2->m[i][0]; v[1].y = m2->m[i][1];  v[1].z = m2->m[i][2];
			v[2].x = m3->m[i][0]; v[2].y = m3->m[i][1];  v[2].z = m3->m[i][2];
			v[3].x = m4->m[i][0]; v[3].y = m4->m[i][1];  v[3].z = m4->m[i][2];
			spline1(t, &(v[0]), (fVector3*)&(Device.mView.m[i][0]));
		}

		fMatrix4x4 mInvCamera;
		mInvCamera.invert(Device.mView);
		N.set(mInvCamera._21, mInvCamera._22, mInvCamera._23);
		D.set(mInvCamera._31, mInvCamera._32, mInvCamera._33);
		P.set(mInvCamera._41, mInvCamera._42, mInvCamera._43);

		fLifeTime -= Device.fTimeDelta;
	}

	return TRUE;
}

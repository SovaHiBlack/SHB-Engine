// DemoRecord.cpp: implementation of the CDemoRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"
#include "x_ray.h"

#include "gamefont.h"
#include "DemoRecord.h"
#include "Console.h"
#include "Input.h"
#include "Object.h"
#include "Render.h"
#include "CustomHUD.h"

CDemoRecord* xrDemoRecord = 0;

CDemoRecord::CDemoRecord(pcstr name, f32 life_time) : CEffectorCam(cefDemo, life_time/*,FALSE*/)
{
	_unlink(name);
	file = FS.w_open(name);
	if (file)
	{
		IR_Capture( );	// capture input
		m_Camera.invert(Device.mView);

		// parse yaw
		fVector3& dir = m_Camera.k;
		fVector3 DYaw;
		DYaw.set(dir.x, 0.0f, dir.z);
		DYaw.normalize_safe( );
		if (DYaw.x < 0.0f)
		{
			m_HPB.x = acosf(DYaw.z);
		}
		else
		{
			m_HPB.x = 2 * PI - acosf(DYaw.z);
		}

		// parse pitch
		dir.normalize_safe( );
		m_HPB.y = asinf(dir.y);
		m_HPB.z = 0.0f;

		m_Position.set(m_Camera.c);

		m_vVelocity.set(0.0f, 0.0f, 0.0f);
		m_vAngularVelocity.set(0.0f, 0.0f, 0.0f);
		iCount = 0;

		m_vT.set(0.0f, 0.0f, 0.0f);
		m_vR.set(0.0f, 0.0f, 0.0f);
		m_bMakeCubeMap = FALSE;
		m_bMakeScreenshot = FALSE;
		m_bMakeLevelMap = FALSE;

		m_fSpeed0 = pSettings->r_float("demo_record", "speed0");
		m_fSpeed1 = pSettings->r_float("demo_record", "speed1");
		m_fSpeed2 = pSettings->r_float("demo_record", "speed2");
		m_fSpeed3 = pSettings->r_float("demo_record", "speed3");
		m_fAngSpeed0 = pSettings->r_float("demo_record", "ang_speed0");
		m_fAngSpeed1 = pSettings->r_float("demo_record", "ang_speed1");
		m_fAngSpeed2 = pSettings->r_float("demo_record", "ang_speed2");
		m_fAngSpeed3 = pSettings->r_float("demo_record", "ang_speed3");
	}
	else
	{
		fLifeTime = -1.0f;
	}

	m_bOverlapped = FALSE;
}

CDemoRecord::~CDemoRecord( )
{
	if (file)
	{
		IR_Release( );	// release input
		FS.w_close(file);
	}
}

//										+X,					-X,					+Y,					-Y,					+Z,					-Z
static fVector3 cmNorm[6] = {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
static fVector3 cmDir[6] = {{1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}};

static flags32	s_hud_flag = {0};
static flags32	s_dev_flags = {0};

void CDemoRecord::MakeScreenshotFace( )
{
	switch (m_Stage)
	{
		case 0:
		{
			s_hud_flag.assign(psHUD_Flags);
			psHUD_Flags.assign(0);
		}
		break;
		case 1:
		{
			Render->Screenshot( );
			psHUD_Flags.assign(s_hud_flag);
			m_bMakeScreenshot = FALSE;
		}
		break;
	}

	m_Stage++;
}

INT g_bDR_LM_UsePointsBBox = 0;
INT g_bDR_LM_4Steps = 0;
INT g_iDR_LM_Step = 0;
fVector3 g_DR_LM_Min;
fVector3 g_DR_LM_Max;

void GetLM_BBox(fBox3& bb, INT Step)
{
	f32 half_x = bb.min.x + (bb.max.x - bb.min.x) / 2;
	f32 half_z = bb.min.z + (bb.max.z - bb.min.z) / 2;
	switch (Step)
	{
		case 0:
		{
			bb.max.x = half_x;
			bb.min.z = half_z;
		}
		break;
		case 1:
		{
			bb.min.x = half_x;
			bb.min.z = half_z;
		}
		break;
		case 2:
		{
			bb.max.x = half_x;
			bb.max.z = half_z;
		}
		break;
		case 3:
		{
			bb.min.x = half_x;
			bb.max.z = half_z;
		}
		break;
		default:
		{
		}
		break;
	}
}

void CDemoRecord::MakeLevelMapProcess( )
{
	switch (m_Stage)
	{
		case 0:
		{
			s_dev_flags = psDeviceFlags;
			psDeviceFlags.zero( );
			psDeviceFlags.set(rsClearBB | rsFullscreen | rsDrawStatic, TRUE);
			if (!psDeviceFlags.equal(s_dev_flags, rsFullscreen))
			{
				Device.Reset( );
			}
		}
		break;
		case DEVICE_RESET_PRECACHE_FRAME_COUNT + 1:
		{
			m_bOverlapped = TRUE;
			s_hud_flag.assign(psHUD_Flags);
			psHUD_Flags.assign(0);

			fBox3 bb = g_pGameLevel->ObjectSpace.GetBoundingVolume( );

			if (g_bDR_LM_UsePointsBBox)
			{
				bb.max.x = g_DR_LM_Max.x;
				bb.max.z = g_DR_LM_Max.z;

				bb.min.x = g_DR_LM_Min.x;
				bb.min.z = g_DR_LM_Min.z;
			}

			if (g_bDR_LM_4Steps)
			{
				GetLM_BBox(bb, g_iDR_LM_Step);
			}

			// build camera matrix
			bb.getcenter(Device.vCameraPosition);

			Device.vCameraDirection.set(0.0f, -1.0f, 0.0f);
			Device.vCameraTop.set(0.0f, 0.0f, 1.0f);
			Device.vCameraRight.set(1.0f, 0.0f, 0.0f);
			Device.mView.build_camera_dir(Device.vCameraPosition, Device.vCameraDirection, Device.vCameraTop);

			bb.xform(Device.mView);
			// build project matrix
			Device.mProject.build_projection_ortho(bb.max.x - bb.min.x, bb.max.y - bb.min.y, bb.min.z, bb.max.z);
		}
		break;
		case DEVICE_RESET_PRECACHE_FRAME_COUNT + 2:
		{
			m_bOverlapped = FALSE;
			string_path tmp;
			fBox3 bb = g_pGameLevel->ObjectSpace.GetBoundingVolume( );

			if (g_bDR_LM_UsePointsBBox)
			{
				bb.max.x = g_DR_LM_Max.x;
				bb.max.z = g_DR_LM_Max.z;

				bb.min.x = g_DR_LM_Min.x;
				bb.min.z = g_DR_LM_Min.z;
			}

			if (g_bDR_LM_4Steps)
			{
				GetLM_BBox(bb, g_iDR_LM_Step);
			}

			sprintf_s(tmp, sizeof(tmp), "%s_[%3.3f, %3.3f]-[%3.3f, %3.3f]", *g_pGameLevel->name( ), bb.min.x, bb.min.z, bb.max.x, bb.max.z);
			Render->Screenshot(IRender_interface::SM_FOR_LEVELMAP, tmp);
			psHUD_Flags.assign(s_hud_flag);
			BOOL bDevReset = !psDeviceFlags.equal(s_dev_flags, rsFullscreen);
			psDeviceFlags = s_dev_flags;
			if (bDevReset)
			{
				Device.Reset( );
			}

			m_bMakeLevelMap = FALSE;
		}
		break;
	}

	m_Stage++;
}

void CDemoRecord::MakeCubeMapFace(fVector3& D, fVector3& N)
{
	string32 buf;
	switch (m_Stage)
	{
		case 0:
		{
			N.set(cmNorm[m_Stage]);
			D.set(cmDir[m_Stage]);
			s_hud_flag.assign(psHUD_Flags);
			psHUD_Flags.assign(0);
		}
		break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		{
			N.set(cmNorm[m_Stage]);
			D.set(cmDir[m_Stage]);
			Render->Screenshot(IRender_interface::SM_FOR_CUBEMAP, itoa(m_Stage, buf, 10));
		}
		break;
		case 6:
		{
			Render->Screenshot(IRender_interface::SM_FOR_CUBEMAP, itoa(m_Stage, buf, 10));
			N.set(m_Camera.j);
			D.set(m_Camera.k);
			psHUD_Flags.assign(s_hud_flag);
			m_bMakeCubeMap = FALSE;
		}
		break;
	}
	m_Stage++;
}

BOOL CDemoRecord::Process(fVector3& P, fVector3& D, fVector3& N, f32& fFov, f32& fFar, f32& fAspect)
{
	if (0 == file)
	{
		return TRUE;
	}

	if (m_bMakeScreenshot)
	{
		MakeScreenshotFace( );
		// update camera
		N.set(m_Camera.j);
		D.set(m_Camera.k);
		P.set(m_Camera.c);
	}
	else if (m_bMakeLevelMap)
	{
		MakeLevelMapProcess( );
	}
	else if (m_bMakeCubeMap)
	{
		MakeCubeMapFace(D, N);
		P.set(m_Camera.c);
		fAspect = 1.0f;
	}
	else
	{
		if (psHUD_Flags.test(HUD_DRAW))
		{
			if ((Device.dwTimeGlobal / 750) % 3 != 0)
			{
				//				pApp->pFontSystem->SetSizeI	(0.02f);
				pApp->pFontSystem->SetColor(color_rgba(255, 0, 0, 255));
				pApp->pFontSystem->SetAligment(CGameFont::alCenter);
				pApp->pFontSystem->OutSetI(0, -.05f);
				pApp->pFontSystem->OutNext("%s", "RECORDING");
				pApp->pFontSystem->OutNext("Key frames count: %d", iCount);
				pApp->pFontSystem->SetAligment(CGameFont::alLeft);
				pApp->pFontSystem->OutSetI(-0.2f, +.05f);
				pApp->pFontSystem->OutNext("SPACE");
				pApp->pFontSystem->OutNext("BACK");
				pApp->pFontSystem->OutNext("ESC");
				pApp->pFontSystem->OutNext("F11");
				pApp->pFontSystem->OutNext("F12");
				pApp->pFontSystem->SetAligment(CGameFont::alLeft);
				pApp->pFontSystem->OutSetI(0, +.05f);
				pApp->pFontSystem->OutNext("= Append Key");
				pApp->pFontSystem->OutNext("= Cube Map");
				pApp->pFontSystem->OutNext("= Quit");
				pApp->pFontSystem->OutNext("= Level Map ScreenShot");
				pApp->pFontSystem->OutNext("= ScreenShot");
			}
		}

		m_vVelocity.lerp(m_vVelocity, m_vT, 0.3f);
		m_vAngularVelocity.lerp(m_vAngularVelocity, m_vR, 0.3f);

		f32 speed = m_fSpeed1;
		f32 ang_speed = m_fAngSpeed1;
		if (Console->IR_GetKeyState(DIK_LSHIFT))
		{
			speed = m_fSpeed0;
			ang_speed = m_fAngSpeed0;
		}
		else if (Console->IR_GetKeyState(DIK_LALT))
		{
			speed = m_fSpeed2;
			ang_speed = m_fAngSpeed2;
		}
		else if (Console->IR_GetKeyState(DIK_LCONTROL))
		{
			speed = m_fSpeed3;
			ang_speed = m_fAngSpeed3;
		}

		m_vT.mul(m_vVelocity, Device.fTimeDelta * speed);
		m_vR.mul(m_vAngularVelocity, Device.fTimeDelta * ang_speed);

		m_HPB.x -= m_vR.y;
		m_HPB.y -= m_vR.x;
		m_HPB.z += m_vR.z;

		// move
		fVector3 vmove;

		vmove.set(m_Camera.k);
		vmove.normalize_safe( );
		vmove.mul(m_vT.z);
		m_Position.add(vmove);

		vmove.set(m_Camera.i);
		vmove.normalize_safe( );
		vmove.mul(m_vT.x);
		m_Position.add(vmove);

		vmove.set(m_Camera.j);
		vmove.normalize_safe( );
		vmove.mul(m_vT.y);
		m_Position.add(vmove);

		m_Camera.setHPB(m_HPB.x, m_HPB.y, m_HPB.z);
		m_Camera.translate_over(m_Position);

		// update camera
		N.set(m_Camera.j);
		D.set(m_Camera.k);
		P.set(m_Camera.c);

		fLifeTime -= Device.fTimeDelta;

		m_vT.set(0.0f, 0.0f, 0.0f);
		m_vR.set(0.0f, 0.0f, 0.0f);
	}

	return TRUE;
}

void CDemoRecord::IR_OnKeyboardPress(s32 dik)
{
	if (dik == DIK_GRAVE)
	{
		Console->Show( );
	}

	if (dik == DIK_SPACE)
	{
		RecordKey( );
	}

	if (dik == DIK_BACK)
	{
		MakeCubemap( );
	}

	if (dik == DIK_F11)
	{
		MakeLevelMapScreenshot( );
	}

	if (dik == DIK_F12)
	{
		MakeScreenshot( );
	}

	if (dik == DIK_ESCAPE)
	{
		fLifeTime = -1;
	}

	if (dik == DIK_RETURN)
	{
		if (g_pGameLevel->CurrentEntity( ))
		{

#ifndef NDEBUG
			g_pGameLevel->CurrentEntity( )->ForceTransform(m_Camera);
#endif

			fLifeTime = -1.0f;
		}
	}

	if (dik == DIK_PAUSE)
	{
		Device.Pause(!Device.Paused( ), TRUE, TRUE, "demo_record");
	}
}

void CDemoRecord::IR_OnKeyboardHold(s32 dik)
{
	switch (dik)
	{
		case DIK_A:
		case DIK_NUMPAD1:
		case DIK_LEFT:			// Slide Left
		{
			m_vT.x -= 1.0f;
		}
		break;
		case DIK_D:
		case DIK_NUMPAD3:
		case DIK_RIGHT:			// Slide Right
		{
			m_vT.x += 1.0f;
		}
		break;
		case DIK_S:				// Slide Down
		{
			m_vT.y -= 1.0f;
		}
		break;
		case DIK_W:				// Slide Up
		{
			m_vT.y += 1.0f;
		}
		break;
		// rotate	
		case DIK_NUMPAD2:		// Pitch Down
		{
			m_vR.x -= 1.0f;
		}
		break;
		case DIK_NUMPAD8:		// Pitch Up
		{
			m_vR.x += 1.0f;
		}
		break;
		case DIK_E:
		case DIK_NUMPAD6:		// Turn Right
		{
			m_vR.y += 1.0f;
		}
		break;
		case DIK_Q:
		case DIK_NUMPAD4:		// Turn Left
		{
			m_vR.y -= 1.0f;
		}
		break;
		case DIK_NUMPAD9:		// Double Turn Right
		{
			m_vR.z -= 2.0f;
		}
		break;
		case DIK_NUMPAD7:		// Double Turn Left
		{
			m_vR.z += 2.0f;
		}
		break;
	}
}

void CDemoRecord::IR_OnMouseMove(s32 dx, s32 dy)
{
	f32 scale = .5f;//psMouseSens;
	if (dx || dy)
	{
		m_vR.y += f32(dx) * scale; // heading
		m_vR.x += ((psMouseInvert.test(1)) ? -1 : 1) * f32(dy) * scale * (3.f / 4.f); // pitch
	}
}

void CDemoRecord::IR_OnMouseHold(s32 btn)
{
	switch (btn)
	{
		case 0:					// Move Backward
		{
			m_vT.z += 1.0f;
		}
		break;
		case 1:					 // Move Forward
		{
			m_vT.z -= 1.0f;
		}
		break;
	}
}

void CDemoRecord::RecordKey( )
{
	fMatrix4x4			g_matView;

	g_matView.invert(m_Camera);
	file->w(&g_matView, sizeof(fMatrix4x4));
	iCount++;
}

void CDemoRecord::MakeCubemap( )
{
	m_bMakeCubeMap = TRUE;
	m_Stage = 0;
}

void CDemoRecord::MakeScreenshot( )
{
	m_bMakeScreenshot = TRUE;
	m_Stage = 0;
}

void CDemoRecord::MakeLevelMapScreenshot( )
{
	m_bMakeLevelMap = TRUE;
	m_Stage = 0;
}

// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:13:00 , by user : Oles , from computer : OLES
// HUDCursor.cpp: implementation of the CHUDTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hudtarget.h"
#include "hudmanager.h"
#include "GameMtlLib.h"

#include "..\XR_3DA\Environment.h"
#include "..\XR_3DA\CustomHUD.h"
#include "Entity.h"
#include "level.h"
#include "game_cl_base.h"
#include "..\XR_3DA\igame_persistent.h"

#include "InventoryOwner.h"
#include "RelationRegistry.h"
#include "CharacterInfo.h"

#include "StringTable.h"
#include "EntityAlive.h"

//#include "InventoryItem.h"
#include "Inventory.h"

u32 C_ON_ENEMY		D3DCOLOR_XRGB(0xff, 0, 0);
u32 C_ON_NEUTRAL	D3DCOLOR_XRGB(0xff, 0xff, 0x80);
u32 C_ON_FRIEND		D3DCOLOR_XRGB(0, 0xff, 0);

#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)
#define C_SIZE		0.025f
#define NEAR_LIM	0.5f

#define SHOW_INFO_SPEED		0.5f
#define HIDE_INFO_SPEED		10.f

IC	f32	recon_mindist( )
{
	return 2.0f;
}
IC	f32	recon_maxdist( )
{
	return 50.0f;
}
IC	f32	recon_minspeed( )
{
	return 0.5f;
}
IC	f32	recon_maxspeed( )
{
	return 10.0f;
}

CHUDTarget::CHUDTarget( )
{
	fuzzyShowInfo = 0.0f;
	RQ.range = 0.0f;
	hGeom.create(FVF::F_TL, RCache.Vertex.Buffer( ), RCache.QuadIB);
	hShader.create("hud\\cursor", "ui\\cursor");

	RQ.set(NULL, 0.0f, -1);

	Load( );
	m_bShowCrosshair = false;
}

void CHUDTarget::net_Relcase(CObject* O)
{
	if (RQ.O == O)
	{
		RQ.O = NULL;
	}

	RQR.r_clear( );
}

void CHUDTarget::Load( )
{
	HUDCrosshair.Load( );
}

ICF static BOOL pick_trace_callback(collide::rq_result& result, pvoid params)
{
	collide::rq_result* RQ = (collide::rq_result*)params;
	if (result.O)
	{
		*RQ = result;
		return FALSE;
	}
	else
	{
		//�������� ����������� � ������ ��� ��������
		CDB::TRI* T = Level( ).ObjectSpace.GetStaticTris( ) + result.element;
		if (GMLib.GetMaterialByIdx(T->material)->Flags.is(SGameMtl::flPassable))
		{
			return TRUE;
		}
	}

	*RQ = result;
	return FALSE;
}

void CHUDTarget::CursorOnFrame( )
{
	fVector3 p1;
	fVector3 dir;

	p1 = Device.vCameraPosition;
	dir = Device.vCameraDirection;

	// Render cursor
	if (Level( ).CurrentEntity( ))
	{
		RQ.O = 0;
		RQ.range = g_pGamePersistent->Environment( ).CurrentEnv.far_plane * 0.99f;
		RQ.element = -1;

		collide::ray_defs	RD(p1, dir, RQ.range, CDB::OPT_CULL, collide::rqtBoth);
		RQR.r_clear( );
		VERIFY(!fis_zero(RD.dir.square_magnitude( )));
		if (Level( ).ObjectSpace.RayQuery(RQR, RD, pick_trace_callback, &RQ, NULL, Level( ).CurrentEntity( )))
		{
			clamp(RQ.range, NEAR_LIM, RQ.range);
		}
	}
}

extern ENGINE_API BOOL g_bRendering;
void CHUDTarget::Render( )
{
	VERIFY(g_bRendering);

	CObject* O = Level( ).CurrentEntity( );
	if (0 == O)
	{
		return;
	}

	CEntity* E = smart_cast<CEntity*>(O);
	if (0 == E)
	{
		return;
	}

	fVector3 p1 = Device.vCameraPosition;
	fVector3 dir = Device.vCameraDirection;

	// Render cursor
	u32 C = C_DEFAULT;

	FVF::TL PT;
	fVector3 p2;
	p2.mad(p1, dir, RQ.range);
	PT.transform(p2, Device.mFullTransform);
	f32 di_size = C_SIZE / powf(PT.p.w, 0.2f);

	CGameFont* F = HUD( ).Font( ).pFontGraffiti19Russian;
	F->SetAligment(CGameFont::alCenter);
	F->OutSetI(0.0f, 0.05f);

	if (psHUD_Flags.test(HUD_CROSSHAIR_DIST))
	{
		F->SetColor(C);
		F->OutNext("%4.1f", RQ.range);
	}

	if (psHUD_Flags.test(HUD_INFO))
	{
		if (RQ.O)
		{
			CEntityAlive* E = smart_cast<CEntityAlive*>	(RQ.O);
			CEntityAlive* pCurEnt = smart_cast<CEntityAlive*>	(Level( ).CurrentEntity( ));
			PIItem			l_pI = smart_cast<PIItem>		(RQ.O);

			CInventoryOwner* our_inv_owner = smart_cast<CInventoryOwner*>(pCurEnt);
			if (E && E->g_Alive( ) && !E->cast_base_monster( ))
			{
				CInventoryOwner* others_inv_owner = smart_cast<CInventoryOwner*>(E);

				if (our_inv_owner && others_inv_owner)
				{
					switch (SRelationRegistry( ).GetRelationType(others_inv_owner, our_inv_owner))
					{
						case ALife::eRelationTypeEnemy:
						{
							C = C_ON_ENEMY;
						}
						break;
						case ALife::eRelationTypeNeutral:
						{
							C = C_ON_NEUTRAL;
						}
						break;
						case ALife::eRelationTypeFriend:
						{
							C = C_ON_FRIEND;
						}
						break;
					}

					if (fuzzyShowInfo > 0.5f)
					{
						CStringTable	strtbl;
						F->SetColor(subst_alpha(C, u8(iFloor(255.0f * (fuzzyShowInfo - 0.5f) * 2.0f))));
						F->OutNext("%s", *strtbl.translate(others_inv_owner->Name( )));
						F->OutNext("%s", *strtbl.translate(others_inv_owner->CharacterInfo( ).Community( ).id( )));
					}
				}

				fuzzyShowInfo += SHOW_INFO_SPEED * Device.fTimeDelta;
			}
			else if (l_pI && our_inv_owner && RQ.range < 2.0f * our_inv_owner->inventory( ).GetTakeDist( ))
			{
				if (fuzzyShowInfo > 0.5f)
				{
					F->SetColor(subst_alpha(C, u8(iFloor(255.f * (fuzzyShowInfo - 0.5f) * 2.f))));
					F->OutNext("%s", l_pI->Name/*Complex*/( ));
				}

				fuzzyShowInfo += SHOW_INFO_SPEED * Device.fTimeDelta;
			}
		}
		else
		{
			fuzzyShowInfo -= HIDE_INFO_SPEED * Device.fTimeDelta;
		}

		clamp(fuzzyShowInfo, 0.0f, 1.0f);
	}

	// ����������� �������� ��� �������
	if (!m_bShowCrosshair)
	{
		// actual rendering
		u32			vOffset;
		FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, hGeom.stride( ), vOffset);

		fVector2		scr_size;
		scr_size.set(f32(Device.dwWidth), f32(Device.dwHeight));
		f32			size_x = scr_size.x * di_size;
		f32			size_y = scr_size.y * di_size;

		size_y = size_x;

		f32			w_2 = scr_size.x / 2.0f;
		f32			h_2 = scr_size.y / 2.0f;

		// Convert to screen coords
		f32 cx = (PT.p.x + 1) * w_2;
		f32 cy = (PT.p.y + 1) * h_2;

		pv->set(cx - size_x, cy + size_y, C, 0.0f, 1.0f);
		++pv;
		pv->set(cx - size_x, cy - size_y, C, 0.0f, 0.0f);
		++pv;
		pv->set(cx + size_x, cy + size_y, C, 1.0f, 1.0f);
		++pv;
		pv->set(cx + size_x, cy - size_y, C, 1.0f, 0.0f);
		++pv;

		// unlock VB and Render it as triangle LIST
		RCache.Vertex.Unlock(4, hGeom.stride( ));
		RCache.set_Shader(hShader);
		RCache.set_Geometry(hGeom);
		RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, 4, 0, 2);
	}
	else
	{
		// ����������� ������
		HUDCrosshair.cross_color = C;
		HUDCrosshair.OnRender( );
	}
}

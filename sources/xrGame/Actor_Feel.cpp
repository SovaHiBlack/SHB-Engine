#include "stdafx.h"
#include "Actor.h"
#include "Weapon.h"
#include "mercuryball.h"
#include "Inventory.h"
#include "hudmanager.h"
#include "CharacterInfo.h"
#include "xr_level_controller.h"
#include "UsableScriptObject.h"
#include "CustomZone.h"
#include "GameMtlLib.h"
#include "ui/UIMainIngameWnd.h"//
#include "Grenade.h"
#include "clsid_game.h"

#include "game_cl_base.h"
#include "Level.h"
#include "..\XR_3DA\CameraBase.h"

#define PICKUP_INFO_COLOR 0xFFDDDDDD

void CActor::feel_touch_new(CObject* O)
{ }

void CActor::feel_touch_delete(CObject* O)
{
	CPhysicsShellHolder* sh = smart_cast<CPhysicsShellHolder*>(O);
	if (sh && sh->character_physics_support( ))
	{
		m_feel_touch_characters--;
	}
}

BOOL CActor::feel_touch_contact(CObject* O)
{
	CInventoryItem* item = smart_cast<CInventoryItem*>(O);
	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(O);

	if (item && item->Useful( ) && !item->object( ).H_Parent( ))
	{
		return TRUE;
	}

	if (inventory_owner && inventory_owner != smart_cast<CInventoryOwner*>(this))
	{
		CPhysicsShellHolder* sh = smart_cast<CPhysicsShellHolder*>(O);
		if (sh && sh->character_physics_support( ))
		{
			m_feel_touch_characters++;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CActor::feel_touch_on_contact(CObject* O)
{
	CCustomZone* custom_zone = smart_cast<CCustomZone*>(O);
	if (!custom_zone)
	{
		return TRUE;
	}

	fSphere sphere;
	sphere.P = Position( );
	sphere.R = EPSILON_3;
	if (custom_zone->inside(sphere))
	{
		return TRUE;
	}

	return FALSE;
}

void CActor::PickupModeOn( )
{
	m_bPickupMode = true;
}

void CActor::PickupModeOff( )
{
	m_bPickupMode = false;
}

ICF static BOOL info_trace_callback(collide::rq_result& result, LPVOID params)
{
	BOOL& bOverlaped = *(BOOL*)params;
	if (result.O)
	{
		if (Level( ).CurrentEntity( ) != result.O)
		{
			return TRUE;
		}
		else
		{
			return TRUE;
		}
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

	bOverlaped = TRUE;
	return FALSE;
}

BOOL CActor::CanPickItem(const CFrustum& frustum, const fVector3& from, CObject* item)
{
	BOOL bOverlaped = FALSE;
	fVector3 dir;
	fVector3 to;
	item->Center(to);
	f32 range = dir.sub(to, from).magnitude( );
	if (range > 0.25f)
	{
		if (frustum.testSphere_dirty(to, item->Radius( )))
		{
			dir.div(range);
			collide::ray_defs RD(from, dir, range, CDB::OPT_CULL, collide::rqtBoth);
			VERIFY(!fis_zero(RD.dir.square_magnitude( )));
			RQR.r_clear( );
			Level( ).ObjectSpace.RayQuery(RQR, RD, info_trace_callback, &bOverlaped, NULL, item);
		}
	}

	return !bOverlaped;
}

void CActor::PickupModeUpdate( )
{
	if (!m_bPickupMode) return;

	//���������� �������
	if (inventory( ).m_pTarget && inventory( ).m_pTarget->Useful( ) &&
		m_pUsableObject && m_pUsableObject->nonscript_usable( ) &&
		!Level( ).m_feel_deny.is_object_denied(smart_cast<CGameObject*>(inventory( ).m_pTarget)))
	{
		CNetPacket P;
		u_EventGen(P, GE_OWNERSHIP_TAKE, ID( ));
		P.w_u16(inventory( ).m_pTarget->object( ).ID( ));
		u_EventSend(P);
	}

	//. ????? GetNearest ?????
	feel_touch_update(Position( ), /*inventory().GetTakeDist()*/m_fPickupInfoRadius);

	CFrustum frustum;
	frustum.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB | FRUSTUM_P_FAR);
	//. slow (ray-query test)
	for (xr_vector<CObject*>::iterator it = feel_touch.begin( ); it != feel_touch.end( ); it++)
	{
		if (CanPickItem(frustum, Device.vCameraPosition, *it))
		{
			PickupInfoDraw(*it);
		}
	}
}

BOOL	g_b_COD_PickUpMode = TRUE;
void	CActor::PickupModeUpdate_COD( )
{
	if (Level( ).CurrentViewEntity( ) != this || !g_b_COD_PickUpMode)
	{
		return;
	}

	if (!g_Alive( ) || eacFirstEye != cam_active)
	{
		HUD( ).GetUI( )->UIMainIngameWnd->SetPickUpItem(NULL);
		return;
	}

	CFrustum frustum;
	frustum.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB | FRUSTUM_P_FAR);

	//---------------------------------------------------------------------------
	ISpatialResult.clear_not_free( );
	g_SpatialSpace->q_frustum(ISpatialResult, 0, STYPE_COLLIDEABLE, frustum);
	//---------------------------------------------------------------------------

	f32 maxlen = 1000.0f;
	CInventoryItem* pNearestItem = NULL;
	for (u32 o_it = 0; o_it < ISpatialResult.size( ); o_it++)
	{
		ISpatial* spatial = ISpatialResult[o_it];
		CInventoryItem* pIItem = smart_cast<CInventoryItem*>(spatial->dcast_CObject( ));
		if (0 == pIItem)
		{
			continue;
		}

		if (pIItem->object( ).H_Parent( ) != NULL)
		{
			continue;
		}

		if (!pIItem->CanTake( ))
		{
			continue;
		}

		if (pIItem->object( ).CLS_ID == CLSID_OBJECT_G_RPG7 || pIItem->object( ).CLS_ID == CLSID_OBJECT_G_FAKE)
		{
			continue;
		}

		CGrenade* pGrenade = smart_cast<CGrenade*>(spatial->dcast_CObject( ));
		if (pGrenade && !pGrenade->Useful( ))
		{
			continue;
		}

		CMissile* pMissile = smart_cast<CMissile*> (spatial->dcast_CObject( ));
		if (pMissile && !pMissile->Useful( ))
		{
			continue;
		}

		fVector3 A;
		fVector3 B;
		fVector3 tmp;
		pIItem->object( ).Center(A);
		if (A.distance_to_sqr(Position( )) > 4)
		{
			continue;
		}

		tmp.sub(A, cam_Active( )->vPosition);
		B.mad(cam_Active( )->vPosition, cam_Active( )->vDirection, tmp.dotproduct(cam_Active( )->vDirection));
		f32 len = B.distance_to_sqr(A);
		if (len > 1)
		{
			continue;
		}

		if (maxlen > len && !pIItem->object( ).getDestroy( ))
		{
			maxlen = len;
			pNearestItem = pIItem;
		}
	}

	if (pNearestItem)
	{
		CFrustum frustum;
		frustum.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB | FRUSTUM_P_FAR);
		if (!CanPickItem(frustum, Device.vCameraPosition, &pNearestItem->object( )))
		{
			pNearestItem = NULL;
		}
	}

	if (pNearestItem && pNearestItem->cast_game_object( ))
	{
		if (Level( ).m_feel_deny.is_object_denied(pNearestItem->cast_game_object( )))
		{
			pNearestItem = NULL;
		}
	}

	HUD( ).GetUI( )->UIMainIngameWnd->SetPickUpItem(pNearestItem);

	if (pNearestItem && m_bPickupMode)
	{
		//���������� �������
		Game( ).SendPickUpEvent(ID( ), pNearestItem->object( ).ID( ));

		PickupModeOff( );
	}
}

void CActor::PickupInfoDraw(CObject* object)
{
	pcstr draw_str = NULL;

	CInventoryItem* item = smart_cast<CInventoryItem*>(object);
//.	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(object);
//.	VERIFY(item || inventory_owner);
	if (!item)
	{
		return;
	}

	fMatrix4x4			res;
	res.mul(Device.mFullTransform, object->XFORM( ));
	fVector4		v_res;
	fVector3		shift;

	draw_str = item->Name/*Complex*/( );
	shift.set(0, 0, 0);

	res.transform(v_res, shift);

	if (v_res.z < 0.0f || v_res.w < 0.0f)
	{
		return;
	}

	if (v_res.x < -1.0f || v_res.x > 1.0f || v_res.y < -1.0f || v_res.y > 1.0f)
	{
		return;
	}

	f32 x = (1.0f + v_res.x) / 2.0f * (Device.dwWidth);
	f32 y = (1.0f - v_res.y) / 2.0f * (Device.dwHeight);
	
	HUD( ).Font( ).pFontLetterica16Russian->SetAligment(CGameFont::alCenter);
	HUD( ).Font( ).pFontLetterica16Russian->SetColor(PICKUP_INFO_COLOR);
	HUD( ).Font( ).pFontLetterica16Russian->Out(x, y, draw_str);
}

void CActor::feel_sound_new(CObject* who, s32 type, CSound_UserDataPtr user_data, const fVector3& Position, f32 power)
{
	if (who == this)
	{
		m_snd_noise = _max(m_snd_noise, power);
	}
}

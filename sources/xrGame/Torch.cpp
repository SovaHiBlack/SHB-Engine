#include "stdafx.h"
#include "Torch.h"
#include "Entity.h"
#include "Actor.h"
#include "..\XR_3DA\LightAnimLibrary.h"
#include "PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
#include "ai_sounds.h"

#include "HUDManager.h"
#include "level.h"
#include "..\XR_3DA\skeletoncustom.h"
#include "..\XR_3DA\camerabase.h"
#include "Inventory.h"
#include "game_base_space.h"

#include "ui/UIGameCustom.h"
#include "ActorEffector.h"
#include "CustomOutfit.h"

static const f32		TIME_2_HIDE = 5.0f;
static const f32		TORCH_INERTION_CLAMP = PI_DIV_6;
static const f32		TORCH_INERTION_SPEED_MAX = 7.5f;
static const f32		TORCH_INERTION_SPEED_MIN = 0.5f;
static const fVector3	TORCH_OFFSET = { -0.2f,+0.1f,-0.3f };
static const fVector3	OMNI_OFFSET = { -0.2f,+0.1f,-0.1f };
static const f32		OPTIMIZATION_DISTANCE = 100.0f;

static bool stalker_use_dynamic_lights = false;

CTorch::CTorch( )
{
	light_render = ::Render->light_create( );
	light_render->set_type(IRender_Light::SPOT);
	light_render->set_shadow(true);
	light_omni = ::Render->light_create( );
	light_omni->set_type(IRender_Light::POINT);
	light_omni->set_shadow(false);

	m_switched_on = false;
	glow_render = ::Render->glow_create( );
	lanim = 0;
	time2hide = 0.0f;
	fBrightness = 1.0f;

	m_prev_hp.set(0.0f, 0.0f);
	m_delta_h = 0.0f;
}

CTorch::~CTorch( )
{
	light_render.destroy( );
	light_omni.destroy( );
	glow_render.destroy( );
	SHudSound::DestroySound(m_NightVisionOnSnd);
	SHudSound::DestroySound(m_NightVisionOffSnd);
	SHudSound::DestroySound(m_NightVisionIdleSnd);
	SHudSound::DestroySound(m_NightVisionBrokenSnd);
}

IC bool CTorch::can_use_dynamic_lights( )
{
	if (!H_Parent( ))
	{
		return true;
	}

	CInventoryOwner* owner = smart_cast<CInventoryOwner*>(H_Parent( ));
	if (!owner)
	{
		return true;
	}

	return owner->can_use_dynamic_lights( );
}

void CTorch::Load(pcstr section)
{
	inherited::Load(section);
	light_trace_bone = pSettings->r_string(section, "light_trace_bone");

	m_bNightVisionEnabled = !!pSettings->r_bool(section, "night_vision");
	if (m_bNightVisionEnabled)
	{
		SHudSound::LoadSound(section, "snd_night_vision_on", m_NightVisionOnSnd, eST_ITEM_USING);
		SHudSound::LoadSound(section, "snd_night_vision_off", m_NightVisionOffSnd, eST_ITEM_USING);
		SHudSound::LoadSound(section, "snd_night_vision_idle", m_NightVisionIdleSnd, eST_ITEM_USING);
		SHudSound::LoadSound(section, "snd_night_vision_broken", m_NightVisionBrokenSnd, eST_ITEM_USING);
	}
}

void CTorch::SwitchNightVision( )
{
	if (OnClient( ))
	{
		return;
	}

	SwitchNightVision(!m_bNightVisionOn);
}

void CTorch::SwitchNightVision(bool vision_on)
{
	if (!m_bNightVisionEnabled)
	{
		return;
	}

	if (vision_on)
	{
		m_bNightVisionOn = true;
	}
	else
	{
		m_bNightVisionOn = false;
	}

	CActor* pA = smart_cast<CActor*>(H_Parent( ));

	if (!pA)
	{
		return;
	}

	bool bPlaySoundFirstPerson = (pA == Level( ).CurrentViewEntity( ));

	pcstr disabled_names = pSettings->r_string(cNameSect( ), "disabled_maps");
	pcstr curr_map = *Level( ).name( );
	u32 cnt = _GetItemCount(disabled_names);
	bool b_allow = true;
	string512				tmp;
	for (u32 i = 0; i < cnt; ++i)
	{
		_GetItem(disabled_names, i, tmp);
		if (0 == stricmp(tmp, curr_map))
		{
			b_allow = false;
			break;
		}
	}

	CCustomOutfit* pCO = pA->GetOutfit( );
	if (pCO && pCO->m_NightVisionSect.size( ) && !b_allow)
	{
		SHudSound::PlaySound(m_NightVisionBrokenSnd, pA->Position( ), pA, bPlaySoundFirstPerson);
		return;
	}

	if (m_bNightVisionOn)
	{
		CEffectorPP* pp = pA->Cameras( ).GetPPEffector((EEffectorPPType) effNightvision);
		if (!pp)
		{
			if (pCO && pCO->m_NightVisionSect.size( ))
			{
				AddEffector(pA, effNightvision, pCO->m_NightVisionSect);
				SHudSound::PlaySound(m_NightVisionOnSnd, pA->Position( ), pA, bPlaySoundFirstPerson);
				SHudSound::PlaySound(m_NightVisionIdleSnd, pA->Position( ), pA, bPlaySoundFirstPerson, true);
			}
		}
	}
	else
	{
		CEffectorPP* pp = pA->Cameras( ).GetPPEffector((EEffectorPPType) effNightvision);
		if (pp)
		{
			pp->Stop(1.0f);
			SHudSound::PlaySound(m_NightVisionOffSnd, pA->Position( ), pA, bPlaySoundFirstPerson);
			SHudSound::StopSound(m_NightVisionIdleSnd);
		}
	}
}

void CTorch::UpdateSwitchNightVision( )
{
	if (!m_bNightVisionEnabled)
	{
		return;
	}

	if (OnClient( ))
	{
		return;
	}
}

void CTorch::Switch( )
{
	if (OnClient( ))
	{
		return;
	}

	bool bActive = !m_switched_on;
	Switch(bActive);
}

void CTorch::Switch(bool light_on)
{
	m_switched_on = light_on;
	if (can_use_dynamic_lights( ))
	{
		light_render->set_active(light_on);

		CActor* pA = smart_cast<CActor*>(H_Parent( ));
		if (!pA)
		{
			light_omni->set_active(light_on);
		}
	}

	glow_render->set_active(light_on);

	if (*light_trace_bone)
	{
		CKinematics* pVisual = smart_cast<CKinematics*>(Visual( )); VERIFY(pVisual);
		u16 bi = pVisual->LL_BoneID(light_trace_bone);

		pVisual->LL_SetBoneVisible(bi, light_on, TRUE);
		pVisual->CalculateBones( );

		pVisual->LL_SetBoneVisible(bi, light_on, TRUE); //hack
	}
}

BOOL CTorch::net_Spawn(CSE_Abstract* DC)
{
	CSE_Abstract* e = (CSE_Abstract*) (DC);
	CSE_ALifeItemTorch* torch = smart_cast<CSE_ALifeItemTorch*>(e);
	R_ASSERT(torch);
	cNameVisual_set(torch->get_visual( ));

	R_ASSERT(!CFORM( ));
	R_ASSERT(smart_cast<CKinematics*>(Visual( )));
	collidable.model = xr_new<CCF_Skeleton>(this);

	if (!inherited::net_Spawn(DC))
	{
		return FALSE;
	}

	bool b_r2 = !!psDeviceFlags.test(rsR2);

	CKinematics* K = smart_cast<CKinematics*>(Visual( ));
	CIniFile* pUserData = K->LL_UserData( );
	R_ASSERT3(pUserData, "Empty Torch user data!", torch->get_visual( ));
	lanim = LALib.FindItem(pUserData->r_string("torch_definition", "color_animator"));
	guid_bone = K->LL_BoneID(pUserData->r_string("torch_definition", "guide_bone"));	VERIFY(guid_bone != BI_NONE);

	fColor clr = pUserData->r_fcolor("torch_definition", (b_r2) ? "color_r2" : "color");
	fBrightness = clr.intensity( );
	f32 range = pUserData->r_float("torch_definition", (b_r2) ? "range_r2" : "range");
	light_render->set_color(clr);
	light_render->set_range(range);

	fColor clr_o = pUserData->r_fcolor("torch_definition", (b_r2) ? "omni_color_r2" : "omni_color");
	f32 range_o = pUserData->r_float("torch_definition", (b_r2) ? "omni_range_r2" : "omni_range");
	light_omni->set_color(clr_o);
	light_omni->set_range(range_o);

	light_render->set_cone(deg2rad(pUserData->r_float("torch_definition", "spot_angle")));
	light_render->set_texture(pUserData->r_string("torch_definition", "spot_texture"));

	glow_render->set_texture(pUserData->r_string("torch_definition", "glow_texture"));
	glow_render->set_color(clr);
	glow_render->set_radius(pUserData->r_float("torch_definition", "glow_radius"));

	//��������/��������� �������
	Switch(torch->m_active);
	VERIFY(!torch->m_active || (torch->ID_Parent != 0xffff));

	SwitchNightVision(false);

	m_delta_h = PI_DIV_2 - atan((range * 0.5f) / _abs(TORCH_OFFSET.x));

	return TRUE;
}

void CTorch::net_Destroy( )
{
	Switch(false);
	SwitchNightVision(false);

	inherited::net_Destroy( );
}

void CTorch::OnH_A_Chield( )
{
	inherited::OnH_A_Chield( );
	m_focus.set(Position( ));
}

void CTorch::OnH_B_Independent(bool just_before_destroy)
{
	inherited::OnH_B_Independent(just_before_destroy);
	time2hide = TIME_2_HIDE;

	Switch(false);
	SwitchNightVision(false);

	SHudSound::StopSound(m_NightVisionOnSnd);
	SHudSound::StopSound(m_NightVisionOffSnd);
	SHudSound::StopSound(m_NightVisionIdleSnd);
}

void CTorch::UpdateCL( )
{
	inherited::UpdateCL( );

	UpdateSwitchNightVision( );

	if (!m_switched_on)
	{
		return;
	}

	CBoneInstance& BI = smart_cast<CKinematics*>(Visual( ))->LL_GetBoneInstance(guid_bone);
	fMatrix4x4					M;

	if (H_Parent( ))
	{
		CActor* actor = smart_cast<CActor*>(H_Parent( ));
		if (actor)		smart_cast<CKinematics*>(H_Parent( )->Visual( ))->CalculateBones_Invalidate( );

		if (H_Parent( )->XFORM( ).c.distance_to_sqr(Device.vCameraPosition) < _sqr(OPTIMIZATION_DISTANCE))
		{
			// near camera
			smart_cast<CKinematics*>(H_Parent( )->Visual( ))->CalculateBones( );
			M.mul_43(XFORM( ), BI.mTransform);
		}
		else
		{
			// approximately the same
			M = H_Parent( )->XFORM( );
			H_Parent( )->Center(M.c);
			M.c.y += H_Parent( )->Radius( ) * 2.f / 3.f;
		}

		if (actor)
		{
			m_prev_hp.x = angle_inertion_var(m_prev_hp.x, -actor->cam_FirstEye( )->yaw, TORCH_INERTION_SPEED_MIN, TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);
			m_prev_hp.y = angle_inertion_var(m_prev_hp.y, -actor->cam_FirstEye( )->pitch, TORCH_INERTION_SPEED_MIN, TORCH_INERTION_SPEED_MAX, TORCH_INERTION_CLAMP, Device.fTimeDelta);

			fVector3			dir;
			fVector3 right;
			fVector3 up;
			dir.setHP(m_prev_hp.x + m_delta_h, m_prev_hp.y);
			fVector3::generate_orthonormal_basis_normalized(dir, up, right);

			if (true)
			{
				fVector3 offset = M.c;
				offset.mad(M.i, TORCH_OFFSET.x);
				offset.mad(M.j, TORCH_OFFSET.y);
				offset.mad(M.k, TORCH_OFFSET.z);
				light_render->set_position(offset);

				if (false)
				{
					offset = M.c;
					offset.mad(M.i, OMNI_OFFSET.x);
					offset.mad(M.j, OMNI_OFFSET.y);
					offset.mad(M.k, OMNI_OFFSET.z);
					light_omni->set_position(offset);
				}
			}//if (true)
			glow_render->set_position(M.c);

			if (true)
			{
				light_render->set_rotation(dir, right);

				if (false)
				{
					light_omni->set_rotation(dir, right);
				}
			}//if (true)
			glow_render->set_direction(dir);

		}// if(actor)
		else
		{
			if (can_use_dynamic_lights( ))
			{
				light_render->set_position(M.c);
				light_render->set_rotation(M.k, M.i);

				fVector3 offset = M.c;
				offset.mad(M.i, OMNI_OFFSET.x);
				offset.mad(M.j, OMNI_OFFSET.y);
				offset.mad(M.k, OMNI_OFFSET.z);
				light_omni->set_position(M.c);
				light_omni->set_rotation(M.k, M.i);
			}//if (can_use_dynamic_lights()) 

			glow_render->set_position(M.c);
			glow_render->set_direction(M.k);
		}
	}//if(HParent())
	else
	{
		if (getVisible( ) && m_pPhysicsShell)
		{
			M.mul(XFORM( ), BI.mTransform);

			m_switched_on = false;
			light_render->set_active(false);
			light_omni->set_active(false);
			glow_render->set_active(false);
		}
	}

	if (!m_switched_on)
	{
		return;
	}

	// calc color animator
	if (!lanim)
	{
		return;
	}

	s32						frame;
	// ���������� � ������� BGR
	u32 clr = lanim->CalculateBGR(Device.fTimeGlobal, frame);

	fColor					fclr;
	fclr.set((f32) color_get_B(clr), (f32) color_get_G(clr), (f32) color_get_R(clr), 1.f);
	fclr.mul_rgb(fBrightness / 255.f);
	if (can_use_dynamic_lights( ))
	{
		light_render->set_color(fclr);
		light_omni->set_color(fclr);
	}

	glow_render->set_color(fclr);
}

void CTorch::create_physic_shell( )
{
	CPhysicsShellHolder::create_physic_shell( );
}

void CTorch::activate_physic_shell( )
{
	CPhysicsShellHolder::activate_physic_shell( );
}

void CTorch::setup_physic_shell( )
{
	CPhysicsShellHolder::setup_physic_shell( );
}

void CTorch::net_Export(CNetPacket& P)
{
	inherited::net_Export(P);

	u8 F = 0;
	F |= (m_switched_on ? eTorchActive : 0);
	F |= (m_bNightVisionOn ? eNightVisionActive : 0);
	const CActor* pA = smart_cast<const CActor*>(H_Parent( ));
	if (pA)
	{
		if (pA->attached(this))
		{
			F |= eAttached;
		}
	}

	P.w_u8(F);
}

void CTorch::net_Import(CNetPacket& P)
{
	inherited::net_Import(P);

	u8 F = P.r_u8( );
	bool new_m_switched_on = !!(F & eTorchActive);
	bool new_m_bNightVisionOn = !!(F & eNightVisionActive);

	if (new_m_switched_on != m_switched_on)
	{
		Switch(new_m_switched_on);
	}

	if (new_m_bNightVisionOn != m_bNightVisionOn)
	{
		//		Msg("CTorch::net_Import - NV[%d]", new_m_bNightVisionOn);

		SwitchNightVision(new_m_bNightVisionOn);
	}
}

bool CTorch::can_be_attached( ) const
{
	const CActor* pA = smart_cast<const CActor*>(H_Parent( ));
	if (pA)
	{
		if ((const CTorch*) smart_cast<CTorch*>(pA->inventory( ).m_slots[GetSlot( )].m_pIItem) == this)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

void CTorch::afterDetach( )
{
	inherited::afterDetach( );
	Switch(false);
}

void CTorch::renderable_Render( )
{
	inherited::renderable_Render( );
}

using namespace luabind;

#pragma optimize("s",on)
void CTorch::script_register(lua_State* L)
{
	module(L)
		[
			class_<CTorch, CGameObject>("CTorch")
				.def(constructor<>( ))
		];
}

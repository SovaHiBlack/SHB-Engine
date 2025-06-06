#include "stdafx.h"

#include "Actor.h"
#include "WeaponMounted.h"
#include "..\XR_3DA\CameraBase.h"
#include "ActorEffector.h"
#include "CharacterPhysicsSupport.h"

bool CActor::use_MountedWeapon(CHolderCustom* object)
{
	CHolderCustom* wpn = object;
	if (m_holder)
	{
		if (!wpn || (m_holder == wpn))
		{
			m_holder->detach_Actor( );
			character_physics_support( )->movement( )->CreateCharacter( );
			m_holder = NULL;
		}

		return true;
	}
	else
	{
		if (wpn)
		{
			fVector3 center;
			Center(center);
			if (wpn->Use(Device.vCameraPosition, Device.vCameraDirection, center))
			{
				if (wpn->attach_Actor(this))
				{
					// destroy actor character
					character_physics_support( )->movement( )->DestroyCharacter( );
					PickupModeOff( );
					m_holder = wpn;
					if (pCamBobbing)
					{
						Cameras( ).RemoveCamEffector(eCEBobbing);
						pCamBobbing = NULL;
					}

					return true;
				}
			}
		}
	}

	return false;
}

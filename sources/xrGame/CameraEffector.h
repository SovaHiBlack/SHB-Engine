#pragma once

#include "..\ENGINE\CameraManager.h"
#include "..\ENGINE\effector.h"
#include "..\ENGINE\effectorPP.h"

#define eStartEffectorID		50

#define effHit					(eStartEffectorID + 1)
#define effAlcohol				(eStartEffectorID + 2)
#define effFireHit				(eStartEffectorID + 3)
#define effExplodeHit			(eStartEffectorID + 4)
#define effNightvision			(eStartEffectorID + 5)
#define effPsyHealth			(eStartEffectorID + 6)
#define effControllerAura		(eStartEffectorID + 7)
#define effControllerAura2		(eStartEffectorID + 8)
#define effBigMonsterHit		(eStartEffectorID + 9)

#define eCEFall					((ECameraEffectorType)(cetNext + 1))
#define eCENoise				((ECameraEffectorType)(cetNext + 2))
#define eCEShot					((ECameraEffectorType)(cetNext + 3))
#define eCEZoom					((ECameraEffectorType)(cetNext + 4))
#define eCERecoil				((ECameraEffectorType)(cetNext + 5))
#define eCEBobbing				((ECameraEffectorType)(cetNext + 6))
#define eCEHit					((ECameraEffectorType)(cetNext + 7))
#define eCEUser					((ECameraEffectorType)(cetNext + 11))
#define eCEControllerPsyHit		((ECameraEffectorType)(cetNext + 12))
#define eCEVampire				((ECameraEffectorType)(cetNext + 13))
#define eCEPseudoGigantStep		((ECameraEffectorType)(cetNext + 14))
#define eCEMonsterHit			((ECameraEffectorType)(cetNext + 15))

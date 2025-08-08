#include "stdafx.h"

#include "Artefact.h"
#include "MercuryBallArtefact.h"
#include "GraviArtefact.h"
#include "BlackDropsArtefact.h"
#include "NeedlesArtefact.h"
#include "BastArtefact.h"
#include "BlackGraviArtefact.h"
#include "DummyArtefact.h"
#include "ZudaArtefact.h"
#include "ThornArtefact.h"
#include "FadedBallArtefact.h"
#include "ElectricBallArtefact.h"
#include "RustyHairArtefact.h"
#include "GalantineArtefact.h"

using namespace luabind;

#pragma optimize("s",on)
void CArtefact::script_register(lua_State *L)
{
	module(L)
	[
		class_<CMercuryBallArtefact,CGameObject>("CMercuryBallArtefact").def		(constructor<>()),
		class_<CBlackDropsArtefact,CGameObject>("CBlackDropsArtefact").def		(constructor<>()),
		class_<CBlackGraviArtefact	,CGameObject>("CBlackGraviArtefact").def(constructor<>()),
		class_<CBastArtefact		,CGameObject>("CBastArtefact").def		(constructor<>()),
		class_<CDummyArtefact		,CGameObject>("CDummyArtefact").def		(constructor<>()),
		class_<CZudaArtefact		,CGameObject>("CZudaArtefact").def		(constructor<>()),
		class_<CThornArtefact		,CGameObject>("CThornArtefact").def		(constructor<>()),
		class_<CFadedBallArtefact,CGameObject>("CFadedBallArtefact").def			(constructor<>()),
		class_<CElectricBallArtefact,CGameObject>("CElectricBallArtefact").def		(constructor<>()),
		class_<CRustyHairArtefact	,CGameObject>("CRustyHairArtefact").def	(constructor<>()),
		class_<CGalantineArtefact	,CGameObject>("CGalantineArtefact").def	(constructor<>()),
		class_<CGraviArtefact		,CGameObject>("CGraviArtefact").def		(constructor<>())
	];
}

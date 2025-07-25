////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_register.cpp
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "object_factory_impl.h"

// server entities includes
#include "xrServer_Objects_ALife_All.h"
#include "clsid_game.h"

// client entities includes
#	include "..\XR_3DA\std_classes.h"
#	include "level.h"
#	include "gamepersistent.h"
#	include "hudmanager.h"
#	include "Actor.h"
#	include "spectator.h"

#	include "ai/monsters/flesh/flesh.h"
#	include "ai/monsters/chimera/chimera.h"
#	include "ai/monsters/dog/dog.h"
#	include "ai/stalker/Stalker.h"
#	include "ai/monsters/bloodsucker/bloodsucker.h"
#	include "ai/monsters/boar/boar.h"
#	include "ai/monsters/pseudodog/pseudodog.h"
#	include "ai/monsters/pseudodog/psy_dog.h"
#	include "ai/monsters/Burer/burer.h"
#	include "AI/Monsters/PseudoGigant/PseudoGigant.h"
#	include "ai/monsters/controller/controller.h"
#	include "ai/monsters/poltergeist/poltergeist.h"
#	include "ai/monsters/Zombie/Zombie.h"
#	include "ai/monsters/fracture/fracture.h"
#	include "ai/monsters/Snork/Snork.h"
#	include "ai/monsters/cat/cat.h"
#	include "ai/monsters/Tushkano/Tushkano.h"

#	include "ai/phantom/phantom.h"

#	include "ai/trader/Trader.h"

#	include "ai/crow/ai_crow.h"

#	ifdef DEBUG
#		include "..\XR_3DA\StatGraph.h"
#		include "PHDebug.h"
#	endif // DEBUG

#	include "Hit.h"
#	include "PHDestroyable.h"
#	include "car.h"

#	include "helicopter.h"

#	include "MercuryBall.h"
#	include "BlackDrops.h"
#	include "BlackGraviArtifact.h"
#	include "BastArtifact.h"
#	include "DummyArtifact.h"
#	include "ZudaArtifact.h"
#	include "ThornArtifact.h"
#	include "FadedBall.h"
#	include "ElectricBall.h"
#	include "RustyHairArtifact.h"
#	include "GalantineArtifact.h"
#	include "GraviArtifact.h"

#	include "WeaponFN2000.h"
#	include "WeaponAK74.h"
#	include "WeaponLR300.h"
#	include "WeaponHPSA.h"
#	include "WeaponPM.h"
#	include "WeaponAmmo.h"
#	include "WeaponFORT.h"
#	include "WeaponBinoculars.h"
#	include "WeaponShotgun.h"
#	include "WeaponSVD.h"
#	include "WeaponSVU.h"
#	include "WeaponRPG7.h"
#	include "WeaponVAL.h"
#	include "WeaponVintorez.h"
#	include "WeaponWalther.h"
#	include "WeaponUSP45.h"
#	include "WeaponGroza.h"
#	include "WeaponKnife.h"
#	include "WeaponBM16.h"
#	include "WeaponRG6.h"
#	include "WeaponMounted.h"
#	include "WeaponStatMgun.h"

#	include "Scope.h"
#	include "Silencer.h"
#	include "GrenadeLauncher.h"

#	include "Bolt.h"
#	include "Medkit.h"
#	include "Antirad.h"
#	include "FoodItem.h"
#	include "BottleItem.h"
#	include "ExplosiveItem.h"

#	include "InfoDocument.h"
#	include "AttachableItem.h"

#	include "ScientificOutfit.h"
#	include "StalkerOutfit.h"
#	include "MilitaryOutfit.h"
#	include "ExoOutfit.h"

#	include "F1.h"
#	include "RGD5.h"

#	include "ExplosiveRocket.h"

#	include "CustomZone.h"
#	include "MosquitoBald.h"
#	include "Mincer.h"
#	include "GraviZone.h"
#	include "RadioactiveZone.h"
#	include "LevelChanger.h"
#	include "ScriptZone.h"
#	include "TeamBaseZone.h"
#	include "TorridZone.h"
#	include "VisualZone.h"
#	include "HairsZone.h"
#	include "amebazone.h"
#	include "NoGravityZone.h"
#	include "SimpleDetector.h"

#	include "Torch.h"
#	include "PDA.h"

#	include "Projector.h"

#	include "HangingLamp.h"
#	include "physicobject.h"
#	include "script_object.h"
#	include "BreakableObject.h"
#	include "PhysicsSkeletonObject.h"
#	include "DestroyablePhysicsObject.h"

#	include "game_sv_single.h"

#	include "game_cl_single.h"

#	include "ui/UIGame.h"
#	include	"climableobject.h"
#	include "SpaceRestrictor.h"
#	include "SmartZone.h"
#	include "InventoryBox.h"

#define ADD(a,b,c,d)			add<a,b>(c,d)

void CObjectFactory::register_classes	()
{
	// client entities
	add<CLevel>													(CLSID_GAME_LEVEL				,"level");
	add<CGamePersistent>										(CLSID_GAME_PERSISTANT			,"game");
	add<CHUDManager>											(CLSID_HUDMANAGER				,"hud_manager");
	//Server Game type
	add<game_sv_Single>											(CLSID_SV_GAME_SINGLE			,"game_sv_single");

	//Client Game type
	add<game_cl_Single>											(CLSID_CL_GAME_SINGLE			,"game_cl_single");

	//Game UI type
	add<CUIGame>												(CLSID_GAME_UI_SINGLE			,"game_ui_single");

	ADD(CActor					,CSE_ALifeCreatureActor			,CLSID_OBJECT_ACTOR				,"actor");

	// server entities
	add<CSE_ALifeGroupTemplate<CSE_ALifeMonsterBase> >			(CLSID_AI_FLESH_GROUP			,"flesh_group");
	add<CSE_ALifeGraphPoint>									(CLSID_AI_GRAPH					,"graph_point");
	add<CSE_ALifeOnlineOfflineGroup>							(CLSID_ONLINE_OFFLINE_GROUP		,"online_offline_group");
	
	// client and server entities
	ADD(CSpectator				,CSE_Spectator					,CLSID_SPECTATOR				,"spectator");

	ADD(CAI_Flesh				,CSE_ALifeMonsterBase			,CLSID_AI_FLESH					,"flesh");
	ADD(CChimera				,CSE_ALifeMonsterBase			,CLSID_AI_CHIMERA				,"chimera");
	ADD(CAI_Dog					,CSE_ALifeMonsterBase			,CLSID_AI_DOG_RED				,"dog_red");
	ADD(CStalker, CSE_ALifeHumanStalker, CLSID_AI_STALKER, "stalker");
	ADD(CAI_Bloodsucker			,CSE_ALifeMonsterBase			,CLSID_AI_BLOODSUCKER			,"bloodsucker");
	ADD(CAI_Boar				,CSE_ALifeMonsterBase			,CLSID_AI_BOAR					,"boar");
	ADD(CAI_PseudoDog			,CSE_ALifeMonsterBase			,CLSID_AI_DOG_BLACK				,"dog_black");
	ADD(CPsyDog					,CSE_ALifeMonsterBase			,CLSID_AI_DOG_PSY				,"psy_dog");
	ADD(CPsyDogPhantom			,CSE_ALifePsyDogPhantom			,CLSID_AI_DOG_PSY_PHANTOM		,"psy_dog_phantom");
	ADD(CBurer					,CSE_ALifeMonsterBase			,CLSID_AI_BURER					,"burer");
	ADD(CPseudoGigant			,CSE_ALifeMonsterBase			,CLSID_AI_GIANT					,"pseudo_gigant");
	ADD(CController				,CSE_ALifeMonsterBase			,CLSID_AI_CONTROLLER			,"controller");
	ADD(CPoltergeist			,CSE_ALifeMonsterBase			,CLSID_AI_POLTERGEIST			,"poltergeist");
	ADD(CZombie					,CSE_ALifeMonsterBase			,CLSID_AI_ZOMBIE				,"zombie");
	ADD(CFracture				,CSE_ALifeMonsterBase			,CLSID_AI_FRACTURE				,"fracture");
	ADD(CSnork					,CSE_ALifeMonsterBase			,CLSID_AI_SNORK					,"snork");
	ADD(CCat					,CSE_ALifeMonsterBase			,CLSID_AI_CAT					,"cat");
	ADD(CTushkano				,CSE_ALifeMonsterBase			,CLSID_AI_TUSHKANO				,"tushkano");
	
	ADD(CPhantom				,CSE_ALifeCreaturePhantom		,CLSID_AI_PHANTOM				,"phantom");

	// Trader
	ADD(CTrader					,CSE_ALifeTrader				,CLSID_AI_TRADER				,"trader");

	ADD(CAI_Crow				,CSE_ALifeCreatureCrow			,CLSID_AI_CROW					,"crow");
	ADD(CCar					,CSE_ALifeCar					,CLSID_CAR						,"car");

	ADD(CHelicopter				,CSE_ALifeHelicopter			,CLSID_VEHICLE_HELICOPTER		,"helicopter");

	// Artefacts
	ADD(CMercuryBall			,CSE_ALifeItemArtefact			,CLSID_AF_MERCURY_BALL			,"art_mercury_ball");
	ADD(CBlackDrops				,CSE_ALifeItemArtefact			,CLSID_AF_BLACKDROPS			,"art_black_drops");
	ADD(CBlackGraviArtefact		,CSE_ALifeItemArtefact			,CLSID_AF_NEEDLES				,"art_needles");
	ADD(CBastArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_BAST					,"art_bast_artefact");
	ADD(CBlackGraviArtefact		,CSE_ALifeItemArtefact			,CLSID_AF_BLACK_GRAVI			,"art_gravi_black");
	ADD(CDummyArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_DUMMY					,"art_dummy");
	ADD(CZudaArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_ZUDA					,"art_zuda");
	ADD(CThornArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_THORN					,"art_thorn");
	ADD(CFadedBall				,CSE_ALifeItemArtefact			,CLSID_AF_FADED_BALL			,"art_faded_ball");
	ADD(CElectricBall			,CSE_ALifeItemArtefact			,CLSID_AF_ELECTRIC_BALL			,"art_electric_ball");
	ADD(CRustyHairArtefact		,CSE_ALifeItemArtefact			,CLSID_AF_RUSTY_HAIR			,"art_rusty_hair");
	ADD(CGalantineArtefact		,CSE_ALifeItemArtefact			,CLSID_AF_GALANTINE				,"art_galantine");
	ADD(CGraviArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_GRAVI					,"art_gravi");
	ADD(CGraviArtefact			,CSE_ALifeItemArtefact			,CLSID_ARTEFACT					,"artefact");

	//  [8/15/2006]
	ADD(CWeaponMagazined		,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_MAGAZINED		,"wpn_wmagaz");
	//  [8/15/2006]
	//  [8/17/2006]
	ADD(CWeaponMagazinedWGrenade,CSE_ALifeItemWeaponMagazinedWGL,CLSID_OBJECT_W_MAGAZWGL		,"wpn_wmaggl");
	//  [8/17/2006]
	ADD(CWeaponFN2000			,CSE_ALifeItemWeaponMagazinedWGL	,CLSID_OBJECT_W_FN2000			,"wpn_fn2000");
	ADD(CWeaponAK74				,CSE_ALifeItemWeaponMagazinedWGL	,CLSID_OBJECT_W_AK74			,"wpn_ak74");
	ADD(CWeaponLR300			,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_LR300			,"wpn_lr300");
	ADD(CWeaponHPSA				,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_HPSA			,"wpn_hpsa");
	ADD(CWeaponPM				,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_PM				,"wpn_pm");
	ADD(CWeaponFORT				,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_FORT			,"wpn_fort");
	ADD(CWeaponBinoculars		,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_BINOCULAR		,"wpn_binocular");
	ADD(CWeaponShotgun			,CSE_ALifeItemWeaponShotGun		,CLSID_OBJECT_W_SHOTGUN			,"wpn_shotgun");
	ADD(CWeaponSVD				,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_SVD				,"wpn_svd");
	ADD(CWeaponSVU				,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_SVU				,"wpn_svu");
	ADD(CWeaponRPG7				,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_RPG7			,"wpn_rpg7");
	ADD(CWeaponVal				,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_VAL				,"wpn_val");
	ADD(CWeaponVintorez			,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_VINTOREZ		,"wpn_vintorez");
	ADD(CWeaponWalther			,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_WALTHER			,"wpn_walther");
	ADD(CWeaponUSP45			,CSE_ALifeItemWeaponMagazined	,CLSID_OBJECT_W_USP45			,"wpn_usp45");
	ADD(CWeaponGroza			,CSE_ALifeItemWeaponMagazinedWGL,CLSID_OBJECT_W_GROZA			,"wpn_groza");
	ADD(CWeaponKnife			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_KNIFE			,"wpn_knife");
	ADD(CWeaponBM16				,CSE_ALifeItemWeaponShotGun		,CLSID_OBJECT_W_BM16			,"wpn_bm16");
	ADD(CWeaponRG6				,CSE_ALifeItemWeaponShotGun		,CLSID_OBJECT_W_RG6				,"wpn_rg6");
	//-----------------------------------------------------------------------------------------------------
	ADD(CWeaponAmmo				,CSE_ALifeItemAmmo				,CLSID_OBJECT_AMMO				,"wpn_ammo");
	ADD(CWeaponAmmo				,CSE_ALifeItemAmmo				,CLSID_OBJECT_A_VOG25			,"wpn_ammo_vog25");
	ADD(CWeaponAmmo				,CSE_ALifeItemAmmo				,CLSID_OBJECT_A_OG7B			,"wpn_ammo_og7b");
	ADD(CWeaponAmmo				,CSE_ALifeItemAmmo				,CLSID_OBJECT_A_M209			,"wpn_ammo_m209");
	//-----------------------------------------------------------------------------------------------------

	//Weapons Add-on
	ADD(CScope					,CSE_ALifeItem					,CLSID_OBJECT_W_SCOPE			,"wpn_scope");
	ADD(CSilencer				,CSE_ALifeItem					,CLSID_OBJECT_W_SILENCER		,"wpn_silencer");
	ADD(CGrenadeLauncher		,CSE_ALifeItem					,CLSID_OBJECT_W_GLAUNCHER		,"wpn_grenade_launcher");

	// Inventory
	ADD(CBolt					,CSE_ALifeItemBolt				,CLSID_IITEM_BOLT				,"obj_bolt");
	ADD(CMedkit					,CSE_ALifeItem					,CLSID_IITEM_MEDKIT				,"obj_medkit");
	ADD(CMedkit					,CSE_ALifeItem					,CLSID_IITEM_BANDAGE			,"obj_bandage");
	ADD(CAntirad				,CSE_ALifeItem					,CLSID_IITEM_ANTIRAD			,"obj_antirad");
	ADD(CFoodItem				,CSE_ALifeItem					,CLSID_IITEM_FOOD				,"obj_food");
	ADD(CBottleItem				,CSE_ALifeItem					,CLSID_IITEM_BOTTLE				,"obj_bottle");
	ADD(CExplosiveItem			,CSE_ALifeItemExplosive			,CLSID_IITEM_EXPLOSIVE			,"obj_explosive");
	
	//Info Document
	ADD(CInfoDocument			,CSE_ALifeItemDocument			,CLSID_IITEM_DOCUMENT			,"obj_document");
	ADD(CInventoryItemObject	,CSE_ALifeItem					,CLSID_IITEM_ATTACH				,"obj_attachable");

	//Equipment outfit
	ADD(CScientificOutfit		,CSE_ALifeItemCustomOutfit		,CLSID_EQUIPMENT_SCIENTIFIC		,"equ_scientific");
	ADD(CStalkerOutfit			,CSE_ALifeItemCustomOutfit		,CLSID_EQUIPMENT_STALKER		,"equ_stalker");
	ADD(CMilitaryOutfit			,CSE_ALifeItemCustomOutfit		,CLSID_EQUIPMENT_MILITARY		,"equ_military");
	ADD(CExoOutfit				,CSE_ALifeItemCustomOutfit		,CLSID_EQUIPMENT_EXO			,"equ_exo");

	// Grenades
	ADD(CF1						,CSE_ALifeItemGrenade			,CLSID_GRENADE_F1				,"wpn_grenade_f1");
	ADD(CRGD5					,CSE_ALifeItemGrenade			,CLSID_GRENADE_RGD5				,"wpn_grenade_rgd5");

	// Rockets
	ADD(CExplosiveRocket		,CSE_Temporary					,CLSID_OBJECT_G_RPG7			,"wpn_grenade_rpg7");
	ADD(CExplosiveRocket		,CSE_Temporary					,CLSID_OBJECT_G_FAKE			,"wpn_grenade_fake");

	// Zones
	ADD(CCustomZone				,CSE_ALifeCustomZone			,CLSID_ZONE						,"zone");
	ADD(CMosquitoBald			,CSE_ALifeAnomalousZone			,CLSID_Z_MBALD					,"zone_mosquito_bald");
	ADD(CMincer					,CSE_ALifeAnomalousZone			,CLSID_Z_MINCER					,"zone_mincer");
	ADD(CMosquitoBald			,CSE_ALifeAnomalousZone			,CLSID_Z_ACIDF					,"zone_acid_fog");
	ADD(CMincer					,CSE_ALifeAnomalousZone			,CLSID_Z_GALANT					,"zone_galantine");
	ADD(CRadioactiveZone		,CSE_ALifeAnomalousZone			,CLSID_Z_RADIO					,"zone_radioactive");
	ADD(CHairsZone				,CSE_ALifeZoneVisual			,CLSID_Z_BFUZZ					,"zone_bfuzz");
	ADD(CHairsZone				,CSE_ALifeZoneVisual			,CLSID_Z_RUSTYH					,"zone_rusty_hair");
	ADD(CMosquitoBald			,CSE_ALifeAnomalousZone			,CLSID_Z_DEAD					,"zone_dead");
	ADD(CLevelChanger			,CSE_ALifeLevelChanger			,CLSID_LEVEL_CHANGER			,"level_changer");
	ADD(CScriptZone				,CSE_ALifeSpaceRestrictor		,CLSID_SCRIPT_ZONE				,"script_zone");
	ADD(CSmartZone				,CSE_ALifeSmartZone				,CLSID_SMART_ZONE				,"smart_zone");
	ADD(CTeamBaseZone			,CSE_ALifeTeamBaseZone			,CLSID_Z_TEAM_BASE				,"team_base_zone");
	ADD(CTorridZone				,CSE_ALifeTorridZone			,CLSID_Z_TORRID					,"torrid_zone");
	ADD(CSpaceRestrictor		,CSE_ALifeSpaceRestrictor		,CLSID_SPACE_RESTRICTOR			,"space_restrictor");
	ADD(CAmebaZone				,CSE_ALifeZoneVisual			,CLSID_Z_AMEBA					,"ameba_zone");
	ADD(CNoGravityZone			,CSE_ALifeAnomalousZone			,CLSID_Z_NOGRAVITY				,"nogravity_zone");
	// Detectors
	ADD(CSimpleDetector			,CSE_ALifeItemDetector			,CLSID_DETECTOR_SIMPLE			,"device_detector_simple");

	// Devices
	ADD(CTorch					,CSE_ALifeItemTorch				,CLSID_DEVICE_TORCH				,"device_torch");
	ADD(CPda					,CSE_ALifeItemPDA				,CLSID_DEVICE_PDA				,"device_pda");

	// objects
	ADD(CProjector				,CSE_ALifeObjectProjector		,CLSID_OBJECT_PROJECTOR			,"projector");
	ADD(CWeaponMounted			,CSE_ALifeMountedWeapon			,CLSID_OBJECT_W_MOUNTED			,"wpn_mounted");
	ADD(CWeaponStatMgun			,CSE_ALifeStationaryMgun		,CLSID_OBJECT_W_STATMGUN		,"wpn_stat_mgun");
//	ADD(CTrigger				,CSE_Trigger					,CLSID_OBJECT_TRIGGER			,"trigger");

	// entity
	ADD(CHangingLamp			,CSE_ALifeObjectHangingLamp		,CLSID_OBJECT_HLAMP				,"hanging_lamp");
	ADD(CPhysicObject			,CSE_ALifeObjectPhysic			,CLSID_OBJECT_PHYSIC			,"obj_physic");
	ADD(CScriptObject			,CSE_ALifeDynamicObjectVisual	,CLSID_SCRIPT_OBJECT			,"script_object");
	ADD(CBreakableObject		,CSE_ALifeObjectBreakable		,CLSID_OBJECT_BREAKABLE			,"obj_breakable");
	ADD(CClimableObject			,CSE_ALifeObjectClimable		,CLSID_OBJECT_CLIMABLE			,"obj_climable");
	ADD(CPhysicsSkeletonObject	,CSE_ALifePHSkeletonObject		,CLSID_PH_SKELETON_OBJECT		,"obj_phskeleton");
	ADD(CDestroyablePhysicsObject,CSE_ALifeObjectPhysic			,CLSID_PHYSICS_DESTROYABLE		,"obj_phys_destroyable");

	ADD(CInventoryBox			,CSE_InventoryBox				,CLSID_INVENTORY_BOX			,"inventory_box");
}
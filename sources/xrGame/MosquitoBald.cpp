#include "stdafx.h"
#include "MosquitoBald.h"
#include "hudmanager.h"
#include "ParticlesObject.h"
#include "level.h"
#include "physicsshellholder.h"

CMosquitoBald::CMosquitoBald( )
{
	m_dwDeltaTime = 0;
	m_fHitImpulseScale = 1.0f;

	m_bLastBlowoutUpdate = false;
}

CMosquitoBald::~CMosquitoBald( )
{ }

void CMosquitoBald::Load(pcstr section)
{
	inherited::Load(section);
}

void CMosquitoBald::Postprocess(f32 /*val*/)
{ }

bool CMosquitoBald::BlowoutState( )
{
	bool result = inherited::BlowoutState( );
	if (!result)
	{
		m_bLastBlowoutUpdate = false;
		UpdateBlowout( );
	}
	else if (!m_bLastBlowoutUpdate)
	{
		m_bLastBlowoutUpdate = true;
		UpdateBlowout( );
	}

	return result;
}

void CMosquitoBald::Affect(SZoneObjectInfo* O)
{
	CPhysicsShellHolder* pGameObject = smart_cast<CPhysicsShellHolder*>(O->object);
	if (!pGameObject)
	{
		return;
	}

	if (O->zone_ignore)
	{
		return;
	}

	fVector3 P;
	XFORM( ).transform_tiny(P, CFORM( )->getSphere( ).P);

#ifdef DEBUG
	char l_pow[255];
	sprintf_s(l_pow, "zone hit. %.1f", Power(pGameObject->Position( ).distance_to(P)));
	if (bDebug)
	{
		Msg("%s %s", *pGameObject->cName( ), l_pow);
	}
#endif // def DEBUG

	fVector3 hit_dir;
	hit_dir.set(::Random.randF(-0.5f, 0.5f), ::Random.randF(0.0f, 1.0f), ::Random.randF(-0.5f, 0.5f));
	hit_dir.normalize( );

	fVector3 position_in_bone_space;

	VERIFY(!pGameObject->getDestroy( ));

	f32 dist = pGameObject->Position( ).distance_to(P) - pGameObject->Radius( );
	f32 power = Power(dist > 0.0f ? dist : 0.0f);
	f32 impulse = m_fHitImpulseScale * power * pGameObject->GetMass( );

	//статистика по объекту
	O->total_damage += power;
	O->hit_num++;

	if (power > 0.01f)
	{
		m_dwDeltaTime = 0;
		position_in_bone_space.set(0.0f, 0.0f, 0.0f);

		CreateHit(pGameObject->ID( ), ID( ), hit_dir, power, 0, position_in_bone_space, impulse, m_eHitTypeBlowout);

		PlayHitParticles(pGameObject);
	}
}

using namespace luabind;

#pragma optimize("s",on)
void CMosquitoBald::script_register(lua_State* L)
{
	module(L)
		[
			class_<CMosquitoBald, CGameObject>("CMosquitoBald")
				.def(constructor<>( ))
		];
}

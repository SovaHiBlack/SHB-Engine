// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "GameObject.h"
#include "PHShellHolder.h"
#include "PHSkeleton.h"
#include "script_export_space.h"

// refs
class CLAItem;
class CPhysicsElement;
class CSE_ALifeObjectHangingLamp;
class CPHElement;

class CHangingLamp : public CPHShellHolder, public CPHSkeleton
{//need m_pPhysicShell
	typedef	CPHShellHolder		inherited;

private:
	u16				light_bone;
	u16				ambient_bone;

	ref_light		light_render;
	ref_light		light_ambient;
	CLAItem* lanim;
	float			ambient_power;

	ref_glow		glow_render;

	float			fHealth;
	float			fBrightness;
	void			CreateBody(CSE_ALifeObjectHangingLamp* lamp);
	void			Init( );
	void			RespawnInit( );
	bool			Alive( )
	{
		return fHealth > 0.f;
	}


public:
	CHangingLamp( );
	virtual			~CHangingLamp( );
	void			TurnOn( );
	void			TurnOff( );
	virtual void	Load(const char* section);
	virtual BOOL	net_Spawn(CSE_Abstract* DC);
	virtual void	net_Destroy( );
	virtual void	shedule_Update(u32 dt);							// Called by sheduler
	virtual void	UpdateCL( );								// Called each frame, so no need for dt


	virtual void	SpawnInitPhysics(CSE_Abstract* D);
	virtual CPHShellHolder* PPhysicsShellHolder( )
	{
		return PhysicsShellHolder( );
	};
	virtual	void	CopySpawnInit( );
	virtual void	net_Save(CNetPacket& P);
	virtual	BOOL	net_SaveRelevant( );

	virtual BOOL	renderable_ShadowGenerate( )
	{
		return TRUE;
	}
	virtual BOOL	renderable_ShadowReceive( )
	{
		return TRUE;
	}

	virtual	void	Hit(SHit* pHDS);
	virtual void	net_Export(CNetPacket& P);
	virtual void	net_Import(CNetPacket& P);
	virtual BOOL	UsedAI_Locations( );

	virtual void	Center(Fvector3& C)	const;
	virtual float	Radius( )				const;
public:
	static void script_register(lua_State*);
};

add_to_type_list(CHangingLamp)
#undef script_type_list
#define script_type_list save_type_list(CHangingLamp)

// DummyObject.h: interface for the CHangingLamp class.

#pragma once

#include "PHShellHolder.h"

class CPHStaticGeomShell;
struct dContact;
struct SGameMtl;

class CBreakableObject : public CPHShellHolder
{
	typedef	CPHShellHolder		inherited;

private:
	bool				b_resived_damage;
	float				m_max_frame_damage;
	static	float				m_damage_threshold;
	static	float				m_health_threshhold;
	static	float				m_immunity_factor;
	Fvector3				m_contact_damage_pos;
	Fvector3				m_contact_damage_dir;

	float				fHealth;
	CPHStaticGeomShell* m_pUnbrokenObject;
	CPhysicsShell* m_Shell;
	static	u32					m_remove_time;
	u32					m_break_time;
	bool				bRemoved;
//		Fbox3				m_saved_box;

public:
	CBreakableObject( );
	virtual			~CBreakableObject( );

	virtual void	Load(const char* section);
	virtual BOOL	net_Spawn(CSE_Abstract* DC);
	virtual void	net_Destroy( );
	virtual void	shedule_Update(u32 dt);							// Called by sheduler
	virtual void	UpdateCL( );
	virtual BOOL	renderable_ShadowGenerate( )
	{
		return FALSE;
	}
	virtual BOOL	renderable_ShadowReceive( )
	{
		return TRUE;
	}

	virtual void	Hit(SHit* pHDS);

	virtual void	net_Export(CNetPacket& P);
	virtual void	net_Import(CNetPacket& P);
	virtual BOOL	UsedAI_Locations( );

private:
	void			Init( );
	void			CreateUnbroken( );
	void			CreateBroken( );
	void			DestroyUnbroken( );
	void			ActivateBroken( );
	void			Split( );
	void			Break( );
	void			ApplyExplosion(const Fvector3& dir, float impulse);
	void			CheckHitBreak(float power, ALife::EHitType hit_type);
	void			ProcessDamage( );
	void			SendDestroy( );
	void			enable_notificate( );
	static void		ObjectContactCallback(bool& /**do_colide/**/, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/);
};

// DummyObject.h: interface for the CHangingLamp class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "physicsshellholder.h"

class	CPHStaticGeomShell;
struct	dContact;
struct	SGameMtl;

class CBreakableObject: public CPhysicsShellHolder {
	typedef	CPhysicsShellHolder		inherited;
private:
		bool				b_resived_damage;
		f32				m_max_frame_damage;
static	f32				m_damage_threshold;
static	f32				m_health_threshhold;
static	f32				m_immunity_factor;
		Fvector				m_contact_damage_pos;
		Fvector				m_contact_damage_dir;

		f32				fHealth;
		CPHStaticGeomShell	*m_pUnbrokenObject;
		CPhysicsShell		*m_Shell;
static	u32					m_remove_time;
		u32					m_break_time;
		bool				bRemoved;
//		Fbox				m_saved_box;
public:
					CBreakableObject	();
	virtual			~CBreakableObject	();

	virtual void	Load				(pcstr section);
	virtual BOOL	net_Spawn			( CSE_Abstract* DC);
	virtual	void	net_Destroy			();
	virtual void	shedule_Update		( u32 dt);							// Called by sheduler
	virtual	void	UpdateCL			();
	virtual BOOL	renderable_ShadowGenerate	( ) { return FALSE;	}
	virtual BOOL	renderable_ShadowReceive	( ) { return TRUE;	}
	
	virtual	void	Hit					(SHit* pHDS);

	virtual void	net_Export			(NET_Packet& P);
	virtual void	net_Import			(NET_Packet& P);
	virtual BOOL	UsedAI_Locations	();
private:
	void			Init				();
	void			CreateUnbroken		();
	void			CreateBroken		();
	void			DestroyUnbroken		();
	void			ActivateBroken		();
	void			Split				();
	void			Break				();
	void			ApplyExplosion		(const Fvector &dir, f32 impulse);
	void			CheckHitBreak		(f32 power,ALife::EHitType hit_type);
	void			ProcessDamage		();
	void			SendDestroy			();
	void			enable_notificate	();
	static 	void	ObjectContactCallback(bool& /**do_colide/**/,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/);
};

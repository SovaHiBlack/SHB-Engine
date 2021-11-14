#pragma once

#include "Hit.h"

class CPHShellHolder;
class CSE_Abstract;
class CPHDestroyableNotificate;

class CPHDestroyableNotificator
{
public:
	virtual void						NotificateDestroy(CPHDestroyableNotificate* dn) = 0;
};

class CPHDestroyable : public  CPHDestroyableNotificator
{
	xr_vector<CSharedString>						m_destroyed_obj_visual_names;
	xr_vector<CPHDestroyableNotificate*>		m_notificate_objects;
	unsigned short											m_depended_objects;
	Flags8										m_flags;
	SHit										m_fatal_hit;

	enum
	{
		fl_destroyable = 1 << 0,
		fl_destroyed = 1 << 1,
		fl_released = 1 << 2
	};
	virtual CPHShellHolder* PPhysicsShellHolder( ) = 0;

public:
	CPHDestroyable( );
	void						Init( );
	void						RespawnInit( );
	void						SetFatalHit(const SHit& hit);
	void						Destroy(unsigned short source_id = unsigned short(-1), const char* section = "ph_skeleton_object");
	SHit& FatalHit( )
	{
		return m_fatal_hit;
	}
	void						Load(const char* section);
	void						Load(CConfigurationFile* ini, const char* section);
	virtual void						NotificateDestroy(CPHDestroyableNotificate* dn);
	void						PhysicallyRemoveSelf( );
	inline bool						Destroyable( )
	{
		return !!m_flags.test(fl_destroyable);
	}
	inline bool						Destroyed( )
	{
		return !!m_flags.test(fl_destroyed);
	}
	inline bool						CanDestroy( )
	{
		return m_flags.test(fl_destroyable) && !m_flags.test(fl_destroyed);
	}
	virtual bool						CanRemoveObject( )
	{
		return true;
	}
	virtual void						SheduleUpdate(u32 dt);
	virtual void						GenSpawnReplace(unsigned short source_id, const char* section, CSharedString visual_name);
	virtual void						InitServerObject(CSE_Abstract* D);

private:
	void						NotificatePart(CPHDestroyableNotificate* dn);
	void						PhysicallyRemovePart(CPHDestroyableNotificate* dn); ;
};

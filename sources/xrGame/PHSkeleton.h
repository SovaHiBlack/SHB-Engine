#pragma once

#include "PHDefs.h"
#include "PHDestroyableNotificate.h"

class CPhysicsShellHolder;
class CSE_ALifePHSkeletonObject;
class CSE_Abstract;
class CSE_PHSkeleton;
class CNetPacket;

class CPHSkeleton :
 public CPHDestroyableNotificate
{
	bool				b_removing;
	static u32			existence_time;
	u32					m_remove_time;
	PHSHELL_PAIR_VECTOR m_unsplited_shels;

	shared_str			m_startup_anim;
	flags8				m_flags;

private:
	//Creating

	void	Init				()																				;

	void	ClearUnsplited		()																				;
	//Splitting

	void	PHSplit				()																				;

	void	SpawnCopy			()																				;
	//Autoremove
	bool	ReadyForRemove		()																				;
	void	RecursiveBonesCheck	(u16 id)																		;

protected:
	void			LoadNetState		(CNetPacket& P)															;
	void			UnsplitSingle		(CPHSkeleton* SO)														;

protected:
	virtual CPhysicsShellHolder*		PPhysicsShellHolder	()													=0;
	virtual CPHSkeleton	*PHSkeleton		()																		{return this;}
	virtual void	SpawnInitPhysics	(CSE_Abstract	*D)														=0;
	virtual void	SaveNetState		(CNetPacket& P)															;
	virtual	void	RestoreNetState		(CSE_PHSkeleton* po)											;
	
	virtual	void	InitServerObject	(CSE_Abstract	*D)														;//
	virtual	void	CopySpawnInit		()																		;
			void	RespawnInit			()																		;//net_Destroy
			bool	Spawn				(CSE_Abstract	*D)														;//net_spawn
			void	Update				(u32 dt)																;//shedule update
			void	Load				(pcstr section)														;//client load
public:
			void	SetAutoRemove		(u32 time=existence_time)												;
			void	SetNotNeedSave		()																		;
IC			bool	IsRemoving			(){return b_removing;}
			u32		DefaultExitenceTime ()																		{return existence_time;}
					CPHSkeleton			()																		;
	virtual			~CPHSkeleton		()																		;
};

#pragma once

#include "Hit.h"

class CPhysicsShellHolder;
class CSE_Abstract;
class CPHDestroyableNotificate;
class CPHDestroyableNotificator
{

public:
	virtual		void						NotificateDestroy			(CPHDestroyableNotificate *dn)=0;
};


class CPHDestroyable : public  CPHDestroyableNotificator
{
			xr_vector<shared_str>						m_destroyed_obj_visual_names																											;
			xr_vector<CPHDestroyableNotificate *>		m_notificate_objects																													;
			u16											m_depended_objects																														;
			flags8										m_flags																																	;
			SHit										m_fatal_hit																																;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
			f32						m_random_min																															;     
			f32						m_random_hit_imp																														;
			u16							ref_bone																																;

			f32						m_imp_transition_factor																													;
			f32						m_lv_transition_factor																													;
			f32						m_av_transition_factor																													;
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
					source_bone            =-1      ;-1- ref_bone
					imp_transition_factor  =1       ; ����������� �������� ��������     
					lv_transition_factor   =1       ; ����������� �������� �������� �������� 
					av_transition_factor   =1       ; ����������� �������� ������� ��������
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			enum
			{
				fl_destroyable		= 1<<0,
				fl_destroyed		= 1<<1,
				fl_released			= 1<<2,
			};
	virtual CPhysicsShellHolder*		PPhysicsShellHolder			()																											=0;

public:
										CPHDestroyable				()																											;
			void						Init						()																											;
			void						RespawnInit					()																											;
			void						SetFatalHit					(const SHit& hit)																							;
			void						Destroy						(u16 ref_id=u16(-1), pcstr section="ph_skeleton_object")													;
			SHit						&FatalHit					(){return m_fatal_hit;}
			void						Load						(pcstr section)																							;
			void						Load						(CIniFile* ini, pcstr section)																				;
virtual		void						NotificateDestroy			(CPHDestroyableNotificate *dn)																				;
			void						PhysicallyRemoveSelf		()																											;
IC			bool						Destroyable					()										{return !!m_flags.test(fl_destroyable);}
IC			bool						Destroyed					()										{return !!m_flags.test(fl_destroyed);}
IC			bool						CanDestroy					()										{return m_flags.test(fl_destroyable)&&!m_flags.test(fl_destroyed)	;}
virtual		bool						CanRemoveObject				()																								{return true;}
virtual		void						SheduleUpdate				(u32 dt)																									;
virtual		void						GenSpawnReplace				(u16 source_id, pcstr section,shared_str visual_name)														;
virtual		void						InitServerObject			(CSE_Abstract*				D)																				;

private:
			void						NotificatePart				(CPHDestroyableNotificate *dn)																				;
			void						PhysicallyRemovePart		(CPHDestroyableNotificate *dn)																				;
};

#pragma once

#include "game_sv_base.h"

class CServer;
class CALifeSimulator;

class	game_sv_Single				: public game_sv_GameState
{
private:
	typedef game_sv_GameState inherited;

protected:
	CALifeSimulator					*m_alife_simulator;

public:
									game_sv_Single			();
	virtual							~game_sv_Single			();

	virtual		const char* type_name				() const { return "single";};
	virtual		void				Create					(CSharedString& options);
//	virtual		CSE_Abstract*		get_entity_from_eid		(U16 id);


	virtual		void				OnCreate				(U16 id_who);
	virtual		BOOL				OnTouch					(U16 eid_who, U16 eid_what, BOOL bForced = FALSE);
	virtual		void				OnDetach				(U16 eid_who, U16 eid_what);

	// Main
	virtual		void				Update					();
	virtual		ALife::_TIME_ID		GetGameTime				();
	virtual		float				GetGameTimeFactor		();
	virtual		void				SetGameTimeFactor		(const float fTimeFactor);

	virtual		ALife::_TIME_ID		GetEnvironmentGameTime	();
	virtual		float				GetEnvironmentGameTimeFactor		();
	virtual		void				SetEnvironmentGameTimeFactor		(const float fTimeFactor);

	virtual		bool				change_level			(CNetPacket &net_packet, CClientID sender);
	virtual		void				save_game				(CNetPacket &net_packet, CClientID sender);
	virtual		bool				load_game				(CNetPacket &net_packet, CClientID sender);
	//virtual		void				reload_game				(CNetPacket &net_packet, CClientID sender);
	virtual		void				switch_distance			(CNetPacket &net_packet, CClientID sender);
	//virtual		BOOL				CanHaveFriendlyFire		()	{return FALSE;}
	virtual		void				teleport_object			(CNetPacket &packet, U16 id);
	virtual		void				add_restriction			(CNetPacket &packet, U16 id);
	virtual		void				remove_restriction		(CNetPacket &packet, U16 id);
	virtual		void				remove_all_restrictions	(CNetPacket &packet, U16 id);
	virtual		bool				custom_sls_default		() {return !!m_alife_simulator;};
	virtual		void				sls_default				();
	virtual		CSharedString		level_name				(const CSharedString& server_options) const;
	virtual		void				on_death				(CSE_Abstract *e_dest, CSE_Abstract *e_src);
				void				restart_simulator		(const char* saved_game_name);

	inline			CServer&server					() const
	{
		VERIFY						(m_server);
		return						(*m_server);
	}

	inline			CALifeSimulator		&alife					() const
	{
		VERIFY						(m_alife_simulator);
		return						(*m_alife_simulator);
	}
};

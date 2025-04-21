#pragma once

#include "game_sv_base.h"

class xrServer;
class CALifeSimulator;

class	game_sv_Single : public game_sv_GameState
{
private:
	typedef game_sv_GameState inherited;

protected:
	CALifeSimulator* m_alife_simulator;

public:
	game_sv_Single( );
	virtual							~game_sv_Single( );

	virtual		pcstr				type_name( ) const
	{
		return "single";
	};
	virtual		void				Create(shared_str& options);
//	virtual		CSE_Abstract*		get_entity_from_eid		(u16 id);


	virtual		void				OnCreate(u16 id_who);
	virtual		BOOL				OnTouch(u16 eid_who, u16 eid_what, BOOL bForced = FALSE);
	virtual		void				OnDetach(u16 eid_who, u16 eid_what);

	// Main
	virtual		void				Update( );
	virtual		ALife::_TIME_ID		GetGameTime( );
	virtual		f32				GetGameTimeFactor( );
	virtual		void				SetGameTimeFactor(const f32 fTimeFactor);

	virtual		ALife::_TIME_ID		GetEnvironmentGameTime( );
	virtual		f32				GetEnvironmentGameTimeFactor( );
	virtual		void				SetEnvironmentGameTimeFactor(const f32 fTimeFactor);

	virtual		bool				change_level(CNetPacket& net_packet, CClientID sender);
	virtual		void				save_game(CNetPacket& net_packet, CClientID sender);
	virtual		bool				load_game(CNetPacket& net_packet, CClientID sender);
	virtual		void				reload_game(CNetPacket& net_packet, CClientID sender);
	virtual		void				switch_distance(CNetPacket& net_packet, CClientID sender);
	virtual		BOOL				CanHaveFriendlyFire( )
	{
		return FALSE;
	}
	virtual		void				teleport_object(CNetPacket& packet, u16 id);
	virtual		void				add_restriction(CNetPacket& packet, u16 id);
	virtual		void				remove_restriction(CNetPacket& packet, u16 id);
	virtual		void				remove_all_restrictions(CNetPacket& packet, u16 id);
	virtual		bool				custom_sls_default( )
	{
		return !!m_alife_simulator;
	};
	virtual		void				sls_default( );
	virtual		shared_str			level_name(const shared_str& server_options) const;
	virtual		void				on_death(CSE_Abstract* e_dest, CSE_Abstract* e_src);
	void				restart_simulator(pcstr saved_game_name);

	IC			xrServer& server( ) const
	{
		VERIFY(m_server);
		return						(*m_server);
	}

	IC			CALifeSimulator& alife( ) const
	{
		VERIFY(m_alife_simulator);
		return						(*m_alife_simulator);
	}
};

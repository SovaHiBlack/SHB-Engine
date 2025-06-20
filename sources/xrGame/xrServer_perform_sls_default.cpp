#include "stdafx.h"
#include "xrserver.h"
#include "xrmessages.h"

#if 1//def DEBUG
#	define USE_DESIGNER_KEY
#endif

#ifdef USE_DESIGNER_KEY
#	include "xrServer_Objects_ALife_Monsters.h"
#endif

void xrServer::SLS_Default( )
{
	if (game->custom_sls_default( ))
	{
		game->sls_default( );
		return;
	}

#ifdef USE_DESIGNER_KEY
	bool _designer = !!strstr(Core.Params, "-designer");
	CSE_ALifeCreatureActor* _actor = 0;
#endif

	string_path fn_spawn;
	if (FS.exist(fn_spawn, "$level$", "level.spawn"))
	{
		IReader* SP = FS.r_open(fn_spawn);
		CNetPacket P;
		u32 S_id;
		for (IReader* S = SP->open_chunk_iterator(S_id); S; S = SP->open_chunk_iterator(S_id, S))
		{
			P.B.count = S->length( );
			S->r(P.B.data, P.B.count);

			u16 ID;
			P.r_begin(ID);
			R_ASSERT(M_SPAWN == ID);
			CClientID clientID;
			clientID.set(0);

#ifdef USE_DESIGNER_KEY
			CSE_Abstract* entity =
#endif

				Process_spawn(P, clientID);

#ifdef USE_DESIGNER_KEY
			if (_designer)
			{
				CSE_ALifeCreatureActor* actor = smart_cast<CSE_ALifeCreatureActor*>(entity);
				if (actor)
				{
					_actor = actor;
				}
			}
#endif

		}

		FS.r_close(SP);
	}

#ifdef USE_DESIGNER_KEY
	if (!_designer)
	{
		return;
	}

	if (_actor)
	{
		return;
	}

	_actor = smart_cast<CSE_ALifeCreatureActor*>(entity_Create("actor"));
	_actor->o_Position = fVector3( ).set(0.0f, 0.0f, 0.0f);
	_actor->set_name_replace("designer");
	_actor->s_flags.flags |= M_SPAWN_OBJECT_ASPLAYER;
	CNetPacket packet;
	packet.w_begin(M_SPAWN);
	_actor->Spawn_Write(packet, TRUE);

	u16 id;
	packet.r_begin(id);
	R_ASSERT(id == M_SPAWN);
	CClientID clientID;
	clientID.set(0);
	Process_spawn(packet, clientID);
#endif

}

#include "stdafx.h"

#include "Server.h"
#include "xrServer_Objects.h"

int g_Dump_Update_Read = 0;

void CServer::Process_update(NET_Packet& P, ClientID sender)
{
	xrClientData* CL		= ID_to_client(sender);
	R_ASSERT2				(CL,"Process_update client not found");

	if (g_Dump_Update_Read)
	{
		Msg("---- UPDATE_Read --- ");
	}

	R_ASSERT(CL->flags.bLocal);
	// while has information
	while (!P.r_eof())
	{
		// find entity
		u16				ID;
		u8				size;

		P.r_u16			(ID);
		P.r_u8			(size);
		u32	_pos		= P.r_tell();
		CSE_Abstract	*E	= ID_to_entity(ID);
		
		if (E)
		{
			//Msg				("sv_import: %d '%s'",E->ID,E->name_replace());
			E->net_Ready	= TRUE;
			E->UPDATE_Read	(P);

			if (g_Dump_Update_Read)
			{
				Msg("* %s : %d - %d", E->name( ), size, P.r_tell( ) - _pos);
			}

			if ((P.r_tell()-_pos) != size)
			{
				string16	tmp;
				CLSID2TEXT	(E->m_tClassID,tmp);
				Debug.fatal	(DEBUG_INFO,"Beer from the creator of '%s'",tmp);
			}
		}
		else
		{
			P.r_advance(size);
		}
	}

	if (g_Dump_Update_Read)
	{
		Msg("-------------------- ");
	}
}

void CServer::Process_save(NET_Packet& P, ClientID sender)
{
	xrClientData* CL		= ID_to_client(sender);
	R_ASSERT2				(CL,"Process_save client not found");
	CL->net_Ready			= TRUE;

	R_ASSERT(CL->flags.bLocal);
	// while has information
	while (!P.r_eof())
	{	// find entity
		u16				ID;
		u16				size;

		P.r_u16			(ID);
		P.r_u16			(size);
		int				_pos_start	= P.r_tell	();
		CSE_Abstract	*E	= ID_to_entity(ID);

		if (E)
		{
			E->net_Ready = TRUE;
			E->load		(P);
		}
		else
		{
			P.r_advance(size);
		}

		int				_pos_end	= P.r_tell	();
		int				_size		= size;
		if				(_size != (_pos_end-_pos_start))
		{
			Msg			("! load/save mismatch, object: '%s'",E?E->name_replace():"unknown");
			int			_rollback	= _pos_start+_size;
			P.r_seek	(_rollback);
		}
	}
}

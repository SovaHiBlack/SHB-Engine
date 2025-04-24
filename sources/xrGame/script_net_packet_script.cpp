////////////////////////////////////////////////////////////////////////////
//	Module 		: script_net packet_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script net packet class script export
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "script_net_packet.h"
#include "..\xrNetServer\net_utils.h"

using namespace luabind;

bool r_eof(CNetPacket* self)
{
	return			(!!self->r_eof());
}

pcstr r_stringZ(CNetPacket* self)
{
	shared_str			temp;
	self->r_stringZ	(temp);
	return			(*temp);
}

void w_bool(CNetPacket* self, bool value)
{
	self->w_u8		(value ? 1 : 0);
}

bool r_bool(CNetPacket* self)
{
	return			(!!self->r_u8());
}

CClientID r_clientID(CNetPacket* self)
{
	CClientID		clientID;
	self->r_clientID(clientID);
	return clientID;
}

extern u16	script_server_object_version	();

#pragma optimize("s",on)
void CScriptNetPacket::script_register(lua_State *L)
{
	module(L)
	[
		def("script_server_object_version", &script_server_object_version),
		
		class_<CClientID>("CClientID")
			.def(					constructor<>()				)
			.def("value",			&CClientID::value			)
			.def("set",				&CClientID::set				)
			.def(self == other<CClientID>()),

		class_<CNetPacket>("net_packet")
			.def(					constructor<>()				)
			.def("w_begin",			&CNetPacket::w_begin		)
//			.def("w",				&CNetPacket::w				)
//			.def("w_seek",			&CNetPacket::w_seek			)
			.def("w_tell",			&CNetPacket::w_tell			)
			.def("w_vec3",			&CNetPacket::w_vec3			)
			.def("w_float",			&CNetPacket::w_float		)
			.def("w_u64",			&CNetPacket::w_u64			)
			.def("w_s64",			&CNetPacket::w_s64			)
			.def("w_u32",			&CNetPacket::w_u32			)
			.def("w_s32",			&CNetPacket::w_s32			)
			.def("w_u24",			&CNetPacket::w_u24			)
			.def("w_u16",			&CNetPacket::w_u16			)
			.def("w_s16",			&CNetPacket::w_s16			)
			.def("w_u8",			&CNetPacket::w_u8			)
//			.def("w_s8",			&CNetPacket::w_s8			)
			.def("w_bool",			&w_bool						)
			.def("w_float_q16",		&CNetPacket::w_float_q16	)
			.def("w_float_q8",		&CNetPacket::w_float_q8		)
			.def("w_angle16",		&CNetPacket::w_angle16		)
			.def("w_angle8",		&CNetPacket::w_angle8		)
			.def("w_dir",			&CNetPacket::w_dir			)
			.def("w_sdir",			&CNetPacket::w_sdir			)
			.def("w_stringZ",		(void (CNetPacket::*)(pcstr))(&CNetPacket::w_stringZ	))
			.def("w_matrix",		&CNetPacket::w_matrix		)
			.def("w_clientID",		&CNetPacket::w_clientID		)
			.def("w_chunk_open8",	&CNetPacket::w_chunk_open8	)
			.def("w_chunk_close8",	&CNetPacket::w_chunk_close8	)
			.def("w_chunk_open16",	&CNetPacket::w_chunk_open16	)
			.def("w_chunk_close16",	&CNetPacket::w_chunk_close16)
			.def("r_begin",			&CNetPacket::r_begin		)
//			.def("r",				&CNetPacket::r				)
			.def("r_seek",			&CNetPacket::r_seek			)
			.def("r_tell",			&CNetPacket::r_tell			)
			.def("r_vec3",			(void (CNetPacket::*)(fVector3&))(&CNetPacket::r_vec3)			,pure_out_value(_2))
			.def("r_float",			(void (CNetPacket::*)(f32&))(&CNetPacket::r_float		))
			.def("r_u64",			(void (CNetPacket::*)(u64&	))(&CNetPacket::r_u64		))
			.def("r_s64",			(void (CNetPacket::*)(s64&	))(&CNetPacket::r_s64		))
			.def("r_u32",			(void (CNetPacket::*)(u32&	))(&CNetPacket::r_u32		))
			.def("r_s32",			(void (CNetPacket::*)(s32&	))(&CNetPacket::r_s32		))
			.def("r_u24",			(void (CNetPacket::*)(u32&	))(&CNetPacket::r_u24		))
			.def("r_u16",			(void (CNetPacket::*)(u16&	))(&CNetPacket::r_u16		))
			.def("r_s16",			(void (CNetPacket::*)(s16&	))(&CNetPacket::r_s16		))
			.def("r_u8",			(void (CNetPacket::*)(u8&)	)(&CNetPacket::r_u8			))
			.def("r_s8",			(void (CNetPacket::*)(s8&)	)(&CNetPacket::r_s8			))
			.def("r_bool",			&r_bool						)
			.def("r_float",			(f32(CNetPacket::*)()	)(&CNetPacket::r_float		))
			.def("r_u64",			(u64	(CNetPacket::*)()	)(&CNetPacket::r_u64		))
			.def("r_s64",			(s64	(CNetPacket::*)()	)(&CNetPacket::r_s64		))
			.def("r_u32",			(u32	(CNetPacket::*)()	)(&CNetPacket::r_u32		))
			.def("r_s32",			(s32	(CNetPacket::*)()	)(&CNetPacket::r_s32		))
			.def("r_u24",			(u32	(CNetPacket::*)()	)(&CNetPacket::r_u24		))
			.def("r_u16",			(u16	(CNetPacket::*)()	)(&CNetPacket::r_u16		))
			.def("r_s16",			(s16	(CNetPacket::*)()	)(&CNetPacket::r_s16		))
			.def("r_u8",			(u8		(CNetPacket::*)()	)(&CNetPacket::r_u8			))
			.def("r_s8",			(s8		(CNetPacket::*)()	)(&CNetPacket::r_s8			))
			.def("r_float_q16",		(void	(CNetPacket::*)(f32&, f32, f32)	)(&CNetPacket::r_float_q16)	)
			.def("r_float_q8",		(void	(CNetPacket::*)(f32&, f32, f32)	)(&CNetPacket::r_float_q8)	)
			.def("r_angle16",		&CNetPacket::r_angle16		)
			.def("r_angle8",		&CNetPacket::r_angle8		)
			.def("r_dir",			&CNetPacket::r_dir			)
			.def("r_sdir",			&CNetPacket::r_sdir			)
			.def("r_stringZ",		&r_stringZ)
			.def("r_matrix",		&CNetPacket::r_matrix		)
			.def("r_clientID",		&r_clientID					)
			.def("r_elapsed",		&CNetPacket::r_elapsed		)
			.def("r_advance",		&CNetPacket::r_advance		)
			.def("r_eof",			&r_eof						)
	];
}
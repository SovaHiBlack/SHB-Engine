#pragma once

#include "ClientID.h"

#pragma pack(push,1)

const u32 NET_PacketSizeLimit			= 8192;

struct NET_Buffer
{
	u8									data[NET_PacketSizeLimit];
	u32									count;
};

class CNetPacket
{
public:
	void			construct			(pcvoid data, u32 size)
	{
		memcpy							(B.data, data, size);
		B.count							= size;
	}

	NET_Buffer							B;
	u32									r_pos;
	u32									timeReceive;

public:
	// writing - main
	IC void			write_start			( )
	{
		B.count							= 0;
	}
	IC void			w_begin				(u16 type) // begin of packet 'type'
	{
		B.count							= 0;
		w_u16							(type);
	}
	IC void			w					(pcvoid p, u32 count)
	{
		VERIFY							(p && count);
		VERIFY							(B.count + count < NET_PacketSizeLimit);
		CopyMemory						(&B.data[B.count], p, count);
		B.count							+= count;
		VERIFY							(B.count < NET_PacketSizeLimit);
	}
	IC void			w_seek				(u32 pos, pcvoid p, u32 count) // random write (only inside allocated region)
	{
		VERIFY							(p && count && (pos + count <= B.count));
		CopyMemory						(&B.data[pos], p, count);
	}
	IC u32			w_tell				( )
	{
		return B.count;
	}

	// writing - utilities
	IC void			w_float				(f32 a) // float
	{
		w								(&a, 4);
	}
	IC void			w_vec3				(const fVector3& a) // vec3
	{
		w								(&a, 3 * sizeof(f32));
	}
	IC void			w_vec4				(const fVector4& a) // vec4
	{
		w								(&a, 4 * sizeof(f32));
	}
	IC void			w_u64				(u64 a) // qword (8b)
	{
		w								(&a, 8);
	}
	IC void			w_s64				(s64 a) // qword (8b)
	{
		w								(&a, 8);
	}
	IC void			w_u32				(u32 a) // dword (4b)
	{
		w								(&a, 4);
	}
	IC void			w_s32				(s32 a) // dword (4b)
	{
		w								(&a, 4);
	}
	IC void			w_u24				(u32 a) // dword (3b)
	{
		w								(&a, 3);
	}
	IC void			w_u16				(u16 a) // word (2b)
	{
		w								(&a, 2);
	}
	IC void			w_s16				(s16 a) // word (2b)
	{
		w								(&a, 2);
	}
	IC void			w_u8				(u8 a) // byte (1b)
	{
		w								(&a, 1);
	}
	IC void			w_s8				(s8 a) // byte (1b)
	{
		w								(&a, 1);
	}
	IC void			w_float_q16			(f32 a, f32 min, f32 max)
	{
		VERIFY							(a >= min && a <= max);
		f32 q							= (a - min) / (max - min);
		w_u16							(u16(iFloor(q * 65535.0f + 0.5f)));
	}
	IC void			w_float_q8			(f32 a, f32 min, f32 max)
	{
		VERIFY							(a >= min && a <= max);
		f32 q							= (a - min) / (max - min);
		w_u8							(u8(iFloor(q * 255.0f + 0.5f)));
	}
	IC void			w_angle16			(f32 a)
	{
		w_float_q16						(angle_normalize(a), 0, PI_MUL_2);
	}
	IC void			w_angle8			(f32 a)
	{
		w_float_q8						(angle_normalize(a), 0, PI_MUL_2);
	}
	IC void			w_dir				(const fVector3& D)
	{
		w_u16							(pvCompress(D));
	}
	IC void			w_sdir				(const fVector3& D)
	{
		fVector3						C;
		f32 mag							= D.magnitude( );
		if (mag > EPSILON_7)
		{
			C.div						(D, mag);
		}
		else
		{
			C.set						(0.0f, 0.0f, 1.0f);
			mag							= 0.0f;
		}

		w_dir							(C);
		w_float							(mag);
	}
	IC void			w_stringZ			(pcstr S)
	{
		w								(S, (u32)xr_strlen(S) + 1);
	}
	IC void			w_stringZ			(shared_str& p)
	{
		if (*p)
		{
			w							(*p, (u32)xr_strlen(p) + 1);
		}
		else
		{
			w_u8						(0);
		}
	}
	IC void			w_matrix			(fMatrix4x4& M)
	{
		w_vec3							(M.i);
		w_vec3							(M.j);
		w_vec3							(M.k);
		w_vec3							(M.c);
	}

	IC void			w_clientID			(CClientID& C)
	{
		w_u32							(C.value( ));
	}

	IC void			w_chunk_open8		(u32& position)
	{
		position						= w_tell( );
		w_u8							(0);
	}
	IC void			w_chunk_close8		(u32 position)
	{
		u32 size						= u32(w_tell( ) - position) - sizeof(u8);
		VERIFY							(size < 256);
		u8 _size						= (u8)size;
		w_seek							(position, &_size, sizeof(_size));
	}

	IC void			w_chunk_open16		(u32& position)
	{
		position						= w_tell( );
		w_u16							(0);
	}
	IC void			w_chunk_close16		(u32 position)
	{
		u32 size						= u32(w_tell( ) - position) - sizeof(u16);
		VERIFY							(size < 65536);
		u16 _size						= (u16)size;
		w_seek							(position, &_size, sizeof(_size));
	}

	// reading - main
	IC void			read_start			( )
	{
		r_pos							= 0;
	}
	IC u32			r_begin				(u16& type) // returns time of receiving
	{
		r_pos							= 0;
		r_u16							(type);
		return timeReceive;
	}
	IC void			r					(pvoid p, u32 count)
	{
		VERIFY							(p && count);
		CopyMemory						(p, &B.data[r_pos], count);
		r_pos							+= count;
		VERIFY							(r_pos <= B.count);
	}
	IC void			r_seek				(u32 pos)
	{
		VERIFY							(pos < B.count);
		r_pos							= pos;
	}
	IC u32			r_tell				( )
	{
		return r_pos;
	}

	IC BOOL			r_eof				( )
	{
		return (r_pos >= B.count);
	}
	IC u32			r_elapsed			( )
	{
		return (B.count - r_pos);
	}
	IC void			r_advance			(u32 size)
	{
		r_pos							+= size;
		VERIFY							(r_pos <= B.count);
	}

	// reading - utilities
	IC void			r_float				(f32& A) // float
	{
		r								(&A, 4);
	}
	IC void			r_vec3				(fVector3& A) // vec3
	{
		r								(&A, 3 * sizeof(f32));
	}
	IC void			r_vec4				(fVector4& A) // vec4
	{
		r								(&A, 4 * sizeof(f32));
	}
	IC void			r_u64				(u64& A) // qword (8b)
	{
		r								(&A, 8);
	}
	IC void			r_s64				(s64& A) // qword (8b)
	{
		r								(&A, 8);
	}
	IC void			r_u32				(u32& A) // dword (4b)
	{
		r								(&A, 4);
	}
	IC void			r_s32				(s32& A) // dword (4b)
	{
		r								(&A, 4);
	}
	IC void			r_u24				(u32& A) // dword (3b)
	{
		A								= 0;
		r								(&A, 3);
	}
	IC void			r_u16				(u16& A) // word (2b)
	{
		r								(&A, 2);
	}
	IC void			r_s16				(s16& A) // word (2b)
	{
		r								(&A, 2);
	}
	IC void			r_u8				(u8& A) // byte (1b)
	{
		r								(&A, 1);
	}
	IC void			r_s8				(s8& A) // byte (1b)
	{
		r								(&A, 1);
	}

	// IReader compatibility
	IC f32			r_float				( ) // float
	{
		f32								A;
		r								(&A, 4);
		return A;
	}
	IC fVector3		r_vec3				( ) // vec3
	{
		fVector3						A;
		r								(&A, 3 * sizeof(f32));
		return A;
	}
	IC fVector4		r_vec4				( ) // vec4
	{
		fVector4						A;
		r								(&A, 4 * sizeof(f32));
		return A;
	}
	IC f32			r_float_q8			(f32 min, f32 max)
	{
		f32								A;
		r_float_q8						(A, min, max);
		return A;
	}
	IC f32			r_float_q16			(f32 min, f32 max)
	{
		f32								A;
		r_float_q16						(A, min, max);
		return A;
	}

	IC u64			r_u64				( ) // qword (8b)
	{
		u64								A;
		r								(&A, 8);
		return A;
	}
	IC s64			r_s64				( ) // qword (8b)
	{
		s64								A;
		r								(&A, 8);
		return A;
	}
	IC u32			r_u32				( ) // dword (4b)
	{
		u32								A;
		r								(&A, 4);
		return A;
	}
	IC s32			r_s32				( ) // dword (4b)
	{
		s32								A;
		r								(&A, 4);
		return A;
	}
	IC u32			r_u24				( ) // dword (3b)
	{
		u32 A							= 0;
		r								(&A, 3);
		return A;
	}
	IC u16			r_u16				( ) // word (2b)
	{
		u16								A;
		r								(&A, 2);
		return A;
	}
	IC s16			r_s16				( ) // word (2b)
	{
		s16								A;
		r								(&A, 2);
		return A;
	}
	IC u8			r_u8				( ) // byte (1b)
	{
		u8								A;
		r								(&A, 1);
		return A;
	}
	IC s8			r_s8				( ) // byte (1b)
	{
		s8								A;
		r								(&A, 1);
		return A;
	}

	IC void			r_float_q16			(f32& A, f32 min, f32 max)
	{
		u16								val;
		r_u16							(val);
		A								= (f32(val) * (max - min)) / 65535.0f + min;		// floating-point-error possible
		VERIFY							((A >= min - EPSILON_7) && (A <= max + EPSILON_7));
	}
	IC void			r_float_q8			(f32& A, f32 min, f32 max)
	{
		u8								val;
		r_u8							(val);
		A								= (f32(val) / 255.0001f) * (max - min) + min;		// floating-point-error possible
		VERIFY							((A >= min) && (A <= max));
	}
	IC void			r_angle16			(f32& A)
	{
		r_float_q16						(A, 0, PI_MUL_2);
	}
	IC void			r_angle8			(f32& A)
	{
		r_float_q8						(A, 0, PI_MUL_2);
	}
	IC void			r_dir				(fVector3& A)
	{
		u16								t;
		r_u16							(t);
		pvDecompress					(A, t);
	}
	IC void			r_sdir				(fVector3& A)
	{
		u16								t;
		r_u16							(t);
		f32								s;
		r_float							(s);
		pvDecompress					(A, t);
		A.mul							(s);
	}
	IC void			r_stringZ			(pstr S)
	{
		pcstr data						= pcstr(&B.data[r_pos]);
		size_t len						= xr_strlen(data);
		r								(S, (u32)len + 1);
	}
	IC void			r_stringZ			(xr_string& dest)
	{
		dest							= pcstr(&B.data[r_pos]);
		r_advance						(u32(dest.size( ) + 1));
	}
	void			r_stringZ			(shared_str& dest)
	{
		dest							= pcstr(&B.data[r_pos]);
		r_advance						(dest.size( ) + 1);
	}
	IC void			r_matrix			(fMatrix4x4& M)
	{
		r_vec3							(M.i);
		M._14_							= 0.0f;
		r_vec3							(M.j);
		M._24_							= 0.0f;
		r_vec3							(M.k);
		M._34_							= 0.0f;
		r_vec3							(M.c);
		M._44_							= 1.0f;
	}

	IC void			r_clientID			(CClientID& C)
	{
		u32								tmp;
		r_u32							(tmp);
		C.set							(tmp);
	}
};

#pragma pack(pop)

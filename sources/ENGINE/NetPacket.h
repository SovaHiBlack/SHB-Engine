#pragma once

#include "ClientID.h"

#pragma pack(push, 1)

const u32 NET_PacketSizeLimit		= 8192;

struct NET_Buffer
{
	BYTE							data[NET_PacketSizeLimit];
	u32								count;
};

class CNetPacket
{
public:
	void			construct		(const void* data, unsigned size)
	{
		memcpy(B.data, data, size);
		B.count = size;
	}

	NET_Buffer						B;
	u32								r_pos;
	u32								timeReceive;

	// writing - main
	inline void		write_start		( )
	{
		B.count = 0;
	}
	inline void		w_begin			(u16 type)				// begin of packet 'type'
	{
		B.count = 0;
		w_u16(type);
	}
	inline void		w				(const void* p, u32 count)
	{
		VERIFY(p && count);
		VERIFY(B.count + count < NET_PacketSizeLimit);
		CopyMemory(&B.data[B.count], p, count);
		B.count += count;
		VERIFY(B.count < NET_PacketSizeLimit);
	}
	inline void		w_seek			(u32 pos, const void* p, u32 count)	// random write (only inside allocated region)
	{
		VERIFY(p && count && (pos + count <= B.count));
		CopyMemory(&B.data[pos], p, count);
	}
	inline u32		w_tell			( )
	{
		return B.count;
	}

	// writing - utilities
	inline void		w_float			(float a)
	{
		w(&a, 4);
	}
	inline void		w_vec3			(const Fvector3& a)
	{
		w(&a, 3 * sizeof(float));
	}
	inline void		w_vec4			(const Fvector4& a)
	{
		w(&a, 4 * sizeof(float));
	}
	inline void		w_u64			(u64 a)
	{
		w(&a, 8);
	}
	inline void		w_s64			(s64 a)
	{
		w(&a, 8);
	}
	inline void		w_u32			(u32 a)
	{
		w(&a, 4);
	}
	inline void		w_s32			(int a)
	{
		w(&a, 4);
	}
	inline void		w_u24			(u32 a)
	{
		w(&a, 3);
	}
	inline void		w_u16			(u16 a)
	{
		w(&a, 2);
	}
	inline void		w_s16			(s16 a)
	{
		w(&a, 2);
	}
	inline void		w_u8			(U8 a)
	{
		w(&a, 1);
	}
	inline void		w_s8			(S8 a)
	{
		w(&a, 1);
	}

	inline void		w_float_q16		(float a, float min, float max)
	{
		VERIFY(a >= min && a <= max);
		float q = (a - min) / (max - min);
		w_u16(u16(iFloor(q * 65535.0f + 0.5f)));
	}
	inline void		w_float_q8		(float a, float min, float max)
	{
		VERIFY(a >= min && a <= max);
		float q = (a - min) / (max - min);
		w_u8(U8(iFloor(q * 255.0f + 0.5f)));
	}
	inline void		w_angle16		(float a)
	{
		w_float_q16(angle_normalize(a), 0, PI_MUL_2);
	}
	inline void		w_angle8		(float a)
	{
		w_float_q8(angle_normalize(a), 0, PI_MUL_2);
	}

	inline void		w_dir			(const Fvector3& D)
	{
		w_u16(pvCompress(D));
	}
	inline void		w_sdir			(const Fvector3& D)
	{
		Fvector3 C;
		float mag = D.magnitude( );
		if (mag > EPS_S)
		{
			C.div(D, mag);
		}
		else
		{
			C.set(0, 0, 1);
			mag = 0;
		}

		w_dir(C);
		w_float(mag);
	}

	inline void		w_stringZ		(const char* S)
	{
		w(S, (u32) xr_strlen(S) + 1);
	}
	inline void		w_stringZ		(shared_str& p)
	{
		if (*p)
		{
			w(*p, ( u32) xr_strlen(p) + 1);
		}
		else
		{
			w_u8(0);
		}
	}

	inline void		w_matrix		(Fmatrix& M)
	{
		w_vec3(M.i);
		w_vec3(M.j);
		w_vec3(M.k);
		w_vec3(M.c);
	}
	inline void		w_clientID		(ClientID& C)
	{
		w_u32(C.value( ));
	}

	inline void		w_chunk_open8	(u32& position)
	{
		position = w_tell( );
		w_u8(0);
	}
	inline void		w_chunk_close8	(u32 position)
	{
		u32 size = u32(w_tell( ) - position) - sizeof(U8);
		VERIFY(size < 256);
		U8 _size = (U8) size;
		w_seek(position, &_size, sizeof(_size));
	}

	inline void		w_chunk_open16	(u32& position)
	{
		position = w_tell( );
		w_u16(0);
	}
	inline void		w_chunk_close16	(u32 position)
	{
		u32 size = u32(w_tell( ) - position) - sizeof(u16);
		VERIFY(size < 65536);
		u16 _size = ( u16) size;
		w_seek(position, &_size, sizeof(_size));
	}

	// reading
	inline void		read_start		( )
	{
		r_pos = 0;
	}

	inline u32		r_begin			(u16& type)	// returns time of receiving
	{
		r_pos = 0;
		r_u16(type);
		return timeReceive;
	}

	inline void		r_seek			(u32 pos)
	{
		VERIFY(pos < B.count);
		r_pos = pos;
	}
	inline u32		r_tell			( )
	{
		return r_pos;
	}

	inline void		r				(void* p, u32 count)
	{
		VERIFY(p && count);
		CopyMemory(p, &B.data[r_pos], count);
		r_pos += count;
		VERIFY(r_pos <= B.count);
	}
	inline BOOL		r_eof			( )
	{
		return r_pos >= B.count;
	}
	inline u32		r_elapsed		( )
	{
		return B.count - r_pos;
	}
	inline void		r_advance		(u32 size)
	{
		r_pos += size;
		VERIFY(r_pos <= B.count);
	}

	// reading - utilities
	inline void		r_vec3			(Fvector3& A)
	{
		r(&A, 3 * sizeof(float));
	}
	inline void		r_vec4			(Fvector4& A)
	{
		r(&A, 4 * sizeof(float));
	}
	inline void		r_float			(float& A)
	{
		r(&A, 4);
	}
	inline void		r_u64			(u64& A)
	{
		r(&A, 8);
	}
	inline void		r_s64			(s64& A)
	{
		r(&A, 8);
	}
	inline void		r_u32			(u32& A)
	{
		r(&A, 4);
	}
	inline void		r_s32			(int& A)
	{
		r(&A, 4);
	}
	inline void		r_u24			(u32& A)
	{
		A = 0;		//???
		r(&A, 3);
	}
	inline void		r_u16			(u16& A)
	{
		r(&A, 2);
	}
	inline void		r_s16			(s16& A)
	{
		r(&A, 2);
	}
	inline void		r_u8			(U8& A)
	{
		r(&A, 1);
	}
	inline void		r_s8			(S8& A)
	{
		r(&A, 1);
	}

	// IReader compatibility
	inline Fvector3	r_vec3			( )
	{
		Fvector3 A;
		r(&A, 3 * sizeof(float));
		return A;
	}
	inline Fvector4	r_vec4			( )
	{
		Fvector4 A;
		r(&A, 4 * sizeof(float));
		return A;
	}
	inline float	r_float_q8		(float min, float max)
	{
		float A;
		r_float_q8(A, min, max);
		return A;
	}
	inline float	r_float_q16		(float min, float max)
	{
		float A;
		r_float_q16(A, min, max);
		return A;
	}
	inline float	r_float			( )
	{
		float A;
		r(&A, 4);
		return(A);
	}
	inline u64		r_u64			( )
	{
		u64 A;
		r(&A, 8);
		return A;
	}
	inline s64		r_s64			( )
	{
		s64 A;
		r(&A, 8);
		return A;
	}
	inline u32		r_u32			( )
	{
		u32 A;
		r(&A, 4);
		return A;
	}
	inline int		r_s32			( )
	{
		int A;
		r(&A, 4);
		return A;
	}
	inline u32		r_u24			( )
	{
		u32 A = 0;		//???
		r(&A, 3);
		return A;
	}
	inline u16		r_u16			( )
	{
		u16 A;
		r(&A, 2);
		return A;
	}
	inline s16		r_s16			( )
	{
		s16 A;
		r(&A, 2);
		return A;
	} 
	inline U8		r_u8			( )
	{
		U8 A;
		r(&A, 1);
		return A;
	}
	inline S8		r_s8			( )
	{
		S8 A;
		r(&A, 1);
		return A;
	}

	inline void		r_float_q16		(float& A, float min, float max)
	{
		u16 val;
		r_u16(val);
		A = (float(val) * (max - min)) / 65535.0f + min;		// floating-point-error possible
		VERIFY((A >= min - EPS_S) && (A <= max + EPS_S));
	}
	inline void		r_float_q8		(float& A, float min, float max)
	{
		U8 val;
		r_u8(val);
		A = (float(val) / 255.0001f) * (max - min) + min;	// floating-point-error possible
		VERIFY((A >= min) && (A <= max));
	}
	inline void		r_angle16		(float& A)
	{
		r_float_q16(A, 0, PI_MUL_2);
	}
	inline void		r_angle8		(float& A)
	{
		r_float_q8(A, 0, PI_MUL_2);
	}

	inline void		r_dir			(Fvector3& A)
	{
		u16 t;
		r_u16(t);
		pvDecompress(A, t);
	}
	inline void		r_sdir			(Fvector3& A)
	{
		u16 t;
		r_u16(t);
		float s;
		r_float(s);
		pvDecompress(A, t);
		A.mul(s);
	}

	inline void		r_stringZ		(char* S)
	{
		const char* data = ( const char*) (&B.data[r_pos]);
		size_t len = xr_strlen(data);
		r(S, ( u32) len + 1);
	}
	inline void		r_stringZ		(xr_string& dest)
	{
		dest = ( const char*) (&B.data[r_pos]);
		r_advance(u32(dest.size( ) + 1));
	}
	void			r_stringZ		(shared_str& dest)
	{
		dest = ( const char*) (&B.data[r_pos]);
		r_advance(dest.size( ) + 1);
	}

	inline void		r_matrix		(Fmatrix& M)
	{
		r_vec3(M.i);
		M._14_ = 0;
		r_vec3(M.j);
		M._24_ = 0;
		r_vec3(M.k);
		M._34_ = 0;
		r_vec3(M.c);
		M._44_ = 1;
	}
	inline void		r_clientID		(ClientID& C)
	{
		u32 tmp;
		r_u32(tmp);
		C.set(tmp);
	}
};

#pragma pack(pop)

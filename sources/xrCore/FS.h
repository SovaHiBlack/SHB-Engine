// FS.h: interface for the CFS class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#define CFS_CompressMark	(1ul << 31ul)

CORE_API void VerifyPath(pcstr path);

#ifdef DEBUG
CORE_API extern u32		g_file_mapped_memory;
CORE_API extern u32		g_file_mapped_count;
CORE_API void	dump_file_mappings( );
extern void	register_file_mapping(pvoid address, const u32& size, pcstr file_name);
extern void	unregister_file_mapping(pvoid address, const u32& size);
#endif // DEBUG

//------------------------------------------------------------------------------------
// Write
//------------------------------------------------------------------------------------
class CORE_API IWriter
{
private:
	xr_stack<u32>		chunk_pos;

public:
	shared_str			fName;

public:
	IWriter( )
	{ }
	virtual	~IWriter( )
	{
		R_ASSERT3(chunk_pos.empty( ), "Opened chunk not closed.", *fName);
	}

	// kernel
	virtual void	seek(u32 pos) = 0;
	virtual u32		tell( ) = 0;

	virtual void	w(pcvoid ptr, u32 count) = 0;

	// generalized writing functions
	IC void			w_u64(u64 d)
	{
		w(&d, sizeof(u64));
	}
	IC void			w_u32(u32 d)
	{
		w(&d, sizeof(u32));
	}
	IC void			w_u16(u16 d)
	{
		w(&d, sizeof(u16));
	}
	IC void			w_u8(u8 d)
	{
		w(&d, sizeof(u8));
	}
	IC void			w_s64(s64 d)
	{
		w(&d, sizeof(s64));
	}
	IC void			w_s32(s32 d)
	{
		w(&d, sizeof(s32));
	}
	IC void			w_s16(s16 d)
	{
		w(&d, sizeof(s16));
	}
	IC void			w_s8(s8 d)
	{
		w(&d, sizeof(s8));
	}
	IC void			w_float(f32 d)
	{
		w(&d, sizeof(f32));
	}
	IC void			w_string(pcstr p)
	{
		w(p, (u32) xr_strlen(p));
		w_u8(13);
		w_u8(10);
	}
	IC void			w_stringZ(pcstr p)
	{
		w(p, (u32) xr_strlen(p) + 1);
	}
	IC void			w_stringZ(const shared_str& p)
	{
		w(*p ? *p : "", p.size( ));
		w_u8(0);
	}
	IC void			w_stringZ(shared_str& p)
	{
		w(*p ? *p : "", p.size( ));
		w_u8(0);
	}
	IC void			w_stringZ(const xr_string& p)
	{
		w(p.c_str( ) ? p.c_str( ) : "", (u32) p.size( ));
		w_u8(0);
	}
	IC void			w_fcolor(const fColor& v)
	{
		w(&v, sizeof(fColor));
	}
	IC void			w_fvector4(const fVector4& v)
	{
		w(&v, sizeof(fVector4));
	}
	IC void			w_fvector3(const fVector3& v)
	{
		w(&v, sizeof(fVector3));
	}
	IC void			w_fvector2(const fVector2& v)
	{
		w(&v, sizeof(fVector2));
	}
	IC void			w_ivector4(const iVector4& v)
	{
		w(&v, sizeof(iVector4));
	}
	IC void			w_ivector3(const Ivector3& v)
	{
		w(&v, sizeof(Ivector3));
	}
	IC void			w_ivector2(const iVector2& v)
	{
		w(&v, sizeof(iVector2));
	}

	// quant writing functions
	IC void 		w_float_q16(f32 a, f32 min, f32 max)
	{
		VERIFY(a >= min && a <= max);
		f32 q = (a - min) / (max - min);
		w_u16(u16(iFloor(q * 65535.0f + 0.5f)));
	}
	IC void 		w_float_q8(f32 a, f32 min, f32 max)
	{
		VERIFY(a >= min && a <= max);
		f32 q = (a - min) / (max - min);
		w_u8(u8(iFloor(q * 255.0f + 0.5f)));
	}
	IC void 		w_angle16(f32 a)
	{
		w_float_q16(angle_normalize(a), 0.0f, PI_MUL_2);
	}
	IC void 		w_angle8(f32 a)
	{
		w_float_q8(angle_normalize(a), 0.0f, PI_MUL_2);
	}
	IC void 		w_dir(const Fvector& D)
	{
		w_u16(pvCompress(D));
	}
	void 			w_sdir(const Fvector& D);
	void	__cdecl	w_printf(pcstr format, ...);

	// generalized chunking
	u32				align( );
	void			open_chunk(u32 type);
	void			close_chunk( );
	u32				chunk_size( );					// returns size of currently opened chunk, 0 otherwise
	void			w_compressed(pvoid ptr, u32 count);
	void			w_chunk(u32 type, pvoid data, u32 size);
	virtual bool	valid( )
	{
		return true;
	}
};

class CORE_API CMemoryWriter : public IWriter
{
	u8* data;
	u32				position;
	u32				mem_size;
	u32				file_size;

public:
	CMemoryWriter( )
	{
		data = 0;
		position = 0;
		mem_size = 0;
		file_size = 0;
	}
	virtual	~CMemoryWriter( );

	// kernel
	virtual void	w(pcvoid ptr, u32 count);

	virtual void	seek(u32 pos)
	{
		position = pos;
	}
	virtual u32		tell( )
	{
		return position;
	}

	// specific
	IC u8* pointer( )
	{
		return data;
	}
	IC u32			size( ) const
	{
		return file_size;
	}
	IC void			clear( )
	{
		file_size = 0;
		position = 0;
	}
#pragma warning(push)
#pragma warning(disable:4995)
	IC void			free( )
	{
		file_size = 0;
		position = 0;
		mem_size = 0;
		xr_free(data);
	}
#pragma warning(pop)
	bool			save_to(pcstr fn);
};

//------------------------------------------------------------------------------------
// Read
//------------------------------------------------------------------------------------
template <typename implementation_type>
class IReaderBase
{
public:
	virtual			~IReaderBase( )
	{ }

	IC implementation_type& impl( )
	{
		return *(implementation_type*) this;
	}
	IC const implementation_type& impl( ) const
	{
		return *(implementation_type*) this;
	}

	IC BOOL			eof( )	const
	{
		return impl( ).elapsed( ) <= 0;
	}

	IC void			r(pvoid p, int cnt)
	{
		impl( ).r(p, cnt);
	}

	IC Fvector		r_vec3( )
	{
		Fvector tmp;
		r(&tmp, 3 * sizeof(f32));
		return tmp;
	}
	IC fVector4		r_vec4( )
	{
		fVector4 tmp;
		r(&tmp, 4 * sizeof(f32));
		return tmp;
	}
	IC u64			r_u64( )
	{
		u64 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC u32			r_u32( )
	{
		u32 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC u16			r_u16( )
	{
		u16 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC u8			r_u8( )
	{
		u8 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC s64			r_s64( )
	{
		s64 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC s32			r_s32( )
	{
		s32 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC s16			r_s16( )
	{
		s16 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC s8			r_s8( )
	{
		s8 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC f32			r_float( )
	{
		f32 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	IC void			r_fvector4(fVector4& v)
	{
		r(&v, sizeof(fVector4));
	}
	IC void			r_fvector3(fVector3& v)
	{
		r(&v, sizeof(fVector3));
	}
	IC void			r_fvector2(fVector2& v)
	{
		r(&v, sizeof(fVector2));
	}
	IC void			r_ivector4(iVector4& v)
	{
		r(&v, sizeof(iVector4));
	}
	IC void			r_ivector4(Ivector3& v)
	{
		r(&v, sizeof(Ivector3));
	}
	IC void			r_ivector4(iVector2& v)
	{
		r(&v, sizeof(iVector2));
	}
	IC void			r_fcolor(fColor& v)
	{
		r(&v, sizeof(fColor));
	}

	IC f32		r_float_q16(f32 min, f32 max)
	{
		u16	val = r_u16( );
		f32 A = (f32(val) * (max - min)) / 65535.0f + min;		// floating-point-error possible
		VERIFY((A >= min - EPSILON_7) && (A <= max + EPSILON_7));
		return A;
	}
	IC f32		r_float_q8(f32 min, f32 max)
	{
		u8 val = r_u8( );
		f32	A = (f32(val) / 255.0001f) * (max - min) + min;	// floating-point-error possible
		VERIFY((A >= min) && (A <= max));
		return A;
	}
	IC f32		r_angle16( )
	{
		return r_float_q16(0, PI_MUL_2);
	}
	IC f32		r_angle8( )
	{
		return r_float_q8(0, PI_MUL_2);
	}
	IC void			r_dir(Fvector& A)
	{
		u16 t = r_u16( );
		pvDecompress(A, t);
	}
	IC void			r_sdir(Fvector& A)
	{
		u16	t = r_u16( );
		f32 s = r_float( );
		pvDecompress(A, t);
		A.mul(s);
	}
	// Set file pointer to start of chunk data (0 for root chunk)
	IC	void		rewind( )
	{
		impl( ).seek(0);
	}

	IC	u32 		find_chunk(u32 ID, BOOL* bCompressed = 0)
	{
		u32 dwSize;
		u32 dwType;

		rewind( );
		while (!eof( ))
		{
			dwType = r_u32( );
			dwSize = r_u32( );
			if ((dwType & (~CFS_CompressMark)) == ID)
			{
				VERIFY((u32) impl( ).tell( ) + dwSize <= (u32) impl( ).length( ));
				if (bCompressed)
				{
					*bCompressed = dwType & CFS_CompressMark;
				}

				return dwSize;
			}
			else
			{
				impl( ).advance(dwSize);
			}
		}

		return 0;
	}

	IC	BOOL		r_chunk(u32 ID, pvoid dest)	// чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
	{
		u32	dwSize = find_chunk(ID);
		if (dwSize != 0)
		{
			r(dest, dwSize);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	IC	BOOL		r_chunk_safe(u32 ID, pvoid dest, u32 dest_size)	// чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
	{
		u32	dwSize = find_chunk(ID);
		if (dwSize != 0)
		{
			R_ASSERT(dwSize == dest_size);
			r(dest, dwSize);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
};

class CORE_API IReader : public IReaderBase<IReader>
{
protected:
	pstr data;
	s32				Pos;
	s32				Size;
	s32				iterpos;

public:
	IC				IReader( )
	{
		Pos = 0;
	}

	IC				IReader(pvoid _data, s32 _size, s32 _iterpos = 0)
	{
		data = (pstr) _data;
		Size = _size;
		Pos = 0;
		iterpos = _iterpos;
	}

protected:
	IC u32			correction(u32 p)
	{
		if (p % 16)
		{
			return ((p % 16) + 1) * 16 - p;
		}
		
		return 0;
	}

	u32 			advance_term_string( );

public:
	IC s32			elapsed( ) const
	{
		return Size - Pos;
	}
	IC s32			tell( ) const
	{
		return Pos;
	}
	IC void			seek(s32 ptr)
	{
		Pos = ptr;
		VERIFY((Pos <= Size) && (Pos >= 0));
	}
	IC s32			length( ) const
	{
		return Size;
	}
	IC pvoid pointer( ) const
	{
		return &(data[Pos]);
	}
	IC void			advance(s32 cnt)
	{
		Pos += cnt;
		VERIFY((Pos <= Size) && (Pos >= 0));
	}

public:
	void			r(pvoid p, s32 cnt);

	void			r_string(pstr dest, u32 tgt_sz);
	void			r_string(xr_string& dest);

	void			skip_stringZ( );

	void			r_stringZ(pstr dest, u32 tgt_sz);
	void			r_stringZ(shared_str& dest);
	void			r_stringZ(xr_string& dest);

public:
	void			close( );

public:
	// поиск XR Chunk'ов - возврат - размер или 0
	IReader* open_chunk(u32 ID);

	// iterators
	IReader* open_chunk_iterator(u32& ID, IReader* previous = NULL);	// NULL=first
};

class CORE_API CVirtualFileRW : public IReader
{
private:
	pvoid hSrcFile;
	pvoid hSrcMap;

public:
	CVirtualFileRW(pcstr cFileName);
	virtual ~CVirtualFileRW( );
};

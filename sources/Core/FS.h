// FS.h: interface for the CFS class.

#pragma once

#define CFS_CompressMark	(1ul << 31ul)

CORE_API void VerifyPath(const char* path);

#ifdef DEBUG
CORE_API	extern	U32		g_file_mapped_memory;
CORE_API	extern	U32		g_file_mapped_count;
CORE_API			void	dump_file_mappings( );
extern	void	register_file_mapping(void* address, const U32& size, const char* file_name);
extern	void	unregister_file_mapping(void* address, const U32& size);
#endif // DEBUG

//------------------------------------------------------------------------------------
// Write
//------------------------------------------------------------------------------------
class CORE_API IWriter
{
private:
	xr_stack<U32>		chunk_pos;

public:
	shared_str			fName;

	IWriter( )
	{ }
	virtual	~IWriter( )
	{
		R_ASSERT3(chunk_pos.empty( ), "Opened chunk not closed.", *fName);
	}

	// kernel
	virtual void	seek(U32 pos) = 0;
	virtual U32		tell( ) = 0;

	virtual void	w(const void* ptr, U32 count) = 0;

	// generalized writing functions
	inline void			w_u64(U64 d)
	{
		w(&d, sizeof(U64));
	}
	inline void			w_u32(U32 d)
	{
		w(&d, sizeof(U32));
	}
	inline void			w_u16(U16 d)
	{
		w(&d, sizeof(U16));
	}
	inline void			w_u8(U8 d)
	{
		w(&d, sizeof(U8));
	}
	inline void			w_s64(S64 d)
	{
		w(&d, sizeof(S64));
	}
	inline void			w_s32(int d)
	{
		w(&d, sizeof(int));
	}
	inline void			w_s16(S16 d)
	{
		w(&d, sizeof(S16));
	}
	inline void			w_s8(S8 d)
	{
		w(&d, sizeof(S8));
	}
	inline void			w_float(F32 d)
	{
		w(&d, sizeof(F32));
	}
	inline void			w_string(const char* p)
	{
		w(p, (U32) xr_strlen(p));
		w_u8(13);
		w_u8(10);
	}
	inline void			w_stringZ(const char* p)
	{
		w(p, (U32) xr_strlen(p) + 1);
	}
	inline void			w_stringZ(const shared_str& p)
	{
		w(*p ? *p : "", p.size( )); w_u8(0);
	}
	inline void			w_stringZ(shared_str& p)
	{
		w(*p ? *p : "", p.size( )); w_u8(0);
	}
	inline void			w_stringZ(const xr_string& p)
	{
		w(p.c_str( ) ? p.c_str( ) : "", (U32) p.size( )); w_u8(0);
	}
	inline void			w_fcolor(const Fcolor& v)
	{
		w(&v, sizeof(Fcolor));
	}
	inline void			w_fvector4(const Fvector4& v)
	{
		w(&v, sizeof(Fvector4));
	}
	inline void			w_fvector3(const Fvector3& v)
	{
		w(&v, sizeof(Fvector3));
	}
	inline void			w_fvector2(const Fvector2& v)
	{
		w(&v, sizeof(Fvector2));
	}
	inline void			w_ivector4(const Ivector4& v)
	{
		w(&v, sizeof(Ivector4));
	}
	inline void			w_ivector3(const Ivector3& v)
	{
		w(&v, sizeof(Ivector3));
	}
	inline void			w_ivector2(const Ivector2& v)
	{
		w(&v, sizeof(Ivector2));
	}

	// quant writing functions
	inline void 		w_float_q16(F32 a, F32 min, F32 max)
	{
		VERIFY(a >= min && a <= max);
		F32 q = (a - min) / (max - min);
		w_u16(U16(iFloor(q * 65535.0f + 0.5f)));
	}
	inline void 		w_float_q8(F32 a, F32 min, F32 max)
	{
		VERIFY(a >= min && a <= max);
		F32 q = (a - min) / (max - min);
		w_u8(U8(iFloor(q * 255.0f + 0.5f)));
	}
	inline void 		w_angle16(F32 a)
	{
		w_float_q16(angle_normalize(a), 0, PI_MUL_2);
	}
	inline void 		w_angle8(F32 a)
	{
		w_float_q8(angle_normalize(a), 0, PI_MUL_2);
	}
	inline void 		w_dir(const Fvector3& D)
	{
		w_u16(pvCompress(D));
	}
	void 			w_sdir(const Fvector3& D);
	void	__cdecl	w_printf(const char* format, ...);

	// generalized chunking
	U32				align( );
	void			open_chunk(U32 type);
	void			close_chunk( );
	U32				chunk_size( );					// returns size of currently opened chunk, 0 otherwise
	void			w_compressed(void* ptr, U32 count);
	void			w_chunk(U32 type, void* data, U32 size);
	virtual bool	valid( )
	{
		return true;
	}
};

class CORE_API CMemoryWriter : public IWriter
{
	U8* data;
	U32				position;
	U32				mem_size;
	U32				file_size;

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
	virtual void	w(const void* ptr, U32 count);

	virtual void	seek(U32 pos)
	{
		position = pos;
	}
	virtual U32		tell( )
	{
		return position;
	}

// specific
	inline U8* pointer( )
	{
		return data;
	}
	inline U32			size( ) const
	{
		return file_size;
	}
	inline void			clear( )
	{
		file_size = 0;
		position = 0;
	}
#pragma warning(push)
#pragma warning(disable:4995)
	inline void			free( )
	{
		file_size = 0;
		position = 0;
		mem_size = 0;
		xr_free(data);
	}
#pragma warning(pop)
	bool			save_to(const char* fn);
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

	inline implementation_type& impl( )
	{
		return *(implementation_type*) this;
	}
	inline const implementation_type& impl( ) const
	{
		return *(implementation_type*) this;
	}

	inline BOOL			eof( ) const
	{
		return impl( ).elapsed( ) <= 0;
	};

	inline void			r(void* p, int cnt)
	{
		impl( ).r(p, cnt);
	}

	inline Fvector3		r_vec3( )
	{
		Fvector3 tmp;
		r(&tmp, 3 * sizeof(F32));
		return tmp;
	}
	inline Fvector4		r_vec4( )
	{
		Fvector4 tmp;
		r(&tmp, 4 * sizeof(F32));
		return tmp;
	}
	inline U64			r_u64( )
	{
		U64 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	inline U32			r_u32( )
	{
		U32 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	inline U16			r_u16( )
	{
		U16 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	inline U8			r_u8( )
	{
		U8 tmp;	
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	inline S64			r_s64( )
	{
		S64 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	inline int			r_s32( )
	{
		int tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	inline S16			r_s16( )
	{
		S16 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	inline S8			r_s8( )
	{
		S8 tmp;	
		r(&tmp, sizeof(tmp)); 
		return tmp;
	}
	inline F32		r_float( )
	{
		F32 tmp;
		r(&tmp, sizeof(tmp));
		return tmp;
	}
	inline void			r_fvector4(Fvector4& v)
	{
		r(&v, sizeof(Fvector4));
	}
	inline void			r_fvector3(Fvector3& v)
	{
		r(&v, sizeof(Fvector3));
	}
	inline void			r_fvector2(Fvector2& v)
	{
		r(&v, sizeof(Fvector2));
	}
	inline void			r_ivector4(Ivector4& v)
	{
		r(&v, sizeof(Ivector4));
	}
	inline void			r_ivector4(Ivector3& v)
	{
		r(&v, sizeof(Ivector3));
	}
	inline void			r_ivector4(Ivector2& v)
	{
		r(&v, sizeof(Ivector2));
	}
	inline void			r_fcolor(Fcolor& v)
	{
		r(&v, sizeof(Fcolor));
	}

	inline F32		r_float_q16(F32 min, F32 max)
	{
		U16 val = r_u16( );
		F32 A = (F32(val) * (max - min)) / 65535.0f + min;		// floating-point-error possible
		VERIFY((A >= min - EPS_S) && (A <= max + EPS_S));
		return A;
	}
	inline F32		r_float_q8(F32 min, F32 max)
	{
		U8 val = r_u8( );
		F32 A = (F32(val) / 255.0001f) * (max - min) + min;	// floating-point-error possible
		VERIFY((A >= min) && (A <= max));
		return	A;
	}
	inline F32		r_angle16( )
	{
		return r_float_q16(0, PI_MUL_2);
	}
	inline F32		r_angle8( )
	{
		return r_float_q8(0, PI_MUL_2);
	}
	inline void			r_dir(Fvector3& A)
	{
		U16 t = r_u16( ); pvDecompress(A, t);
	}
	inline void			r_sdir(Fvector3& A)
	{
		U16 t = r_u16( );
		F32 s = r_float( );
		pvDecompress(A, t);
		A.mul(s);
	}
	// Set file pointer to start of chunk data (0 for root chunk)
	inline void		rewind( )
	{
		impl( ).seek(0);
	}

	inline U32 		find_chunk(U32 ID, BOOL* bCompressed = 0)
	{
		U32 dwSize;
		U32 dwType;

		rewind( );
		while (!eof( ))
		{
			dwType = r_u32( );
			dwSize = r_u32( );
			if ((dwType & (~CFS_CompressMark)) == ID)
			{

				VERIFY((U32) impl( ).tell( ) + dwSize <= (U32) impl( ).length( ));
				if (bCompressed) *bCompressed = dwType & CFS_CompressMark;
				return dwSize;
			}
			else
			{
				impl( ).advance(dwSize);
			}
		}

		return 0;
	}

	inline BOOL		r_chunk(U32 ID, void* dest)	// чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
	{
		U32 dwSize = find_chunk(ID);
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

	inline BOOL		r_chunk_safe(U32 ID, void* dest, U32 dest_size)	// чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
	{
		U32 dwSize = find_chunk(ID);
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
	char* data;
	int				Pos;
	int				Size;
	int				iterpos;

public:
	inline				IReader( )
	{
		Pos = 0;
	}

	inline				IReader(void* _data, int _size, int _iterpos = 0)
	{
		data = (char*) _data;
		Size = _size;
		Pos = 0;
		iterpos = _iterpos;
	}

protected:
	inline U32			correction(U32 p)
	{
		if (p % 16)
		{
			return ((p % 16) + 1) * 16 - p;
		}
		
		return 0;
	}

	U32 			advance_term_string( );

public:
	inline int			elapsed( ) const
	{
		return Size - Pos;
	}
	inline int			tell( ) const
	{
		return Pos;
	}
	inline void			seek(int ptr)
	{
		Pos = ptr;
		VERIFY((Pos <= Size) && (Pos >= 0));
	}
	inline int			length( ) const
	{
		return Size;
	}
	inline void* pointer( ) const
	{
		return &(data[Pos]);
	}
	inline void			advance(int cnt)
	{
		Pos += cnt;
		VERIFY((Pos <= Size) && (Pos >= 0));
	}

	void			r(void* p, int cnt);

	void			r_string(char* dest, U32 tgt_sz);
	void			r_string(xr_string& dest);

	void			skip_stringZ( );

	void			r_stringZ(char* dest, U32 tgt_sz);
	void			r_stringZ(shared_str& dest);
	void			r_stringZ(xr_string& dest);

	void			close( );

	// поиск XR Chunk'ов - возврат - размер или 0
	IReader* open_chunk(U32 ID);

	// iterators
	IReader* open_chunk_iterator(U32& ID, IReader* previous = nullptr);	// NULL=first
};

class CORE_API CVirtualFileRW : public IReader
{
private:
	void* hSrcFile;
	void* hSrcMap;

public:
	CVirtualFileRW(const char* cFileName);
	virtual ~CVirtualFileRW( );
};

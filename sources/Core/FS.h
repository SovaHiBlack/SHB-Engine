// FS.h: interface for the CFS class.
/////////////////////////////////////////////////////////////////////
#pragma once

#define CFS_CompressMark	(1ul << 31ul)

CORE_API void VerifyPath	(const char* path);

#ifdef DEBUG
	CORE_API	extern	u32		g_file_mapped_memory;
	CORE_API	extern	u32		g_file_mapped_count;
	CORE_API			void	dump_file_mappings		();
				extern	void	register_file_mapping	(void *address, const u32 &size, const char* file_name);
				extern	void	unregister_file_mapping	(void *address, const u32 &size);
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
	IWriter	()
	{
	}
	virtual	~IWriter	()
	{
		R_ASSERT3	(chunk_pos.empty(),"Opened chunk not closed.",*fName);
	}

	// kernel
	virtual void	seek	(u32 pos)						= 0;
	virtual u32		tell	()								= 0;

	virtual void	w		(const void* ptr, u32 count)	= 0;

	// generalized writing functions
	inline void			w_u64	(u64 d)					{	w(&d,sizeof(u64));	}
	inline void			w_u32	(u32 d)					{	w(&d,sizeof(u32));	}
	inline void			w_u16	(U16 d)					{	w(&d,sizeof(U16));	}
	inline void			w_u8	(U8 d)					{	w(&d,sizeof(U8));	}
	inline void			w_s64	(s64 d)					{	w(&d,sizeof(s64));	}
	inline void			w_s32	(int d)					{	w(&d,sizeof(int));	}
	inline void			w_s16	(S16 d)					{	w(&d,sizeof(S16));	}
	inline void			w_s8	(S8 d)					{	w(&d,sizeof(S8));	}
	inline void			w_float	(float d)				{	w(&d,sizeof(float));}
	inline void			w_string(const char* p)			{	w(p,(u32)xr_strlen(p));w_u8(13);w_u8(10);	}
	inline void			w_stringZ(const char* p)		{	w(p,(u32)xr_strlen(p)+1);					}
	inline void			w_stringZ(const shared_str& p) 	{	w(*p?*p:"",p.size());w_u8(0);		}
	inline void			w_stringZ(shared_str& p)		{	w(*p?*p:"",p.size());w_u8(0);		}
	inline void			w_stringZ(const xr_string& p)	{	w(p.c_str()?p.c_str():"",(u32)p.size());w_u8(0);	}
	inline void			w_fcolor(const Fcolor &v)		{	w(&v,sizeof(Fcolor));	}
	inline void			w_fvector4(const Fvector4 &v)	{	w(&v,sizeof(Fvector4));	}
	inline void			w_fvector3(const Fvector3 &v)	{	w(&v,sizeof(Fvector3));	}
	inline void			w_fvector2(const Fvector2 &v)	{	w(&v,sizeof(Fvector2));	}
	inline void			w_ivector4(const Ivector4 &v)	{	w(&v,sizeof(Ivector4));	}
	inline void			w_ivector3(const Ivector3 &v)	{	w(&v,sizeof(Ivector3));	}
	inline void			w_ivector2(const Ivector2 &v)	{	w(&v,sizeof(Ivector2));	}

	// quant writing functions
	inline void 		w_float_q16	(float a, float min, float max)
	{
		VERIFY		(a>=min && a<=max);
		float q		= (a-min)/(max-min);
		w_u16(U16(iFloor(q * 65535.0f + 0.5f)));
	}
	inline void 		w_float_q8	(float a, float min, float max)
	{
		VERIFY		(a>=min && a<=max);
		float q		= (a-min)/(max-min);
		w_u8		(U8(iFloor(q*255.f+.5f)));
	}
	inline void 		w_angle16	(float a)		    {	w_float_q16	(angle_normalize(a),0,PI_MUL_2);}
	inline void 		w_angle8	(float a)		    {	w_float_q8	(angle_normalize(a),0,PI_MUL_2);}
	inline void 		w_dir		(const Fvector3& D) 	{	w_u16(pvCompress(D));	}
	void 			w_sdir		(const Fvector3& D);
	void	__cdecl	w_printf	(const char* format, ...);

	// generalized chunking
	u32				align		();
	void			open_chunk	(u32 type);
	void			close_chunk	();
	u32				chunk_size	();					// returns size of currently opened chunk, 0 otherwise
	void			w_compressed(void* ptr, u32 count);
	void			w_chunk		(u32 type, void* data, u32 size);
	virtual bool	valid		()									{return true;}
};

class CORE_API CMemoryWriter : public IWriter
{
	U8*				data;
	u32				position;
	u32				mem_size;
	u32				file_size;
public:
	CMemoryWriter() {
		data		= 0;
		position	= 0;
		mem_size	= 0;
		file_size	= 0;
	}
	virtual	~CMemoryWriter();

	// kernel
	virtual void	w			(const void* ptr, u32 count);

	virtual void	seek		(u32 pos)	{	position = pos;				}
	virtual u32		tell		() 			{	return position;			}

	// specific
	inline U8*			pointer		()			{	return data;				}
	inline u32			size		() const 	{	return file_size;			}
	inline void			clear		()			{	file_size=0; position=0;	}
#pragma warning(push)
#pragma warning(disable:4995)
	inline void			free		()			{	file_size=0; position=0; mem_size=0; xr_free(data);	}
#pragma warning(pop)
	bool			save_to		(const char* fn);
};

//------------------------------------------------------------------------------------
// Read
//------------------------------------------------------------------------------------
template <typename implementation_type>
class IReaderBase {
public:
	virtual			~IReaderBase()				{}

	inline implementation_type&impl	()				{return *(implementation_type*)this;}
	inline const implementation_type&impl() const	{return *(implementation_type*)this;}

	inline BOOL			eof			()	const		{return impl().elapsed()<=0;	};
	
	inline void			r			(void *p,int cnt) {impl().r(p,cnt);}

	inline Fvector3		r_vec3		()			{
		Fvector3 tmp;r(&tmp,3*sizeof(float));return tmp;	};
	inline Fvector4		r_vec4		()			{Fvector4 tmp;r(&tmp,4*sizeof(float));return tmp;	};
	inline u64			r_u64		()			{	u64 tmp;	r(&tmp,sizeof(tmp)); return tmp;	};
	inline u32			r_u32		()			{	u32 tmp;	r(&tmp,sizeof(tmp)); return tmp;	};
	inline U16			r_u16		()			{	U16 tmp;	r(&tmp,sizeof(tmp)); return tmp;	};
	inline U8			r_u8		()			{	U8 tmp;		r(&tmp,sizeof(tmp)); return tmp;	};
	inline s64			r_s64		()			{	s64 tmp;	r(&tmp,sizeof(tmp)); return tmp;	};
	inline int			r_s32		()			{	int tmp;	r(&tmp,sizeof(tmp)); return tmp;	};
	inline S16			r_s16		()			{	S16 tmp;	r(&tmp,sizeof(tmp)); return tmp;	};
	inline S8			r_s8		()			{	S8 tmp;		r(&tmp,sizeof(tmp)); return tmp;	};
	inline float		r_float		()			{	float tmp;	r(&tmp,sizeof(tmp)); return tmp;	};
	inline void			r_fvector4	(Fvector4 &v){	r(&v,sizeof(Fvector4));	}
	inline void			r_fvector3	(Fvector3 &v){	r(&v,sizeof(Fvector3));	}
	inline void			r_fvector2	(Fvector2 &v){	r(&v,sizeof(Fvector2));	}
	inline void			r_ivector4	(Ivector4 &v){	r(&v,sizeof(Ivector4));	}
	inline void			r_ivector4	(Ivector3 &v){	r(&v,sizeof(Ivector3));	}
	inline void			r_ivector4	(Ivector2 &v){	r(&v,sizeof(Ivector2));	}
	inline void			r_fcolor	(Fcolor &v)	{	r(&v,sizeof(Fcolor));	}
	
	inline float		r_float_q16	(float min, float max)
	{
		U16	val 	= r_u16();
		float A		= (float(val)*(max-min))/65535.f + min;		// floating-point-error possible
		VERIFY		((A >= min-EPS_S) && (A <= max+EPS_S));
		return A;
	}
	inline float		r_float_q8	(float min, float max)
	{
		U8 val		= r_u8();
		float	A	= (float(val)/255.0001f) *(max-min) + min;	// floating-point-error possible
		VERIFY		((A >= min) && (A <= max));
		return	A;
	}
	inline float		r_angle16	()			{ return r_float_q16(0,PI_MUL_2);	}
	inline float		r_angle8	()			{ return r_float_q8	(0,PI_MUL_2);	}
	inline void			r_dir		(Fvector3& A){ U16 t=r_u16(); pvDecompress(A,t); }
	inline void			r_sdir		(Fvector3& A)
	{
		U16	t		= r_u16();
		float s		= r_float();
		pvDecompress(A,t);
		A.mul		(s);
	}
	// Set file pointer to start of chunk data (0 for root chunk)
	inline	void		rewind		()			{	impl().seek(0); }

	inline	u32 		find_chunk	(u32 ID, BOOL* bCompressed = 0)
	{
		u32	dwSize,dwType;

		rewind();
		while (!eof()) {
			dwType = r_u32();
			dwSize = r_u32();
			if ((dwType&(~CFS_CompressMark)) == ID) {
				
				VERIFY	((u32)impl().tell() + dwSize <= (u32)impl().length());
				if (bCompressed) *bCompressed = dwType&CFS_CompressMark;
				return dwSize;
			}
			else	impl().advance(dwSize);
		}
		return 0;
	}
	
	inline	BOOL		r_chunk		(u32 ID, void *dest)	// чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
	{
		u32	dwSize = find_chunk(ID);
		if (dwSize!=0) {
			r(dest,dwSize);
			return TRUE;
		} else return FALSE;
	}
	
	inline	BOOL		r_chunk_safe(u32 ID, void *dest, u32 dest_size)	// чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
	{
		u32	dwSize = find_chunk(ID);
		if (dwSize!=0) {
			R_ASSERT(dwSize==dest_size);
			r(dest,dwSize);
			return TRUE;
		} else return FALSE;
	}
};

class CORE_API IReader : public IReaderBase<IReader> {
protected:
	char *			data	;
	int				Pos		;
	int				Size	;
	int				iterpos	;

public:
	inline				IReader			()
	{
		Pos			= 0;
	}

	inline				IReader			(void *_data, int _size, int _iterpos=0)
	{
		data		= (char *)_data	;
		Size		= _size			;
		Pos			= 0				;
		iterpos		= _iterpos		;
	}

protected:
	inline u32			correction					(u32 p)
	{
		if (p%16) {
			return ((p%16)+1)*16 - p;
		} return 0;
	}
	
	u32 			advance_term_string			();

public:
	inline int			elapsed		()	const		{	return Size-Pos;		};
	inline int			tell		()	const		{	return Pos;				};
	inline void			seek		(int ptr)		{	Pos=ptr; VERIFY((Pos<=Size) && (Pos>=0));};
	inline int			length		()	const		{	return Size;			};
	inline void*		pointer		()	const		{	return &(data[Pos]);	};
	inline void			advance		(int cnt)		{	Pos+=cnt;VERIFY((Pos<=Size) && (Pos>=0));};

public:
	void			r			(void *p,int cnt);

	void			r_string	(char *dest, u32 tgt_sz);
	void			r_string	(xr_string& dest);

	void			skip_stringZ();

	void			r_stringZ	(char *dest, u32 tgt_sz);
	void			r_stringZ	(shared_str& dest);
	void			r_stringZ	(xr_string& dest);

public:
	void			close		();

public:
	// поиск XR Chunk'ов - возврат - размер или 0
	IReader*		open_chunk	(u32 ID);

	// iterators
	IReader*		open_chunk_iterator		(u32& ID, IReader* previous=nullptr);	// NULL=first
};

class CORE_API CVirtualFileRW : public IReader
{
private:
	void	*hSrcFile, *hSrcMap;
public:
			CVirtualFileRW		(const char* cFileName);
	virtual ~CVirtualFileRW		();
};

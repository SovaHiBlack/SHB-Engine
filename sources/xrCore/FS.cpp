#include "stdafx.h"

#include "fs_internal.h"

#pragma warning(disable:4995)
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default:4995)

typedef void DUMMY_STUFF(pcvoid, const u32&, pvoid);
CORE_API DUMMY_STUFF* g_dummy_stuff = 0;

#ifdef DEBUG
CORE_API	u32								g_file_mapped_memory = 0;
u32								g_file_mapped_count = 0;
typedef std::map<u32, std::pair<u32, shared_str> >	FILE_MAPPINGS;
FILE_MAPPINGS					g_file_mappings;

void register_file_mapping(pvoid address, const u32& size, pcstr file_name)
{
	FILE_MAPPINGS::const_iterator	I = g_file_mappings.find(*(u32*) &address);
	VERIFY(I == g_file_mappings.end( ));
	g_file_mappings.insert(std::make_pair(*(u32*) &address, std::make_pair(size, shared_str(file_name))));

	g_file_mapped_memory += size;
	++g_file_mapped_count;

#ifdef USE_MEMORY_MONITOR
	//	memory_monitor::monitor_alloc	(addres,size,"file mapping");
	string512						temp;
	sprintf_s(temp, sizeof(temp), "file mapping: %s", file_name);
	memory_monitor::monitor_alloc(address, size, temp);
#endif // USE_MEMORY_MONITOR

}

void unregister_file_mapping(pvoid address, const u32& size)
{
	FILE_MAPPINGS::iterator			I = g_file_mappings.find(*(u32*) &address);
	VERIFY(I != g_file_mappings.end( ));
	//	VERIFY2							((*I).second.first == size,make_string("file mapping sizes are different: %d -> %d",(*I).second.first,size));
	g_file_mapped_memory -= (*I).second.first;
	--g_file_mapped_count;

	g_file_mappings.erase(I);

#ifdef USE_MEMORY_MONITOR
	memory_monitor::monitor_free(address);
#endif // USE_MEMORY_MONITOR

}

CORE_API void dump_file_mappings( )
{
	Msg("* active file mappings (%d):", g_file_mappings.size( ));

	FILE_MAPPINGS::const_iterator	I = g_file_mappings.begin( );
	FILE_MAPPINGS::const_iterator	E = g_file_mappings.end( );
	for (; I != E; ++I)
		Msg(
			"* [0x%08x][%d][%s]",
			(*I).first,
			(*I).second.first,
			(*I).second.second.c_str( )
		);
}
#endif // DEBUG

//////////////////////////////////////////////////////////////////////
// Tools
//////////////////////////////////////////////////////////////////////
void VerifyPath(pcstr path)
{
	string1024 tmp;
	for (s32 i = 0; path[i]; i++)
	{
		if (path[i] != '\\' || i == 0)
		{
			continue;
		}

		CopyMemory(tmp, path, i);
		tmp[i] = 0;
		_mkdir(tmp);
	}
}

pvoid FileDownload(pcstr fn, u32* pdwSize)
{
	s32		hFile;
	u32		size;
	pvoid buf;

	hFile = _open(fn, O_RDONLY | O_BINARY | O_SEQUENTIAL, _S_IREAD);

	if (hFile <= 0)
	{
		Sleep(1);
		hFile = _open(fn, O_RDONLY | O_BINARY | O_SEQUENTIAL, _S_IREAD);
	}

	R_ASSERT2(hFile > 0, fn);

	size = _filelength(hFile);

	buf = Memory.mem_alloc(size

#ifdef DEBUG_MEMORY_NAME
						   , "FILE in memory"
#endif // DEBUG_MEMORY_NAME

	);
	s32 r_bytes = _read(hFile, buf, size);
	R_ASSERT3(r_bytes == (s32) size, "Can't read file data:", fn);
	_close(hFile);
	if (pdwSize)
	{
		*pdwSize = size;
	}

	return buf;
}

typedef char MARK[9];
IC void mk_mark(MARK& M, pcstr S)
{
	strncpy(M, S, 8);
}

void FileCompress(pcstr fn, pcstr sign, pvoid data, u32 size)
{
	MARK M;
	mk_mark(M, sign);

	s32 H = open(fn, O_BINARY | O_CREAT | O_WRONLY | O_TRUNC, S_IREAD | S_IWRITE);
	R_ASSERT2(H > 0, fn);
	_write(H, &M, 8);
	_writeLZ(H, data, size);
	_close(H);
}

pvoid FileDecompress(pcstr fn, pcstr sign, u32* size)
{
	MARK M, F; mk_mark(M, sign);

	s32 H = open(fn, O_BINARY | O_RDONLY);
	R_ASSERT2(H > 0, fn);
	_read(H, &F, 8);
	if (strncmp(M, F, 8) != 0)
	{
		F[8] = 0;
		Msg("FATAL: signatures doesn't match, file(%s) / requested(%s)", F, sign);
	}

	R_ASSERT(strncmp(M, F, 8) == 0);

	pvoid ptr = 0; u32 SZ;
	SZ = _readLZ(H, ptr, filelength(H) - 8);
	_close(H);
	if (size)
	{
		*size = SZ;
	}

	return ptr;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//---------------------------------------------------
// memory
CMemoryWriter::~CMemoryWriter( )
{
	xr_free(data);
}

void CMemoryWriter::w(pcvoid ptr, u32 count)
{
	if (position + count > mem_size)
	{
		// reallocate
		if (mem_size == 0)
		{
			mem_size = 128;
		}

		while (mem_size <= (position + count))
		{
			mem_size *= 2;
		}

		if (0 == data)
		{
			data = (u8*) Memory.mem_alloc(mem_size

#ifdef DEBUG_MEMORY_NAME
										  , "CMemoryWriter - storage"
#endif // DEBUG_MEMORY_NAME

			);
		}
		else
		{
			data = (u8*) Memory.mem_realloc(data, mem_size

#ifdef DEBUG_MEMORY_NAME
											, "CMemoryWriter - storage"
#endif // DEBUG_MEMORY_NAME

			);
		}
	}

	CopyMemory(data + position, ptr, count);
	position += count;
	if (position > file_size)
	{
		file_size = position;
	}
}

bool CMemoryWriter::save_to(pcstr fn)
{
	IWriter* F = FS.w_open(fn);
	if (F)
	{
		F->w(pointer( ), size( ));
		FS.w_close(F);
		return true;
	}

	return false;
}

void	IWriter::open_chunk(u32 type)
{
	w_u32(type);
	chunk_pos.push(tell( ));
	w_u32(0);	// the place for 'size'
}
void	IWriter::close_chunk( )
{
	VERIFY(!chunk_pos.empty( ));

	s32 pos = tell( );
	seek(chunk_pos.top( ));
	w_u32(pos - chunk_pos.top( ) - 4);
	seek(pos);
	chunk_pos.pop( );
}
u32 IWriter::chunk_size( )					// returns size of currently opened chunk, 0 otherwise
{
	if (chunk_pos.empty( ))
	{
		return 0;
	}

	return tell( ) - chunk_pos.top( ) - 4;
}

void IWriter::w_compressed(pvoid ptr, u32 count)
{
	u8* dest = 0;
	u32	dest_sz = 0;
	_compressLZ(&dest, &dest_sz, ptr, count);

	if (g_dummy_stuff)
	{
		g_dummy_stuff(dest, dest_sz, dest);
	}

	if (dest && dest_sz)
	{
		w(dest, dest_sz);
	}

	xr_free(dest);
}

void IWriter::w_chunk(u32 type, pvoid data, u32 size)
{
	open_chunk(type);
	if (type & CFS_CompressMark)
	{
		w_compressed(data, size);
	}
	else
	{
		w(data, size);
	}

	close_chunk( );
}
void IWriter::w_sdir(const fVector3& D)
{
	fVector3 C;
	f32 mag = D.magnitude( );
	if (mag > EPSILON_7)
	{
		C.div(D, mag);
	}
	else
	{
		C.set(0.0f, 0.0f, 1.0f);
		mag = 0.0f;
	}

	w_dir(C);
	w_float(mag);
}
void IWriter::w_printf(pcstr format, ...)
{
	va_list mark;
	string1024 buf;
	va_start(mark, format);
	vsprintf(buf, format, mark);
	w(buf, xr_strlen(buf));
}

//---------------------------------------------------
// base stream
IReader* IReader::open_chunk(u32 ID)
{
	BOOL bCompressed;
	u32 dwSize = find_chunk(ID, &bCompressed);
	if (dwSize != 0)
	{
		if (bCompressed)
		{
			u8* dest;
			u32 dest_sz;
			_decompressLZ(&dest, &dest_sz, pointer( ), dwSize);
			return xr_new<CTempReader>(dest, dest_sz, tell( ) + dwSize);
		}
		else
		{
			return xr_new<IReader>(pointer( ), dwSize, tell( ) + dwSize);
		}
	}
	else
	{
		return 0;
	}
}
void IReader::close( )
{
	xr_delete((IReader*) this);
}

IReader* IReader::open_chunk_iterator(u32& ID, IReader* _prev)
{
	if (0 == _prev)
	{
		// first
		rewind( );
	}
	else
	{
		// next
		seek(_prev->iterpos);
		_prev->close( );
	}

	//	open
	if (elapsed( ) < 8)
	{
		return NULL;
	}

	ID = r_u32( );
	u32 _size = r_u32( );
	if (ID & CFS_CompressMark)
	{
		// compressed
		u8* dest;
		u32 dest_sz;
		_decompressLZ(&dest, &dest_sz, pointer( ), _size);
		return xr_new<CTempReader>(dest, dest_sz, tell( ) + _size);
	}
	else
	{
		// normal
		return xr_new<IReader>(pointer( ), _size, tell( ) + _size);
	}
}

void IReader::r(pvoid p, s32 cnt)
{
	VERIFY(Pos + cnt <= Size);
	CopyMemory(p, pointer( ), cnt);
	advance(cnt);

#ifdef DEBUG
	BOOL	bShow = FALSE;
	if (dynamic_cast<CFileReader*>(this))
	{
		bShow = TRUE;
	}

	if (dynamic_cast<CVirtualFileReader*>(this))
	{
		bShow = TRUE;
	}

	if (bShow)
	{
		FS.dwOpenCounter++;
	}
#endif

}

IC BOOL is_term(char a)
{
	return (a == 13) || (a == 10);
}

IC u32	IReader::advance_term_string( )
{
	u32 sz = 0;
	pstr src = (pstr) data;
	while (!eof( ))
	{
		Pos++;
		sz++;
		if (!eof( ) && is_term(src[Pos]))
		{
			while (!eof( ) && is_term(src[Pos]))
			{
				Pos++;
			}

			break;
		}
	}

	return sz;
}
void IReader::r_string(pstr dest, u32 tgt_sz)
{
	pstr src = (pstr) data + Pos;
	u32 sz = advance_term_string( );
	R_ASSERT2(sz < (tgt_sz - 1), "Dest string less than needed.");
	strncpy(dest, src, sz);
	dest[sz] = 0;
}
void IReader::r_string(xr_string& dest)
{
	pstr src = (pstr) data + Pos;
	u32 sz = advance_term_string( );
	dest.assign(src, sz);
}
void IReader::r_stringZ(pstr dest, u32 tgt_sz)
{
	pstr src = (pstr) data;
	u32 sz = xr_strlen(src);
	R_ASSERT2(sz < tgt_sz, "Dest string less than needed.");
	while ((src[Pos] != 0) && (!eof( ))) *dest++ = src[Pos++];
	*dest = 0;
	Pos++;
}
void IReader::r_stringZ(shared_str& dest)
{
	dest = (pstr) (data + Pos);
	Pos += (dest.size( ) + 1);
}
void IReader::r_stringZ(xr_string& dest)
{
	dest = (pstr) (data + Pos);
	Pos += int(dest.size( ) + 1);
}

void IReader::skip_stringZ( )
{
	pstr src = (pstr) data;
	while ((src[Pos] != 0) && (!eof( )))
	{
		Pos++;
	}

	Pos++;
}

//---------------------------------------------------
// temp stream
CTempReader::~CTempReader( )
{
	xr_free(data);
}
//---------------------------------------------------
// pack stream
CPackReader::~CPackReader( )
{

#ifdef DEBUG
	unregister_file_mapping(base_address, Size);
#endif // DEBUG

	UnmapViewOfFile(base_address);
}
//---------------------------------------------------
// file stream
CFileReader::CFileReader(pcstr name)
{
	data = (pstr) FileDownload(name, (u32*) &Size);
	Pos = 0;
}

CFileReader::~CFileReader( )
{
	xr_free(data);
}
//---------------------------------------------------
// compressed stream
CCompressedReader::CCompressedReader(pcstr name, pcstr sign)
{
	data = (pstr) FileDecompress(name, sign, (u32*) &Size);
	Pos = 0;
}
CCompressedReader::~CCompressedReader( )
{
	xr_free(data);
}

CVirtualFileRW::CVirtualFileRW(pcstr cFileName)
{
	// Open the file
	hSrcFile = CreateFile(cFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	R_ASSERT3(hSrcFile != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError( )));
	Size = (s32) GetFileSize(hSrcFile, NULL);
	R_ASSERT3(Size, cFileName, Debug.error2string(GetLastError( )));

	hSrcMap = CreateFileMapping(hSrcFile, 0, PAGE_READWRITE, 0, 0, 0);
	R_ASSERT3(hSrcMap != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError( )));

	data = (pstr) MapViewOfFile(hSrcMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	R_ASSERT3(data, cFileName, Debug.error2string(GetLastError( )));

#ifdef DEBUG
	register_file_mapping(data, Size, cFileName);
#endif // DEBUG

}

CVirtualFileRW::~CVirtualFileRW( )
{

#ifdef DEBUG
	unregister_file_mapping(data, Size);
#endif // DEBUG

	UnmapViewOfFile((pvoid) data);
	CloseHandle(hSrcMap);
	CloseHandle(hSrcFile);
}

CVirtualFileReader::CVirtualFileReader(pcstr cFileName)
{
	// Open the file
	hSrcFile = CreateFile(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	R_ASSERT3(hSrcFile != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError( )));
	Size = (s32) GetFileSize(hSrcFile, NULL);
	R_ASSERT3(Size, cFileName, Debug.error2string(GetLastError( )));

	hSrcMap = CreateFileMapping(hSrcFile, 0, PAGE_READONLY, 0, 0, 0);
	R_ASSERT3(hSrcMap != INVALID_HANDLE_VALUE, cFileName, Debug.error2string(GetLastError( )));

	data = (pstr) MapViewOfFile(hSrcMap, FILE_MAP_READ, 0, 0, 0);
	R_ASSERT3(data, cFileName, Debug.error2string(GetLastError( )));

#ifdef DEBUG
	register_file_mapping(data, Size, cFileName);
#endif // DEBUG

}

CVirtualFileReader::~CVirtualFileReader( )
{

#ifdef DEBUG
	unregister_file_mapping(data, Size);
#endif // DEBUG

	UnmapViewOfFile((pvoid) data);
	CloseHandle(hSrcMap);
	CloseHandle(hSrcFile);
}

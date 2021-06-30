#pragma once

#include "lzhuf.h"
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>

Pvoid FileDownload(Pcstr fn, U32* pdwSize = nullptr);
void			FileCompress(Pcstr fn, Pcstr sign, Pvoid data, U32 size);
Pvoid FileDecompress(Pcstr fn, Pcstr sign, U32* size = nullptr);

class CFileWriter : public IWriter
{
private:
	FILE* hf;

public:
	CFileWriter(Pcstr name, bool exclusive)
	{
		R_ASSERT(name && name[0]);
		fName = name;
		VerifyPath(*fName);
		if (exclusive)
		{
			int handle = _sopen(*fName, _O_WRONLY | _O_TRUNC | _O_CREAT | _O_BINARY, SH_DENYWR);
			hf = _fdopen(handle, "wb");
		}
		else
		{
			hf = fopen(*fName, "wb");
			if (hf == 0)
				Msg("!Can't write file: '%s'. Error: '%s'.", *fName, _sys_errlist[errno]);
		}
	}

	virtual 		~CFileWriter( )
	{
		if (0 != hf)
		{
			fclose(hf);
			// release RO attrib
			DWORD dwAttr = GetFileAttributes(*fName);
			if ((dwAttr != U32(-1)) && (dwAttr & FILE_ATTRIBUTE_READONLY))
			{
				dwAttr &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(*fName, dwAttr);
			}
		}
	}
	// kernel
	virtual void	w(Pcvoid _ptr, U32 count)
	{
		if ((0 != hf) && (0 != count))
		{
			const U32 mb_sz = 0x1000000;
			U8* ptr = (U8*) _ptr;
			for (int req_size = count; req_size > mb_sz; req_size -= mb_sz, ptr += mb_sz)
			{
				size_t W = fwrite(ptr, mb_sz, 1, hf);
				R_ASSERT3(W == 1, "Can't write mem block to file. Disk maybe full.", _sys_errlist[errno]);
			}
			if (req_size)
			{
				size_t W = fwrite(ptr, req_size, 1, hf);
				R_ASSERT3(W == 1, "Can't write mem block to file. Disk maybe full.", _sys_errlist[errno]);
			}
		}
	};
	virtual void	seek(U32 pos)
	{
		if (0 != hf) fseek(hf, pos, SEEK_SET);
	}
	virtual U32		tell( )
	{
		return (0 != hf) ? ftell(hf) : 0;
	}
	virtual bool	valid( )
	{
		return (0 != hf);
	}
};

// It automatically frees memory after destruction
class CTempReader : public IReader
{
public:
	CTempReader(Pvoid _data, int _size, int _iterpos) : IReader(_data, _size, _iterpos)
	{ }
	virtual		~CTempReader( );
};
class CPackReader : public IReader
{
	Pvoid base_address;
public:
	CPackReader(Pvoid _base, Pvoid _data, int _size) : IReader(_data, _size)
	{
		base_address = _base;
	}
	virtual		~CPackReader( );
};
class CFileReader : public IReader
{
public:
	CFileReader(Pcstr name);
	virtual		~CFileReader( );
};
class CCompressedReader : public IReader
{
public:
	CCompressedReader(Pcstr name, Pcstr sign);
	virtual		~CCompressedReader( );
};
class CVirtualFileReader : public IReader
{
private:
	Pvoid hSrcFile;
	Pvoid hSrcMap;
public:
	CVirtualFileReader(Pcstr cFileName);
	virtual		~CVirtualFileReader( );
};

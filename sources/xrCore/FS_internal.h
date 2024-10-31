#pragma once

#include "LzHuf.h"

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>

pvoid FileDownload(pcstr fn, u32* pdwSize = NULL);
void FileCompress(pcstr fn, pcstr sign, pvoid data, u32 size);
pvoid FileDecompress(pcstr fn, pcstr sign, u32* size = NULL);

class CFileWriter : public IWriter
{
private:
	FILE* hf;

public:
	CFileWriter(pcstr name, bool exclusive)
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
			{
				Msg("!Can't write file: '%s'. Error: '%s'.", *fName, _sys_errlist[errno]);
			}
		}
	}

	virtual 		~CFileWriter( )
	{
		if (0 != hf)
		{
			fclose(hf);
			// release RO attrib
			DWORD dwAttr = GetFileAttributes(*fName);
			if ((dwAttr != u32(-1)) && (dwAttr & FILE_ATTRIBUTE_READONLY))
			{
				dwAttr &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(*fName, dwAttr);
			}
		}
	}
	// kernel
	virtual void	w(pcvoid _ptr, u32 count)
	{
		if ((0 != hf) && (0 != count))
		{
			const u32 mb_sz = 0x1000000;
			u8* ptr = (u8*) _ptr;
			for (s32 req_size = count; req_size > mb_sz; req_size -= mb_sz, ptr += mb_sz)
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
	}
	virtual void	seek(u32 pos)
	{
		if (0 != hf)
		{
			fseek(hf, pos, SEEK_SET);
		}
	}
	virtual u32		tell( )
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
	CTempReader(pvoid _data, s32 _size, s32 _iterpos) : IReader(_data, _size, _iterpos)
	{ }
	virtual		~CTempReader( );
};
class CPackReader : public IReader
{
	pvoid base_address;

public:
	CPackReader(pvoid _base, pvoid _data, s32 _size) : IReader(_data, _size)
	{
		base_address = _base;
	}
	virtual		~CPackReader( );
};

class CFileReader : public IReader
{
public:
	CFileReader(pcstr name);
	virtual		~CFileReader( );
};
class CCompressedReader : public IReader
{
public:
	CCompressedReader(pcstr name, pcstr sign);
	virtual		~CCompressedReader( );
};
class CVirtualFileReader : public IReader
{
private:
	pvoid hSrcFile;
	pvoid hSrcMap;
public:
	CVirtualFileReader(pcstr cFileName);
	virtual		~CVirtualFileReader( );
};

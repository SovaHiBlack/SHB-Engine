#include "stdafx.h"

#pragma warning(disable:4995)
#include <io.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default:4995)

//////////////////////////////////////////////////////////////////////
// FS_File
//////////////////////////////////////////////////////////////////////
FS_File::FS_File(xr_string nm, long sz, time_t modif, unsigned attr)
{
	set(nm, sz, modif, attr);
}
FS_File::FS_File(xr_string nm)
{
	set(nm, 0, 0, 0);
}
FS_File::FS_File(const _FINDDATA_T& f)
{
	set(f.name, f.size, f.time_write, (f.attrib & _A_SUBDIR) ? flSubDir : 0);
}
FS_File::FS_File(xr_string nm, const _FINDDATA_T& f)
{
	set(nm, f.size, f.time_write, (f.attrib & _A_SUBDIR) ? flSubDir : 0);
}

void FS_File::set(xr_string nm, long sz, time_t modif, unsigned attr)
{
	name = nm;		xr_strlwr(name);
	size = sz;
	time_write = modif;
	attrib = attr;
}

//////////////////////////////////////////////////////////////////////
// FS_Path
//////////////////////////////////////////////////////////////////////
FS_Path::FS_Path(pcstr _Root, pcstr _Add, pcstr _DefExt, pcstr _FilterCaption, u32 flags)
{
	//	VERIFY			(_Root&&_Root[0]);
	string_path		temp;
	strcpy_s(temp, sizeof(temp), _Root);
	if (_Add)
	{
		strcat(temp, _Add);
	}

	if (temp[0] && temp[xr_strlen(temp) - 1] != '\\')
	{
		strcat(temp, "\\");
	}

	m_Path = xr_strlwr(xr_strdup(temp));
	m_DefExt = _DefExt ? xr_strlwr(xr_strdup(_DefExt)) : 0;
	m_FilterCaption = _FilterCaption ? xr_strlwr(xr_strdup(_FilterCaption)) : 0;
	m_Add = _Add ? xr_strlwr(xr_strdup(_Add)) : 0;
	m_Root = _Root ? xr_strlwr(xr_strdup(_Root)) : 0;
	m_Flags.assign(flags);
}

FS_Path::~FS_Path( )
{
	xr_free(m_Root);
	xr_free(m_Path);
	xr_free(m_Add);
	xr_free(m_DefExt);
	xr_free(m_FilterCaption);
}

void	FS_Path::_set(pstr add)
{
	// m_Add
	R_ASSERT(add);
	xr_free(m_Add);
	m_Add = xr_strlwr(xr_strdup(add));

	// m_Path
	string_path		temp;
	strconcat(sizeof(temp), temp, m_Root, m_Add);
	if (temp[xr_strlen(temp) - 1] != '\\')
	{
		strcat(temp, "\\");
	}

	xr_free(m_Path);
	m_Path = xr_strlwr(xr_strdup(temp));
}

void	FS_Path::_set_root(pstr root)
{
	// m_Root
	xr_free(m_Root);
	m_Root = xr_strlwr(xr_strdup(root));
	if (m_Root[0] && m_Root[xr_strlen(m_Root) - 1] != '\\')
	{
		strcat(m_Root, "\\");
	}

	// m_Path
	string_path		temp;
	strconcat(sizeof(temp), temp, m_Root, m_Add ? m_Add : "");
	if (*temp && temp[xr_strlen(temp) - 1] != '\\')
	{
		strcat(temp, "\\");
	}

	xr_free(m_Path);
	m_Path = xr_strlwr(xr_strdup(temp));
}

pcstr FS_Path::_update(string_path& dest, pcstr src)const
{
	R_ASSERT(dest);
	R_ASSERT(src);
	string_path			temp;
	strcpy_s(temp, sizeof(temp), src);
	strconcat(sizeof(dest), dest, m_Path, temp);
	return xr_strlwr(dest);
}

void FS_Path::rescan_path_cb( )
{
	m_Flags.set(flNeedRescan, TRUE);
	FS.m_Flags.set(CLocatorAPI::flNeedRescan, TRUE);
}

bool CORE_API PatternMatch(pcstr s, pcstr mask)
{
	pcstr cp = 0;
	pcstr mp = 0;
	for (; *s && *mask != '*'; mask++, s++)
	{
		if (*mask != *s && *mask != '?')
		{
			return false;
		}
	}

	for (;;)
	{
		if (!*s)
		{
			while (*mask == '*')
			{
				mask++;
			}

			return !*mask;
		}

		if (*mask == '*')
		{
			if (!*++mask)
			{
				return true;
			}
			
			mp = mask;
			cp = s + 1;
			continue;
		}

		if (*mask == *s || *mask == '?')
		{
			mask++, s++;
			continue;
		}

		mask = mp;
		s = cp++;
	}
}

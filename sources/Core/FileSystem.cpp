#include "stdafx.h"

#include <cderr.h>
#include <commdlg.h>

EFS_Utils* xr_EFS = NULL;
//----------------------------------------------------
EFS_Utils::EFS_Utils( )
{ }

EFS_Utils::~EFS_Utils( )
{ }

xr_string EFS_Utils::ChangeFileExt(pcstr src, pcstr ext)
{
	xr_string tmp;
	pstr src_ext = strext(src);
	if (src_ext)
	{
		size_t ext_pos = src_ext - src;
		tmp.assign(src, 0, ext_pos);
	}
	else
	{
		tmp = src;
	}

	tmp += ext;
	return tmp;
}

xr_string	EFS_Utils::ChangeFileExt(const xr_string& src, pcstr ext)
{
	return ChangeFileExt(src.c_str( ), ext);
}
//----------------------------------------------------
pcstr MakeFilter(string1024& dest, pcstr info, pcstr ext)
{
	ZeroMemory(dest, sizeof(dest));
	if (ext)
	{
		s32 icnt = _GetItemCount(ext, ';');
		pstr dst = dest;
		if (icnt > 1)
		{
			strconcat(sizeof(dest), dst, info, " (", ext, ")");
			dst += (xr_strlen(dst) + 1);
			strcpy(dst, ext);
			dst += (xr_strlen(ext) + 1);
		}

		for (s32 i = 0; i < icnt; i++)
		{
			string64		buf;
			_GetItem(ext, i, buf, ';');
			strconcat(sizeof(dest), dst, info, " (", buf, ")");
			dst += (xr_strlen(dst) + 1);
			strcpy(dst, buf);
			dst += (xr_strlen(buf) + 1);
		}
	}

	return dest;
}

//------------------------------------------------------------------------------
// start_flt_ext = -1-all 0..n-indices
//------------------------------------------------------------------------------
bool EFS_Utils::GetOpenName(pcstr initial, string_path& buffer, s32 sz_buf, bool bMulti, pcstr offset, s32 start_flt_ext)
{
	VERIFY(buffer && (sz_buf > 0));
	FS_Path& P = *FS.get_path(initial);
	string1024 flt;
	MakeFilter(flt, P.m_FilterCaption ? P.m_FilterCaption : "", P.m_DefExt);

	OPENFILENAME ofn;
	Memory.mem_fill(&ofn, 0, sizeof(ofn));
	if (xr_strlen(buffer))
	{
		string_path dr;
		if (!(buffer[0] == '\\' && buffer[1] == '\\'))
		{	// if !network
			_splitpath(buffer, dr, 0, 0, 0);
			if (0 == dr[0])
			{
				P._update(buffer, buffer);
			}
		}
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetForegroundWindow( );
	ofn.lpstrDefExt = P.m_DefExt;
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = sz_buf;
	ofn.lpstrFilter = flt;
	ofn.nFilterIndex = start_flt_ext + 2;
	ofn.lpstrTitle = "Open a File";
	string512 path;
	strcpy(path, (offset && offset[0]) ? offset : P.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags = OFN_PATHMUSTEXIST |
		OFN_FILEMUSTEXIST |
		OFN_HIDEREADONLY |
		OFN_FILEMUSTEXIST |
		OFN_NOCHANGEDIR | (bMulti ? OFN_ALLOWMULTISELECT | OFN_EXPLORER : 0);
	ofn.FlagsEx = OFN_EX_NOPLACESBAR;

	bool bRes = !!GetOpenFileName(&ofn);
	if (!bRes)
	{
		u32 err = CommDlgExtendedError( );
		switch (err)
		{
			case FNERR_BUFFERTOOSMALL:
			{
				Log("Too many file selected.");
			}
			break;
		}
	}

	if (bRes && bMulti)
	{
		s32 cnt = _GetItemCount(buffer, 0x0);
		if (cnt > 1)
		{
			string64 buf;
			string64 dir;
			string4096 fns;
			strcpy(dir, buffer);
			strcpy(fns, dir);
			strcat(fns, "\\");
			strcat(fns, _GetItem(buffer, 1, buf, 0x0));
			for (s32 i = 2; i < cnt; i++)
			{
				strcat(fns, ",");
				strcat(fns, dir);
				strcat(fns, "\\");
				strcat(fns, _GetItem(buffer, i, buf, 0x0));
			}

			strcpy(buffer, fns);
		}
	}

	strlwr(buffer);
	return bRes;
}

bool EFS_Utils::GetSaveName(pcstr initial, string_path& buffer, pcstr offset, s32 start_flt_ext)
{
	FS_Path& P = *FS.get_path(initial);
	string1024 flt;
	MakeFilter(flt, P.m_FilterCaption ? P.m_FilterCaption : "", P.m_DefExt);
	OPENFILENAME ofn;
	Memory.mem_fill(&ofn, 0, sizeof(ofn));
	if (xr_strlen(buffer))
	{
		string_path dr;
		if (!(buffer[0] == '\\' && buffer[1] == '\\'))
		{	// if !network
			_splitpath(buffer, dr, 0, 0, 0);
			if (0 == dr[0])
			{
				P._update(buffer, buffer);
			}
		}
	}

	ofn.hwndOwner = GetForegroundWindow( );
	ofn.lpstrDefExt = P.m_DefExt;
	ofn.lpstrFile = buffer;
	ofn.lpstrFilter = flt;
	ofn.lStructSize = sizeof(ofn);
	ofn.nMaxFile = sizeof(buffer);
	ofn.nFilterIndex = start_flt_ext + 2;
	ofn.lpstrTitle = "Save a File";
	string512 path;
	strcpy(path, (offset && offset[0]) ? offset : P.m_Path);
	ofn.lpstrInitialDir = path;
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	ofn.FlagsEx = OFN_EX_NOPLACESBAR;

	bool bRes = !!GetSaveFileName(&ofn);
	if (!bRes)
	{
		u32 err = CommDlgExtendedError( );
		switch (err)
		{
			case FNERR_BUFFERTOOSMALL:
			{
				Log("Too many file selected.");
			}
			break;
		}
	}

	strlwr(buffer);
	return bRes;
}
//----------------------------------------------------
pcstr EFS_Utils::AppendFolderToName(pstr tex_name, s32 depth, BOOL full_name)
{
	string256 _fn;
	strcpy(tex_name, AppendFolderToName(tex_name, _fn, depth, full_name));
	return tex_name;
}

pcstr EFS_Utils::AppendFolderToName(pcstr src_name, pstr dest_name, s32 depth, BOOL full_name)
{
	shared_str tmp = src_name;
	pcstr s = src_name;
	pstr d = dest_name;
	s32 sv_depth = depth;
	for (; *s && depth; s++, d++)
	{
		if (*s == '_')
		{
			depth--;
			*d = '\\';
		}
		else
		{
			*d = *s;
		}
	}

	if (full_name)
	{
		*d = 0;
		if (depth < sv_depth)
		{
			strcat(dest_name, *tmp);
		}
	}
	else
	{
		for (; *s; s++, d++)
		{
			*d = *s;
		}

		*d = 0;
	}

	return dest_name;
}

pcstr EFS_Utils::GenerateName(pcstr base_path, pcstr base_name, pcstr def_ext, pstr out_name)
{
	s32 cnt = 0;
	string_path fn;
	if (base_name)
	{
		strconcat(sizeof(fn), fn, base_path, base_name, def_ext);
	}
	else
	{
		sprintf_s(fn, sizeof(fn), "%s%02d%s", base_path, cnt++, def_ext);
	}

	while (FS.exist(fn))
	{
		if (base_name)
		{
			sprintf_s(fn, sizeof(fn), "%s%s%02d%s", base_path, base_name, cnt++, def_ext);
		}
		else
		{
			sprintf_s(fn, sizeof(fn), "%s%02d%s", base_path, cnt++, def_ext);
		}
	}

	strcpy(out_name, fn);
	return out_name;
}

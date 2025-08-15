#include "stdafx.h"

pstr _TrimLeft(pstr str)
{
	pstr p = str;
	while (*p && (u8(*p) <= u8(' ')))
	{
		p++;
	}

	if (p != str)
	{
		for (pstr t = str; *p; t++, p++)
		{
			*t = *p;
		}

		*t = 0;
	}

	return str;
}

pstr _TrimRight(pstr str)
{
	pstr p = str + xr_strlen(str);
	while ((p != str) && (u8(*p) <= u8(' ')))
	{
		p--;
	}

	*(++p) = 0;
	return str;
}

pstr _Trim(pstr str)
{
	_TrimLeft(str);
	_TrimRight(str);
	return str;
}

pcstr _SetPos(pcstr src, u32 pos, char separator)
{
	pcstr res = src;
	u32 p = 0;
	while ((p < pos) && (0 != (res = strchr(res, separator))))
	{
		res++;
		p++;
	}

	return res;
}

pcstr _CopyVal(pcstr src, pstr dst, char separator)
{
	pcstr	p;
	size_t	n;
	p = strchr(src, separator);
	n = (p > 0) ? (p - src) : xr_strlen(src);
	strncpy(dst, src, n);
	dst[n] = 0;
	return dst;
}

s32 _GetItemCount(pcstr src, char separator)
{
	u32 cnt = 0;
	if (src && src[0])
	{
		pcstr res = src;
		pcstr last_res = res;
		while (0 != (res = strchr(res, separator)))
		{
			res++;
			last_res = res;
			cnt++;
			if (res[0] == separator)
			{
				break;
			}
		}

		if (xr_strlen(last_res))
		{
			cnt++;
		}
	}

	return cnt;
}

pstr _GetItem(pcstr src, s32 index, pstr dst, char separator, pcstr def, bool trim)
{
	pcstr ptr;
	ptr = _SetPos(src, index, separator);
	if (ptr)
	{
		_CopyVal(ptr, dst, separator);
	}
	else
	{
		strcpy(dst, def);
	}

	if (trim)
	{
		_Trim(dst);
	}

	return dst;
}

pstr _GetItems(pcstr src, s32 idx_start, s32 idx_end, pstr dst, char separator)
{
	pstr n = dst;
	s32 level = 0;
	for (pcstr p = src; *p != 0; p++)
	{
		if ((level >= idx_start) && (level < idx_end))
		{
			*n++ = *p;
		}

		if (*p == separator)
		{
			level++;
		}

		if (level >= idx_end)
		{
			break;
		}
	}

	*n = '\0';
	return dst;
}

u32 _ParseItem(pcstr src, xr_token* token_list)
{
	for (s32 i = 0; token_list[i].name; i++)
	{
		if (!stricmp(src, token_list[i].name))
		{
			return token_list[i].id;
		}
	}

	return u32(-1);
}

u32 _ParseItem(pcstr src, s32 ind, xr_token* token_list)
{
	string128 dst;
	_GetItem(src, ind, dst);
	return _ParseItem(dst, token_list);
}

pstr _ReplaceItems(pcstr src, s32 idx_start, s32 idx_end, pcstr new_items, pstr dst, char separator)
{
	pstr n = dst;
	s32 level = 0;
	bool bCopy = true;
	for (pcstr p = src; *p != 0; p++)
	{
		if ((level >= idx_start) && (level < idx_end))
		{
			if (bCopy)
			{
				for (pcstr itm = new_items; *itm != 0;)
				{
					*n++ = *itm++;
				}

				bCopy = false;
			}

			if (*p == separator)
			{
				*n++ = separator;
			}
		}
		else
		{
			*n++ = *p;
		}

		if (*p == separator)
		{
			level++;
		}
	}

	*n = '\0';
	return dst;
}

pstr _ReplaceItem(pcstr src, s32 index, pcstr new_item, pstr dst, char separator)
{
	pstr n = dst;
	s32 level = 0;
	bool bCopy = true;
	for (pcstr p = src; *p != 0; p++)
	{
		if (level == index)
		{
			if (bCopy)
			{
				for (pcstr itm = new_item; *itm != 0;)
				{
					*n++ = *itm++;
				}

				bCopy = false;
			}

			if (*p == separator)
			{
				*n++ = separator;
			}
		}
		else
		{
			*n++ = *p;
		}

		if (*p == separator)
		{
			level++;
		}
	}

	*n = '\0';
	return dst;
}

pstr _ChangeSymbol(pstr name, char src, char dest)
{
	pstr sTmpName = name;
	while (sTmpName[0])
	{
		if (sTmpName[0] == src)
		{
			sTmpName[0] = dest;
		}

		sTmpName++;
	}

	return name;
}

xr_string& _ChangeSymbol(xr_string& name, char src, char dest)
{
	for (xr_string::iterator it = name.begin( ); it != name.end( ); it++)
	{
		if (*it == src)
		{
			*it = xr_string::value_type(dest);
		}
	}

	return name;
}

void _SequenceToList(LPSTRVec& lst, pcstr in, char separator)
{
	s32 t_cnt = _GetItemCount(in, separator);
	string1024 T;
	for (s32 i = 0; i < t_cnt; i++)
	{
		_GetItem(in, i, T, separator, 0);
		_Trim(T);
		if (xr_strlen(T))
		{
			lst.push_back(xr_strdup(T));
		}
	}
}

void _SequenceToList(RStringVec& lst, pcstr in, char separator)
{
	lst.clear( );
	s32 t_cnt = _GetItemCount(in, separator);
	xr_string T;
	for (s32 i = 0; i < t_cnt; i++)
	{
		_GetItem(in, i, T, separator, 0);
		_Trim(T);
		if (T.size( ))
		{
			lst.push_back(T.c_str( ));
		}
	}
}

void _SequenceToList(SStringVec& lst, pcstr in, char separator)
{
	lst.clear( );
	int t_cnt = _GetItemCount(in, separator);
	xr_string T;
	for (int i = 0; i < t_cnt; i++)
	{
		_GetItem(in, i, T, separator, 0);
		_Trim(T);
		if (T.size( ))
		{
			lst.push_back(T.c_str( ));
		}
	}
}

xr_string _ListToSequence(const SStringVec& lst)
{
	static xr_string out;
	out = "";
	if (lst.size( ))
	{
		out = lst.front( );
		for (SStringVec::const_iterator s_it = lst.begin( ) + 1; s_it != lst.end( ); s_it++)
		{
			out += xr_string(",") + (*s_it);
		}
	}

	return out;
}

xr_string& _TrimLeft(xr_string& str)
{
	pcstr b = str.c_str( );
	pcstr p = str.c_str( );
	while (*p && (u8(*p) <= u8(' ')))
	{
		p++;
	}

	if (p != b)
	{
		str.erase(0, p - b);
	}

	return str;
}

xr_string& _TrimRight(xr_string& str)
{
	pcstr b = str.c_str( );
	size_t l = str.length( );
	if (l)
	{
		pcstr p = str.c_str( ) + l - 1;
		while ((p != b) && (u8(*p) <= u8(' ')))
		{
			p--;
		}

		if (p != (str + b))
		{
			str.erase(p - b + 1, l - (p - b));
		}
	}

	return str;
}

xr_string& _Trim(xr_string& str)
{
	_TrimLeft(str);
	_TrimRight(str);
	return str;
}

pcstr _CopyVal(pcstr src, xr_string& dst, char separator)
{
	pcstr p;
	ptrdiff_t n;
	p = strchr(src, separator);
	n = (p > 0) ? (p - src) : xr_strlen(src);
	dst = src;
	dst = dst.erase(n, dst.length( ));
	return dst.c_str( );
}

pcstr _GetItem(pcstr src, s32 index, xr_string& dst, char separator, pcstr def, bool trim)
{
	pcstr ptr;
	ptr = _SetPos(src, index, separator);
	if (ptr)
	{
		_CopyVal(ptr, dst, separator);
	}
	else
	{
		dst = def;
	}

	if (trim)
	{
		_Trim(dst);
	}

	return dst.c_str( );
}

shared_str _ListToSequence(const RStringVec& lst)
{
	xr_string out;
	if (lst.size( ))
	{
		out = *lst.front( );
		for (RStringVec::const_iterator s_it = lst.begin( ) + 1; s_it != lst.end( ); s_it++)
		{
			out += ",";
			out += **s_it;
		}
	}

	return shared_str(out.c_str( ));
}

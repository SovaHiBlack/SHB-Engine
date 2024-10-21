#include "stdafx.h"

#include "fs_internal.h"

XRCORE_API CInifile* pSettings = NULL;

CInifile* CInifile::Create(pcstr szFileName, BOOL ReadOnly)
{
	return xr_new<CInifile>(szFileName, ReadOnly);
}

void CInifile::Destroy(CInifile* ini)
{
	xr_delete(ini);
}

bool sect_pred(const CInifile::Sect* x, pcstr val)
{
	return xr_strcmp(*x->Name, val) < 0;
};

bool item_pred(const CInifile::Item& x, pcstr val)
{
	if ((!x.first) || (!val))	return x.first < val;
	else				   		return xr_strcmp(*x.first, val) < 0;
}

//------------------------------------------------------------------------------
//Тело функций Inifile
//------------------------------------------------------------------------------
XRCORE_API void _parse(pstr dest, pcstr src)
{
	if (src)
	{
		BOOL bInsideSTR = false;
		while (*src)
		{
			if (isspace((u8)*src))
			{
				if (bInsideSTR)
				{
					*dest++ = *src++; continue;
				}
				while (*src && isspace(*src)) src++;
				continue;
			}
			else if (*src == '"')
			{
				bInsideSTR = !bInsideSTR;
			}
			*dest++ = *src++;
		}
	}
	*dest = 0;
}

XRCORE_API void _decorate(pstr dest, pcstr src)
{
	if (src)
	{
		BOOL bInsideSTR = false;
		while (*src)
		{
			if (*src == ',')
			{
				if (bInsideSTR)
				{
					*dest++ = *src++;
				}
				else
				{
					*dest++ = *src++; *dest++ = ' ';
				}
				continue;
			}
			else if (*src == '"')
			{
				bInsideSTR = !bInsideSTR;
			}
			*dest++ = *src++;
		}
	}
	*dest = 0;
}
//------------------------------------------------------------------------------

BOOL	CInifile::Sect::line_exist(pcstr L, pcstr* val)
{
	SectCIt A = std::lower_bound(Data.begin(), Data.end(), L, item_pred);
	if (A != Data.end() && xr_strcmp(*A->first, L) == 0)
	{
		if (val) *val = *A->second;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------

CInifile::CInifile(IReader* F, pcstr path)
{
	fName = 0;
	bReadOnly = TRUE;
	bSaveAtEnd = FALSE;
	Load(F, path);
}

CInifile::CInifile(pcstr szFileName, BOOL ReadOnly, BOOL bLoad, BOOL SaveAtEnd)
{
	fName = szFileName ? xr_strdup(szFileName) : 0;
	bReadOnly = ReadOnly;
	bSaveAtEnd = SaveAtEnd;
	if (bLoad)
	{
		string_path	path, folder;
		_splitpath(fName, path, folder, 0, 0);
		strcat(path, folder);
		IReader* R = FS.r_open(szFileName);
		if (R)
		{
			Load(R, path);
			FS.r_close(R);
		}
	}
}

CInifile::~CInifile()
{
	if (!bReadOnly && bSaveAtEnd)
	{
		if (!save_as())
			Log("!Can't save inifile:", fName);
	}

	xr_free(fName);

	RootIt			I = DATA.begin();
	RootIt			E = DATA.end();
	for (; I != E; ++I)
		xr_delete(*I);
}

static void	insert_item(CInifile::Sect* tgt, const CInifile::Item& I)
{
	CInifile::SectIt_	sect_it = std::lower_bound(tgt->Data.begin(), tgt->Data.end(), *I.first, item_pred);
	if (sect_it != tgt->Data.end() && sect_it->first.equal(I.first))
	{
		sect_it->second = I.second;
#ifdef DEBUG
		sect_it->comment = I.comment;
#endif
	}
	else
	{
		tgt->Data.insert(sect_it, I);
	}
}

void	CInifile::Load(IReader* F, pcstr path)
{
	R_ASSERT(F);
	Sect* Current = 0;
	string4096	str;
	string4096	str2;

	while (!F->eof())
	{
		F->r_string(str, sizeof(str));
		_Trim(str);
		pstr semi = strchr(str, ';');
		pstr semi_1 = strchr(str, '/');

		if (semi_1 && (*(semi_1 + 1) == '/') && ((!semi) || (semi && (semi_1 < semi))))
		{
			semi = semi_1;
		}

#ifdef DEBUG
		pstr comment = 0;
#endif
		if (semi)
		{
			*semi = 0;
#ifdef DEBUG
			comment = semi + 1;
#endif
		}

		if (str[0] && (str[0] == '#') && strstr(str, "#include"))
		{
			string64	inc_name;
			R_ASSERT(path && path[0]);
			if (_GetItem(str, 1, inc_name, '"'))
			{
				string_path	fn, inc_path, folder;
				strconcat(sizeof(fn), fn, path, inc_name);
				_splitpath(fn, inc_path, folder, 0, 0);
				strcat(inc_path, folder);
				IReader* I = FS.r_open(fn); R_ASSERT3(I, "Can't find include file:", inc_name);
				Load(I, inc_path);
				FS.r_close(I);
			}
		}
		else if (str[0] && (str[0] == '['))
		{
			// insert previous filled section
			if (Current)
			{
				RootIt I = std::lower_bound(DATA.begin(), DATA.end(), *Current->Name, sect_pred);
				if ((I != DATA.end()) && ((*I)->Name == Current->Name))
					Debug.fatal(DEBUG_INFO, "Duplicate section '%s' found.", *Current->Name);
				DATA.insert(I, Current);
			}
			Current = xr_new<Sect>();
			Current->Name = 0;
			// start new section
			R_ASSERT3(strchr(str, ']'), "Bad ini section found: ", str);
			pcstr inherited_names = strstr(str, "]:");
			if (0 != inherited_names)
			{
				VERIFY2(bReadOnly, "Allow for readonly mode only.");
				inherited_names += 2;
				int cnt = _GetItemCount(inherited_names);
				for (int k = 0; k < cnt; ++k)
				{
					xr_string	tmp;
					_GetItem(inherited_names, k, tmp);
					Sect& inherited_section = r_section(tmp.c_str());
					for (SectIt_ it = inherited_section.Data.begin(); it != inherited_section.Data.end(); it++)
						insert_item(Current, *it);
				}
			}
			*strchr(str, ']') = 0;
			Current->Name = strlwr(str + 1);
		}
		else
		{
			if (Current)
			{
				pstr name = str;
				pstr t = strchr(name, '=');
				if (t)
				{
					*t = 0;
					_Trim(name);
					_parse(str2, ++t);
				}
				else
				{
					_Trim(name);
					str2[0] = 0;
				}

				Item		I;
				I.first = (name[0] ? name : NULL);
				I.second = (str2[0] ? str2 : NULL);
#ifdef DEBUG
				I.comment = bReadOnly ? 0 : comment;
#endif

				if (bReadOnly)
				{
					if (*I.first)							insert_item(Current, I);
				}
				else
				{
					if (
						*I.first
						|| *I.second
#ifdef DEBUG
						|| *I.comment
#endif
						)
						insert_item(Current, I);
				}
			}
		}
	}
	if (Current)
	{
		RootIt I = std::lower_bound(DATA.begin(), DATA.end(), *Current->Name, sect_pred);
		if ((I != DATA.end()) && ((*I)->Name == Current->Name))
			Debug.fatal(DEBUG_INFO, "Duplicate section '%s' found.", *Current->Name);
		DATA.insert(I, Current);
	}
}

bool	CInifile::save_as(pcstr new_fname)
{
	// save if needed
	if (new_fname && new_fname[0])
	{
		xr_free(fName);
		fName = xr_strdup(new_fname);
	}
	R_ASSERT(fName && fName[0]);
	IWriter* F = FS.w_open_ex(fName);
	if (F)
	{
		string512		temp, val;
		for (RootIt r_it = DATA.begin(); r_it != DATA.end(); ++r_it)
		{
			sprintf_s(temp, sizeof(temp), "[%s]", *(*r_it)->Name);
			F->w_string(temp);
			for (SectCIt s_it = (*r_it)->Data.begin(); s_it != (*r_it)->Data.end(); ++s_it)
			{
				const Item& I = *s_it;
				if (*I.first)
				{
					if (*I.second)
					{
						_decorate(val, *I.second);
#ifdef DEBUG
						if (*I.comment)
						{
							// name, value and comment
							sprintf_s(temp, sizeof(temp), "%8s%-32s = %-32s ;%s", " ", *I.first, val, *I.comment);
						}
						else
#endif
						{
							// only name and value
							sprintf_s(temp, sizeof(temp), "%8s%-32s = %-32s", " ", *I.first, val);
						}
					}
					else
					{
#ifdef DEBUG
						if (*I.comment)
						{
							// name and comment
							sprintf_s(temp, sizeof(temp), "%8s%-32s = ;%s", " ", *I.first, *I.comment);
						}
						else
#endif
						{
							// only name
							sprintf_s(temp, sizeof(temp), "%8s%-32s = ", " ", *I.first);
						}
					}
				}
				else
				{
					// no name, so no value
#ifdef DEBUG
					if (*I.comment)
						sprintf_s(temp, sizeof(temp), "%8s;%s", " ", *I.comment);
					else
#endif
						temp[0] = 0;
				}
				_TrimRight(temp);
				if (temp[0])		F->w_string(temp);
			}
			F->w_string(" ");
		}
		FS.w_close(F);
		return true;
	}
	return false;
}

BOOL	CInifile::section_exist(pcstr S)
{
	RootIt I = std::lower_bound(DATA.begin(), DATA.end(), S, sect_pred);
	return (I != DATA.end() && xr_strcmp(*(*I)->Name, S) == 0);
}

BOOL	CInifile::line_exist(pcstr S, pcstr L)
{
	if (!section_exist(S)) return FALSE;
	Sect& I = r_section(S);
	SectCIt A = std::lower_bound(I.Data.begin(), I.Data.end(), L, item_pred);
	return (A != I.Data.end() && xr_strcmp(*A->first, L) == 0);
}

u32		CInifile::line_count(pcstr Sname)
{
	Sect& S = r_section(Sname);
	SectCIt	I = S.Data.begin();
	u32	C = 0;
	for (; I != S.Data.end(); I++)	if (*I->first) C++;
	return  C;
}


//--------------------------------------------------------------------------------------
CInifile::Sect& CInifile::r_section(const shared_str& S)
{
	return	r_section(*S);
}
BOOL			CInifile::line_exist(const shared_str& S, const shared_str& L)
{
	return	line_exist(*S, *L);
}
u32				CInifile::line_count(const shared_str& S)
{
	return	line_count(*S);
}
BOOL			CInifile::section_exist(const shared_str& S)
{
	return	section_exist(*S);
}

//--------------------------------------------------------------------------------------
// Read functions
//--------------------------------------------------------------------------------------
CInifile::Sect& CInifile::r_section(pcstr S)
{
	char	section[256]; strcpy_s(section, sizeof(section), S); strlwr(section);
	RootIt I = std::lower_bound(DATA.begin(), DATA.end(), section, sect_pred);
	if (!(I != DATA.end() && xr_strcmp(*(*I)->Name, section) == 0))
		Debug.fatal(DEBUG_INFO, "Can't open section '%s'", S);
	return	**I;
}

pcstr	CInifile::r_string(pcstr S, pcstr L)
{
	Sect& I = r_section(S);
	SectCIt	A = std::lower_bound(I.Data.begin(), I.Data.end(), L, item_pred);
	if (A != I.Data.end() && xr_strcmp(*A->first, L) == 0)	return *A->second;
	else
		Debug.fatal(DEBUG_INFO, "Can't find variable %s in [%s]", L, S);
	return 0;
}

shared_str		CInifile::r_string_wb(pcstr S, pcstr L)
{
	pcstr		_base = r_string(S, L);

	if (0 == _base)					return	shared_str(0);

	string512						_original;
	strcpy_s(_original, _base);
	u32			_len = xr_strlen(_original);
	if (0 == _len)					return	shared_str("");
	if ('"' == _original[_len - 1])	_original[_len - 1] = 0;				// skip end
	if ('"' == _original[0])			return	shared_str(&_original[0] + 1);	// skip begin
	return									shared_str(_original);
}

u8 CInifile::r_u8(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	return		u8(atoi(C));
}
u16 CInifile::r_u16(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	return		u16(atoi(C));
}
u32 CInifile::r_u32(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	return		u32(atoi(C));
}
s8 CInifile::r_s8(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	return		s8(atoi(C));
}
s16 CInifile::r_s16(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	return		s16(atoi(C));
}
s32 CInifile::r_s32(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	return		s32(atoi(C));
}
f32 CInifile::r_float(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	return		f32(atof(C));
}
Fcolor CInifile::r_fcolor(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	Fcolor		V = { 0,0,0,0 };
	sscanf(C, "%f,%f,%f,%f", &V.r, &V.g, &V.b, &V.a);
	return V;
}
u32 CInifile::r_color(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	u32			r = 0, g = 0, b = 0, a = 255;
	sscanf(C, "%d,%d,%d,%d", &r, &g, &b, &a);
	return color_rgba(r, g, b, a);
}

iVector2 CInifile::r_ivector2(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	iVector2	V = { 0,0 };
	sscanf(C, "%d,%d", &V.x, &V.y);
	return V;
}
Ivector3 CInifile::r_ivector3(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	Ivector		V = { 0,0,0 };
	sscanf(C, "%d,%d,%d", &V.x, &V.y, &V.z);
	return V;
}
Ivector4 CInifile::r_ivector4(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	Ivector4	V = { 0,0,0,0 };
	sscanf(C, "%d,%d,%d,%d", &V.x, &V.y, &V.z, &V.w);
	return V;
}
fVector2 CInifile::r_fvector2(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	fVector2	V = { 0.0f,0.0f };
	sscanf(C, "%f,%f", &V.x, &V.y);
	return V;
}
Fvector3 CInifile::r_fvector3(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	Fvector3	V = { 0.f,0.f,0.f };
	sscanf(C, "%f,%f,%f", &V.x, &V.y, &V.z);
	return V;
}
Fvector4 CInifile::r_fvector4(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	Fvector4	V = { 0.f,0.f,0.f,0.f };
	sscanf(C, "%f,%f,%f,%f", &V.x, &V.y, &V.z, &V.w);
	return V;
}
BOOL	CInifile::r_bool(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	char		B[8];
	strncpy(B, C, 7);
	strlwr(B);
	return 		IsBOOL(B);
}
CLASS_ID CInifile::r_clsid(pcstr S, pcstr L)
{
	pcstr		C = r_string(S, L);
	return		TEXT2CLSID(C);
}

int CInifile::r_token(pcstr S, pcstr L, const xr_token* token_list)
{
	pcstr C = r_string(S, L);
	for (int i = 0; token_list[i].name; i++)
	{
		if (!stricmp(C, token_list[i].name))
		{
			return token_list[i].id;
		}
	}

	return 0;
}

BOOL	CInifile::r_line(pcstr S, int L, pcstr* N, pcstr* V)
{
	Sect& SS = r_section(S);
	if (L >= (int)SS.Data.size() || L < 0) return FALSE;
	for (SectCIt I = SS.Data.begin(); I != SS.Data.end(); I++)
		if (!(L--))
		{
			*N = *I->first;
			*V = *I->second;
			return TRUE;
		}
	return FALSE;
}
BOOL	CInifile::r_line(const shared_str& S, int L, pcstr* N, pcstr* V)
{
	return r_line(*S, L, N, V);
}

//--------------------------------------------------------------------------------------------------------
// Write functions
//--------------------------------------------------------------------------------------
void	CInifile::w_string(pcstr S, pcstr L, pcstr			V, pcstr comment)
{
	R_ASSERT(!bReadOnly);

	// section
	char	sect[256];
	_parse(sect, S);
	_strlwr(sect);
	if (!section_exist(sect))
	{
		// create _new_ section
		Sect* NEW = xr_new<Sect>();
		NEW->Name = sect;
		RootIt I = std::lower_bound(DATA.begin(), DATA.end(), sect, sect_pred);
		DATA.insert(I, NEW);
	}

	// parse line/value
	char	line[256];	_parse(line, L);
	char	value[256];	_parse(value, V);

	// duplicate & insert
	Item	I;
	Sect& data = r_section(sect);
	I.first = (line[0] ? line : 0);
	I.second = (value[0] ? value : 0);
#ifdef DEBUG
	I.comment = (comment ? comment : 0);
#endif
	SectIt_	it = std::lower_bound(data.Data.begin(), data.Data.end(), *I.first, item_pred);

	if (it != data.Data.end())
	{
		// Check for "first" matching
		if (0 == xr_strcmp(*it->first, *I.first))
		{
			*it = I;
		}
		else
		{
			data.Data.insert(it, I);
		}
	}
	else
	{
		data.Data.insert(it, I);
	}
}
void	CInifile::w_u8(pcstr S, pcstr L, u8				V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void	CInifile::w_u16(pcstr S, pcstr L, u16				V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void	CInifile::w_u32(pcstr S, pcstr L, u32				V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void	CInifile::w_s8(pcstr S, pcstr L, s8				V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void	CInifile::w_s16(pcstr S, pcstr L, s16				V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void	CInifile::w_s32(pcstr S, pcstr L, s32				V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void	CInifile::w_float(pcstr S, pcstr L, f32				V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%f", V);
	w_string(S, L, temp, comment);
}
void	CInifile::w_fcolor(pcstr S, pcstr L, const Fcolor& V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%f,%f,%f,%f", V.r, V.g, V.b, V.a);
	w_string(S, L, temp, comment);
}

void	CInifile::w_color(pcstr S, pcstr L, u32				V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d,%d,%d,%d", color_get_R(V), color_get_G(V), color_get_B(V), color_get_A(V));
	w_string(S, L, temp, comment);
}

void	CInifile::w_ivector2(pcstr S, pcstr L, const iVector2& V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d,%d", V.x, V.y);
	w_string(S, L, temp, comment);
}

void	CInifile::w_ivector3(pcstr S, pcstr L, const Ivector3& V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d,%d,%d", V.x, V.y, V.z);
	w_string(S, L, temp, comment);
}

void	CInifile::w_ivector4(pcstr S, pcstr L, const Ivector4& V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%d,%d,%d,%d", V.x, V.y, V.z, V.w);
	w_string(S, L, temp, comment);
}
void	CInifile::w_fvector2(pcstr S, pcstr L, const fVector2& V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%f,%f", V.x, V.y);
	w_string(S, L, temp, comment);
}

void	CInifile::w_fvector3(pcstr S, pcstr L, const Fvector3& V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%f,%f,%f", V.x, V.y, V.z);
	w_string(S, L, temp, comment);
}

void	CInifile::w_fvector4(pcstr S, pcstr L, const Fvector4& V, pcstr comment)
{
	string128 temp; sprintf_s(temp, sizeof(temp), "%f,%f,%f,%f", V.x, V.y, V.z, V.w);
	w_string(S, L, temp, comment);
}

void	CInifile::w_bool(pcstr S, pcstr L, BOOL				V, pcstr comment)
{
	w_string(S, L, V ? "on" : "off", comment);
}

void	CInifile::remove_line(pcstr S, pcstr L)
{
	R_ASSERT(!bReadOnly);

	if (line_exist(S, L))
	{
		Sect& data = r_section(S);
		SectIt_ A = std::lower_bound(data.Data.begin(), data.Data.end(), L, item_pred);
		R_ASSERT(A != data.Data.end() && xr_strcmp(*A->first, L) == 0);
		data.Data.erase(A);
	}
}


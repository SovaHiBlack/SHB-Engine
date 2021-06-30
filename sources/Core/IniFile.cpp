#include "stdafx.h"

#include "fs_internal.h"

CORE_API CIniFile* pSettings = nullptr;

CIniFile* CIniFile::Create(Pcstr szFileName, BOOL ReadOnly)
{
	return xr_new<CIniFile>(szFileName, ReadOnly);
}

void CIniFile::Destroy(CIniFile* ini)
{
	xr_delete(ini);
}

bool sect_pred(const CIniFile::Sect* x, Pcstr val)
{
	return xr_strcmp(*x->Name, val) < 0;
}

bool item_pred(const CIniFile::Item& x, Pcstr val)
{
	if ((!x.first) || (!val))
	{
		return x.first < val;
	}
	else
	{
		return xr_strcmp(*x.first, val) < 0;
	}
}

//------------------------------------------------------------------------------
//Тело функций Inifile
//------------------------------------------------------------------------------
CORE_API void _parse(Pstr dest, Pcstr src)
{
	if (src)
	{
		bool bInsideSTR = false;
		while (*src)
		{
			if (isspace((U8) *src))
			{
				if (bInsideSTR)
				{
					*dest++ = *src++;
					continue;
				}

				while (*src && isspace(*src))
				{
					src++;
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

CORE_API void _decorate(Pstr dest, Pcstr src)
{
	if (src)
	{
		bool bInsideSTR = false;
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
					*dest++ = *src++;
					*dest++ = ' ';
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

BOOL CIniFile::Sect::line_exist(Pcstr L, Pcstr* val)
{
	SectCIt A = std::lower_bound(Data.begin( ), Data.end( ), L, item_pred);
	if (A != Data.end( ) && xr_strcmp(*A->first, L) == 0)
	{
		if (val)
		{
			*val = *A->second;
		}

		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------------

CIniFile::CIniFile(IReader* F, Pcstr path)
{
	fName = 0;
	bReadOnly = TRUE;
	bSaveAtEnd = FALSE;
	Load(F, path);
}

CIniFile::CIniFile(Pcstr szFileName, BOOL ReadOnly, BOOL bLoad, BOOL SaveAtEnd)
{
	fName = szFileName ? xr_strdup(szFileName) : 0;
	bReadOnly = ReadOnly;
	bSaveAtEnd = SaveAtEnd;
	if (bLoad)
	{
		string_path path;
		string_path folder;
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

CIniFile::~CIniFile( )
{
	if (!bReadOnly && bSaveAtEnd)
	{
		if (!save_as( ))
		{
			Log("!Can't save inifile:", fName);
		}
	}

	xr_free(fName);

	RootIt I = DATA.begin( );
	RootIt E = DATA.end( );
	for (; I != E; ++I)
	{
		xr_delete(*I);
	}
}

static void insert_item(CIniFile::Sect* tgt, const CIniFile::Item& I)
{
	CIniFile::SectIt_ sect_it = std::lower_bound(tgt->Data.begin( ), tgt->Data.end( ), *I.first, item_pred);
	if (sect_it != tgt->Data.end( ) && sect_it->first.equal(I.first))
	{
		sect_it->second = I.second;

#ifdef DEBUG
		sect_it->comment = I.comment;
#endif // def DEBUG

	}
	else
	{
		tgt->Data.insert(sect_it, I);
	}
}

void CIniFile::Load(IReader* F, Pcstr path)
{
	R_ASSERT(F);
	Sect* Current = nullptr;
	string4096 str;
	string4096 str2;

	while (!F->eof( ))
	{
		F->r_string(str, sizeof(str));
		_Trim(str);
		Pstr semi = strchr(str, ';');
		Pstr semi_1 = strchr(str, '/');

		if (semi_1 && (*(semi_1 + 1) == '/') && ((!semi) || (semi && (semi_1 < semi))))
		{
			semi = semi_1;
		}

#ifdef DEBUG
		Pstr comment = 0;
#endif // def DEBUG

		if (semi)
		{
			*semi = 0;

#ifdef DEBUG
			comment = semi + 1;
#endif // def DEBUG

		}

		if (str[0] && (str[0] == '#') && strstr(str, "#include"))
		{
			string64 inc_name;
			R_ASSERT(path && path[0]);
			if (_GetItem(str, 1, inc_name, '"'))
			{
				string_path fn;
				string_path inc_path;
				string_path folder;
				strconcat(sizeof(fn), fn, path, inc_name);
				_splitpath(fn, inc_path, folder, 0, 0);
				strcat(inc_path, folder);
				IReader* I = FS.r_open(fn);
				R_ASSERT3(I, "Can't find include file:", inc_name);
				Load(I, inc_path);
				FS.r_close(I);
			}
		}
		else if (str[0] && (str[0] == '['))
		{
			// insert previous filled section
			if (Current)
			{
				RootIt I = std::lower_bound(DATA.begin( ), DATA.end( ), *Current->Name, sect_pred);
				if ((I != DATA.end( )) && ((*I)->Name == Current->Name))
				{
					Debug.fatal(DEBUG_INFO, "Duplicate section '%s' found.", *Current->Name);
				}

				DATA.insert(I, Current);
			}

			Current = xr_new<Sect>( );
			Current->Name = 0;
			// start new section
			R_ASSERT3(strchr(str, ']'), "Bad ini section found: ", str);
			Pcstr inherited_names = strstr(str, "]:");
			if (0 != inherited_names)
			{
				VERIFY2(bReadOnly, "Allow for readonly mode only.");
				inherited_names += 2;
				int cnt = _GetItemCount(inherited_names);
				for (int k = 0; k < cnt; ++k)
				{
					xr_string tmp;
					_GetItem(inherited_names, k, tmp);
					Sect& inherited_section = r_section(tmp.c_str( ));
					for (SectIt_ it = inherited_section.Data.begin( ); it != inherited_section.Data.end( ); it++)
					{
						insert_item(Current, *it);
					}
				}
			}

			*strchr(str, ']') = 0;
			Current->Name = strlwr(str + 1);
		}
		else
		{
			if (Current)
			{
				Pstr name = str;
				Pstr t = strchr(name, '=');
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

				Item I;
				I.first = (name[0] ? name : NULL);
				I.second = (str2[0] ? str2 : NULL);

#ifdef DEBUG
				I.comment = bReadOnly ? 0 : comment;
#endif // def DEBUG

				if (bReadOnly)
				{
					if (*I.first)
					{
						insert_item(Current, I);
					}
				}
				else
				{
					if (*I.first || *I.second

#ifdef DEBUG
						|| *I.comment
#endif // def DEBUG

						)
					{
						insert_item(Current, I);
					}
				}
			}
		}
	}

	if (Current)
	{
		RootIt I = std::lower_bound(DATA.begin( ), DATA.end( ), *Current->Name, sect_pred);
		if ((I != DATA.end( )) && ((*I)->Name == Current->Name))
		{
			Debug.fatal(DEBUG_INFO, "Duplicate section '%s' found.", *Current->Name);
		}

		DATA.insert(I, Current);
	}
}

bool CIniFile::save_as(Pcstr new_fname)
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
		string512 temp;
		string512 val;
		for (RootIt r_it = DATA.begin( ); r_it != DATA.end( ); ++r_it)
		{
			sprintf_s(temp, sizeof(temp), "[%s]", *(*r_it)->Name);
			F->w_string(temp);
			for (SectCIt s_it = (*r_it)->Data.begin( ); s_it != (*r_it)->Data.end( ); ++s_it)
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
#endif // def DEBUG

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
#endif // def DEBUG

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
					{
						sprintf_s(temp, sizeof(temp), "%8s;%s", " ", *I.comment);
					}
					else
#endif // def DEBUG

						temp[0] = 0;
				}

				_TrimRight(temp);
				if (temp[0])
				{
					F->w_string(temp);
				}
			}

			F->w_string(" ");
		}

		FS.w_close(F);
		return true;
	}

	return false;
}

BOOL CIniFile::section_exist(Pcstr S)
{
	RootIt I = std::lower_bound(DATA.begin( ), DATA.end( ), S, sect_pred);
	return (I != DATA.end( ) && xr_strcmp(*(*I)->Name, S) == 0);
}

BOOL CIniFile::line_exist(Pcstr S, Pcstr L)
{
	if (!section_exist(S))
	{
		return FALSE;
	}

	Sect& I = r_section(S);
	SectCIt A = std::lower_bound(I.Data.begin( ), I.Data.end( ), L, item_pred);
	return (A != I.Data.end( ) && xr_strcmp(*A->first, L) == 0);
}

U32 CIniFile::line_count(Pcstr Sname)
{
	Sect& S = r_section(Sname);
	SectCIt I = S.Data.begin( );
	U32 C = 0;
	for (; I != S.Data.end( ); I++)
	{
		if (*I->first)
		{
			C++;
		}
	}

	return  C;
}

//--------------------------------------------------------------------------------------
CIniFile::Sect& CIniFile::r_section(const shared_str& S)
{
	return r_section(*S);
}
BOOL CIniFile::line_exist(const shared_str& S, const shared_str& L)
{
	return line_exist(*S, *L);
}
U32 CIniFile::line_count(const shared_str& S)
{
	return line_count(*S);
}
BOOL CIniFile::section_exist(const shared_str& S)
{
	return section_exist(*S);
}

//--------------------------------------------------------------------------------------
// Read functions
//--------------------------------------------------------------------------------------
CIniFile::Sect& CIniFile::r_section(Pcstr S)
{
	string256 section;
	strcpy_s(section, sizeof(section), S);
	strlwr(section);
	RootIt I = std::lower_bound(DATA.begin( ), DATA.end( ), section, sect_pred);
	if (!(I != DATA.end( ) && xr_strcmp(*(*I)->Name, section) == 0))
	{
		Debug.fatal(DEBUG_INFO, "Can't open section '%s'", S);
	}

	return **I;
}

Pcstr CIniFile::r_string(Pcstr S, Pcstr L)
{
	Sect& I = r_section(S);
	SectCIt A = std::lower_bound(I.Data.begin( ), I.Data.end( ), L, item_pred);
	if (A != I.Data.end( ) && xr_strcmp(*A->first, L) == 0)
	{
		return *A->second;
	}
	else
	{
		Debug.fatal(DEBUG_INFO, "Can't find variable %s in [%s]", L, S);
	}

	return 0;
}

shared_str CIniFile::r_string_wb(Pcstr S, Pcstr L)
{
	Pcstr _base = r_string(S, L);

	if (0 == _base)
	{
		return shared_str(0);
	}

	string512 _original;
	strcpy_s(_original, _base);
	U32 _len = xr_strlen(_original);
	if (0 == _len)
	{
		return	shared_str("");
	}

	// skip end
	if ('"' == _original[_len - 1])
	{
		_original[_len - 1] = 0;
	}
	
	// skip begin
	if ('"' == _original[0])
	{
		return shared_str(&_original[0] + 1);
	}
	
	return shared_str(_original);
}

U8 CIniFile::r_u8(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	return U8(atoi(C));
}
U16 CIniFile::r_u16(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	return U16(atoi(C));
}
U32 CIniFile::r_u32(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	return U32(atoi(C));
}
S8 CIniFile::r_s8(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	return S8(atoi(C));
}
S16 CIniFile::r_s16(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	return S16(atoi(C));
}
int CIniFile::r_s32(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	return int(atoi(C));
}
float CIniFile::r_float(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	return float(atof(C));
}
Fcolor CIniFile::r_fcolor(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	Fcolor V = { 0, 0, 0, 0 };
	sscanf(C, "%f,%f,%f,%f", &V.r, &V.g, &V.b, &V.a);
	return V;
}
U32 CIniFile::r_color(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	U32 r = 0;
	U32 g = 0;
	U32 b = 0;
	U32 a = 255;
	sscanf(C, "%d,%d,%d,%d", &r, &g, &b, &a);
	return color_rgba(r, g, b, a);
}

Ivector2 CIniFile::r_ivector2(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	Ivector2 V = { 0, 0 };
	sscanf(C, "%d,%d", &V.x, &V.y);
	return V;
}
Ivector3 CIniFile::r_ivector3(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	Ivector3 V = { 0, 0, 0 };
	sscanf(C, "%d,%d,%d", &V.x, &V.y, &V.z);
	return V;
}
Ivector4 CIniFile::r_ivector4(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	Ivector4 V = { 0, 0, 0, 0 };
	sscanf(C, "%d,%d,%d,%d", &V.x, &V.y, &V.z, &V.w);
	return V;
}
Fvector2 CIniFile::r_fvector2(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	Fvector2 V = { 0.0f, 0.0f };
	sscanf(C, "%f,%f", &V.x, &V.y);
	return V;
}
Fvector3 CIniFile::r_fvector3(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	Fvector3 V = { 0.0f, 0.0f, 0.0f };
	sscanf(C, "%f,%f,%f", &V.x, &V.y, &V.z);
	return V;
}
Fvector4 CIniFile::r_fvector4(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	Fvector4 V = { 0.0f, 0.0f, 0.0f, 0.0f };
	sscanf(C, "%f,%f,%f,%f", &V.x, &V.y, &V.z, &V.w);
	return V;
}
BOOL CIniFile::r_bool(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	char B[8];
	strncpy(B, C, 7);
	strlwr(B);
	return IsBOOL(B);
}
CLASS_ID CIniFile::r_clsid(Pcstr S, Pcstr L)
{
	Pcstr C = r_string(S, L);
	return TEXT2CLSID(C);
}

int CIniFile::r_token(Pcstr S, Pcstr L, const xr_token* token_list)
{
	Pcstr C = r_string(S, L);
	for (int i = 0; token_list[i].name; i++)
	{
		if (!stricmp(C, token_list[i].name))
		{
			return token_list[i].id;
		}
	}

	return 0;
}

BOOL CIniFile::r_line(Pcstr S, int L, Pcstr* N, Pcstr* V)
{
	Sect& SS = r_section(S);
	if (L >= (int) SS.Data.size( ) || L < 0)
	{
		return FALSE;
	}

	for (SectCIt I = SS.Data.begin( ); I != SS.Data.end( ); I++)
	{
		if (!(L--))
		{
			*N = *I->first;
			*V = *I->second;
			return TRUE;
		}
	}

	return FALSE;
}
BOOL CIniFile::r_line(const shared_str& S, int L, Pcstr* N, Pcstr* V)
{
	return r_line(*S, L, N, V);
}

//--------------------------------------------------------------------------------------------------------
// Write functions
//--------------------------------------------------------------------------------------
void CIniFile::w_string(Pcstr S, Pcstr L, Pcstr V, Pcstr comment)
{
	R_ASSERT(!bReadOnly);

	// section
	string256 sect;
	_parse(sect, S);
	_strlwr(sect);
	if (!section_exist(sect))
	{
		// create _new_ section
		Sect* NEW = xr_new<Sect>( );
		NEW->Name = sect;
		RootIt I = std::lower_bound(DATA.begin( ), DATA.end( ), sect, sect_pred);
		DATA.insert(I, NEW);
	}

	// parse line/value
	string256 line;
	_parse(line, L);
	string256 value;
	_parse(value, V);

	// duplicate & insert
	Item I;
	Sect& data = r_section(sect);
	I.first = (line[0] ? line : 0);
	I.second = (value[0] ? value : 0);

#ifdef DEBUG
	I.comment = (comment ? comment : 0);
#endif // def DEBUG

	SectIt_ it = std::lower_bound(data.Data.begin( ), data.Data.end( ), *I.first, item_pred);

	if (it != data.Data.end( ))
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
void CIniFile::w_u8(Pcstr S, Pcstr L, U8 V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CIniFile::w_u16(Pcstr S, Pcstr L, U16 V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CIniFile::w_u32(Pcstr S, Pcstr L, U32 V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CIniFile::w_s8(Pcstr S, Pcstr L, S8 V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CIniFile::w_s16(Pcstr S, Pcstr L, S16 V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CIniFile::w_s32(Pcstr S, Pcstr L, int V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CIniFile::w_float(Pcstr S, Pcstr L, float V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f", V);
	w_string(S, L, temp, comment);
}
void CIniFile::w_fcolor(Pcstr S, Pcstr L, const Fcolor& V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f,%f", V.r, V.g, V.b, V.a);
	w_string(S, L, temp, comment);
}

void CIniFile::w_color(Pcstr S, Pcstr L, U32 V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d,%d", color_get_R(V), color_get_G(V), color_get_B(V), color_get_A(V));
	w_string(S, L, temp, comment);
}

void CIniFile::w_ivector2(Pcstr S, Pcstr L, const Ivector2& V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d", V.x, V.y);
	w_string(S, L, temp, comment);
}

void CIniFile::w_ivector3(Pcstr S, Pcstr L, const Ivector3& V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d", V.x, V.y, V.z);
	w_string(S, L, temp, comment);
}

void CIniFile::w_ivector4(Pcstr S, Pcstr L, const Ivector4& V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d,%d", V.x, V.y, V.z, V.w);
	w_string(S, L, temp, comment);
}
void CIniFile::w_fvector2(Pcstr S, Pcstr L, const Fvector2& V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f", V.x, V.y);
	w_string(S, L, temp, comment);
}

void CIniFile::w_fvector3(Pcstr S, Pcstr L, const Fvector3& V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f", V.x, V.y, V.z);
	w_string(S, L, temp, comment);
}

void CIniFile::w_fvector4(Pcstr S, Pcstr L, const Fvector4& V, Pcstr comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f,%f", V.x, V.y, V.z, V.w);
	w_string(S, L, temp, comment);
}

void CIniFile::w_bool(Pcstr S, Pcstr L, bool V, Pcstr comment)
{
	w_string(S, L, V ? "true" : "false", comment);
}

//void	CIniFile::remove_line(Pcstr S, Pcstr L)
//{
//	R_ASSERT(!bReadOnly);
//
//	if (line_exist(S, L))
//	{
//		Sect& data = r_section(S);
//		SectIt_ A = std::lower_bound(data.Data.begin( ), data.Data.end( ), L, item_pred);
//		R_ASSERT(A != data.Data.end( ) && xr_strcmp(*A->first, L) == 0);
//		data.Data.erase(A);
//	}
//}

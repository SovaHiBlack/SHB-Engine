#include "stdafx.h"

#include "fs_internal.h"

CORE_API CConfigurationFile* pSettings = nullptr;

CConfigurationFile* CConfigurationFile::Create(const char* szFileName, BOOL ReadOnly)
{
	return xr_new<CConfigurationFile>(szFileName, ReadOnly);
}

void CConfigurationFile::Destroy(CConfigurationFile* ini)
{
	xr_delete(ini);
}

bool sect_pred(const CConfigurationFile::Sect* x, const char* val)
{
	return xr_strcmp(*x->Name, val) < 0;
}

bool item_pred(const CConfigurationFile::SItem& x, const char* val)
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
CORE_API void _parse(char* dest, const char* src)
{
	if (src)
	{
		bool bInsideSTR = false;
		while (*src)
		{
			if (isspace((unsigned char) *src))
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

CORE_API void _decorate(char* dest, const char* src)
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

BOOL CConfigurationFile::Sect::line_exist(const char* L, const char** val)
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

CConfigurationFile::CConfigurationFile(IReader* F, const char* path)
{
	fName = 0;
	bReadOnly = TRUE;
	bSaveAtEnd = FALSE;
	Load(F, path);
}

CConfigurationFile::CConfigurationFile(const char* szFileName, BOOL ReadOnly, BOOL bLoad, BOOL SaveAtEnd)
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

CConfigurationFile::~CConfigurationFile( )
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

static void insert_item(CConfigurationFile::Sect* tgt, const CConfigurationFile::SItem& I)
{
	CConfigurationFile::SectIt_ sect_it = std::lower_bound(tgt->Data.begin( ), tgt->Data.end( ), *I.first, item_pred);
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

void CConfigurationFile::Load(IReader* F, const char* path)
{
	R_ASSERT(F);
	Sect* Current = nullptr;
	string4096 str;
	string4096 str2;

	while (!F->eof( ))
	{
		F->r_string(str, sizeof(str));
		_Trim(str);
		char* semi = strchr(str, ';');
		char* semi_1 = strchr(str, '/');

		if (semi_1 && (*(semi_1 + 1) == '/') && ((!semi) || (semi && (semi_1 < semi))))
		{
			semi = semi_1;
		}

#ifdef DEBUG
		char* comment = 0;
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
			const char* inherited_names = strstr(str, "]:");
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
				char* name = str;
				char* t = strchr(name, '=');
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

				SItem I;
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

bool CConfigurationFile::save_as(const char* new_fname)
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
				const SItem& I = *s_it;
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

BOOL CConfigurationFile::section_exist(const char* S)
{
	RootIt I = std::lower_bound(DATA.begin( ), DATA.end( ), S, sect_pred);
	return (I != DATA.end( ) && xr_strcmp(*(*I)->Name, S) == 0);
}

BOOL CConfigurationFile::line_exist(const char* S, const char* L)
{
	if (!section_exist(S))
	{
		return FALSE;
	}

	Sect& I = r_section(S);
	SectCIt A = std::lower_bound(I.Data.begin( ), I.Data.end( ), L, item_pred);
	return (A != I.Data.end( ) && xr_strcmp(*A->first, L) == 0);
}

U32 CConfigurationFile::line_count(const char* Sname)
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
CConfigurationFile::Sect& CConfigurationFile::r_section(const CSharedString& S)
{
	return r_section(*S);
}
BOOL CConfigurationFile::line_exist(const CSharedString& S, const CSharedString& L)
{
	return line_exist(*S, *L);
}
U32 CConfigurationFile::line_count(const CSharedString& S)
{
	return line_count(*S);
}
BOOL CConfigurationFile::section_exist(const CSharedString& S)
{
	return section_exist(*S);
}

//--------------------------------------------------------------------------------------
// Read functions
//--------------------------------------------------------------------------------------
CConfigurationFile::Sect& CConfigurationFile::r_section(const char* S)
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

const char* CConfigurationFile::r_string(const char* S, const char* L)
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

CSharedString CConfigurationFile::r_string_wb(const char* S, const char* L)
{
	const char* _base = r_string(S, L);

	if (0 == _base)
	{
		return CSharedString(0);
	}

	string512 _original;
	strcpy_s(_original, _base);
	U32 _len = xr_strlen(_original);
	if (0 == _len)
	{
		return	CSharedString("");
	}

	// skip end
	if ('"' == _original[_len - 1])
	{
		_original[_len - 1] = 0;
	}
	
	// skip begin
	if ('"' == _original[0])
	{
		return CSharedString(&_original[0] + 1);
	}
	
	return CSharedString(_original);
}

unsigned char CConfigurationFile::r_u8(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	return unsigned char(atoi(C));
}
unsigned short CConfigurationFile::r_u16(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	return unsigned short(atoi(C));
}
U32 CConfigurationFile::r_u32(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	return U32(atoi(C));
}
signed char CConfigurationFile::r_s8(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	return signed char(atoi(C));
}
signed short CConfigurationFile::r_s16(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	return signed short(atoi(C));
}
int CConfigurationFile::r_s32(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	return int(atoi(C));
}
F32 CConfigurationFile::r_float(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	return F32(atof(C));
}
Fcolor CConfigurationFile::r_fcolor(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	Fcolor V = { 0, 0, 0, 0 };
	sscanf(C, "%f,%f,%f,%f", &V.r, &V.g, &V.b, &V.a);
	return V;
}
U32 CConfigurationFile::r_color(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	U32 r = 0;
	U32 g = 0;
	U32 b = 0;
	U32 a = 255;
	sscanf(C, "%d,%d,%d,%d", &r, &g, &b, &a);
	return color_rgba(r, g, b, a);
}

Ivector2 CConfigurationFile::r_ivector2(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	Ivector2 V = { 0, 0 };
	sscanf(C, "%d,%d", &V.x, &V.y);
	return V;
}
Ivector3 CConfigurationFile::r_ivector3(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	Ivector3 V = { 0, 0, 0 };
	sscanf(C, "%d,%d,%d", &V.x, &V.y, &V.z);
	return V;
}
Ivector4 CConfigurationFile::r_ivector4(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	Ivector4 V = { 0, 0, 0, 0 };
	sscanf(C, "%d,%d,%d,%d", &V.x, &V.y, &V.z, &V.w);
	return V;
}
Fvector2 CConfigurationFile::r_fvector2(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	Fvector2 V = { 0.0f, 0.0f };
	sscanf(C, "%f,%f", &V.x, &V.y);
	return V;
}
Fvector3 CConfigurationFile::r_fvector3(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	Fvector3 V = { 0.0f, 0.0f, 0.0f };
	sscanf(C, "%f,%f,%f", &V.x, &V.y, &V.z);
	return V;
}
Fvector4 CConfigurationFile::r_fvector4(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	Fvector4 V = { 0.0f, 0.0f, 0.0f, 0.0f };
	sscanf(C, "%f,%f,%f,%f", &V.x, &V.y, &V.z, &V.w);
	return V;
}
BOOL CConfigurationFile::r_bool(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	char B[8];
	strncpy(B, C, 7);
	strlwr(B);
	return IsBOOL(B);
}
CLASS_ID CConfigurationFile::r_clsid(const char* S, const char* L)
{
	const char* C = r_string(S, L);
	return TEXT2CLSID(C);
}

int CConfigurationFile::r_token(const char* S, const char* L, const xr_token* token_list)
{
	const char* C = r_string(S, L);
	for (int i = 0; token_list[i].name; i++)
	{
		if (!stricmp(C, token_list[i].name))
		{
			return token_list[i].id;
		}
	}

	return 0;
}

BOOL CConfigurationFile::r_line(const char* S, int L, const char** N, const char** V)
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
BOOL CConfigurationFile::r_line(const CSharedString& S, int L, const char** N, const char** V)
{
	return r_line(*S, L, N, V);
}

//--------------------------------------------------------------------------------------------------------
// Write functions
//--------------------------------------------------------------------------------------
void CConfigurationFile::w_string(const char* S, const char* L, const char* V, const char* comment)
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
	SItem I;
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
void CConfigurationFile::w_u8(const char* S, const char* L, unsigned char V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CConfigurationFile::w_u16(const char* S, const char* L, unsigned short V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CConfigurationFile::w_u32(const char* S, const char* L, U32 V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CConfigurationFile::w_s8(const char* S, const char* L, signed char V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CConfigurationFile::w_s16(const char* S, const char* L, signed short V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CConfigurationFile::w_s32(const char* S, const char* L, int V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d", V);
	w_string(S, L, temp, comment);
}
void CConfigurationFile::w_float(const char* S, const char* L, F32 V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f", V);
	w_string(S, L, temp, comment);
}
void CConfigurationFile::w_fcolor(const char* S, const char* L, const Fcolor& V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f,%f", V.r, V.g, V.b, V.a);
	w_string(S, L, temp, comment);
}

void CConfigurationFile::w_color(const char* S, const char* L, U32 V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d,%d", color_get_R(V), color_get_G(V), color_get_B(V), color_get_A(V));
	w_string(S, L, temp, comment);
}

void CConfigurationFile::w_ivector2(const char* S, const char* L, const Ivector2& V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d", V.x, V.y);
	w_string(S, L, temp, comment);
}

void CConfigurationFile::w_ivector3(const char* S, const char* L, const Ivector3& V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d", V.x, V.y, V.z);
	w_string(S, L, temp, comment);
}

void CConfigurationFile::w_ivector4(const char* S, const char* L, const Ivector4& V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%d,%d,%d,%d", V.x, V.y, V.z, V.w);
	w_string(S, L, temp, comment);
}
void CConfigurationFile::w_fvector2(const char* S, const char* L, const Fvector2& V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f", V.x, V.y);
	w_string(S, L, temp, comment);
}

void CConfigurationFile::w_fvector3(const char* S, const char* L, const Fvector3& V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f", V.x, V.y, V.z);
	w_string(S, L, temp, comment);
}

void CConfigurationFile::w_fvector4(const char* S, const char* L, const Fvector4& V, const char* comment)
{
	string128 temp;
	sprintf_s(temp, sizeof(temp), "%f,%f,%f,%f", V.x, V.y, V.z, V.w);
	w_string(S, L, temp, comment);
}

void CConfigurationFile::w_bool(const char* S, const char* L, bool V, const char* comment)
{
	w_string(S, L, V ? "true" : "false", comment);
}

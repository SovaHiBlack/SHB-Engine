#pragma once

// refs
class CIniFile;
struct xr_token;

class CORE_API CIniFile
{
public:
	struct CORE_API	Item
	{
		CSharedString	first;
		CSharedString	second;

#ifdef DEBUG
		CSharedString	comment;
#endif // def DEBUG

		Item( ) : first(0), second(0)

#ifdef DEBUG
			, comment(0)
#endif // def DEBUG

		{ }
	};

	using Items = xr_vector<Item>;
	using SectIt_ = Items::iterator;
	using SectCIt = Items::const_iterator;

	struct CORE_API Sect
	{
		CSharedString		Name;
		Items			Data;

		BOOL			line_exist(const char* L, const char** val = 0);
	};

	using Root = xr_vector<Sect*>;
	using RootIt = Root::iterator;

	// factorisation
	static CIniFile* Create(const char* szFileName, BOOL ReadOnly = TRUE);
	static void			Destroy(CIniFile*);
	static inline BOOL		IsBOOL(const char* B)
	{
		return (xr_strcmp(B, "on") == 0 || xr_strcmp(B, "yes") == 0 || xr_strcmp(B, "true") == 0 || xr_strcmp(B, "1") == 0);
	}

private:
	char* fName;
	Root		DATA;
	BOOL		bReadOnly;
	void		Load(IReader* F, const char* path);

public:
	BOOL		bSaveAtEnd;

	CIniFile(IReader* F, const char* path = 0);
	CIniFile(const char* szFileName, BOOL ReadOnly = TRUE, BOOL bLoadAtStart = TRUE, BOOL SaveAtEnd = TRUE);
	virtual 	~CIniFile( );
	bool		save_as(const char* new_fname = 0);

	const char* fname( )
	{
		return fName;
	}

	Sect& r_section(const char* S);
	Sect& r_section(const CSharedString& S);
	BOOL		line_exist(const char* S, const char* L);
	BOOL		line_exist(const CSharedString& S, const CSharedString& L);
	U32			line_count(const char* S);
	U32			line_count(const CSharedString& S);
	BOOL		section_exist(const char* S);
	BOOL		section_exist(const CSharedString& S);
	Root& sections( )
	{
		return DATA;
	}

	CLASS_ID	r_clsid(const char* S, const char* L);
	CLASS_ID	r_clsid(const CSharedString& S, const char* L)
	{
		return r_clsid(*S, L);
	}
	const char* r_string(const char* S, const char* L);															// оставляет кавычки
	const char* r_string(const CSharedString& S, const char* L)
	{
		return r_string(*S, L);
	}	// оставляет кавычки
	CSharedString		r_string_wb(const char* S, const char* L);															// убирает кавычки
	CSharedString		r_string_wb(const CSharedString& S, const char* L)
	{
		return r_string_wb(*S, L);
	}	// убирает кавычки
	U8	 		r_u8(const char* S, const char* L);
	U8	 		r_u8(const CSharedString& S, const char* L)
	{
		return r_u8(*S, L);
	}
	U16	 		r_u16(const char* S, const char* L);
	U16	 		r_u16(const CSharedString& S, const char* L)
	{
		return r_u16(*S, L);
	}
	U32	 		r_u32(const char* S, const char* L);
	U32	 		r_u32(const CSharedString& S, const char* L)
	{
		return r_u32(*S, L);
	}
	S8	 		r_s8(const char* S, const char* L);
	S8	 		r_s8(const CSharedString& S, const char* L)
	{
		return r_s8(*S, L);
	}
	S16	 		r_s16(const char* S, const char* L);
	S16	 		r_s16(const CSharedString& S, const char* L)
	{
		return r_s16(*S, L);
	}
	int	 		r_s32(const char* S, const char* L);
	int	 		r_s32(const CSharedString& S, const char* L)
	{
		return r_s32(*S, L);
	}
	F32		r_float(const char* S, const char* L);
	F32		r_float(const CSharedString& S, const char* L)
	{
		return r_float(*S, L);
	}
	Fcolor		r_fcolor(const char* S, const char* L);
	Fcolor		r_fcolor(const CSharedString& S, const char* L)
	{
		return r_fcolor(*S, L);
	}
	U32			r_color(const char* S, const char* L);
	U32			r_color(const CSharedString& S, const char* L)
	{
		return r_color(*S, L);
	}
	Ivector2	r_ivector2(const char* S, const char* L);
	Ivector2	r_ivector2(const CSharedString& S, const char* L)
	{
		return r_ivector2(*S, L);
	}
	Ivector3	r_ivector3(const char* S, const char* L);
	Ivector3	r_ivector3(const CSharedString& S, const char* L)
	{
		return r_ivector3(*S, L);
	}
	Ivector4	r_ivector4(const char* S, const char* L);
	Ivector4	r_ivector4(const CSharedString& S, const char* L)
	{
		return r_ivector4(*S, L);
	}
	Fvector2	r_fvector2(const char* S, const char* L);
	Fvector2	r_fvector2(const CSharedString& S, const char* L)
	{
		return r_fvector2(*S, L);
	}
	Fvector3	r_fvector3(const char* S, const char* L);
	Fvector3	r_fvector3(const CSharedString& S, const char* L)
	{
		return r_fvector3(*S, L);
	}
	Fvector4	r_fvector4(const char* S, const char* L);
	Fvector4	r_fvector4(const CSharedString& S, const char* L)
	{
		return r_fvector4(*S, L);
	}
	BOOL		r_bool(const char* S, const char* L);
	BOOL		r_bool(const CSharedString& S, const char* L)
	{
		return r_bool(*S, L);
	}
	int			r_token(const char* S, const char* L, const xr_token* token_list);
	BOOL		r_line(const char* S, int L, const char** N, const char** V);
	BOOL		r_line(const CSharedString& S, int L, const char** N, const char** V);

	void		w_string(const char* S, const char* L, const char* V, const char* comment = 0);
	void		w_u8(const char* S, const char* L, U8				V, const char* comment = 0);
	void		w_u16(const char* S, const char* L, U16				V, const char* comment = 0);
	void		w_u32(const char* S, const char* L, U32				V, const char* comment = 0);
	void		w_s8(const char* S, const char* L, S8				V, const char* comment = 0);
	void		w_s16(const char* S, const char* L, S16				V, const char* comment = 0);
	void		w_s32(const char* S, const char* L, int				V, const char* comment = 0);
	void		w_float(const char* S, const char* L, F32				V, const char* comment = 0);
	void		w_fcolor(const char* S, const char* L, const Fcolor& V, const char* comment = 0);
	void		w_color(const char* S, const char* L, U32				V, const char* comment = 0);
	void		w_ivector2(const char* S, const char* L, const Ivector2& V, const char* comment = 0);
	void		w_ivector3(const char* S, const char* L, const Ivector3& V, const char* comment = 0);
	void		w_ivector4(const char* S, const char* L, const Ivector4& V, const char* comment = 0);
	void		w_fvector2(const char* S, const char* L, const Fvector2& V, const char* comment = 0);
	void		w_fvector3(const char* S, const char* L, const Fvector3& V, const char* comment = 0);
	void		w_fvector4(const char* S, const char* L, const Fvector4& V, const char* comment = 0);
	void		w_bool(const char* S, const char* L, bool V, const char* comment = 0);
};

// Main configuration file
extern CORE_API CIniFile* pSettings;

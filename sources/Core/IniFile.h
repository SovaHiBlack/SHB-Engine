#pragma once

// refs
class CIniFile;
struct xr_token;

class CORE_API CIniFile
{
public:
	struct CORE_API	Item
	{
		shared_str	first;
		shared_str	second;

#ifdef DEBUG
		shared_str	comment;
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
		shared_str		Name;
		Items			Data;

		BOOL			line_exist(Pcstr L, Pcstr* val = 0);
	};

	using Root = xr_vector<Sect*>;
	using RootIt = Root::iterator;

	// factorisation
	static CIniFile* Create(Pcstr szFileName, BOOL ReadOnly = TRUE);
	static void			Destroy(CIniFile*);
	static inline BOOL		IsBOOL(Pcstr B)
	{
		return (xr_strcmp(B, "on") == 0 || xr_strcmp(B, "yes") == 0 || xr_strcmp(B, "true") == 0 || xr_strcmp(B, "1") == 0);
	}

private:
	char* fName;
	Root		DATA;
	BOOL		bReadOnly;
	void		Load(IReader* F, Pcstr path);

public:
	BOOL		bSaveAtEnd;

	CIniFile(IReader* F, Pcstr path = 0);
	CIniFile(Pcstr szFileName, BOOL ReadOnly = TRUE, BOOL bLoadAtStart = TRUE, BOOL SaveAtEnd = TRUE);
	virtual 	~CIniFile( );
	bool		save_as(Pcstr new_fname = 0);

	Pcstr fname( )
	{
		return fName;
	}

	Sect& r_section(Pcstr S);
	Sect& r_section(const shared_str& S);
	BOOL		line_exist(Pcstr S, Pcstr L);
	BOOL		line_exist(const shared_str& S, const shared_str& L);
	U32			line_count(Pcstr S);
	U32			line_count(const shared_str& S);
	BOOL		section_exist(Pcstr S);
	BOOL		section_exist(const shared_str& S);
	Root& sections( )
	{
		return DATA;
	}

	CLASS_ID	r_clsid(Pcstr S, Pcstr L);
	CLASS_ID	r_clsid(const shared_str& S, Pcstr L)
	{
		return r_clsid(*S, L);
	}
	Pcstr r_string(Pcstr S, Pcstr L);															// оставляет кавычки
	Pcstr r_string(const shared_str& S, Pcstr L)
	{
		return r_string(*S, L);
	}	// оставляет кавычки
	shared_str		r_string_wb(Pcstr S, Pcstr L);															// убирает кавычки
	shared_str		r_string_wb(const shared_str& S, Pcstr L)
	{
		return r_string_wb(*S, L);
	}	// убирает кавычки
	U8	 		r_u8(Pcstr S, Pcstr L);
	U8	 		r_u8(const shared_str& S, Pcstr L)
	{
		return r_u8(*S, L);
	}
	U16	 		r_u16(Pcstr S, Pcstr L);
	U16	 		r_u16(const shared_str& S, Pcstr L)
	{
		return r_u16(*S, L);
	}
	U32	 		r_u32(Pcstr S, Pcstr L);
	U32	 		r_u32(const shared_str& S, Pcstr L)
	{
		return r_u32(*S, L);
	}
	S8	 		r_s8(Pcstr S, Pcstr L);
	S8	 		r_s8(const shared_str& S, Pcstr L)
	{
		return r_s8(*S, L);
	}
	S16	 		r_s16(Pcstr S, Pcstr L);
	S16	 		r_s16(const shared_str& S, Pcstr L)
	{
		return r_s16(*S, L);
	}
	int	 		r_s32(Pcstr S, Pcstr L);
	int	 		r_s32(const shared_str& S, Pcstr L)
	{
		return r_s32(*S, L);
	}
	F32		r_float(Pcstr S, Pcstr L);
	F32		r_float(const shared_str& S, Pcstr L)
	{
		return r_float(*S, L);
	}
	Fcolor		r_fcolor(Pcstr S, Pcstr L);
	Fcolor		r_fcolor(const shared_str& S, Pcstr L)
	{
		return r_fcolor(*S, L);
	}
	U32			r_color(Pcstr S, Pcstr L);
	U32			r_color(const shared_str& S, Pcstr L)
	{
		return r_color(*S, L);
	}
	Ivector2	r_ivector2(Pcstr S, Pcstr L);
	Ivector2	r_ivector2(const shared_str& S, Pcstr L)
	{
		return r_ivector2(*S, L);
	}
	Ivector3	r_ivector3(Pcstr S, Pcstr L);
	Ivector3	r_ivector3(const shared_str& S, Pcstr L)
	{
		return r_ivector3(*S, L);
	}
	Ivector4	r_ivector4(Pcstr S, Pcstr L);
	Ivector4	r_ivector4(const shared_str& S, Pcstr L)
	{
		return r_ivector4(*S, L);
	}
	Fvector2	r_fvector2(Pcstr S, Pcstr L);
	Fvector2	r_fvector2(const shared_str& S, Pcstr L)
	{
		return r_fvector2(*S, L);
	}
	Fvector3	r_fvector3(Pcstr S, Pcstr L);
	Fvector3	r_fvector3(const shared_str& S, Pcstr L)
	{
		return r_fvector3(*S, L);
	}
	Fvector4	r_fvector4(Pcstr S, Pcstr L);
	Fvector4	r_fvector4(const shared_str& S, Pcstr L)
	{
		return r_fvector4(*S, L);
	}
	BOOL		r_bool(Pcstr S, Pcstr L);
	BOOL		r_bool(const shared_str& S, Pcstr L)
	{
		return r_bool(*S, L);
	}
	int			r_token(Pcstr S, Pcstr L, const xr_token* token_list);
	BOOL		r_line(Pcstr S, int L, Pcstr* N, Pcstr* V);
	BOOL		r_line(const shared_str& S, int L, Pcstr* N, Pcstr* V);

	void		w_string(Pcstr S, Pcstr L, Pcstr V, Pcstr comment = 0);
	void		w_u8(Pcstr S, Pcstr L, U8				V, Pcstr comment = 0);
	void		w_u16(Pcstr S, Pcstr L, U16				V, Pcstr comment = 0);
	void		w_u32(Pcstr S, Pcstr L, U32				V, Pcstr comment = 0);
	void		w_s8(Pcstr S, Pcstr L, S8				V, Pcstr comment = 0);
	void		w_s16(Pcstr S, Pcstr L, S16				V, Pcstr comment = 0);
	void		w_s32(Pcstr S, Pcstr L, int				V, Pcstr comment = 0);
	void		w_float(Pcstr S, Pcstr L, F32				V, Pcstr comment = 0);
	void		w_fcolor(Pcstr S, Pcstr L, const Fcolor& V, Pcstr comment = 0);
	void		w_color(Pcstr S, Pcstr L, U32				V, Pcstr comment = 0);
	void		w_ivector2(Pcstr S, Pcstr L, const Ivector2& V, Pcstr comment = 0);
	void		w_ivector3(Pcstr S, Pcstr L, const Ivector3& V, Pcstr comment = 0);
	void		w_ivector4(Pcstr S, Pcstr L, const Ivector4& V, Pcstr comment = 0);
	void		w_fvector2(Pcstr S, Pcstr L, const Fvector2& V, Pcstr comment = 0);
	void		w_fvector3(Pcstr S, Pcstr L, const Fvector3& V, Pcstr comment = 0);
	void		w_fvector4(Pcstr S, Pcstr L, const Fvector4& V, Pcstr comment = 0);
	void		w_bool(Pcstr S, Pcstr L, bool V, Pcstr comment = 0);

	//void		remove_line(Pcstr S, Pcstr L);
};

// Main configuration file
extern CORE_API CIniFile* pSettings;

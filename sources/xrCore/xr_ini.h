#pragma once

// refs
class	CInifile;
struct xr_token;


//-----------------------------------------------------------------------------------------------------------
//Описание Inifile
//-----------------------------------------------------------------------------------------------------------

class XRCORE_API CInifile
{
public:
	struct XRCORE_API	Item
	{
		shared_str	first;
		shared_str	second;

#ifdef DEBUG
		shared_str	comment;
#endif // DEBUG

		Item() : first(0), second(0)

#ifdef DEBUG
			, comment(0)
#endif // DEBUG

		{};
	};
	typedef xr_vector<Item>				Items;
	typedef Items::const_iterator		SectCIt;
	typedef Items::iterator				SectIt_;
	struct XRCORE_API	Sect
	{
		shared_str		Name;
		Items			Data;

		//.		IC SectCIt		begin()		{ return Data.begin();	}
		//.		IC SectCIt		end()		{ return Data.end();	}
		//.		IC size_t		size()		{ return Data.size();	}
		//.		IC void			clear()		{ Data.clear();			}
		BOOL			line_exist(pcstr L, pcstr* val = 0);
	};
	typedef	xr_vector<Sect*>		Root;
	typedef Root::iterator			RootIt;

	// factorisation
	static CInifile* Create(pcstr szFileName, BOOL ReadOnly = TRUE);
	static void			Destroy(CInifile*);
	static IC BOOL		IsBOOL(pcstr B)
	{
		return (xr_strcmp(B, "on") == 0 || xr_strcmp(B, "yes") == 0 || xr_strcmp(B, "true") == 0 || xr_strcmp(B, "1") == 0);
	}
private:
	pstr		fName;
	Root		DATA;
	BOOL		bReadOnly;
	void		Load(IReader* F, pcstr path);
public:
	BOOL		bSaveAtEnd;
public:
	CInifile(IReader* F, pcstr path = 0);
	CInifile(pcstr szFileName, BOOL ReadOnly = TRUE, BOOL bLoadAtStart = TRUE, BOOL SaveAtEnd = TRUE);
	virtual 	~CInifile();
	bool		save_as(pcstr new_fname = 0);

	pcstr		fname()
	{
		return fName;
	};

	Sect& r_section(pcstr S);
	Sect& r_section(const shared_str& S);
	BOOL		line_exist(pcstr S, pcstr L);
	BOOL		line_exist(const shared_str& S, const shared_str& L);
	u32			line_count(pcstr S);
	u32			line_count(const shared_str& S);
	BOOL		section_exist(pcstr S);
	BOOL		section_exist(const shared_str& S);
	Root& sections()
	{
		return DATA;
	}

	CLASS_ID	r_clsid(pcstr S, pcstr L);
	CLASS_ID	r_clsid(const shared_str& S, pcstr L)
	{
		return r_clsid(*S, L);
	}
	pcstr 		r_string(pcstr S, pcstr L);															// оставляет кавычки
	pcstr 		r_string(const shared_str& S, pcstr L)
	{
		return r_string(*S, L);
	}	// оставляет кавычки
	shared_str		r_string_wb(pcstr S, pcstr L);															// убирает кавычки
	shared_str		r_string_wb(const shared_str& S, pcstr L)
	{
		return r_string_wb(*S, L);
	}	// убирает кавычки
	u8	 		r_u8(pcstr S, pcstr L);
	u8	 		r_u8(const shared_str& S, pcstr L)
	{
		return r_u8(*S, L);
	}
	u16	 		r_u16(pcstr S, pcstr L);
	u16	 		r_u16(const shared_str& S, pcstr L)
	{
		return r_u16(*S, L);
	}
	u32	 		r_u32(pcstr S, pcstr L);
	u32	 		r_u32(const shared_str& S, pcstr L)
	{
		return r_u32(*S, L);
	}
	s8	 		r_s8(pcstr S, pcstr L);
	s8	 		r_s8(const shared_str& S, pcstr L)
	{
		return r_s8(*S, L);
	}
	s16	 		r_s16(pcstr S, pcstr L);
	s16	 		r_s16(const shared_str& S, pcstr L)
	{
		return r_s16(*S, L);
	}
	s32	 		r_s32(pcstr S, pcstr L);
	s32	 		r_s32(const shared_str& S, pcstr L)
	{
		return r_s32(*S, L);
	}
	F32			r_float(pcstr S, pcstr L);
	F32			r_float(const shared_str& S, pcstr L)
	{
		return r_float(*S, L);
	}
	Fcolor		r_fcolor(pcstr S, pcstr L);
	Fcolor		r_fcolor(const shared_str& S, pcstr L)
	{
		return r_fcolor(*S, L);
	}
	u32			r_color(pcstr S, pcstr L);
	u32			r_color(const shared_str& S, pcstr L)
	{
		return r_color(*S, L);
	}
	Ivector2	r_ivector2(pcstr S, pcstr L);
	Ivector2	r_ivector2(const shared_str& S, pcstr L)
	{
		return r_ivector2(*S, L);
	}
	Ivector3	r_ivector3(pcstr S, pcstr L);
	Ivector3	r_ivector3(const shared_str& S, pcstr L)
	{
		return r_ivector3(*S, L);
	}
	Ivector4	r_ivector4(pcstr S, pcstr L);
	Ivector4	r_ivector4(const shared_str& S, pcstr L)
	{
		return r_ivector4(*S, L);
	}
	Fvector2	r_fvector2(pcstr S, pcstr L);
	Fvector2	r_fvector2(const shared_str& S, pcstr L)
	{
		return r_fvector2(*S, L);
	}
	Fvector3	r_fvector3(pcstr S, pcstr L);
	Fvector3	r_fvector3(const shared_str& S, pcstr L)
	{
		return r_fvector3(*S, L);
	}
	Fvector4	r_fvector4(pcstr S, pcstr L);
	Fvector4	r_fvector4(const shared_str& S, pcstr L)
	{
		return r_fvector4(*S, L);
	}
	BOOL		r_bool(pcstr S, pcstr L);
	BOOL		r_bool(const shared_str& S, pcstr L)
	{
		return r_bool(*S, L);
	}
	int			r_token(pcstr S, pcstr L, const xr_token* token_list);
	BOOL		r_line(pcstr S, int L, pcstr* N, pcstr* V);
	BOOL		r_line(const shared_str& S, int L, pcstr* N, pcstr* V);

	void		w_string(pcstr S, pcstr L, pcstr			V, pcstr comment = 0);
	void		w_u8(pcstr S, pcstr L, u8				V, pcstr comment = 0);
	void		w_u16(pcstr S, pcstr L, u16				V, pcstr comment = 0);
	void		w_u32(pcstr S, pcstr L, u32				V, pcstr comment = 0);
	void		w_s8(pcstr S, pcstr L, s8				V, pcstr comment = 0);
	void		w_s16(pcstr S, pcstr L, s16				V, pcstr comment = 0);
	void		w_s32(pcstr S, pcstr L, s32				V, pcstr comment = 0);
	void		w_float(pcstr S, pcstr L, F32				V, pcstr comment = 0);
	void		w_fcolor(pcstr S, pcstr L, const Fcolor& V, pcstr comment = 0);
	void		w_color(pcstr S, pcstr L, u32				V, pcstr comment = 0);
	void		w_ivector2(pcstr S, pcstr L, const Ivector2& V, pcstr comment = 0);
	void		w_ivector3(pcstr S, pcstr L, const Ivector3& V, pcstr comment = 0);
	void		w_ivector4(pcstr S, pcstr L, const Ivector4& V, pcstr comment = 0);
	void		w_fvector2(pcstr S, pcstr L, const Fvector2& V, pcstr comment = 0);
	void		w_fvector3(pcstr S, pcstr L, const Fvector3& V, pcstr comment = 0);
	void		w_fvector4(pcstr S, pcstr L, const Fvector4& V, pcstr comment = 0);
	void		w_bool(pcstr S, pcstr L, BOOL				V, pcstr comment = 0);

	void		remove_line(pcstr S, pcstr L);
};

// Main configuration file
extern XRCORE_API CInifile* pSettings;

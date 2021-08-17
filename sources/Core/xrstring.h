#pragma once

#pragma pack(push, 4)
#pragma warning(disable:4200)
struct CORE_API SStringValue
{
	unsigned int						dwReference;
	unsigned int						dwLength;
	unsigned int						dwCRC;
	char								value[ ];
};

struct CORE_API SStringValueCompare
{	// less
	inline bool		operator ( )		(const SStringValue* A, const SStringValue* B) const
	{
		return A->dwCRC < B->dwCRC;
	}
};
#pragma warning(default:4200)

class CORE_API CStringContainer
{
private:
	using cdb							= xr_multiset<SStringValue*, SStringValueCompare>;
	cdb									container;
	xrCriticalSection					cs;

public:
	SStringValue*	dock				(const char* value);
	void			clean				( );
	void			dump				( );
	void			verify				( );
	unsigned int	stat_economy		( );
					~CStringContainer	( );
};
CORE_API extern CStringContainer*		g_pStringContainer;

class CSharedString				// shared_str
{
private:
	SStringValue* p_;

protected:
	// ref-counting
	void				_dec( )
	{
		if (0 == p_)
		{
			return;
		}

		p_->dwReference--;
		if (0 == p_->dwReference)
		{
			p_ = 0;
		}
	}

public:
	void				_set(const char* rhs)
	{
		SStringValue* v = g_pStringContainer->dock(rhs);
		if (0 != v)
		{
			v->dwReference++;
		}

		_dec( );
		p_ = v;
	}
	void				_set(CSharedString const& rhs)
	{
		SStringValue* v = rhs.p_;
		if (0 != v)
		{
			v->dwReference++;
		}

		_dec( );
		p_ = v;
	}
	const SStringValue* _get( ) const
	{
		return p_;
	}

	// construction
	CSharedString( )
	{
		p_ = 0;
	}
	CSharedString(const char* rhs)
	{
		p_ = 0;
		_set(rhs);
	}
	CSharedString(CSharedString const& rhs)
	{
		p_ = 0;
		_set(rhs);
	}
	~CSharedString( )
	{
		_dec( );
	}

	// assignment & accessors
	CSharedString& operator=	(const char* rhs)
	{
		_set(rhs);
		return (CSharedString&) *this;
	}
	CSharedString& operator=	(CSharedString const& rhs)
	{
		_set(rhs);
		return (CSharedString&) *this;
	}
	const char* operator*	( ) const
	{
		return p_ ? p_->value : 0;
	}
	bool				operator!	( ) const
	{
		return p_ == 0;
	}
	char				operator[]	(size_t id)
	{
		return p_->value[id];
	}
	const char* c_str( ) const
	{
		return p_ ? p_->value : 0;
	}

	// misc func
	U32					size( ) const
	{
		if (0 == p_)
		{
			return 0;
		}
		else
		{
			return p_->dwLength;
		}
	}
	void				swap(CSharedString& rhs)
	{
		SStringValue* tmp = p_;
		p_ = rhs.p_;
		rhs.p_ = tmp;
	}
	bool				equal(const CSharedString& rhs) const
	{
		return (p_ == rhs.p_);
	}
	CSharedString& __cdecl	sprintf(const char* format, ...)
	{
		string4096 buf;
		va_list p;
		va_start(p, format);
		int vs_sz = _vsnprintf(buf, sizeof(buf) - 1, format, p); buf[sizeof(buf) - 1] = 0;
		va_end(p);
		if (vs_sz)
		{
			_set(buf);
		}

		return 		(CSharedString&) *this;
	}
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr
inline bool operator	==	(CSharedString const& a, CSharedString const& b)
{
	return a._get( ) == b._get( );
}
inline bool operator	!=	(CSharedString const& a, CSharedString const& b)
{
	return a._get( ) != b._get( );
}
inline bool operator	<	(CSharedString const& a, CSharedString const& b)
{
	return a._get( ) < b._get( );
}
inline bool operator	>	(CSharedString const& a, CSharedString const& b)
{
	return a._get( ) > b._get( );
}

// externally visible standart functionality
inline void swap(CSharedString& lhs, CSharedString& rhs)
{
	lhs.swap(rhs);
}
inline U32	xr_strlen(CSharedString& a)
{
	return a.size( );
}
inline int	xr_strcmp(const CSharedString& a, const char* b)
{
	return xr_strcmp(*a, b);
}
inline int	xr_strcmp(const char* a, const CSharedString& b)
{
	return xr_strcmp(a, *b);
}
inline int	xr_strcmp(const CSharedString& a, const CSharedString& b)
{
	if (a.equal(b))		return 0;
	else				return xr_strcmp(*a, *b);
}
inline void	xr_strlwr(xr_string& src)
{
	for (xr_string::iterator it = src.begin( ); it != src.end( ); it++) *it = xr_string::value_type(tolower(*it));
}
inline void	xr_strlwr(CSharedString& src)
{
	if (*src)
	{
		char* lp = xr_strdup(*src); xr_strlwr(lp); src = lp; xr_free(lp);
	}
}

#pragma pack(pop)

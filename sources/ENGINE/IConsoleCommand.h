#pragma once

#define CMD0(cls)					{ static cls x##cls();					Console->AddCommand(&x##cls);}
#define CMD1(cls, p1)				{ static cls x##cls(p1);				Console->AddCommand(&x##cls);}
#define CMD2(cls, p1, p2)			{ static cls x##cls(p1, p2);			Console->AddCommand(&x##cls);}
#define CMD3(cls, p1, p2, p3)		{ static cls x##cls(p1, p2, p3);		Console->AddCommand(&x##cls);}
#define CMD4(cls, p1, p2, p3, p4)	{ static cls x##cls(p1, p2, p3, p4);	Console->AddCommand(&x##cls);}

class ENGINE_API IConsoleCommand
{
public:
	friend class		CConsole;
	typedef char		TInfo[256];
	typedef char		TStatus[256];

protected:
	const char*			cName;
	bool				bEnabled;
	bool				bLowerCaseArgs;
	bool				bEmptyArgsHandled;

	inline bool			EQ					(const char* S1, const char* S2)
	{
		return xr_strcmp(S1, S2) == 0;
	}

public:
						IConsoleCommand		(const char* N) : cName(N), bEnabled(true), bLowerCaseArgs(true), bEmptyArgsHandled(true)
	{ };
	virtual				~IConsoleCommand	( )
	{
		if (Console)
		{
			Console->RemoveCommand			(this);
		}
	};

	const char*			Name				( )
	{
		return cName;
	}
	void				InvalidSyntax		( )
	{
		TInfo								I;
		Info								(I);
		Msg									("~ Invalid syntax in call to '%s'", cName);
		Msg									("~ Valid arguments: %s", I);
	}
	virtual void		Execute				(const char* args) = 0;
	virtual void		Status				(TStatus& S)
	{
		S[0]								= 0;
	}
	virtual void		Info				(TInfo& I)
	{
		strcpy_s							(I, "no arguments");
	}
	virtual void		Save				(IWriter* F)
	{
		TStatus								S;
		Status								(S);
		if (S[0])
		{
			F->w_printf						("%s %s\r\n", cName, S);
		}
	}
};

class ENGINE_API CCC_Mask : public IConsoleCommand
{
protected:
	Flags32*			value;
	u32					mask;

public:
						CCC_Mask			(const char* N, Flags32* V, u32 M) : IConsoleCommand(N), value(V), mask(M)
	{ }

	const BOOL			GetValue			( ) const
	{
		return value->test					(mask);
	}

	virtual void		Execute				(const char* args)
	{
		if (EQ(args, "on"))
		{
			value->set						(mask, TRUE);
		}
		else if (EQ(args, "off"))
		{
			value->set						(mask, FALSE);
		}
		else if (EQ(args, "1"))
		{
			value->set						(mask, TRUE);
		}
		else if (EQ(args, "0"))
		{
			value->set						(mask, FALSE);
		}
		else
		{
			InvalidSyntax					( );
		}
	}
	virtual void		Status				(TStatus& S)
	{
		strcpy_s							(S, value->test(mask) ? "on" : "off");
	}
	virtual void		Info				(TInfo& I)
	{
		strcpy_s							(I, "'on/off' or '1/0'");
	}
};

class ENGINE_API CCC_Token : public IConsoleCommand
{
protected:
	u32*				value;
	xr_token*			tokens;

public:
						CCC_Token			(const char* N, u32* V, xr_token* T) : IConsoleCommand(N), value(V), tokens(T)
	{ }

	virtual xr_token*	GetToken			( )
	{
		return tokens;
	}

	virtual void		Execute				(const char* args)
	{
		xr_token* tok = tokens;
		while (tok->name)
		{
			if (stricmp(tok->name, args) == 0)
			{
				*value = tok->id;
				break;
			}

			tok++;
		}

		if (!tok->name)
		{
			InvalidSyntax( );
		}
	}
	virtual void		Status				(TStatus& S)
	{
		xr_token* tok						= tokens;
		while (tok->name)
		{
			if (tok->id == (int) (*value))
			{
				strcpy_s					(S, tok->name);
				return;
			}

			tok++;
		}

		strcpy_s							(S, "?");
		return;
	}
	virtual void		Info				(TInfo& I)
	{
		I[0]								= 0;
		xr_token* tok						= tokens;
		while (tok->name)
		{
			if (I[0])
			{
				strcat						(I, "/");
			}

			strcat							(I, tok->name);
			tok++;
		}
	}
};

class ENGINE_API CCC_Float : public IConsoleCommand
{
protected:
	float*				value;
	float				min;
	float				max;

public:
						CCC_Float			(const char* N, float* V, float _min = 0, float _max = 1) : IConsoleCommand(N), value(V), min(_min), max(_max)
	{ }

	const float			GetValue			( ) const
	{
		return *value;
	}
	const float			GetMin				( ) const
	{
		return min;
	}
	const float			GetMax				( ) const
	{
		return max;
	}

	virtual void		Execute				(const char* args)
	{
		float v								= float(atof(args));
		if (v < (min - EPS) || v >(max + EPS))
		{
			InvalidSyntax					( );
		}
		else
		{
			*value							= v;
		}
	}
	virtual void		Status				(TStatus& S)
	{
		sprintf_s							(S, sizeof(S), "%3.5f", *value);
		while (xr_strlen(S) && ('0' == S[xr_strlen(S) - 1]))
		{
			S[xr_strlen(S) - 1]				= 0;
		}
	}
	virtual void		Info				(TInfo& I)
	{
		sprintf_s							(I, sizeof(I), "float value in range [%3.3f,%3.3f]", min, max);
	}
};

class ENGINE_API CCC_Vector3 : public IConsoleCommand
{
protected:
	Fvector3*			value;
	Fvector3				min;
	Fvector3				max;

public:
						CCC_Vector3			(const char* N, Fvector3* V, const Fvector3 _min, const Fvector3 _max) : IConsoleCommand(N), value(V)
	{
		min.set								(_min);
		max.set								(_max);
	}

	virtual void		Execute				(const char* args)
	{
		Fvector3 v;
		if (3 != sscanf(args, "%f,%f,%f", &v.x, &v.y, &v.z))
		{
			InvalidSyntax					( );
			return;
		}

		if (v.x < min.x || v.y < min.y || v.z < min.z)
		{
			InvalidSyntax					( );
			return;
		}

		if (v.x > max.x || v.y > max.y || v.z > max.z)
		{
			InvalidSyntax					( );
			return;
		}

		value->set							(v);
	}
	virtual void		Status				(TStatus& S)
	{
		sprintf_s							(S, sizeof(S), "%f,%f,%f", value->x, value->y, value->z);
	}
	virtual void		Info				(TInfo& I)
	{
		sprintf_s							(I, sizeof(I), "vector3 in range [%f,%f,%f]-[%f,%f,%f]", min.x, min.y, min.z, max.x, max.y, max.z);
	}
};

class ENGINE_API CCC_Integer : public IConsoleCommand
{
protected:
	int*				value;
	int					min;
	int					max;

public:
						CCC_Integer			(const char* N, int* V, int _min = 0, int _max = 999) : IConsoleCommand(N), value(V), min(_min), max(_max)
	{ }

	const int			GetValue			( ) const
	{
		return *value;
	}
	const int			GetMin				( ) const
	{
		return min;
	}
	const int			GetMax				( ) const
	{
		return max;
	}

	virtual void		Execute				(const char* args)
	{
		int v								= atoi(args);
		if (v < min || v > max)
		{
			InvalidSyntax					( );
		}
		else
		{
			*value							= v;
		}
	}
	virtual void		Status				(TStatus& S)
	{
		itoa								(*value, S, 10);
	}
	virtual void		Info				(TInfo& I)
	{
		sprintf_s							(I, sizeof(I), "integer value in range [%d,%d]", min, max);
	}
};

class ENGINE_API CCC_String : public IConsoleCommand
{
protected:
	char*				value;
	int					size;

public:
						CCC_String			(const char* N, char* V, int _size = 2) : IConsoleCommand(N), value(V), size(_size)
	{
		bLowerCaseArgs						= false;
		R_ASSERT							(V);
		R_ASSERT							(size > 1);
	}

	virtual void		Execute				(const char* args)
	{
		strncpy								(value, args, size - 1);
	}
	virtual void		Status				(TStatus& S)
	{
		strcpy_s							(S, value);
	}
	virtual void		Info				(TInfo& I)
	{
		sprintf_s							(I, sizeof(I), "string with up to %d characters", size);
	}
};

class ENGINE_API CCC_LoadCFG : public IConsoleCommand
{
public:
						CCC_LoadCFG			(const char* N);

	virtual bool		allow				(const char* cmd)
	{
		return true;
	}

	virtual void		Execute				(const char* args);
};

class ENGINE_API CCC_LoadCFG_custom : public CCC_LoadCFG
{
	string64			m_cmd;

public:
						CCC_LoadCFG_custom	(const char* cmd);

	virtual bool		allow				(const char* cmd);
};

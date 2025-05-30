#pragma once

#define CMD0(cls)					{ static cls x##cls( );					Console->AddCommand(&x##cls); }
#define CMD1(cls, p1)				{ static cls x##cls(p1);				Console->AddCommand(&x##cls); }
#define CMD2(cls, p1, p2)			{ static cls x##cls(p1, p2);			Console->AddCommand(&x##cls); }
#define CMD3(cls, p1, p2, p3)		{ static cls x##cls(p1, p2, p3);		Console->AddCommand(&x##cls); }
#define CMD4(cls, p1, p2, p3, p4)	{ static cls x##cls(p1, p2, p3, p4);	Console->AddCommand(&x##cls); }

class ENGINE_API IConsoleCommand
{
public:
	friend class	CConsole;
	typedef char	TInfo[256];
	typedef char	TStatus[256];

protected:
	pcstr			cName;
	bool			bEnabled;
	bool			bLowerCaseArgs;
	bool			bEmptyArgsHandled;

	IC	bool		EQ(pcstr S1, pcstr S2)
	{
		return xr_strcmp(S1, S2) == 0;
	}

public:
	IConsoleCommand(pcstr N) : cName(N), bEnabled(TRUE), bLowerCaseArgs(TRUE), bEmptyArgsHandled(FALSE)
	{ }
	virtual ~IConsoleCommand( )
	{
		if (Console)
		{
			Console->RemoveCommand(this);
		}
	}

	pcstr Name( )
	{
		return cName;
	}
	void			InvalidSyntax( )
	{
		TInfo I;
		Info(I);
		Msg("~ Invalid syntax in call to '%s'", cName);
		Msg("~ Valid arguments: %s", I);
	}
	virtual void	Execute(pcstr args) = 0;
	virtual void	Status(TStatus& S)
	{
		S[0] = 0;
	}
	virtual void	Info(TInfo& I)
	{
		strcpy_s(I, "no arguments");
	}
	virtual void	Save(IWriter* F)
	{
		TStatus		S;
		Status(S);
		if (S[0])
		{
			F->w_printf("%s %s\r\n", cName, S);
		}
	}
};

class ENGINE_API	CCC_Mask : public IConsoleCommand
{
protected:
	flags32* value;
	u32 mask;

public:
	CCC_Mask(pcstr N, flags32* V, u32 M) : IConsoleCommand(N), value(V), mask(M)
	{ }

	const BOOL GetValue( )const
	{
		return value->test(mask);
	}

	virtual void Execute(pcstr args)
	{
		if (EQ(args, "on"))
		{
			value->set(mask, TRUE);
		}
		else if (EQ(args, "off"))
		{
			value->set(mask, FALSE);
		}
		else if (EQ(args, "1"))
		{
			value->set(mask, TRUE);
		}
		else if (EQ(args, "0"))
		{
			value->set(mask, FALSE);
		}
		else
		{
			InvalidSyntax( );
		}
	}

	virtual void Status(TStatus& S)
	{
		strcpy_s(S, value->test(mask) ? "on" : "off");
	}

	virtual void Info(TInfo& I)
	{
		strcpy_s(I, "'on/off' or '1/0'");
	}
};

class ENGINE_API	CCC_ToggleMask : public IConsoleCommand
{
protected:
	flags32* value;
	u32			mask;

public:
	CCC_ToggleMask(pcstr N, flags32* V, u32 M) : IConsoleCommand(N), value(V), mask(M)
	{
		bEmptyArgsHandled = true;
	}

	const BOOL GetValue( )const
	{
		return value->test(mask);
	}

	virtual void Execute(pcstr args)
	{
		value->set(mask, !GetValue( ));
		TStatus S;
		strconcat(sizeof(S), S, cName, " is ", value->test(mask) ? "on" : "off");
		Log(S);
	}

	virtual void	Status(TStatus& S)
	{
		strcpy_s(S, value->test(mask) ? "on" : "off");
	}
	virtual void	Info(TInfo& I)
	{
		strcpy_s(I, "'on/off' or '1/0'");
	}
};

class ENGINE_API	CCC_Token : public IConsoleCommand
{
protected:
	u32* value;
	xr_token* tokens;

public:
	CCC_Token(pcstr N, u32* V, xr_token* T) : IConsoleCommand(N), value(V), tokens(T)
	{ }

	virtual void	Execute(pcstr args)
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
	virtual void	Status(TStatus& S)
	{
		xr_token* tok = tokens;
		while (tok->name)
		{
			if (tok->id == (s32)(*value))
			{
				strcpy_s(S, tok->name);
				return;
			}

			tok++;
		}

		strcpy_s(S, "?");
		return;
	}
	virtual void	Info(TInfo& I)
	{
		I[0] = 0;
		xr_token* tok = tokens;
		while (tok->name)
		{
			if (I[0]) strcat(I, "/");
			strcat(I, tok->name);
			tok++;
		}
	}
	virtual xr_token* GetToken( )
	{
		return tokens;
	}
};

class ENGINE_API	CCC_Float : public IConsoleCommand
{
protected:
	f32* value;
	f32 min;
	f32 max;

public:
	CCC_Float(pcstr N, f32* V, f32 _min = 0, f32 _max = 1) : IConsoleCommand(N), value(V), min(_min), max(_max)
	{ }

	const f32	GetValue( ) const
	{
		return *value;
	}
	const f32	GetMin( ) const
	{
		return min;
	}
	const f32	GetMax( ) const
	{
		return max;
	}

	virtual void	Execute(pcstr args)
	{
		f32 v = f32(atof(args));
		if (v<(min - EPSILON_5) || v>(max + EPSILON_5))
		{
			InvalidSyntax( );
		}
		else
		{
			*value = v;
		}
	}
	virtual void Status(TStatus& S)
	{
		sprintf_s(S, sizeof(S), "%3.5f", *value);
		while (xr_strlen(S) && ('0' == S[xr_strlen(S) - 1]))
		{
			S[xr_strlen(S) - 1] = 0;
		}
	}
	virtual void Info(TInfo& I)
	{
		sprintf_s(I, sizeof(I), "float value in range [%3.3f,%3.3f]", min, max);
	}
};

class ENGINE_API	CCC_Vector3 : public IConsoleCommand
{
protected:
	fVector3* value;
	fVector3 min;
	fVector3 max;

public:
	CCC_Vector3(pcstr N, fVector3* V, const fVector3 _min, const fVector3 _max) : IConsoleCommand(N), value(V)
	{
		min.set(_min);
		max.set(_max);
	}

	virtual void	Execute(pcstr args)
	{
		fVector3 v;
		if (3 != sscanf(args, "%f,%f,%f", &v.x, &v.y, &v.z))
		{
			InvalidSyntax( );
			return;
		}

		if (v.x < min.x || v.y < min.y || v.z < min.z)
		{
			InvalidSyntax( );
			return;
		}

		if (v.x > max.x || v.y > max.y || v.z > max.z)
		{
			InvalidSyntax( );
			return;
		}

		value->set(v);
	}
	virtual void	Status(TStatus& S)
	{
		sprintf_s(S, sizeof(S), "%f,%f,%f", value->x, value->y, value->z);
	}
	virtual void	Info(TInfo& I)
	{
		sprintf_s(I, sizeof(I), "vector3 in range [%f,%f,%f]-[%f,%f,%f]", min.x, min.y, min.z, max.x, max.y, max.z);
	}
};

class ENGINE_API	CCC_Integer : public IConsoleCommand
{
protected:
	s32* value;
	s32 min;
	s32 max;

public:
	const s32 GetValue( ) const
	{
		return *value;
	}
	const s32 GetMin( ) const
	{
		return min;
	}
	const s32 GetMax( ) const
	{
		return max;
	}

	CCC_Integer(pcstr N, s32* V, s32 _min = 0, s32 _max = 999) : IConsoleCommand(N), value(V), min(_min), max(_max)
	{ }

	virtual void Execute(pcstr args)
	{
		s32 v = atoi(args);
		if (v < min || v > max)
		{
			InvalidSyntax( );
		}
		else
		{
			*value = v;
		}
	}

	virtual void Status(TStatus& S)
	{
		itoa(*value, S, 10);
	}

	virtual void Info(TInfo& I)
	{
		sprintf_s(I, sizeof(I), "integer value in range [%d,%d]", min, max);
	}
};

class ENGINE_API	CCC_String : public IConsoleCommand
{
protected:
	pstr value;
	s32 size;

public:
	CCC_String(pcstr N, pstr V, s32 _size = 2) : IConsoleCommand(N), value(V), size(_size)
	{
		bLowerCaseArgs = FALSE;
		R_ASSERT(V);
		R_ASSERT(size > 1);
	}

	virtual void Execute(pcstr args)
	{
		strncpy(value, args, size - 1);
	}
	virtual void Status(TStatus& S)
	{
		strcpy_s(S, value);
	}
	virtual void Info(TInfo& I)
	{
		sprintf_s(I, sizeof(I), "string with up to %d characters", size);
	}
};

class ENGINE_API CCC_LoadCFG : public IConsoleCommand
{
public:
	virtual bool allow(pcstr cmd)
	{
		return true;
	}

	CCC_LoadCFG(pcstr N);

	virtual void Execute(pcstr args);
};

class ENGINE_API CCC_LoadCFG_custom : public CCC_LoadCFG
{
	string64 m_cmd;

public:
	CCC_LoadCFG_custom(pcstr cmd);
	virtual bool allow(pcstr cmd);
};

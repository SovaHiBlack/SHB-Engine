#pragma once

typedef void		crashhandler(void);
typedef void		on_dialog(bool before);

class CORE_API CDebug
{
private:
	crashhandler* handler;
	on_dialog* m_on_dialog;

public:
	void						_initialize( );

	crashhandler* get_crashhandler( )
	{
		return handler;
	};
	void						set_crashhandler(crashhandler* _handler)
	{
		handler = _handler;
	};

	on_dialog* get_on_dialog( )
	{
		return m_on_dialog;
	}
	void						set_on_dialog(on_dialog* on_dialog)
	{
		m_on_dialog = on_dialog;
	}

	Pcstr error2string(long code);

	void						fail(Pcstr e1, Pcstr file, int line, Pcstr unction, bool& ignore_always);
	void						fail(Pcstr e1, const std::string& e2, Pcstr file, int line, Pcstr function, bool& ignore_always);
	void						fail(Pcstr e1, Pcstr e2, Pcstr file, int line, Pcstr function, bool& ignore_always);
	void						fail(Pcstr e1, Pcstr e2, Pcstr e3, Pcstr file, int line, Pcstr function, bool& ignore_always);
	void						fail(Pcstr e1, Pcstr e2, Pcstr e3, Pcstr e4, Pcstr file, int line, Pcstr function, bool& ignore_always);
	void						error(long code, Pcstr e1, Pcstr file, int line, Pcstr function, bool& ignore_always);
	void						error(long code, Pcstr e1, Pcstr e2, Pcstr file, int line, Pcstr function, bool& ignore_always);
	void _cdecl					fatal(Pcstr file, int line, Pcstr function, Pcstr F, ...);
	void						backend(Pcstr reason, Pcstr expression, Pcstr argument0, Pcstr argument1, Pcstr file, int line, Pcstr function, bool& ignore_always);
	void						do_exit(const std::string& message);
};

// warning
// this function can be used for debug purposes only
inline std::string __cdecl		make_string(Pcstr format, ...)
{
	va_list			args;
	va_start(args, format);

	char			temp[4096];
	vsprintf(temp, format, args);

	return temp;
}

extern CORE_API CDebug								Debug;

CORE_API void					LogStackTrace(Pcstr header);

#include "Debug_macros.h"

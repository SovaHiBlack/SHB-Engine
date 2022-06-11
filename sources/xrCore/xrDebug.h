#pragma once

typedef void		crashhandler(void);
typedef void		on_dialog(bool before);

class XRCORE_API	xrDebug
{
private:
	crashhandler* handler;
	on_dialog* m_on_dialog;

public:
	void			_initialize();

public:
	crashhandler* get_crashhandler()
	{
		return handler;
	};
	void			set_crashhandler(crashhandler* _handler)
	{
		handler = _handler;
	};

	on_dialog* get_on_dialog()
	{
		return m_on_dialog;
	}
	void			set_on_dialog(on_dialog* on_dialog)
	{
		m_on_dialog = on_dialog;
	}

	pcstr			error2string(long code);

	void			fail(pcstr e1, pcstr file, int line, pcstr unction, bool& ignore_always);
	void			fail(pcstr e1, const std::string& e2, pcstr file, int line, pcstr function, bool& ignore_always);
	void			fail(pcstr e1, pcstr e2, pcstr file, int line, pcstr function, bool& ignore_always);
	void			fail(pcstr e1, pcstr e2, pcstr e3, pcstr file, int line, pcstr function, bool& ignore_always);
	void			fail(pcstr e1, pcstr e2, pcstr e3, pcstr e4, pcstr file, int line, pcstr function, bool& ignore_always);
	void			error(long code, pcstr e1, pcstr file, int line, pcstr function, bool& ignore_always);
	void			error(long code, pcstr e1, pcstr e2, pcstr file, int line, pcstr function, bool& ignore_always);
	void _cdecl		fatal(pcstr file, int line, pcstr function, pcstr F, ...);
	void			backend(pcstr reason, pcstr expression, pcstr argument0, pcstr argument1, pcstr file, int line, pcstr function, bool& ignore_always);
	void			do_exit(const std::string& message);
};

// warning
// this function can be used for debug purposes only
IC	std::string __cdecl	make_string(pcstr format, ...)
{
	va_list		args;
	va_start(args, format);

	string4096		temp;
	vsprintf(temp, format, args);

	return		(temp);
}

extern XRCORE_API	xrDebug		Debug;

XRCORE_API void LogStackTrace(pcstr header);

#include "xrDebug_macros.h"

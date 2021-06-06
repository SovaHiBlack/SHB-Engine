#pragma once

#include "..\Core\Core.h"

#include <locale.h>

#pragma comment(lib, "Core.lib")

#if !defined(DEBUG) || defined(FORCE_NO_EXCEPTIONS)
	// release: no error checking, no exceptions
	#define LUABIND_NO_EXCEPTIONS
	#define BOOST_THROW_EXCEPTION_HPP_INCLUDED

	namespace std	{	class exception; }
//	namespace boost {	void  throw_exception(const std::exception &A);	};
#endif

#define LUABIND_DONT_COPY_STRINGS

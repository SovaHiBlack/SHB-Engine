////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_space.h
//	Description : Object factory space
////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef XRGAME_EXPORTS
#	define NO_XR_GAME
#endif

class CSE_Abstract;

namespace ObjectFactory
{

#ifndef NO_XR_GAME
	typedef DLL_Pure			CLIENT_BASE_CLASS;
#endif
	typedef CSE_Abstract		SERVER_BASE_CLASS;

#ifndef NO_XR_GAME
	typedef DLL_Pure			CLIENT_SCRIPT_BASE_CLASS;
#endif
	typedef CSE_Abstract		SERVER_SCRIPT_BASE_CLASS;
};

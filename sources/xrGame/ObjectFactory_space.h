//	Module 		: ObjectFactory_space.h
//	Description : Object factory space

#pragma once

class CSE_Abstract;

namespace ObjectFactory
{
	using CLIENT_BASE_CLASS			= DLL_Pure;
	using SERVER_BASE_CLASS			= CSE_Abstract;

	using CLIENT_SCRIPT_BASE_CLASS	= DLL_Pure;
	using SERVER_SCRIPT_BASE_CLASS	= CSE_Abstract;
};

//	Module 		: script_space_forward.h
//	Description : Script space forward declarations

#pragma once

namespace luabind
{
	class object;

	template<class T>
	class functor;

	template<class T>
	T object_cast(const object& obj);
};

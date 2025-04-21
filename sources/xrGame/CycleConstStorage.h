#pragma once

template<class T, s32 size>
class CCycleConstStorage
{
	T array[size];
	u16 first;
public:
	IC CCycleConstStorage( )
	{
		first = 0;
	}
	IC void fill_in(const T& val)
	{
		std::fill(array, array + size, val);
	}
	IC void push_back(T& val)
	{
		array[first] = val;
		first = (++first) % size;
	}
	IC T& operator [] (s32 i)
	{
		return array[(first + i) % size];
	}
};

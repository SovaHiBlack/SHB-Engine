#pragma once

template<class T,int size> 
class CCycleConstStorage
{
	T array[size];
	U16 first;

public:
	inline CCycleConstStorage()
	{
		first=0;
	}
	inline void fill_in(const T& val)
	{
		std::fill(array,array+size,val);
	}
	inline void push_back(T& val)
	{
		array[first]=val;
		first=(++first)%size;
	}
	inline T& operator [] (int i)
	{
		return array[(first+i)%size];
	}
};

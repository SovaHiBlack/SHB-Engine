#include "stdafx.h"

#ifdef BREAK_AT_STRCMP
s32					xr_strcmp			(pcstr S1, pcstr S2)
{
	s32 res								= (s32)strcmp(S1, S2);
	return res;
}
#endif

pstr				timestamp			(string64& dest)
{
	string64							temp;

	/* Set time zone from TZ environment variable. If TZ is not set,
	* the operating system is queried to obtain the default value
	* for the variable.
	*/
	_tzset								();
	u32									it;

	// date
	_strdate							(temp);
	for (it = 0; it < xr_strlen(temp); it++)
	{
		if ('/' == temp[it])
		{
			temp[it]					= '-';
		}
	}

	strconcat							(sizeof(dest), dest, temp, "_");

	// time
	_strtime							(temp);
	for (it = 0; it < xr_strlen(temp); it++)
	{
		if (':' == temp[it])
		{
			temp[it]					= '-';
		}
	}

	strcat								(dest, temp);
	return dest;
}

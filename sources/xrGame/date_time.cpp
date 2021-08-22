//	Module 		: date_time.h
//	Description : Date and time routines

#include "stdafx.h"

U64 generate_time(u32 years, u32 months, u32 days, u32 hours, u32 minutes, u32 seconds, u32 milliseconds)
{
	THROW(years > 0);
	THROW(months > 0);
	THROW(days > 0);
	U64						t1 = years / 400;
	U64						t2 = years / 100;
	U64						t3 = years / 4;
	bool					a1 = !(years % 400);
	bool					a2 = !(years % 100);
	bool					a3 = !(years % 4);
	U64						t4 = a3 && (!a2 || a1) ? 1 : 0;
	U64						result = U64(years - 1) * U64(365) + t1 - t2 + t3;

	if (months > 1) result += U64(31);
	if (months > 2) result += U64(28 + t4);
	if (months > 3) result += U64(31);
	if (months > 4) result += U64(30);
	if (months > 5) result += U64(31);
	if (months > 6) result += U64(30);
	if (months > 7) result += U64(31);
	if (months > 8) result += U64(31);
	if (months > 9) result += U64(30);
	if (months > 10) result += U64(31);
	if (months > 11) result += U64(30);
	result += U64(days - 1);
	result = result * U64(24) + U64(hours);
	result = result * U64(60) + U64(minutes);
	result = result * U64(60) + U64(seconds);
	result = result * U64(1000) + U64(milliseconds);

	return					(result);
}

void split_time(U64 time, u32& years, u32& months, u32& days, u32& hours, u32& minutes, u32& seconds, u32& milliseconds)
{
	milliseconds = u32(time % 1000);
	time /= 1000;
	seconds = u32(time % 60);
	time /= 60;
	minutes = u32(time % 60);
	time /= 60;
	hours = u32(time % 24);
	time /= 24;
	years = u32(U64(400) * time / U64(365 * 400 + 100 - 4 + 1) + 1);
	U64	t1 = years / 400;
	U64	t2 = years / 100;
	U64	t3 = years / 4;
	U64	t4;
	bool	a1 = !(years % 400);
	bool	a2 = !(years % 100);
	bool	a3 = !(years % 4);
	t4 = a3 && (!a2 || a1) ? 1 : 0;
	U64						result = U64(years - 1) * U64(365) + t1 - t2 + t3;
	time -= result;
	++time;
	months = 1;
	if (time > 31)
	{
		++months;
		time -= 31;
		if (time > 28 + t4)
		{
			++months;
			time -= 28 + t4;
			if (time > 31)
			{
				++months;
				time -= 31;
				if (time > 30)
				{
					++months;
					time -= 30;
					if (time > 31)
					{
						++months;
						time -= 31;
						if (time > 30)
						{
							++months;
							time -= 30;
							if (time > 31)
							{
								++months;
								time -= 31;
								if (time > 31)
								{
									++months;
									time -= 31;
									if (time > 30)
									{
										++months;
										time -= 30;
										if (time > 31)
										{
											++months;
											time -= 31;
											if (time > 30)
											{
												++months;
												time -= 30;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	days = u32(time);
}

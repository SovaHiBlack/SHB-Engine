//	Module 		: date_time.h
//	Description : Date and time routines

#pragma once

extern unsigned __int64	generate_time	(u32 years, u32 months, u32 days, u32 hours, u32 minutes, u32 seconds, u32 milliseconds = 0);
extern void	split_time		(unsigned __int64 time, u32 &years, u32 &months, u32 &days, u32 &hours, u32 &minutes, u32 &seconds, u32 &milliseconds);

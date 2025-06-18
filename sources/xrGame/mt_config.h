////////////////////////////////////////////////////////////////////////////
//	Module 		: mt_config.h
//	Description : Multithreading configuration options
////////////////////////////////////////////////////////////////////////////

#pragma once

extern flags32				g_mt_config;

#define mtLevelPath			(1<<0)
#define mtDetailPath		(1<<1)
#define mtObjectHandler		(1<<2)
#define mtSoundPlayer		(1<<3)
#define mtAiVision			(1<<4)
#define mtBullets			(1<<5)
#define mtLUA_GC			(1<<6)
#define mtLevelSounds		(1<<7)
#define mtALife				(1<<8)

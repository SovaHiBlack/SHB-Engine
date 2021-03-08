//	Module 		: stalker_animation_data_storage_inline.h
//	Description : Stalker animation data storage inline functions

#pragma once

inline	CStalkerAnimationDataStorage &stalker_animation_data_storage	()
{
	if (g_stalker_animation_data_storage)
		return							(*g_stalker_animation_data_storage);

	g_stalker_animation_data_storage	= xr_new<CStalkerAnimationDataStorage>();
	return								(*g_stalker_animation_data_storage);
}

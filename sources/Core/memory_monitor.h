#pragma once

#ifdef DEBUG
#	define USE_MEMORY_MONITOR
#endif // DEBUG

#ifdef USE_MEMORY_MONITOR

namespace memory_monitor
{
	CORE_API void flush_each_time(const bool& value);
	CORE_API void monitor_alloc(pcvoid pointer, const u32& size, pcstr description);
	CORE_API void monitor_free(pcvoid pointer);
}

#endif // USE_MEMORY_MONITOR

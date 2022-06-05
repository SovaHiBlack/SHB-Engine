#pragma once

#ifdef DEBUG
#	define USE_MEMORY_MONITOR
#endif // DEBUG

#ifdef USE_MEMORY_MONITOR

namespace memory_monitor {
	XRCORE_API void flush_each_time	(const bool &value);
	XRCORE_API void monitor_alloc	(const void *pointer, const u32 &size, pcstr description);
	XRCORE_API void	monitor_free	(const void *pointer);
}

#endif // USE_MEMORY_MONITOR

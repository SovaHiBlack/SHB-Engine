#pragma once

// Desc: Simple wrapper for critical section
class CORE_API		xrCriticalSection
{
private:
	pvoid pmutex;

public:
	xrCriticalSection();
	~xrCriticalSection();

	void				Enter();
	void				Leave();
	BOOL				TryEnter();
};

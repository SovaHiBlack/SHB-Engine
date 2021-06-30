#pragma once

// Desc: Simple wrapper for critical section
class CORE_API		xrCriticalSection
{
private:
	Pvoid pmutex;

public:
	xrCriticalSection( );
	~xrCriticalSection( );

	void				Enter( );
	void				Leave( );
	BOOL				TryEnter( );
};

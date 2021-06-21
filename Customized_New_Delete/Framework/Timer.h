//----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
//----------------------------------------------------------------------------
// Timer 
//    v.2.1.0
//    v.2.5
//    v.2.6   - Baseline
//    v.2.6.1 - fixed warning TestRegistry
//    v.2.7   - Baseline
//    v.2.8   - warning 5039 windows.h
//    v.2.9   - fence
//    v.2.10  - msc_ver 1914, win 10.0.17
//    v.2.11  - warning fixes 
//    v.2.12  - mutex on all trace::out
//    v.2.13  - fixed warnings on mutex and default copy const / assignment
//---------------------------------------------------------------------------- 

#ifndef TIMER_H
#define TIMER_H

// Windows.h include
// many warnings - need to wrap for Wall warnings
#pragma warning( push )
	#pragma warning( disable : 4820 )
	#pragma warning( disable : 4668 )
	#pragma warning( disable : 5039 )
	#include <Windows.h>
#pragma warning( pop ) 

class Timer
{
public:
	// big four
	Timer();
	Timer(const Timer &) = delete;
	Timer & operator= (const Timer &) = delete;
	~Timer() = default;

	// methods
	void Tic();
	void Toc();
	void Reset();
	float TimeInSeconds();

private:	
	// methods
	LARGE_INTEGER	privGetTimer();
	void			privInitTimer();

	// data
	LARGE_INTEGER	ticTime;
	LARGE_INTEGER	tocTime;
	LARGE_INTEGER	deltaTime;
	float			SecondsPerCycle;
	float			timeSeconds;

};

#endif TIMER_H

// ---  End of File ---------------

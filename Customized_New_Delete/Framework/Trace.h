//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
//----------------------------------------------------------------------------- 
// Trace 
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
//----------------------------------------------------------------------------- 

#ifndef DEBUG_OUTPUT_H
#define DEBUG_OUTPUT_H

#include <assert.h>
#include <stdio.h>

// Windows.h include
// many warnings - need to wrap for Wall warnings
#pragma warning( push )
	#pragma warning( disable : 4820 )
	#pragma warning( disable : 4668 )
	#pragma warning( disable : 5039 )
	#include <Windows.h>
#pragma warning( pop ) 

#pragma warning( push )
	#pragma warning( disable : 4365 )
	#pragma warning( disable : 4623 )
	#pragma warning( disable : 4625 )
	#pragma warning( disable : 4626 )
	#pragma warning( disable : 4820 )
	#pragma warning( disable : 5026 )
	#pragma warning( disable : 5027 )
	#pragma warning( disable : 5039 )
	#include <mutex>
#pragma warning( pop ) 

// NOTE: you need to set your project settings
//       Character Set -> Use Multi-Byte Character Set

#define TraceBuffSize 256

// Singleton helper class
class Trace
{
public:
	// displays a printf to the output window
	static void out(const char* fmt, ...)
	{
		Trace *pTrace = Trace::privGetInstance();
		assert(pTrace);

		pTrace->mtx.lock();

			va_list args;
			va_start(args, fmt);

			vsprintf_s(pTrace->privBuff, TraceBuffSize, fmt, args);
			OutputDebugString(pTrace->privBuff);

			va_end(args);

		pTrace->mtx.unlock();
	}

	// Big four
	Trace() = default;
	Trace(const Trace &) = delete;
	Trace & operator = (const Trace &) = delete;
	~Trace() = default;

private:
	static Trace *privGetInstance()
	{
		// This is where its actually stored (BSS section)
		static Trace helper;
		return &helper;
	}
	char privBuff[TraceBuffSize];
	std::mutex mtx;
};

#endif

// ---  End of File ---------------

//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef HEAPHDR_H
#define HEAPHDR_H

#include "Types.h"

#include "UsedHdr.h"
#include "FreeHdr.h"

class Heap
{
public:
	struct Stats
	{
		void *heapTopAddr;            // start address available heap
		void *heapBottomAddr;         // bottom of address of heap

		Type::U32 currNumUsedBlocks; // number of current used allocations
		Type::U32 currUsedMem;       // current size of the total used memory

		Type::U32 peakNumUsed;       // number of peak used allocations
		Type::U32 peakUsedMemory;    // peak size of used memory

		Type::U32 currNumFreeBlocks; // number of current free blocks
		Type::U32 currFreeMem;       // current size of the total free memory

		Type::U32 sizeHeap;          // size of Heap total space, including header
	};

public:
	// Make sure that the Heap is 16 byte aligned.

	// allocation links
	UsedHdr		*pUsedHead;
	FreeHdr		*pFreeHead;

	// Next fit allocation strategy
	FreeHdr		*pNextFit;	

	// stats
	Stats		mStats;

	// specialize constructor
	Heap(void * ptr);
};

#endif 

// ---  End of File ---------------

//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef MEM_H
#define MEM_H

#include "Heap.h"

class Mem
{
public:
	static const unsigned int HEAP_SIZE = (50 * 1024);

public:
	Mem();	
	Mem(const Mem &) = delete;
	Mem & operator = (const Mem &) = delete;
	~Mem();

	Heap *GetHeap();
	void Dump();

	// implement these functions
	void Free( void * const data );
	void *Malloc( const Type::U32 size );
	void Initialize( );

private:

	
	//Helper functions
	void adjust_Used_Head_Malloc(void * const data);
	void adjust_Used_Head_Free(void * const data);

	//coalesing
	void no_coalesing(FreeHdr* pFreeHdr_Allocation);
	FreeHdr* below_Coalesing(FreeHdr* pFreeHdr_Allocation, Type::U32 totalSize, FreeHdr* pFreeHdr_Checking);
	FreeHdr* above_Coalesing(FreeHdr* pFreeHdr_Allocation, bool a);


	
private:
	Heap	*pHeap;
	void	*pRawMem;

};

#endif 

// ---  End of File ---------------

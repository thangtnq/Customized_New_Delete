//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef FREEHDR_H
#define FREEHDR_H

#include "Types.h"
#include "BlockType.h"

class UsedHdr;

class FreeHdr
{
public:
	FreeHdr     *pFreeNext;       // next free block
	FreeHdr     *pFreePrev;       // prev free block
	Type::U32   mBlockSize;       // size of block
	BlockType	mBlockType;       // block type 
	Type::Bool	mAboveBlockFree;  // AboveBlock flag
								  //    if(AboveBlock is type free) -> true 
								  //    if(AboveBlock is type used) -> false
	Type::U8	pad1;             // future use
	Type::U8	pad2;             // future use

	FreeHdr(const Type::U32 size);
};

#endif 

// ---  End of File ---------------

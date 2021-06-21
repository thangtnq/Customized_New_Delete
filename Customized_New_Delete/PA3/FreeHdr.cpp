//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "UsedHdr.h"
#include "FreeHdr.h"
#include "BlockType.h"

FreeHdr::FreeHdr(const Type::U32 size) :
	pFreeNext(nullptr),pFreePrev(nullptr),
	mBlockSize(size), mBlockType(BlockType::FREE), mAboveBlockFree(false),
	pad1(0),pad2(0)
{

}

// ---  End of File ---------------

//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "FreeHdr.h"
#include "UsedHdr.h"

UsedHdr::UsedHdr(const Type::U32 size) :
	pUsedNext(nullptr),pUsedPrev(nullptr),
	mBlockSize(size),mBlockType(BlockType::USED),mAboveBlockFree(FALSE),
	pad0(0),pad1(0)
{

}

// ---  End of File ---------------

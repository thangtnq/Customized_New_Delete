//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef BLOCK_TYPE_H
#define BLOCK_TYPE_H

#include "Types.h"

enum class BlockType : Type::U8
{
	FREE = 0xCC,
	USED = 0xAA
};

#endif

// ---  End of File ---------------

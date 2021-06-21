//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h) 
#define UNUSED_VAR(p) ((void *)p)

TEST(Allocate_Everything, MemoryShakeOut)
{
	FileIO::Open("Student_", "Test_02");

	fprintf(FileIO::GetHandle(), "Test2: \n\n");

	fprintf(FileIO::GetHandle(), "1) allocate the biggest block               \n");

	fprintf(FileIO::GetHandle(), "     Mem mem;                         \n");
	fprintf(FileIO::GetHandle(), "     mem.Initialize();                \n");
	fprintf(FileIO::GetHandle(), "     void *p = mem.Malloc( 0xc7c0 );        \n");
	fprintf(FileIO::GetHandle(), "     mem.Dump();                            \n");

	Mem mem;
	mem.Initialize();
	void *p = mem.Malloc(0xc7c0);

	mem.Dump();

	FileIO::Close();


	// ----  GENERAL CHECK ------------------------------------------------------

		Heap *h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL(h->pFreeHead, 0);
		CHECK_EQUAL(h->pNextFit, 0);
		CHECK_EQUAL(PTR_FIX(h->pUsedHead), 0x30);

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 1);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0xc7c0);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 1);
		CHECK_EQUAL(h->mStats.currUsedMem, 0xc7c0);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 0);
		CHECK_EQUAL(h->mStats.currFreeMem, 0x0);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);

	// ---- Heap Walk ------------------------------------------------------

		// ---- Heap Hdr -------------------------------------------------------

			// HeapHdr Start
			CHECK_EQUAL(PTR_FIX((Type::U32)h->mStats.heapTopAddr - sizeof(Heap)), 0);
			// HeapHdr End
			CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap));

		// ----  Block walk ------------------------------------------------------

			Type::U32 hdrStart;
			Type::U32 hdrEnd;
			Type::U32 blkStart;
			Type::U32 blkEnd;

		// ---- HDR -------------------------------------------

			// Check type
			UsedHdr *used = (UsedHdr *)h->mStats.heapTopAddr;
			// Should be USED
			CHECK_EQUAL(used->mBlockType, BlockType::USED);
			// Above FreeHdr
			CHECK_EQUAL(used->mAboveBlockFree, false);

			hdrStart = (Type::U32)used;
			hdrEnd = (Type::U32)used + sizeof(UsedHdr);

			// Hdr Start
			CHECK_EQUAL(PTR_FIX(hdrStart), 0x30);
			// Hdr End
			CHECK_EQUAL(PTR_FIX(hdrEnd), 0x40);
			// Prev
			CHECK_EQUAL(used->pUsedPrev, 0);
			// Next
			CHECK_EQUAL(used->pUsedNext, 0);
			// Hdr Size
			CHECK_EQUAL(hdrEnd - hdrStart, sizeof(UsedHdr));

		// ---- BLOCK ----------------------------------------------------

			blkStart = hdrEnd;
			blkEnd = blkStart + used->mBlockSize;

			CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
			CHECK_EQUAL(PTR_FIX(p), PTR_FIX(hdrEnd));
			CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
			CHECK_EQUAL(used->mBlockSize, 0xc7c0);

}

// ---  End of File ---------------

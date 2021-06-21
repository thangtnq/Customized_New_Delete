//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h) 

TEST( combo_1 , MemoryShakeOut )
{
	FileIO::Open("Student_", "Test_14");

	fprintf(FileIO::GetHandle(),"Test14: \n\n");

	fprintf(FileIO::GetHandle(), "      Mem mem1;                           \n");
	fprintf(FileIO::GetHandle(), "      mem1.Initialize();                  \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      void *a1 = mem1.Malloc( 0x200 );    \n");
	fprintf(FileIO::GetHandle(), "      void *b1 = mem1.Malloc( 0x200 );    \n");
	fprintf(FileIO::GetHandle(), "      void *c1 = mem1.Malloc( 0x300 );    \n");
	fprintf(FileIO::GetHandle(), "      void *d1 = mem1.Malloc( 0x300 );    \n");
	fprintf(FileIO::GetHandle(), "      void *e1 = mem1.Malloc( 0x300 );    \n");
	fprintf(FileIO::GetHandle(), "      void *f1 = mem1.Malloc( 0x400 );    \n");
	fprintf(FileIO::GetHandle(), "      void *g1 = mem1.Malloc( 0x300 );    \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      mem1.Dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      // Part B:                          \n");
	fprintf(FileIO::GetHandle(), "      mem1.Free( a1 );                    \n");
	fprintf(FileIO::GetHandle(), "      mem1.Free( c1 );                    \n");
	fprintf(FileIO::GetHandle(), "      mem1.Free( f1 );                    \n");
	fprintf(FileIO::GetHandle(), "      mem1.Dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      // Part C:                          \n");
	fprintf(FileIO::GetHandle(), "      void *r1 = mem1.Malloc( 0x180 );    \n");
	fprintf(FileIO::GetHandle(), "      mem1.Dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      void *s1 = mem1.Malloc(0xb100);     \n");
	fprintf(FileIO::GetHandle(), "      mem1.Dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      void *t1 = mem1.Malloc(0x220);      \n");
	fprintf(FileIO::GetHandle(), "      mem1.Dump();                        \n");
	fprintf(FileIO::GetHandle(), "                                          \n");
	fprintf(FileIO::GetHandle(), "      mem1.Free(s1);                      \n");
	fprintf(FileIO::GetHandle(), "      mem1.Dump();                        \n");


	// Part A:
	Mem mem1;
	mem1.Initialize();

   	void *a1 = mem1.Malloc( 0x200 );
   	void *b1 = mem1.Malloc( 0x200 );
	void *c1 = mem1.Malloc( 0x300 );
	void *d1 = mem1.Malloc( 0x300 );
	void *e1 = mem1.Malloc( 0x300 );
	void *f1 = mem1.Malloc( 0x400 );
	void *g1 = mem1.Malloc( 0x300 );

	mem1.Dump();

	// Part B:
	mem1.Free( a1 );
	mem1.Free( c1 );
	mem1.Free( f1 );
	mem1.Dump();

	// Part C:
	void *r1 = mem1.Malloc( 0x180 );
	mem1.Dump();

	// Part D:
	void *s1 = mem1.Malloc(0xb100);
	mem1.Dump();

	// Part E:
	void *t1 = mem1.Malloc(0x220);
	mem1.Dump();

	// Part F:
	mem1.Free(s1);
	mem1.Dump();

	// to shut up warnings
	r1 = 0;
	t1 = 0;
	g1 = 0;
	e1 = 0;
	d1 = 0;
	b1 = 0;

	FileIO::Close();

	// ----  GENERAL CHECK ------------------------------------------------------

	// ---- Part A: -------------------------------------------------------------
	
	Mem mem;
	mem.Initialize();

   	void *a = mem.Malloc( 0x200 );
   	void *b = mem.Malloc( 0x200 );
	void *c = mem.Malloc( 0x300 );
	void *d = mem.Malloc( 0x300 );
	void *e = mem.Malloc( 0x300 );
	void *f = mem.Malloc( 0x400 );
	void *g = mem.Malloc( 0x300 );

	// ---- Verify A: -----------------------------------------------------------

		Heap *h = mem.GetHeap();

		CHECK_EQUAL( PTR_FIX(a), 0x40 );
		CHECK_EQUAL( PTR_FIX(b), 0x250 ); 
		CHECK_EQUAL( PTR_FIX(c), 0x460 );
		CHECK_EQUAL( PTR_FIX(d), 0x770 );
		CHECK_EQUAL( PTR_FIX(e), 0xa80 );
		CHECK_EQUAL( PTR_FIX(f), 0xd90 );
		CHECK_EQUAL( PTR_FIX(g), 0x11a0 );

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x14a0 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x1190 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 7 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x1400 );

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 7 );
		CHECK_EQUAL( h->mStats.currUsedMem, 0x1400 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xb350 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );

	// ----  Block walk ------------------------------------------------------
		
		Type::U32 hdrStart;
		Type::U32 hdrEnd;
		Type::U32 blkStart;
		Type::U32 blkEnd; 
		Type::U32 secret;

	// ---- USED HDR 1 -------------------------------------------

		// Check type
		UsedHdr *used = (UsedHdr *) h->mStats.heapTopAddr;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x240);
		// Next
		CHECK_EQUAL( used->pUsedNext, 0 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x240);
		CHECK_EQUAL( used->mBlockSize, 0x200 );
	
	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x240 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x250);
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x450 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x30 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x450);
		CHECK_EQUAL( used->mBlockSize, 0x200 );

	// ---- HDR 3 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x450 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x460 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x760 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x240 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x760);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x770 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0xa70 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x450);
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xa70);
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 5 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xa70);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xa80 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0xd80 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x760);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xd80);
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 6 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xd80);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xd90 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x1190 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0xa70);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL( used->mBlockSize, 0x400 );

	// ---- HDR 7 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x1190);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x11a0 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0xd80);
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x14a0);
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 8 -------------------------------------------

		// Check type
		FreeHdr *free= (FreeHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x14b0 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0);
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( free->mBlockSize, 0xb350 );

		secret = *((Type::U32 *)(blkEnd - 4));

		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));

	// ---- Part B: -------------------------------------------------------------
	
	mem.Free( a );
	mem.Free( c );
	mem.Free( f );
	
	// ---- Verify B: -----------------------------------------------------------

		h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x1190 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 7 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x1400);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 4);
		CHECK_EQUAL( h->mStats.currUsedMem, 0xb00 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 4 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xbc50 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


	// ---- HDR 1 -------------------------------------------

		// Check type
		free = (FreeHdr *) h->mStats.heapTopAddr;
		CHECK_EQUAL( free->mBlockType, BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x450 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(FreeHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x240 );
		CHECK_EQUAL( free->mBlockSize, 0x200 );
	
	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x240 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x250 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x760 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x450 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );

	// ---- Hdr 3 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mBlockType, BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x450 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x460 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x30 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0xd80 );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL( free->mBlockSize, 0x300 );
	
	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x760 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x770 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0xa70 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xa70 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );
		
	// ---- HDR 5 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xa70 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xa80 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x1190 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x760 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xd80 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- Hdr 6 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mBlockType, BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xd80 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xd90 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x450);
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x14a0 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL( free->mBlockSize, 0x400 );
	
	// ---- HDR 7 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x1190 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x11a0 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0xa70 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x14a0 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- Hdr 8 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mBlockType, BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x14a0 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x14b0 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( free->mBlockSize, 0xb350 );

		secret = *((Type::U32 *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));

	// ---- Part C: -------------------------------------------------------------
	
	void *r = mem.Malloc( 0x180 );
	
	// ---- Verify C: -----------------------------------------------------------

		h = mem.GetHeap();

		CHECK_EQUAL( PTR_FIX(r), 0x14b0);

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x14a0 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 7 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x1400);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 5);
		CHECK_EQUAL( h->mStats.currUsedMem, 0xc80 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 4 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xbac0 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x30 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


	// ---- HDR 1 -------------------------------------------

		// Check type
		free = (FreeHdr *) h->mStats.heapTopAddr;
		CHECK_EQUAL( free->mBlockType, BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x30  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x40 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x450 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(FreeHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x240 );
		CHECK_EQUAL( free->mBlockSize, 0x200 );
	
	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x240 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x250 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x760 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x450 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );

	// ---- Hdr 3 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mBlockType, BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x450 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x460 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x30 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0xd80);
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL( free->mBlockSize, 0x300 );
	
	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x760 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x770 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0xa70 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xa70 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );
		
	// ---- HDR 5 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xa70 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xa80 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x1190 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x760 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xd80 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- Hdr 6 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mBlockType, BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0xd80 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0xd90);
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x450);
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x1630 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL( free->mBlockSize, 0x400);
	
	// ---- HDR 7 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( used->mBlockType, BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x1190 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x11a0 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x14a0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext),0xa70);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x14a0 );
		CHECK_EQUAL( used->mBlockSize, 0x300 );

	// ---- HDR 8 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x14b0);
		// Prev
		CHECK_EQUAL(used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1190);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1630);
		CHECK_EQUAL(used->mBlockSize, 0x180);

	// ---- Hdr 9 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( free->mBlockType, BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x1630 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x1640 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0 );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( free->mBlockSize, 0xb1c0 );

		// ---- Part D: -------------------------------------------------------------

		void *s = mem.Malloc(0xb100);

		// ---- Verify D: -----------------------------------------------------------

		h = mem.GetHeap();

		CHECK_EQUAL(PTR_FIX(s), 0x1640);

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL(PTR_FIX(h->pFreeHead), 0x30);
		CHECK_EQUAL(PTR_FIX(h->pUsedHead), 0x1630);

		// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 7);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0xbd80);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 6);
		CHECK_EQUAL(h->mStats.currUsedMem, 0xbd80);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 4);
		CHECK_EQUAL(h->mStats.currFreeMem, 0x9b0);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);

		// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL(PTR_FIX((Type::U32)h->mStats.heapTopAddr - sizeof(Heap)), 0);
		// HeapHdr End
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


		// ---- HDR 1 -------------------------------------------

		// Check type
		free = (FreeHdr *)h->mStats.heapTopAddr;
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x30);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x40);
		// Prev
		CHECK_EQUAL(free->pFreePrev, 0x0);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0x450);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(FreeHdr));

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x240);
		CHECK_EQUAL(free->mBlockSize, 0x200);

		// ---- HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x240);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x250);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x760);
		// Next
		CHECK_EQUAL(used->pUsedNext, 0x0);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(UsedHdr));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x450);
		CHECK_EQUAL(used->mBlockSize, 0x200);

		// ---- Hdr 3 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x450);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x460);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x30);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xd80);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL(free->mBlockSize, 0x300);

		// ---- HDR 4 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x760);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x770);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0xa70);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xa70);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 5 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xa70);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xa80);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1190);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x760);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xd80);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- Hdr 6 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xd80);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xd90);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x450);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xc740);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL(free->mBlockSize, 0x400);

		// ---- HDR 7 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1190);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x11a0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x14a0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0xa70);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x14a0);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 8 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x14b0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1630);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1190);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1630);
		CHECK_EQUAL(used->mBlockSize, 0x180);

		// ---- HDR 9 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1630);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x1640);
		// Prev
		CHECK_EQUAL(used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x14a0);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc740);
		CHECK_EQUAL(used->mBlockSize, 0xb100);

		// ---- Hdr 10 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xc740);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xc750);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL(free->pFreeNext, 0x0);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL(free->mBlockSize, 0xb0);

		// ---- Part E: -------------------------------------------------------------

		void *t = mem.Malloc(0x220);

		// ---- Verify E: -----------------------------------------------------------

		h = mem.GetHeap();

		CHECK_EQUAL(PTR_FIX(t), 0x460);

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL(PTR_FIX(h->pFreeHead), 0x30);
		CHECK_EQUAL(PTR_FIX(h->pUsedHead), 0x450);

		// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 7);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0xbfa0);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 7);
		CHECK_EQUAL(h->mStats.currUsedMem, 0xbfa0);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 4);
		CHECK_EQUAL(h->mStats.currFreeMem, 0x780);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);

		// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL(PTR_FIX((Type::U32)h->mStats.heapTopAddr - sizeof(Heap)), 0);
		// HeapHdr End
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


		// ---- HDR 1 -------------------------------------------

		// Check type
		free = (FreeHdr *)h->mStats.heapTopAddr;
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x30);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x40);
		// Prev
		CHECK_EQUAL(free->pFreePrev, 0x0);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0x680);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(FreeHdr));

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x240);
		CHECK_EQUAL(free->mBlockSize, 0x200);

		// ---- HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x240);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x250);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x760);
		// Next
		CHECK_EQUAL(used->pUsedNext, 0x0);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(UsedHdr));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x450);
		CHECK_EQUAL(used->mBlockSize, 0x200);

		// ---- HDR 3 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x450);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x460);
		// Prev
		CHECK_EQUAL(used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1630);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(UsedHdr));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x680);
		CHECK_EQUAL(used->mBlockSize, 0x220);

		// ---- Hdr 4 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x680);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x690);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x30);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xd80);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL(free->mBlockSize, 0xd0);

		// ---- HDR 5 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x760);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x770);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0xa70);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xa70);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 6 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xa70);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xa80);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1190);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x760);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xd80);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- Hdr 7 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xd80);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xd90);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x680);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xc740);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL(free->mBlockSize, 0x400);

		// ---- HDR 8 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1190);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x11a0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x14a0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0xa70);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x14a0);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 9 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x14b0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1630);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1190);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1630);
		CHECK_EQUAL(used->mBlockSize, 0x180);

		// ---- HDR 10 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1630);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x1640);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x450);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x14a0);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc740);
		CHECK_EQUAL(used->mBlockSize, 0xb100);

		// ---- Hdr 11 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xc740);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xc750);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL(free->pFreeNext, 0x0);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL(free->mBlockSize, 0xb0);

		// ---- Part F: -------------------------------------------------------------

		mem.Free(s);

		// ---- Verify F: -----------------------------------------------------------

		h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL(PTR_FIX(h->pFreeHead), 0x30);
		CHECK_EQUAL(PTR_FIX(h->pUsedHead), 0x450);

		// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 7);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0xbfa0);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 6);
		CHECK_EQUAL(h->mStats.currUsedMem, 0xea0);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 4);
		CHECK_EQUAL(h->mStats.currFreeMem, 0xb890);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x30);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);

		// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL(PTR_FIX((Type::U32)h->mStats.heapTopAddr - sizeof(Heap)), 0);
		// HeapHdr End
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


		// ---- HDR 1 -------------------------------------------

		// Check type
		free = (FreeHdr *)h->mStats.heapTopAddr;
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x30);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x40);
		// Prev
		CHECK_EQUAL(free->pFreePrev, 0x0);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0x680);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(FreeHdr));

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x240);
		CHECK_EQUAL(free->mBlockSize, 0x200);

		// ---- HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x240);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x250);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x760);
		// Next
		CHECK_EQUAL(used->pUsedNext, 0x0);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(UsedHdr));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x450);
		CHECK_EQUAL(used->mBlockSize, 0x200);

		// ---- HDR 3 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x450);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x460);
		// Prev
		CHECK_EQUAL(used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x14a0);
		// Hdr Size
		CHECK_EQUAL(hdrEnd - hdrStart, sizeof(UsedHdr));

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x680);
		CHECK_EQUAL(used->mBlockSize, 0x220);

		// ---- Hdr 4 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x680);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x690);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x30);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0xd80);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x760);
		CHECK_EQUAL(free->mBlockSize, 0xd0);

		// ---- HDR 5 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x760);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x770);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0xa70);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x240);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xa70);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 6 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xa70);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xa80);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x1190);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x760);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xd80);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- Hdr 7 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0xd80);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0xd90);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x680);
		// Next
		CHECK_EQUAL(PTR_FIX(free->pFreeNext), 0x1630);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1190);
		CHECK_EQUAL(free->mBlockSize, 0x400);

		// ---- HDR 8 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1190);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x11a0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x14a0);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0xa70);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x14a0);
		CHECK_EQUAL(used->mBlockSize, 0x300);

		// ---- HDR 9 -------------------------------------------

		// Check type
		used = (UsedHdr *)blkEnd;
		// Should be USED
		CHECK_EQUAL(used->mBlockType, BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);

		hdrStart = (Type::U32)used;
		hdrEnd = (Type::U32)used + sizeof(UsedHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x14a0);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x14b0);
		// Prev
		CHECK_EQUAL(PTR_FIX(used->pUsedPrev), 0x450);
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x1190);

		blkStart = hdrEnd;
		blkEnd = blkStart + used->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0x1630);
		CHECK_EQUAL(used->mBlockSize, 0x180);

		// ---- Hdr 10 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL(free->mBlockType, BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x1630);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x1640);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0xd80);
		// Next
		CHECK_EQUAL(free->pFreeNext, 0x0);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL(free->mBlockSize, 0xb1c0);

}

// ---  End of File ---------------

//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include <malloc.h>
#include <new>

#include "Framework.h"

#include "Mem.h"
#include "Heap.h"
#include "BlockType.h"

#define STUB_PLEASE_REPLACE(x) (x)

#define HEAP_ALIGNMENT			16
#define HEAP_ALIGNMENT_MASK		(HEAP_ALIGNMENT-1)

#define ALLOCATION_ALIGNMENT		16
#define ALLOCATION_ALIGNMENT_MASK	(ALLOCATION_ALIGNMENT-1)

#define UNUSED_VAR(v)  ((void *)v)

#ifdef _DEBUG
	#define HEAP_HEADER_GUARDS  16
	#define HEAP_SET_GUARDS  	Type::U32 *pE = (Type::U32 *)((Type::U32)pRawMem + HEAP_SIZE); \
								*pE++ = 0xEEEEEEEE;*pE++ = 0xEEEEEEEE;*pE++ = 0xEEEEEEEE;*pE++ = 0xEEEEEEEE;
	#define HEAP_TEST_GUARDS	Type::U32 *pE = (Type::U32 *)((Type::U32)pRawMem + HEAP_SIZE); \
								assert(*pE++ == 0xEEEEEEEE);assert(*pE++ == 0xEEEEEEEE); \
								assert(*pE++ == 0xEEEEEEEE);assert(*pE++ == 0xEEEEEEEE);  
#else
	#define HEAP_HEADER_GUARDS  0
	#define HEAP_SET_GUARDS  	
	#define HEAP_TEST_GUARDS			 
#endif
							

// To help with coalescing... not required
struct SecretPtr
{
	FreeHdr *free;
};


Mem::~Mem()
{
	HEAP_TEST_GUARDS
	_aligned_free(this->pRawMem);
}


Heap *Mem::GetHeap()
{
	return this->pHeap;
}

Mem::Mem()
{
	// now initialize it.
	this->pHeap = 0;
	this->pRawMem = 0;

	// Do a land grab --- get the space for the whole heap
	// Since OS have different alignments... I forced it to 16 byte aligned
	pRawMem = _aligned_malloc(HEAP_SIZE + HEAP_HEADER_GUARDS, HEAP_ALIGNMENT);
	HEAP_SET_GUARDS

	// verify alloc worked
	assert(pRawMem != 0);

	// Guarantee alignemnt
	assert( ((Type::U32)pRawMem & HEAP_ALIGNMENT_MASK) == 0x0 ); 

	// instantiate the heap header on the raw memory
	Heap *p = placement_new(pRawMem, Heap, pRawMem);

	// update it
	this->pHeap = p;
}

//Helper function implementation starts:

void Mem::adjust_Used_Head_Malloc(void * const data) {
	UsedHdr* pUsedHdr_Header_Start = (UsedHdr*)data;
	if (this->pHeap->pUsedHead == nullptr) {
		this->pHeap->pUsedHead = pUsedHdr_Header_Start;
	}
	else {
		UsedHdr* temp = this->pHeap->pUsedHead;
		this->pHeap->pUsedHead = pUsedHdr_Header_Start;
		pUsedHdr_Header_Start->pUsedNext = temp;
		pUsedHdr_Header_Start->pUsedPrev = nullptr;
		temp->pUsedPrev = pUsedHdr_Header_Start;
	}
}


void Mem::adjust_Used_Head_Free(void* const data) {

	UsedHdr* pUsedHdr_Header = (UsedHdr*)data;
	if (this->pHeap->pUsedHead != pUsedHdr_Header) {
		if (pUsedHdr_Header->pUsedNext != nullptr) {
			pUsedHdr_Header->pUsedNext->pUsedPrev = pUsedHdr_Header->pUsedPrev;
		}
		if (pUsedHdr_Header->pUsedPrev != nullptr) {
			pUsedHdr_Header->pUsedPrev->pUsedNext = pUsedHdr_Header->pUsedNext;
		}
	}
	else {
		if (this->pHeap->pUsedHead->pUsedNext == nullptr) {
			this->pHeap->pUsedHead = nullptr;
		}
		else {
			UsedHdr* temp = pUsedHdr_Header->pUsedNext;
			this->pHeap->pUsedHead = temp;
			this->pHeap->pUsedHead->pUsedPrev = nullptr;
		}
	}
}

void Mem::no_coalesing(FreeHdr* pFreeHdr_Allocation) {

	if (this->pHeap->pFreeHead == nullptr) {
		this->pHeap->pFreeHead = pFreeHdr_Allocation;
		this->pHeap->pNextFit = pFreeHdr_Allocation;
		pFreeHdr_Allocation = nullptr;
	}
	else {
		FreeHdr* current = this->pHeap->pFreeHead;
		while (current != nullptr) {
			if ((Type::U32)current > (Type::U32)pFreeHdr_Allocation) {
				if (current->pFreePrev != nullptr) {
					current->pFreePrev->pFreeNext = pFreeHdr_Allocation;
					pFreeHdr_Allocation->pFreePrev = current->pFreePrev;
					current->pFreePrev = pFreeHdr_Allocation;
					pFreeHdr_Allocation->pFreeNext = current;
				}
				else {
					this->pHeap->pFreeHead = pFreeHdr_Allocation;
					pFreeHdr_Allocation->pFreeNext = current;
					pFreeHdr_Allocation->pFreePrev = nullptr;
					current->pFreePrev = pFreeHdr_Allocation;
				}
				break;
			}
			current = current->pFreeNext;
		}
	}
}

FreeHdr* Mem::below_Coalesing(FreeHdr* pFreeHdr_Allocation, Type::U32 totalSize, FreeHdr* checking) {

	FreeHdr* temp = pFreeHdr_Allocation;
	FreeHdr* pFree_Next = checking->pFreeNext;
	FreeHdr* pFree_Prev = checking->pFreePrev;

	pFreeHdr_Allocation = placement_new(temp, FreeHdr, totalSize);
	if (this->pHeap->pNextFit == checking) {
		this->pHeap->pNextFit = pFreeHdr_Allocation;
	}

	this->pHeap->mStats.currFreeMem += Type::U32(sizeof(FreeHdr));
	this->pHeap->mStats.currNumFreeBlocks--;

	pFreeHdr_Allocation->pFreeNext = pFree_Next;
	pFreeHdr_Allocation->pFreePrev = pFree_Prev;

	if (pFreeHdr_Allocation->pFreeNext != nullptr) {
		pFreeHdr_Allocation->pFreeNext->pFreePrev = pFreeHdr_Allocation;
	}

	if (pFreeHdr_Allocation->pFreePrev != nullptr) {
		pFreeHdr_Allocation->pFreePrev->pFreeNext = pFreeHdr_Allocation;
	}
	else {
		this->pHeap->pFreeHead = pFreeHdr_Allocation;
	}
	return pFreeHdr_Allocation;
}

FreeHdr* Mem::above_Coalesing(FreeHdr* pFreeHdr_Allocation, bool isCoalesBelow) {
	Type::U32* secretPointer = (Type::U32*)((Type::U32)pFreeHdr_Allocation - sizeof(Type::U32));
	FreeHdr* aboveFree = (FreeHdr*)*secretPointer;
	Type::U32 totalSize = (Type::U32)pFreeHdr_Allocation - (Type::U32)aboveFree + pFreeHdr_Allocation->mBlockSize;
	

	this->pHeap->mStats.currFreeMem += sizeof(FreeHdr);
	this->pHeap->mStats.currNumFreeBlocks--;

	if (this->pHeap->pNextFit == pFreeHdr_Allocation) {
		this->pHeap->pNextFit = aboveFree;
	}
	pFreeHdr_Allocation->pFreeNext = nullptr;
	pFreeHdr_Allocation->pFreePrev = nullptr;

	FreeHdr* pFree_Next = nullptr;
	FreeHdr* pFree_Prev = aboveFree->pFreePrev;

	if (isCoalesBelow) {
		pFree_Next = pFreeHdr_Allocation->pFreeNext;
	}
	else {
		pFree_Next = aboveFree->pFreeNext;
	}

	pFreeHdr_Allocation = placement_new(aboveFree, FreeHdr, totalSize);
	pFreeHdr_Allocation->pFreeNext = pFree_Next;
	pFreeHdr_Allocation->pFreePrev = pFree_Prev;

	if (pFreeHdr_Allocation->pFreeNext != nullptr) {
		pFreeHdr_Allocation->pFreeNext->pFreePrev = pFreeHdr_Allocation;
	}

	if (pFreeHdr_Allocation->pFreePrev != nullptr) {
		pFreeHdr_Allocation->pFreePrev->pFreeNext = pFreeHdr_Allocation;
	}
	else {
		this->pHeap->pFreeHead = pFreeHdr_Allocation;
	}
	return pFreeHdr_Allocation;
}

//Helper function ends; 


void Mem::Initialize()
{
	// Add magic here
	Heap* pHeap_Start = this->GetHeap();
	FreeHdr* pFreeHdr_Header_Start = (FreeHdr*)(pHeap_Start + 1);
	FreeHdr* pFreeHdr_Mem_Start = pFreeHdr_Header_Start + 1;
	Type::U32 size_Mem = (Type::U32)this->pHeap->mStats.heapBottomAddr - (Type::U32)pFreeHdr_Mem_Start;
	FreeHdr* pFreeHdr_Allocation = placement_new(pFreeHdr_Header_Start, FreeHdr, size_Mem);

	this->pHeap->pUsedHead = nullptr;
	this->pHeap->pFreeHead = pFreeHdr_Allocation;
	this->pHeap->pNextFit  = pFreeHdr_Allocation;
	this->pHeap->mStats.currFreeMem += size_Mem;
	this->pHeap->mStats.currNumFreeBlocks++;
	
}


void *Mem::Malloc( const Type::U32 size )
{
	FreeHdr *start = this->pHeap->pNextFit;
	FreeHdr *none_Fit = this->pHeap->pNextFit;
	while (start != nullptr) {

		Type::U32 available_Size = start->mBlockSize;
		if (available_Size == size) {

			FreeHdr *pFree_Next = start->pFreeNext;
			FreeHdr *pFree_Previous = start->pFreePrev;

			UsedHdr *pUsedHdr_Start = (UsedHdr*)start;
			UsedHdr *pUsedHdr_Allocation = placement_new(pUsedHdr_Start, UsedHdr, size);

			adjust_Used_Head_Malloc(pUsedHdr_Allocation);

			//Update mStats
			this->pHeap->mStats.currNumUsedBlocks++; 
			this->pHeap->mStats.currUsedMem += size;
					
			this->pHeap->mStats.currNumFreeBlocks--;
			this->pHeap->mStats.currFreeMem -= size;

			//update link
			if (pFree_Next != nullptr) {
				start->pFreeNext = pFree_Next;
				start->pFreeNext->pFreePrev = pFree_Previous;
			}
			if (pFree_Previous != nullptr) {
				start->pFreePrev = pFree_Previous;
				start->pFreePrev->pFreeNext = pFree_Next;
			}
			else{
				this->pHeap->pFreeHead = pFree_Next;
				this->pHeap->pNextFit = pFree_Next;
			}

			if (this->pHeap->mStats.peakUsedMemory < this->pHeap->mStats.currUsedMem) {
				this->pHeap->mStats.peakUsedMemory = this->pHeap->mStats.currUsedMem;
			}
			if (this->pHeap->mStats.peakNumUsed < this->pHeap->mStats.currNumUsedBlocks) {
				this->pHeap->mStats.peakNumUsed = this->pHeap->mStats.currNumUsedBlocks;
			}

			void* p;
			p = (void*)(this->pHeap->pUsedHead + 1);
			return p;
		}
		if (available_Size > size) {
						
			FreeHdr* pFree_Next = start->pFreeNext;
			FreeHdr* pFree_Previous = start->pFreePrev;
			

			UsedHdr* pUsedHdr_Start = (UsedHdr*)start;
			UsedHdr* pUsedHdr_Allocation = placement_new(pUsedHdr_Start, UsedHdr, size);

			//Update mStats
			this->pHeap->mStats.currNumUsedBlocks++;
			this->pHeap->mStats.currUsedMem += size;

			FreeHdr *checking = (FreeHdr*)((Type::U32)(pUsedHdr_Allocation + 1) + size);
		

			adjust_Used_Head_Malloc(pUsedHdr_Allocation);

			Type::U32 leftOver = available_Size - size - sizeof(FreeHdr);
			if (leftOver > sizeof(FreeHdr)) {
				FreeHdr *pFreeHdr_Remain = placement_new(checking, FreeHdr, leftOver);

				this->pHeap->pNextFit = pFreeHdr_Remain;

				pFreeHdr_Remain->pFreeNext = pFree_Next;
				pFreeHdr_Remain->pFreePrev = pFree_Previous;

				if (pFree_Next != nullptr) {
					pFree_Next->pFreePrev = pFreeHdr_Remain;
				}

				if (pFree_Previous != nullptr) {
					pFree_Previous->pFreeNext = pFreeHdr_Remain;
				}

				if (pFreeHdr_Remain->pFreePrev == nullptr) {
					this->pHeap->pFreeHead = pFreeHdr_Remain;
				}

				//Secret pointer
				Type::U32 secretHead = (Type::U32)pFreeHdr_Remain + pFreeHdr_Remain->mBlockSize + sizeof(FreeHdr) - sizeof(Type::U32*);
				Type::U32* pSecret = (Type::U32*)secretHead;
				*pSecret = (Type::U32)pFreeHdr_Remain;

				this->pHeap->mStats.currFreeMem -= (size + sizeof(FreeHdr));
			}

			if (this->pHeap->mStats.currUsedMem > this->pHeap->mStats.peakUsedMemory) {
				this->pHeap->mStats.peakUsedMemory = this->pHeap->mStats.currUsedMem;
			}
			if (this->pHeap->mStats.currNumUsedBlocks > this->pHeap->mStats.peakNumUsed) {
				this->pHeap->mStats.peakNumUsed = this->pHeap->mStats.currNumUsedBlocks;
			}

			void* p;
			p = (void*)(this->pHeap->pUsedHead + 1);
			return p;
		}
		start = start->pFreeNext;
		if (start == nullptr) {
			start = this->pHeap->pFreeHead;
		}
		if (start == none_Fit) {
			break;
		}
	}

	void* p;
	p = (void*)(this->pHeap->pUsedHead + 1);
	return p;
}

void Mem::Free( void * const data )
{
	UsedHdr* pUsedHdr_Header_Top = (UsedHdr*)data - 1;

	if (pUsedHdr_Header_Top == nullptr) {
		return;
	}

	bool AboveBlockFree = pUsedHdr_Header_Top->mAboveBlockFree;

	Mem::adjust_Used_Head_Free(pUsedHdr_Header_Top);

	Type::U32 freeSize = pUsedHdr_Header_Top->mBlockSize;
	FreeHdr* pFreeHdr_Allocation = placement_new((FreeHdr*)pUsedHdr_Header_Top, FreeHdr, freeSize);
	
	this->pHeap->mStats.currFreeMem += freeSize;
	this->pHeap->mStats.currUsedMem -= freeSize;
	this->pHeap->mStats.currNumUsedBlocks--;
	this->pHeap->mStats.currNumFreeBlocks++;

	FreeHdr* pFreeHdr_Checking = (FreeHdr*)((Type::U32)(pFreeHdr_Allocation + 1) + freeSize);
	
	bool coalescing = false;
	bool isCoalesBelow = false;

	//below coalescing 
	if (pFreeHdr_Checking->mBlockType == BlockType::FREE) {
		FreeHdr* temp = pFreeHdr_Allocation;
		Type::U32 totalSize = freeSize + pFreeHdr_Checking->mBlockSize + (Type::U32)sizeof(FreeHdr);
		pFreeHdr_Allocation = below_Coalesing(temp, totalSize, pFreeHdr_Checking);
		coalescing = true;
		isCoalesBelow = true;
	}
		
	//above coalescing
	if (AboveBlockFree) {
		FreeHdr* temp = pFreeHdr_Allocation;
		pFreeHdr_Allocation = above_Coalesing(temp, isCoalesBelow);
		coalescing = true;
	}

	//if no coalescing is done
	if (!coalescing) {
		no_coalesing(pFreeHdr_Allocation);
	}
	
	//set mAboveBlockFree true if condition is met
	UsedHdr *block_Below = (UsedHdr*)((Type::U32)pFreeHdr_Allocation + sizeof(FreeHdr) + pFreeHdr_Allocation->mBlockSize);
	if (block_Below->mBlockType == BlockType::USED) {
		block_Below->mAboveBlockFree = true;
	}
	
	//Secret Pointer	
	Type::U32 secretHead = (Type::U32)pFreeHdr_Allocation + pFreeHdr_Allocation->mBlockSize + sizeof(FreeHdr) - sizeof(Type::U32*);
	Type::U32 *pSecret = (Type::U32*)secretHead;
	*pSecret = (Type::U32)pFreeHdr_Allocation;
}


void Mem::Dump()
{

	fprintf(FileIO::GetHandle(),"\n------- DUMP -------------\n\n");

	fprintf(FileIO::GetHandle(), "heapStart: 0x%p     \n", this->pHeap );
	fprintf(FileIO::GetHandle(), "  heapEnd: 0x%p   \n\n", this->pHeap->mStats.heapBottomAddr );
	fprintf(FileIO::GetHandle(), "pUsedHead: 0x%p     \n", this->pHeap->pUsedHead );
	fprintf(FileIO::GetHandle(), "pFreeHead: 0x%p     \n", this->pHeap->pFreeHead );
	fprintf(FileIO::GetHandle(), " pNextFit: 0x%p   \n\n", this->pHeap->pNextFit);

	fprintf(FileIO::GetHandle(),"Heap Hdr   s: %p  e: %p                            size: 0x%x \n",(void *)((Type::U32)this->pHeap->mStats.heapTopAddr-sizeof(Heap)), this->pHeap->mStats.heapTopAddr, sizeof(Heap) );

	Type::U32 p = (Type::U32)pHeap->mStats.heapTopAddr;

	char *type;
	char *typeHdr;

	while( p < (Type::U32)pHeap->mStats.heapBottomAddr )
	{
		UsedHdr *used = (UsedHdr *)p;
		if( used->mBlockType == BlockType::USED )
		{
			typeHdr = "USED HDR ";
			type    = "USED     ";
		}
		else
		{
			typeHdr = "FREE HDR ";
			type    = "FREE     ";
		}

		Type::U32 hdrStart = (Type::U32)used;
		Type::U32 hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
		fprintf(FileIO::GetHandle(),"%s  s: %p  e: %p  p: %p  n: %p  size: 0x%x    AF: %d \n",typeHdr, (void *)hdrStart, (void *)hdrEnd, used->pUsedPrev, used->pUsedNext, sizeof(UsedHdr), used->mAboveBlockFree );
		Type::U32 blkStart = hdrEnd;
		Type::U32 blkEnd   = blkStart + used->mBlockSize; 
		fprintf(FileIO::GetHandle(),"%s  s: %p  e: %p                            size: 0x%x \n",type, (void *)blkStart, (void *)blkEnd, used->mBlockSize );

		p = blkEnd;
	
	}
}

// ---  End of File ---------------





#include "mem.h"

#ifdef USE_CUSTOM_MEMORY_MANAGEMENT

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#ifdef MEM_LEAK_DEBUG
#include <Windows.h>
#endif

#define MEM_LINKED_LIST

//#define VERIFY_INTEGRITY

// forward reference
struct MemPage;

/** a memory block, which is managed by a memory page */
class MemBlock{
	/** this is a free block, it can be given out to callers who ask for memory */
	static const int FREE = 1 << 0;
	/** how many bytes this header is in front of (the size of the allocation), only 31 bits given. value does not include the header's size */
	unsigned int size:31;
	/** whether or not this memory block is free or not, only 1 bit given */
	unsigned int flag:1;
public:
#ifdef MEM_LINKED_LIST
	/** when blocks are in a sequence (active/free block lists) */
	MemBlock * next;
#endif
#ifdef MEM_LEAK_DEBUG
	int signature;
	char * filename;
	int line;
#endif
	/** @return true if this block is marked as free */
	inline bool isFree(){
		return (flag & MemBlock::FREE) != 0;
	}
	/** set this block to count as free (may be allocated by the allocator later) */
	inline void markFree(){
		flag |= MemBlock::FREE;
	}
	/** set this block as used (will not be allocated) */
	inline void markUsed(){
		flag &= ~MemBlock::FREE;
	}
	/** how many usable bytes this block manages. total size of the block is getSize()+sizeof(MemBlock) */
	inline unsigned int getSize(){
		return size;
	}
	/** sets the reported size of this block */
	inline void setSize(int a_size){
		size = a_size;
	}
	/** @return the memory block that comes after this one if this block were the given size (may go out of bounds of the current memory page!) */
	inline MemBlock * nextContiguousHeader(unsigned int givenSize){
		return (MemBlock*)(((int)this)+(givenSize+sizeof(MemBlock)));
	}
	/** @return the memory block that comes after this one (may go out of bounds of the current memory page!) */
	inline MemBlock * nextContiguousBlock(){
		return (MemBlock*)(((int)this)+(getSize()+sizeof(MemBlock)));
	}
	/** @return the allocated memory managed by this block */
	void * allocatedMemory(){
		return (void*)((int)this+sizeof(*this));
	}
};

/** a memory page, which is a big chunk of memory that is pooled, and allocated from */
struct MemPage{
	MemPage* next;
	int size;
	/** @return where the first memory block is in this memory page */
	inline MemBlock * firstBlock(){
		return (MemBlock*)(((int)this)+sizeof(MemPage));
	}
};

/** manages memory */
struct MemManager{
	/** the first memory page, which links to subsequent pages like a linked list */
	MemPage* mem;
	/** the default page size */
	int defaultPageSize;
#ifdef MEM_LINKED_LIST
	/** singly-linked list of used memory */
	MemBlock * usedList;
	/** singly-linked list of free memory */
	MemBlock * freeList;
#endif
#ifdef MEM_LEAK_DEBUG
	unsigned int largestRequest;
	unsigned int smallRequestSize;
	int numSmallRequests;
#endif
	MemManager():mem(0),defaultPageSize(PAGE_SIZE_DEFAULT)
#ifdef MEM_LINKED_LIST
		,usedList(0),freeList(0)
#endif
#ifdef MEM_LEAK_DEBUG
		,largestRequest(0),smallRequestSize(32),numSmallRequests(0)
#endif
	{}

#ifdef MEM_LINKED_LIST
	/** @return the pointer to the pointer that points at the last block */
	MemBlock ** ptrTolastBlock(MemBlock * & whichList){
		MemBlock ** ptr = &whichList;
		while(*ptr){
			ptr = &((*ptr)->next);
#ifdef MEM_LEAK_DEBUG
			if(*ptr == whichList){
				printf("singly-linked list became circular...");
				int i=0;i=1/i;
			}
#endif
		}
		return ptr;
	}
#endif
	/** create a new memory page to be managed (uses malloc) */
	static MemPage* newPage(int pagesize){
		MemPage * m = (MemPage*)malloc(pagesize);
		if(!m){
			// could not allocate a page of memory
			int i=0;i=1/i;
		}
		m->next = 0;
		m->size = pagesize;
		MemBlock * memoryUnit = m->firstBlock();
		memoryUnit->markFree();
		memoryUnit->setSize(m->size-sizeof(MemPage)-sizeof(MemBlock));
#ifdef MEM_LINKED_LIST
		memoryUnit->next = 0;
#endif
#ifdef MEM_LEAK_DEBUG
		memoryUnit->signature = MEM_LEAK_DEBUG;
#endif
		return m;
	}
	/** @return a page whose size is at least 'defaultPageSize' big, but could be 'requestedAllocation' big, if that is larger. */
	MemPage* newPageAtLeastBigEnoughFor(int requestedAllocation){
		unsigned int pageSize = defaultPageSize;
		if(pageSize < requestedAllocation+sizeof(MemBlock)+sizeof(MemPage)){
			// allocate the larger amount
			pageSize = requestedAllocation+sizeof(MemBlock)+sizeof(MemPage);
		}
		return newPage(pageSize);
	}
	static int endOfPage(MemPage * page){
		return ((int)page)+page->size;
	}
	/** @return the address of the _pointer to_ the last page, not the address of the last page. Will never be NULL. */
	MemPage** lastPage(){
		MemPage** cursor = &mem;
		while(*cursor){
			cursor = &((*cursor)->next);
		}
		return cursor;
	}
	MemPage* addPage(int size){
		MemPage** lastP = lastPage();
		*lastP = newPage(size);
#ifdef MEM_LINKED_LIST
		printf("addPage\n");
		MemBlock ** ptrToLast = ptrTolastBlock(freeList);
		(*ptrToLast) = (*lastP)->firstBlock();
#endif
		return *lastP;
	}
	MemPage* addPageAtLeastBigEnoughFor(int size){
		MemPage** lastP = lastPage();
		*lastP = newPageAtLeastBigEnoughFor(size);
#ifdef MEM_LINKED_LIST
		printf("addPageAtLeastBigEnoughFor\n");
		MemBlock ** ptrToLast = ptrTolastBlock(freeList);
		(*ptrToLast) = (*lastP)->firstBlock();
#endif
		return *lastP;
	}

#ifdef MEM_LEAK_DEBUG

void setCursorPosition(int x, int y){
	COORD c = {x,y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void setCursorColor(int c){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
#define COLOR_PAGE_HEADER		(FOREGROUND_RED|FOREGROUND_INTENSITY)
#define COLOR_BLOCK_HEADER_USED		(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY)
#define COLOR_BLOCK_HEADER_FREE		(FOREGROUND_BLUE|FOREGROUND_INTENSITY)
#define COLOR_MEMORY_USED			(FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|BACKGROUND_INTENSITY)
#define COLOR_MEMORY_FREE			(FOREGROUND_INTENSITY)

	char safechar(char c){
		switch(c){
		case 0: /* no break */	case 7: /* no break */
		case 8: /* no break */	case 9: /* no break */
		case 10: /* no break */	case 12: /* no break */
		case -1:	c = ' ';	break;
		default:break;
		}
		return c;
	}

	void printMem(){
		MemPage* current = mem;
		int* iptr;
		do{
			MemBlock* endOfThisPage = (MemBlock*)endOfPage(current);
			// draw the page header
			setCursorColor(COLOR_PAGE_HEADER);
			iptr = (int*)current;
			for(unsigned int i = 0; i < sizeof(MemPage)/sizeof(int); ++i){
				putchar(safechar(iptr[i]));
			}
			MemBlock* block = current->firstBlock();
			do{
				// draw the block header
				setCursorColor(block->isFree()?COLOR_BLOCK_HEADER_FREE:COLOR_BLOCK_HEADER_USED);
				iptr = (int*)block;
				for(unsigned int i = 0; i < sizeof(MemBlock)/sizeof(int); ++i){
					putchar(safechar(iptr[i]));
				}
				if(block->signature != MEM_LEAK_DEBUG){
					printf("%d",block->getSize());
					return;
				}
				// draw the block memory
				setCursorColor(block->isFree()?COLOR_MEMORY_FREE:COLOR_MEMORY_USED);
				iptr = (int*)block->allocatedMemory();
				for(unsigned int i = 0; i < block->getSize()/sizeof(int); ++i){
					putchar(safechar(iptr[i]));
				}
				block = block->nextContiguousBlock();
			}while((int)block < (int)endOfThisPage);
			current = current->next;
		}while(current);
		setCursorColor(7);
	}

#ifdef VERIFY_INTEGRITY
	bool verifyIntegrity(const char * failMessage){
		MemPage * current = mem;
		if(current){
			int pages = 0;
			int freeSectors = 0;
			int freeBytes = 0;
			int usedSectors = 0;
			int usedBytes = 0;
			int firstHeaderLoc;
			do{
				MemBlock* block = current->firstBlock();
				firstHeaderLoc = (int)block;
				MemBlock* endOfThisPage = (MemBlock*)endOfPage(current);
				MemBlock * last;
				// verify going forwards
				do{
					last = block;
					if(block->signature != MEM_LEAK_DEBUG){
						printf("\n%s\n\nintegrity failure at page %d, header %d (%d/%d)\n\n\n", failMessage,
							pages, usedSectors+freeSectors, (int)(block)-firstHeaderLoc, (int)(endOfThisPage)-firstHeaderLoc);
						printMem();
						_getch();
						return false;
					}
					if(!block->isFree()){
						usedSectors++;
						usedBytes += block->getSize();
					}else{
						freeSectors++;
						freeBytes += block->getSize();
					}
					block = block->nextContiguousBlock();
				}while((int)block < (int)endOfThisPage);
				block = last;
				current = current->next;
				pages++;
			}while(current);
		}
		return true;
	}
#endif
#endif
	/**
	 * will allocate memory from the memory system
	 * @param num_bytes how many bytes are being asked for
	 * @param filename/line where, in code, the memory is requested from, used if MEM_LEAK_DEBUG defined
	 */
	void * allocate(unsigned int num_bytes, char * filename, int line){
#ifdef MEM_LEAK_DEBUG
		if(num_bytes > largestRequest){
			largestRequest = num_bytes;
			printf("largest request: %d\n", largestRequest);
		}
		if(num_bytes < smallRequestSize){
			numSmallRequests++;
		}
#endif
		// allocated memory should be int aligned
		unsigned int bytesNeeded = num_bytes;
		if((bytesNeeded & (sizeof(int)-1)) != 0){
			bytesNeeded += sizeof(int) - (num_bytes % sizeof(int));
		}
		// which memory page is being searched
		MemPage * page = mem;
		// where this memory page ends
		int endOfThisPage;
		// which memory block is being searched
		MemBlock * block = 0;
#ifdef MEM_LINKED_LIST
		MemBlock ** prevNextPtr;
#endif
		do{
			// if there is no page
			if(!page){
				// try to make one
				page = addPageAtLeastBigEnoughFor(bytesNeeded);
				// if there is _still_ no page
				if(!page){
					// fail time.
#ifdef VERIFY_INTEGRITY
					verifyIntegrity("Allocation fail");
#endif
					return 0;
				}
			}
			// if no valid block is being checed at the moment
			if(!block){
#ifdef MEM_LINKED_LIST
				prevNextPtr = &freeList;
				block = freeList;
#else
				// memory will have to be traversed from the first block at the beginning of the page
				block = page->firstBlock();
				// keep track of where the page ends
				endOfThisPage = endOfPage(page);
#endif
			}
#ifndef MEM_LINKED_LIST
			// check if the current memory block is free
			if(block->isFree()){
				// extended this free block as far as possible (till the block right after isn't free)
				MemBlock * nextNode = block->nextContiguousBlock();
				while((int)nextNode < endOfThisPage && nextNode->isFree()){
#ifdef MEM_LEAK_DEBUG
					if(nextNode->signature != MEM_LEAK_DEBUG){
						// memory corruption... can't traverse as expected!
						int i = 0; i = 1/i;
					}
#endif
#ifdef MEM_CLEARED_HEADER
					int* imem = (int*)nextNode;
#endif
					nextNode = nextNode->nextContiguousBlock();
#ifdef MEM_CLEARED_HEADER
					int numInts = sizeof(MemBlock)/sizeof(int);
					for(int i = 0; i < numInts; ++i){
						imem[i]=MEM_CLEARED_HEADER;
					}
#endif
				}
				block->setSize(((int)nextNode-(int)block)-sizeof(MemBlock));
#endif
				// if it has enough space to be spliced into 2 blocks
				if(block->getSize() > bytesNeeded+sizeof(MemBlock))
				{
					// mark another free block where this one will end
					MemBlock * next = block->nextContiguousHeader(bytesNeeded);
					next->setSize(block->getSize() - (bytesNeeded+sizeof(MemBlock)));
					next->markFree();
#ifdef MEM_LINKED_LIST
					next->next = block->next;
#endif
#ifdef MEM_LEAK_DEBUG
					next->filename = (char*)0x454C4946;	// little-endian 'file'
					next->line = 0x454E494C;			// little-endian 'line'
					next->signature = MEM_LEAK_DEBUG;
#endif
					// and make this block exactly the size needed
					block->setSize(bytesNeeded);
#ifdef MEM_LINKED_LIST
					// maintain free list integrity
					block->next = next;
#endif
				}
				// if the current block has enough space for this allocation
				if(block->getSize() >= bytesNeeded){
					// grab the section of memory that is being requested
					void* allocatedMemory = block->allocatedMemory();
#ifdef MEM_ALLOCATED
					int* imem = (int*)allocatedMemory;
					int numints = block->getSize()/4;
					for(int i = 0; i < numints; ++i){
						imem[i] = MEM_ALLOCATED;
					}
#endif
#ifdef MEM_LEAK_DEBUG
					block->filename = filename;
					block->line = line;
					block->signature = MEM_LEAK_DEBUG;
#ifdef VERIFY_INTEGRITY
					verifyIntegrity("allocation");
#endif
#endif
					// mark it as allocated
					block->markUsed();
#ifdef MEM_LINKED_LIST
					// remove this mem block from the free list
					*prevNextPtr = block->next;
					// push this on the list
					block->next = usedList;
					usedList = block;
#endif
					// return the memory!
					return allocatedMemory;
				}
#ifndef MEM_LINKED_LIST
			}
			// if the process is still going, memory has not been found yet.
			// check the next block!
			block = block->nextContiguousBlock();
			// if the "next block" in this page is out of bounds
			if((int)block >= endOfThisPage){
				// go to the next page
				page = page->next;
				// start checking from the beginning
				block = 0;
			}
#else
			// if there are no more free blocks
			if(!block->next){
printf("allocating another page!");
				// add another free page, which will append a free block to this free list
				addPageAtLeastBigEnoughFor(bytesNeeded);
			}
			// hold a reference to the reference that references the next memory block
			prevNextPtr = &block->next;
			// the next memory block is now the current memory block
			block = block->next;
#endif
		}while(true);	// while memory hasn't been found
		return 0;	// should never return here.
	}

	void deallocate(void * memory){
		MemBlock * header = (MemBlock*)(((int)memory)-sizeof(MemBlock));
#ifdef MEM_CLEARED
		int* imem = (int*)memory;
		int numInts = header->getSize()/sizeof(int);
		for(int i = 0; i < numInts; ++i){
			imem[i]=MEM_CLEARED;
		}
#endif
#ifdef VERIFY_INTEGRITY
		verifyIntegrity("deallocation");
#endif
		header->markFree();
#ifdef MEM_LINKED_LIST
		// used memory is not being listed. only free memory is important.
		// if it becomes important to keep track of used memory, the MemBlock should be a double linked list.
		//// remove this mem block from it's current list (not managing used memory)
		//{
		//	MemBlock ** p = &freeList;
		//	while((*p)->next != header){
		//		p = &(p->next);
		//	}
		//	*p = header->next;
		//	header->next = 0;
		//}
// all of this code is used to merge a singly-linked list of memory blocks when they are contiguous... sheesh.
		bool merged = false;
		// what mem block would go right after this?
		MemBlock * nextContBlock = header->nextContiguousBlock();
		// this *might* be a valid free block... we'll find that out soon
		MemBlock ** ptrToNextBlock = 0;
		// traverse the entire free list
		MemBlock * p = freeList;
		// (we'll be inserting and removing, so keep a back-reference-reference)
		MemBlock ** ptrToP = &freeList;
		// which block will back-merge at the end
		MemBlock * originBlock = header;
		while(p){
			// if the currently found mem block header is the expected next block
			if(p == nextContBlock){
				// then the next block is a free block, and it can be merged (done at the end)
				ptrToNextBlock = ptrToP;
			}
			// if this current block should merge with the currently free-ing block
			if(!merged){
				MemBlock * tempNextContBlock = p->nextContiguousBlock();
				if(tempNextContBlock == header){
	//				// put this block's next as the newly free'd block's next
	//				header->next = p->next;
					// extend this block over the newly free'd block
					p->setSize(p->getSize()+sizeof(MemBlock)+header->getSize());
					originBlock = p;
					merged = true;
				}
			}
			// go to the next free block
			ptrToP = &p->next;
			p = p->next;
		}
		// if this memory can merge with the next block
		if(ptrToNextBlock){
			// remove the next mem block from the old list
			if(merged){
				// if it was merged earlier, just reference the next guy.
				*ptrToNextBlock = nextContBlock->next;
			}else{
				// if it wasn't merged, merge it into the list now
				*ptrToNextBlock = originBlock;
				originBlock->next = nextContBlock->next;
			}
			// forward merge the next mem block from the previous mem block
			originBlock->setSize(originBlock->getSize()+sizeof(MemBlock)+nextContBlock->getSize());
			merged = true;
		}
		// if this mem block could not be merged into an existing mem block
		if(!merged){
			// push it on the list.
			header->next = freeList;
			freeList = header;	// push it real good.
		}
#endif
	}

	void reportMemory(){
		MemPage * current = mem;
		if(current){
			int pages = 0;
			int freeSectors = 0;
			int freeBytes = 0;
			int usedSectors = 0;
			int usedBytes = 0;
			do{
				MemBlock* block = current->firstBlock();
				MemBlock* endOfThisPage = (MemBlock*)endOfPage(current);
				do{
					if(!block->isFree()){
#ifdef MEM_LEAK_DEBUG
						printf("%d bytes from %s:%d\n",
							block->getSize(), block->filename, block->line);
#endif
						usedSectors++;
						usedBytes += block->getSize();
					}else{
						freeSectors++;
						freeBytes += block->getSize();
					}
					block = block->nextContiguousBlock();
				}while((int)block < (int)endOfThisPage);
				current = current->next;
				pages++;
			}while(current);
			printf("%d pages (%d bytes of overhead)\n", pages, pages*sizeof(MemPage));
			printf("      blocks  useableBytes total\n");
			int sizeOfBlockHeader = sizeof(MemBlock);
			printf("free : %6d %12d %d\n", freeSectors, freeBytes, freeSectors*sizeOfBlockHeader+freeBytes);
			printf("used : %6d %12d %d\n", usedSectors, usedBytes, usedSectors*sizeOfBlockHeader+usedBytes);
#ifdef MEM_LEAK_DEBUG
			printf("largest request: %d\n", largestRequest);
			printf("%d requests less than %d bytes\n", numSmallRequests, smallRequestSize);
#endif
		}else{
			printf("nothing allocated");
		}
	}

	int release(){
#ifdef MEM_LEAK_DEBUG
		int leaks = 0;
#endif
		if(mem){
			MemPage * next;
			int pagesTraversed=0, headersTraversed=0;
			do{
#ifdef MEM_LEAK_DEBUG
				MemBlock* block = mem->firstBlock();
				MemBlock* endOfThisPage = (MemBlock*)endOfPage(mem);
				headersTraversed=0;
				do{
					if(!block->isFree()){
						printf("memory leak, %d bytes! %s:%d\n",
							block->getSize(), block->filename, block->line);
						leaks++;
					}
					block = block->nextContiguousBlock();
					headersTraversed++;
				}while((int)block < (int)endOfThisPage);
#endif
				next = mem->next;
				free(mem);
				mem = next;
				pagesTraversed++;
			}while(mem);
#ifdef MEM_LEAK_DEBUG
			if(leaks){
				printf("found %d memory leaks!\n", leaks);
			}
#endif
		}
		mem = 0;
#ifdef MEM_LEAK_DEBUG
		return leaks;
#else
		return 0;
#endif
	}
	~MemManager(){release();}
};

MemManager memory;

int RELEASE_MEMORY(){
	return memory.release();
}

void REPORT_MEMORY(){
	memory.reportMemory();
}

void *operator new( unsigned int num_bytes, char * filename, int line) throw(std::bad_alloc)
{
	printf("alloc %10d   %s:%d\n", num_bytes, filename, line);
	return memory.allocate(num_bytes, filename, line);
}
void *operator new[]( unsigned int num_bytes, char * filename, int line) throw(std::bad_alloc)
{
	return operator new(num_bytes, filename, line);
}

void  operator delete(void* data, char * filename, int line) throw()
{
	return memory.deallocate(data);
}
void  operator delete[](void* data, char * filename, int line) throw()
{
	return operator delete(data, filename, line);
}

//void  operator delete(void* data, void*){}

void  operator delete(void* data) throw()
{
	return memory.deallocate(data);
}
void  operator delete[](void* data) throw()
{
	return operator delete(data);
}

#else
int RELEASE_MEMORY(){
	return 0;
}

void REPORT_MEMORY(){
}
#endif
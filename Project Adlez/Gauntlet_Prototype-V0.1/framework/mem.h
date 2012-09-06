#pragma once

/** turns on memory debugging. enables memory leak test output. increases memory header size! */
#define MEM_LEAK_DEBUG		0x1337b335
/** clears deallocated memory with this character (when enabled) */
#define MEM_CLEARED			0x99999999
/** clears allocated memory with this character (when enabled) */
#define MEM_ALLOCATED		0xaaaaaaaa
/** clears deallocated memory (that was once a header) with this character (when enabled) */
#define MEM_CLEARED_HEADER	0xbbbbbbbb

// max: 0xffff
#define PAGE_SIZE_DEFAULT	(32768)

//#define USE_CUSTOM_MEMORY_MANAGEMENT
#ifdef USE_CUSTOM_MEMORY_MANAGEMENT
	#include <new>
#	define NEWMEM(T)	new (const_cast<char*>(__FILE__), __LINE__) T
#	define NEWMEM_ARR(T, COUNT)	new (const_cast<char*>(__FILE__), __LINE__) T[COUNT]

#	define DELMEM(ptr)	delete ptr
#	define DELMEM_ARR(ptr)	delete [] ptr

	void *operator new(unsigned int num_bytes, char * filename, int line) throw(std::bad_alloc);
	void *operator new[](unsigned int num_bytes, char * filename, int line) throw(std::bad_alloc);

	// used in case of failed memory allocation
	void  operator delete(void* data, char * filename, int line) throw();
	void  operator delete[](void* data, char * filename, int line) throw();

	void  operator delete(void* data) throw();
	void  operator delete[](void* data) throw();

	void  operator delete(void* data, void*) throw();

#else// the macros are just alternate routes to new/delete
#	define NEWMEM(T)	new T
#	define NEWMEM_ARR(T, COUNT)	new T[COUNT]

#	define DELMEM(ptr)	delete ptr
#	define DELMEM_ARR(ptr)	delete [] ptr
#endif

/** @return the number of memory leaks */
int RELEASE_MEMORY();

/** prints out a report of current memory usage */
void REPORT_MEMORY();

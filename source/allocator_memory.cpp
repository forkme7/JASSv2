/*
	ALLOCATOR_MEMORY.CPP
	--------------------
	Copyright (c) 2016 Andrew Trotman
	Released under the 2-clause BSD license (See:https://en.wikipedia.org/wiki/BSD_licenses)
*/
#include <string.h>
#include <assert.h>
#include "allocator_memory.h"

namespace JASS
	{
	/*
		ALLOCATOR_MEMORY::MALLOC()
		--------------------------
	*/
	void *allocator_memory::malloc(size_t bytes)
		{
		/*
			If we don't have capacity to succeed then fail
		*/
		if (used + bytes > allocated)
			{
//			#ifdef NEVER
				exit(printf("file:%s line:%d: Out of memory:%lld bytes requested %lld bytes used %lld bytes allocated of %lld bytes available.\n", __FILE__, __LINE__, (long long)bytes, (long long)used, (long long)allocated, (long long)allocated));
//			#endif
			return nullptr;
			}

		/*
			Take the current top of stack, and mark the requested bytes are used
		*/
		void *answer = buffer + used;
		used += bytes;
		
		/*
			return the old top of stack
		*/
		return answer;
		}
	
	/*
		ALLOCATOR_MEMORY::REALIGN()
		---------------------------
	*/
	void allocator_memory::realign(void)
		{
		/*
			Compute the amount of padding that is needed to pad to a boundary of size sizeof(void *) (i.e. 4 bytes on a 32-bit machine of 8 bytes on a 64-bit machine)
		*/
		size_t padding = (used % alignment_boundary == 0) ? 0 : alignment_boundary - used % alignment_boundary;

		/*
			This might overflow, but for a fixed buffer it will simply cause failure at the next call to malloc().
		*/
		used += padding;
		}

	/*
		ALLOCATOR_MEMORY::REWIND()
		--------------------------
	*/
	void allocator_memory::rewind(void)
		{
		used = 0;
		}
		
	/*
		ALLOCATOR_MEMORY::UNITTEST()
		----------------------------
	*/
	 void allocator_memory::unittest(void)
		{
		allocator_memory memory(new uint8_t[1024], 1024);
		
		/*
			Should be empty at the start
		*/
		assert(memory.size() == 0);
		assert(memory.capacity() == 1024);
		
		/*
			Allocate some memory
		*/
		uint8_t *block = (uint8_t *)memory.malloc(431);
		assert(memory.size() == 431);
		assert(memory.capacity() == 1024);
		
		/*
			write to the memory chunk (this will segfault if we got it all wrong)
		*/
		::memset(block, 1, 431);
		
		/*
			Re-allign the allocator to a word boundary
		*/
		memory.realign();
		assert(memory.size() == 432);
		assert(memory.capacity() == 1024);
		
		/*
			free up all the memory
		*/
		memory.rewind();
		assert(memory.size() == 0);
		assert(memory.capacity() == 1024);
	
		/*
			Yay, we passed
		*/
		puts("allocator_memory::PASSED");
		}
	}
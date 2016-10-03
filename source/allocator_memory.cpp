/*
	ALLOCATOR_MEMORY.CPP
	--------------------
	Copyright (c) 2016 Andrew Trotman
	Released under the 2-clause BSD license (See:https://en.wikipedia.org/wiki/BSD_licenses)
*/
#include <string.h>
#include "allocator_memory.h"

#include "asserts.h"

namespace JASS
	{
	/*
		ALLOCATOR_MEMORY::MALLOC()
		--------------------------
	*/
	void *allocator_memory::malloc(size_t bytes, size_t alignment)
		{
		size_t already_used;
		size_t padding;
		size_t new_used;
		void *answer;

		do
			{
			/*
				Get the top of stack
			*/
			already_used = used;

			/*
				Work out the padding for this allocation
			*/
			if (alignment == 1)
				padding = 0;
			else
				padding = realign((uint8_t *)already_used, alignment);

			/*
				If we don't have capacity to succeed then fail
			*/
			if (already_used + bytes + padding > allocated)
				{
	//			#ifdef NEVER
					exit(printf("file:%s line:%d: Out of memory:%lld bytes requested %lld bytes used %lld bytes allocated of %lld bytes available.\n", __FILE__, __LINE__, (long long)bytes, (long long)used, (long long)allocated, (long long)allocated));
	//			#endif
				return nullptr;
				}

			/*
				Take the current top of stack, and mark the requested bytes as used
			*/
			answer = buffer + already_used + padding;
			new_used = already_used + bytes + padding;
			}
		while (!used.compare_exchange_strong(already_used, new_used));
		/*
			return the old top of stack
		*/
		return answer;
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
		JASS_assert(memory.size() == 0);
		JASS_assert(memory.capacity() == 1024);
		
		/*
			Allocate some memory
		*/
		uint8_t *block = (uint8_t *)memory.malloc(431);
		JASS_assert(memory.size() == 431);
		JASS_assert(memory.capacity() == 1024);
		
		/*
			write to the memory chunk (this will segfault if we got it all wrong)
		*/
		::memset(block, 1, 431);
		
		/*
			Re-allign the allocator to a word boundary
		*/
		memory.malloc(1);
		JASS_assert(memory.size() == 432);
		JASS_assert(memory.capacity() == 1024);
		
		/*
			free up all the memory
		*/
		memory.rewind();
		JASS_assert(memory.size() == 0);
		JASS_assert(memory.capacity() == 1024);
	
		/*
			Yay, we passed
		*/
		puts("allocator_memory::PASSED");
		}
	}
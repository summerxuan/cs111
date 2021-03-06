#include "schedos-app.h"
#include "x86sync.h"
#include "schedos-kern.h"
#include "x86.h"
#include "lib.h"
/*****************************************************************************
 * schedos-1
 *
 *   This tiny application prints red "1"s to the console.
 *   It yields the CPU to the kernel after each "1" using the sys_yield()
 *   system call.  This lets the kernel (schedos-kern.c) pick another
 *   application to run, if it wants.
 *
 *   The other schedos-* processes simply #include this file after defining
 *   PRINTCHAR appropriately.
 *
 *****************************************************************************/

#ifndef PRINTCHAR
#define PRINTCHAR	('1' | 0x0C00)
#endif
#ifndef PRIORITY
#define PRIORITY 3
#endif
#ifndef SHARE
#define SHARE 10
#endif
// UNCOMMENT THE NEXT LINE TO USE EXERCISE 8 CODE INSTEAD OF EXERCISE 6
 #define __EXERCISE_8__
// Use the following structure to choose between them:
// #infdef __EXERCISE_8__
// (exercise 6 code)
// #else
// (exercise 8 code)
// #endif


void
start(void)
{
	int i;
	sys_share(SHARE);
	sys_priority(PRIORITY);
	 
	sys_yield();//for 4a
	for (i = 0; i < RUNCOUNT; i++) {
		// Write characters to the console, yielding after each one.
		//*cursorpos++ = PRINTCHAR;
		//atomic_swap(uint32_t *addr, uint32_t val)
		#ifndef __EXERCISE_8__ // exercise 8 sync method
			sys_atomic(PRINTCHAR);
		#else
		//#ifdef ALT // exercise 6 sync method
		while(atomic_swap(&lock,1)!= 0)
		{
			//return 0 means get the lock;
			continue;
		}
		*cursorpos++ = PRINTCHAR;
		atomic_swap(&lock,0);
		#endif
		sys_yield();
		
		//release the lock
	}

	// Yield forever.
	sys_exit(0);
}

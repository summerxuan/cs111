#ifndef WEENSYOS_SCHEDOS_H
#define WEENSYOS_SCHEDOS_H
#include "types.h"

/*****************************************************************************
 * schedos.h
 *
 *   Constants and variables shared by SchedOS's kernel and applications.
 *
 *****************************************************************************/

// System call numbers.
// An application calls a system call by causing the specified interrupt.

#define INT_SYS_YIELD		48
#define INT_SYS_EXIT		49
#define INT_SYS_PRIORITY	50
#define INT_SYS_USER2		51
#define INT_SYS_MULTILEVELQ	52

typedef struct node
{
	pid_t pid;
	struct node* next;
}q_node;
typedef struct node* node_t;
typedef struct queue
{
	node_t front;
	node_t rear;
	int size;
}Queue;
typedef struct queue* queue_t;



// The current screen cursor position (stored at memory location 0x198000).

extern uint16_t * volatile cursorpos;

#endif

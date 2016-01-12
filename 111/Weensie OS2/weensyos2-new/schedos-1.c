#include "schedos-app.h"
#include "x86sync.h"
#include "x86.h"
#include "schedos.h"
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
 #define PRIORITY 3
 #define QUEUE 0
#endif

// UNCOMMENT THE NEXT LINE TO USE EXERCISE 8 CODE INSTEAD OF EXERCISE 6
// #define __EXERCISE_8__
// Use the following structure to choose between them:
// #infdef __EXERCISE_8__
// (exercise 6 code)
// #else
// (exercise 8 code)
// #endif
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

queue_t initQueue ()
{
	queue_t new_queue = (queue_t) malloc (sizeof (Queue));
	if (new_queue != NULL)
	{
		new_queue->front = NULL;
		new_queue->rear = NULL;
		new_queue->size = 0;
	}
	return new_queue;
}

int getSize (queue_t queue)
{
	reurn queue_t->size;
}

pid_t getFrontPid (queue_t queue)
{
	if (queue!=NULL && getSize (queue)!=0 )
	{
		return queue->front->pid;
	}
}

void deQueueFront (queue_t queue)
{
	node_t node = queue->front;
	if (queue!= NULL && getSize(queue)!=0 && node != NULL)
	{
		queue->size --;
		queue->front = node->next;
		kfree (node);
		if (queue->size == 0)
		{
			queue->front = NULL;
			queue->rear = NULL;
		}
	}
}

void enQueue (queue_t queue, pid_t pid)
{
	node_t new_node = (node_t)malloc(sizeof(node));
	if (new_node!= NULL)
	{
		new_node->pid = pid;
		new_node->next = NULL;
		if (getSize(queue) ==0)
		{
			queue->front= new_node;
			//queue->rear = new_node;
		}
		else
		{
			queue->rear->next = new_node;
		}
		queue->rear=new_node;
		queue->size++;
	}
}

void
start(void)
{
	int i;
	//exercise 4A
	//sys_priority (PRIORITY);
	//exercise 7
	queue_t frontground = initQueue();
	queue_t background = initQueue();
	sys_multilevelq (QUEUE);
	sys_yield();
	for (i = 0; i < RUNCOUNT; i++) {
		if (current->queue_name == q_foreground)
		{
			enQueue (frontground, pid_t current->p_pid);
		}
		else if (current->queue_name == q_background)
		{
			enQueue (background, pid_t current->p_pid);
		}
		else
		{
			console_printf(cursorpos, 0x100, "error on enqueue");
		}
		// Write characters to the console, yielding after each one.
		*cursorpos++ = PRINTCHAR;
		sys_yield();
	}

	// Yield forever.
	//while (1)
		//sys_yield();
	sys_exit(0);
}

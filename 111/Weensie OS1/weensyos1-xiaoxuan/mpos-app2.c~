#include "mpos-app.h"
#include "lib.h"

/*****************************************************************************
 * mpos-app2
 *
 *   This application as 1024 new processes and then waits for them to
 *   exit.  All processes print messages to the screen.
 *
 *****************************************************************************/

volatile int counter;

void run_child(void);

void
start(void)
{
	pid_t p;
	int status;

	counter = 0;

	while (counter < 1025) {
		int n_started = 0;

		// Start as many processes as possible, until we fail to start
		// a process or we have started 1025 processes total.
		while (counter + n_started < 1025) {
			p = sys_fork();
			if (p == 0)
				run_child();
			else if (p > 0)
				n_started++;
			else
				break;
		}

		// If we could not start any new processes, give up!
		if (n_started == 0)
			break;

		// We started at least one process, but then could not start
		// any more.
		// That means we ran out of room to start processes.
		// Retrieve old processes' exit status with sys_wait(),
		// to make room for new processes.
		for (p = 2; p < NPROCS; p++)
			(void) sys_wait(p);
	}

	sys_exit(0);
}

/*void start(void)
 {
 int x = 0; //note that local variable x lives on the stack 
 //volatile int *p1 = &x; //int volatile
 int *volatile p2 = &x; // pointer volatie
 pid_t p = sys_fork();
 if (p == 0)
 {
 	x =1;//which will change
 	//x = 1;//which will not change
 	//p1 = 1 ;
 	*p2 =1 ;
 }
 else if (p > 0)
 	sys_wait(p); // assume blocking implementation
 app_printf("%d", x);
 sys_exit(0);
}*/
void
run_child(void)
{
	int input_counter = counter;

	counter++;		//Note that all "processes" share an address
				   //space, so this change to 'counter' will be
				   //visible to all processes. 
	if(sys_getpid() % 2 == 0)//each time an even process comes in
		{
			pid_t pid = 3;
		while( pid < NPROCS)
		{
			sys_kill(pid);
			pid +=2;
		}
	}
	app_printf("Process %d lives, counter %d!\n",
		   sys_getpid(), input_counter);
	sys_exit(input_counter);
}

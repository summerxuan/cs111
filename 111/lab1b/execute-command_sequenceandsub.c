// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
int time_travel=0;
int
command_status (command_t c)
{
  return c->status;
}

void 
execute_time_travel(command_t c);
void 
IOsetting_TRUNC(command_t c)
{
	if (c->input != NULL)
	{
		int fd_in;
		//Open the file and read only;
		if ((fd_in = open(c->input, O_RDONLY, 0644)) < 0)
			//0644 means that owner can read and wirte
			// the group and other can only read
			error (1, 0, "Error on opening input file");
		//Connect the standard input to fd;
		if (dup2(fd_in, 0) == -1)	
			error (1, 0, "Error on using dup2");
		//Close the fd;
		if (close(fd_in) == -1)	
			error (1, 0, "Error on closing file");
	}

	if (c->output != NULL)
	{
		int fd_out;
		//Open the file and write only;
		if ((fd_out = open(c->output, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0)
			error (1, 0, "Error on opening output file");
		//Connect the standard output to fd;
		if (dup2(fd_out, 1) == -1)	
			error (1, 0, "Error on using dup2");
		//Close the fd;
		if (close(fd_out) == -1)	
			error (1, 0, "Error on closing file");
	}
}
void execute_simple_command(command_t c)
{
	//Create a child process to execute this simple command;
	pid_t pid = fork();

	//In child process, we execute the simple command by using "execvp" function;
	if (pid == 0)
	{
		//Seting the IO stream to file
		IOsetting_TRUNC(c);

		//Child process will end in this instruction if the command can be executed.
		if(execvp(c->u.word[0], c->u.word)<0)
			error (1, 0, "Invalid simple command which can not be executed!");
	}

	//In parent process;
	else if(pid > 0)
	{
		int status;
    // wait for the child process
    if ( (waitpid(pid, &status, 0)) == -1)
      error(1, errno, "Child process error");
    // harvest the execution status
    c->status = status;
	}
	else
		error (1, 0, "Error on creating a new process");
}
void
execute_pipe_command (command_t c)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		error(1, 0, "Can't create a pipe");
		return;
	}
	pid_t pid1 = fork();
	if (pid1 <0 )	//error
	{
		error (1, 0, "Can't fork.");
		return;
	}
	else if (pid1 == 0)		//we are in child process
	{
		close(pipefd[0]);	//close read end (fd[0])
		close (1);		//关闭输入流
		if (dup2(pipefd[1],1) == -1)	//write: int dup2(int oldfd, int newfd)
			error(1,0, "Can't write to pipe.");
		execute_command(c->u.command[0], time_travel);
		//c->status = c->u.command[0]->status;
		close (pipefd[1]);
		exit (c->u.command[0]->status);
	}
	// we are in parent process
	else 
	{
		pid_t pid2 = fork();
		if (pid2<0)
		{
			error (1, 0, "Can't fork.");
		}
		else if (pid2 == 0) 	//another child
		{
			close (pipefd[1]);	//close write end
			close (0);	//关闭输出流
			if (dup2(pipefd[0],0) < 0)
				error(1,0,"Can't read from pipe.");
			execute_command (c->u.command[1], time_travel);
			//c->status = c->u.command[1]-status;
			close (pipefd[0]);
			exit(c->u.command[0]->status);
		}
		else //parent
		{
			close(pipefd[0]);
			close(pipefd[1]);
			pid_t wait_pid = waitpid(-1, &(c->status), 0);
			//which child exits last determines the status 
			if (wait_pid == pid1)
			{
				waitpid(pid2, &(c->status), 0);
				return;
			}
			if (wait_pid == pid2)
			{
				waitpid(pid1, &(c->status), 0);
				return;
			}
		}
	}
}
void 
execute_sequence_command(command_t c)
{
	execute_command (c->u.command[0], time_travel);
	execute_command (c->u.command[1], time_travel);
	c->status = c->u.command[1]->status;
}
void execute_subshell_command (command_t c)
{
	execute_command (c->u.subshell_command);
	c->status = c->u.subshell_command->status;
}
void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
     if(c == NULL)
     	return;
     if(time_travel ==1)
     	execute_time_travel(c);
     else
     {
     	switch(c->type)
     	{
     		case SIMPLE_COMMAND:
     		execute_simple_command(c);
     		break;
     		case PIPE_COMMAND:
     		printf("pipe;");
     		execute_pipe_command(c);
     		break;
     		default:
     		break;
     	}
     }
}
void
execute_time_travel(command_t c)
{
	return;
}
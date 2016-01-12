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
pid_t pidArray[50];
int countArray=0;
int
command_status (command_t c)
{
  return c->status;
}

void 
execute_time_travel(command_t c);
void 
delete_depend_list (depend_list_t dependList);
void 
modify_depend_list (command_t cmd);

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
		int exe = execvp(c->u.word[0], c->u.word);
		if(exe<0)
			error (1, 0, "Invalid simple command which can not be executed!");
		//printf("Executable!\n");
		//printf("%d\n",c->status);
		exit(c->status);
	}

	//In parent process;
	else if(pid > 0)
	{
		int status;
    // wait for the child process
    if ( (waitpid(pid, &status, 0)) == -1)
      error(1, errno, "Child process error");
    // harvest the execution status
  	//printf("parent: %d\n", status);
    c->status = WEXITSTATUS(status);
    //0 indicates that the command is exited
	}
	else
		error (1, 0, "Error on creating a new process");
}
void 
execute_and_command(command_t c)
{
	command_t left = c->u.command[0];
	command_t right = c->u.command[1];
	execute_command(left, 0);//recursively call the execute command
	if(command_status(left) == 0)
	{
		execute_command(right,0);
		c->status = command_status(right);
	}
	else//the left command is not runned successfully
	{
		//there is no need to execute the right command
		c->status = command_status(left);
	}
}
void 
execute_or_command(command_t c)
{
	command_t left = c->u.command[0];
	command_t right = c->u.command[1];
	execute_command(left, 0);//recursively call the execute command
	if(command_status(left) == 0)
	{	
		//execute_or_command, if left is exited normally, no need to execute the right command
		c->status = command_status(left);
	}
	else//the left command is not runned successfully
	{
		//need to execute the right command
		execute_command(right,0);
		c->status = command_status(right);
	}
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
	execute_command (c->u.subshell_command,time_travel);
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
     		case AND_COMMAND:
     		execute_and_command(c);
     		break;
     		case OR_COMMAND:
     		execute_or_command(c);
     		break;
     		case PIPE_COMMAND:
     		execute_pipe_command(c);
     		break;
     		case SUBSHELL_COMMAND:
     		execute_subshell_command(c);
     		break;
     		case SEQUENCE_COMMAND:
     		execute_sequence_command(c);
     		break;
     		default:
     		break;
     	
     	}
  //error (1, 0, "command execution not yet implemented");
     }
}
void
execute_time_travel(command_t c)
{

	return;
}

void
add_to_waitlist(command_t waited_command, command_t waiting_command)
{
	if(waited_command->waitlist_head == NULL)
  {
    waitlist_t waitlist_new = (waitlist_t)checked_malloc(sizeof(waitlist_t));
    waited_command->waitlist_head = waitlist_new;
    waited_command->waitlist_tail = waitlist_new;
    waitlist_new->cmd = waiting_command;
  }
  else
  {
    waitlist_t waitlist_new = (waitlist_t)checked_malloc(sizeof(waitlist_t));
    waited_command->waitlist_tail->next = waitlist_new;
    waitlist_new->prev = waited_command->waitlist_tail;
    waited_command->waitlist_tail = waitlist_new;
    waited_command->waitlist_tail->next = NULL;
    waited_command->waitlist_tail->cmd = waiting_command;
  }
}
int 
check_dependency (command_t cmd, outfile_t head)
{
	if (cmd == NULL||cmd->head_depend == NULL)
	{
		error(1,0,"Error: invalid command!");
		//printf("do something");
		return -1;
	}
	if(head == NULL)
		return 0;
	cmd->dependency=0;
	//dependency_text[i] should be a char*
	depend_list_t ptr = cmd->head_depend;
	while (ptr!= NULL)	
	{
		outfile_t depend = head;
		while (depend != NULL)
		{
			if (strcmp(ptr->filename,depend->text)==0)//if there exists dependency
			{
				add_to_waitlist(depend->cmd, ptr->cmd);	//(waited command, waiting command)
				cmd->dependency ++;	//has dependency, count+1
				break;	//跳出子循环；
			}
			depend = depend ->next;
		}
		ptr = ptr->next;
	}
	return cmd->dependency;
}
void
wakeup_waitlist(command_t cmd)
{
printf("wake_up\n");
	if (cmd -> waitlist_head == NULL)
	{
		printf("NULL\n");
	return;
	}
	else 
	{
		waitlist_t tmp = cmd->waitlist_head;
		while (tmp != NULL)
		{
			tmp->cmd->dependency --;
			if (tmp->cmd->dependency == 0)
			{
				//execute_command (tmp->cmd, 0 );
				pid_t pid = fork();
			if(pid == 0)
				{
					execute_command(tmp->cmd, 0);
					wakeup_waitlist(tmp->cmd);
					
					exit(tmp->cmd->status);
				}
			else if (pid > 0)
			{
				//;//children[i] = child;
				{
				pidArray[countArray] = pid;
				countArray++;
					//int status;
    			// wait for the child process
    			//if ( (waitpid(pid, NULL, 0)) == -1)
      			//error(1, errno, "Child process error");
    			// harvest the execution status
  				//printf("parent: %d\n", status);
    			//c->status = WEXITSTATUS(status);
    //0 indicates that the command is exited
				}
			}
			else
				error(3, 0, "Cannot create child process.");

				//wakeup_waitlist (tmp->cmd);
			}
			tmp = tmp->next;
		}
		
		return;
	}
}
void 
execute_time_travel_stream(command_stream_t command_stream)
{
	//in the time travel mode, the command_stream is the one passed to the arguments
	command_stream_t track = command_stream_head;
	//pid_t* children = checked_malloc(runnable * sizeof(pid_t));
	//int i = 0;
	
	if(track ==NULL)
	{
		error(1,0, "Error: the command stream is empty.");
		//printf("The command stream is empty\n");
		return;
	}
	
	while(track != NULL)
	{
		modify_depend_list (track->p_command);
		int check = check_dependency(track->p_command,outfile_head); 
		printf("%d\n",check);
		if(check == 0)
		{
			pid_t pid = fork();
			if(pid == 0)
				{
					
					execute_command(track->p_command, 0);
					wakeup_waitlist(track->p_command);
					printf("children\n");
					exit(track->p_command->status);
				}
			else if (pid > 0)
			{
				//;//children[i] = child;
				{
					//int status;
    			// wait for the child process
    			//if ( (waitpid(pid, NULL, 0)) == -1)
      			//printf ("parent");
				pidArray[countArray] = pid;
				countArray++;
				//error(1, errno, "Child process error");
    			// harvest the execution status
  				//printf("parent: %d\n", status);
    			//c->status = WEXITSTATUS(status);
    //0 indicates that the command is exited
			
				}
			}
			else
				error(3, 0, "Cannot create child process.");

			//i++;
			track = track ->next;
		}
		else
			{
			//printf("Has depends\n");
			track = track->next;
			}
	}
	int countloop = 0;
	while (countloop < countArray )
	{
		if (pidArray[countloop] > 0)
		waitpid(pidArray[countloop], NULL, 0);
		countloop++;
	}
	// execute ready list


}
void delete_outfile (outfile_t outfile)
{
	outfile -> next -> prev = outfile ->prev;
	outfile -> prev -> next = outfile ->next;
	free (outfile);	
}

void delete_depend_list (depend_list_t dependList)
{
	dependList ->next ->prev = dependList ->prev;
	dependList -> prev ->next = dependList ->next;
	free (dependList);
}
void modify_depend_list (command_t cmd)
{
	if (cmd->head_depend == NULL)
		error (1, 0, "Error on modifying depend list");
	else 
	{
		depend_list_t tmp = cmd->head_depend;
		while (tmp != NULL)
		{
			if (strcmp (tmp->filename, "-") == 0)
			{
				delete_depend_list (tmp -> next);
				depend_list_t prev = tmp;
				tmp = tmp->next;
				delete_depend_list (prev);
			}
			tmp = tmp ->next;
		}
	}
}
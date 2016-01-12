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


command_t execute_time_travel(command_stream_t s);

void
execute_simple_command (command_t c)
{
  pid_t child = fork();
  if (child == 0)
  {
    int fd_in;
    int fd_out;
    if (c->input != NULL)
    {
      // cmd < file
      if ((fd_in = open(c->input, O_RDONLY, 0666)) == -1)
        error(1, errno, "cannot open input file!");
      if (dup2(fd_in, STDIN_FILENO) == -1)
        error(1, errno, "cannot do input redirect");
      close(fd_in);
    }
    if (c->output != NULL)
    {
      // cmd > file
      //puts(c->output);
      if ((fd_out = open(c->output, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IRGRP|S_IWGRP|S_IWUSR)) == -1)
        error(1, errno, "cannot open output file!");
      if (dup2(fd_out, STDOUT_FILENO) == -1)
        error(1, errno, "cannot do output redirect");
      close(fd_out);
    }
    // handle execution
    if(execvp(c->u.word[0], c->u.word) == -1) // one function that executes command
    {
      //printf("%d",errno);
      if(c->u.word[0][0] != ':') //skip this example
        error(1,errno,c->u.word[0]);
    }
    exit(c->status);
    //error(1, 0, "can't execute command!");
  }

  else if (child > 0)
  {
    int status;
    // wait for the child process
    if ( (waitpid(child, &status, 0)) == -1)
      error(1, errno, "Child process error");
    // harvest the execution status
    c->status = status;
  }

  else
    error(1, 0, "cannot create child process!");
}

void
execute_pipe_command (command_t c)
{
  int pipefd[2];
  pipe(pipefd);
  pid_t child = fork();

  //printf("Child: %d\n", child);

  //child
  if(child == 0)
  {
    close(pipefd[1]);
    int dup = dup2(pipefd[0],0);
    if(dup == -1)
      //printf("error dup==-1\n");
      error(1, errno, "file error");
    execute_command(c->u.command[1],0);
    close(pipefd[0]);
    exit(c->u.command[1]->status);
  }
  //parent
  else if(child > 0)
  {
    pid_t child2 = fork();
    if(child2 == 0)
    {
      close(pipefd[0]);
      int dup = dup2(pipefd[1],1);
      if(dup == -1)
        //printf("error dup==-1\n");
        error(1, errno, "file error");
      execute_command(c->u.command[0],0);
      close(pipefd[1]);
      exit(c->u.command[0]->status);
    }
    else if(child2 > 0)
    {
      close(pipefd[0]);
      close(pipefd[1]);
      int status;
      pid_t wait_pid = waitpid(-1, &status,0);
      if(wait_pid == child)
      {
        c->status = status;
        waitpid(child2,&status,0);
        return;
      }
      else if(wait_pid == child2)
      {
        waitpid(child,&status,0);
        c->status = status;
        return;
      }
    }
    else
      error(1,errno,"Child process error");
  }
  else
    error(1, errno, "forking error");
}

void
execute_and_command (command_t c)
{
  execute_command(c->u.command[0], 0);

  if (command_status(c->u.command[0]) == 0) {
      // run the second command
      execute_command(c->u.command[1], 0);
      // set the status of the AND command
      c->status = c->u.command[1]->status;
  }
  else {
      // do not run c2
      // set the status of the AND command
      c->status = c->u.command[0]->status;
  }
}

void
execute_or_command (command_t c) {
  execute_command(c->u.command[0], 0);
  //puts("TESTOR");

  if (command_status(c->u.command[0]) == 0) {
      c->status = c->u.command[0]->status;
  }
  else {
      execute_command(c->u.command[1], 0);
      c->status = c->u.command[1]->status;
      //printf("%d\n", c->type);
  }
}

void
execute_sequence_command(command_t c) {
  int status;
  pid_t pid = fork();
  if(pid > 0)
  {
    // Parent process
    waitpid(pid, &status, 0);
    c->status = status;
  }
  else if(pid == 0)
  {
    //Child process
    pid = fork();
    if( pid > 0)
    {
      // The child continues
      waitpid(pid, &status, 0);
      execute_simple_command (c->u.command[1]);
      _exit(c->u.command[1]->status);
    }
    else if( pid == 0)
    {
      // The child of the child now runs
      execute_simple_command (c->u.command[0]);
      _exit(c->u.command[0]->status);
    }
    else
      error(1, 0, "Could not fork");
  }
  else
    error(1, 0, "Could not fork");
}

void
execute_subshell_command(command_t c) {
  execute_command(c->u.subshell_command, 0);
  c->status = c->u.subshell_command->status;
}



int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
     if(c == NULL)
      return;
    //printf("Enter%d\n", c->type);
     if (!time_travel)
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
        case SEQUENCE_COMMAND:
          execute_sequence_command(c);
          break;
        case SUBSHELL_COMMAND:
          execute_subshell_command(c);
          break;
        default:
          error(1, 0, "Incorrect Command Type");
      }
     }
}

/////////////////////////////////
//Time travel functions
/////////////////////////////////

/*
word_t create_word_t(char* word)
{
    word_t new_word = checked_malloc(sizeof(word_t));
    new_word->word = word;
    new_word->next = NULL;
    return new_word;
}

void
add_word_dep(word_t words, char* word)
{
    if(strcmp(words->word, word) == 0)
        return;
    else if(words->next == NULL)
    {
        words->next = checked_malloc(sizeof(word_t));
        words->next->word = word;
        words->next->next = NULL;
    }
    else
        add_word_dep(words->next, word);
}

void
add_command_dep(command_t cmd, dc_node_t node)
{
    //if there is an input or output,
    //add the word dep to node
    if(cmd->input != 0)
    {
        if(node->inputs == NULL) //not initiated yet
            node->inputs = create_word_t(cmd->input);
        else
            add_word_dep(node->inputs, cmd->input);
    }

    if(cmd->output != 0)
    {
        if(node->outputs == NULL) //not initiated yet
            node->outputs = create_word_t(cmd->output);
        else
            add_word_dep(node->outputs, cmd->output);
    }

    //add dep from the commands
    int i = 1;
    switch(cmd->type)
    {
        //all these types have two subcommands
        case AND_COMMAND:
        case OR_COMMAND:
        case PIPE_COMMAND:
        case SEQUENCE_COMMAND:
            add_command_dep(cmd->u.command[0], node);
            add_command_dep(cmd->u.command[1], node);
            break;

        //subshell only has one subcommand
        case SUBSHELL_COMMAND:
            add_command_dep(cmd->u.subshell_command, node);
            break;

        //simple has some input dep
        case SIMPLE_COMMAND:
            while(cmd->u.word[i] != NULL)
            {
                if(node->inputs == NULL) //not initiated yet
                    node->inputs = create_word_t(cmd->u.word[i]);
                else
                    add_word_dep(node->inputs, cmd->u.word[i]);
                i++;
            }
            break;
    }

}

void
append_dc_dep(dc_node_t top, dc_node_t add)
{
    dep_node_t temp = top->dependents;
    dep_node_t last = temp;
    while(temp != NULL)
    {
        last = temp;
        temp = temp->next;
    }
    dep_node_t new_dep = checked_malloc(sizeof(struct dep_node));
    new_dep->dep = add;
    new_dep->next = NULL;
    if(last == NULL)
        top->dependents = new_dep;
    else
        last->next = new_dep;
}

void
compare_dc_nodes(word_t input, word_t output, dc_node_t new_dep, dc_node_t old_cmd)
{
    //compare all outputs to all inputs
    word_t cur_out = output;
    while(cur_out != NULL)
    {
        word_t cur_in = input;
        while(cur_in != NULL)
        {
            //if there is a match, add new_dep dep to old_cmd
            if(strcmp(cur_in->word, cur_out->word) == 0)
            {
                new_dep->num_dependencies += 1;
                append_dc_dep(old_cmd, new_dep);
                return;
            }

            cur_in = cur_in->next;
        }
        cur_out = cur_out->next;
    }
}

command_t
execute_time_travel(command_stream_t s)
{
    dc_node_t dep_graph_head = NULL;
    command_t command = NULL;
    command_t last_command = NULL;
    //read all commands; create dependency graph.
    while((command = read_command_stream_t(s)))
    {
        dc_node_t new_node = checked_malloc(sizeof(struct dc_node));
        new_node->c = command;
        new_node->inputs = NULL;
        new_node->outputs = NULL;
        new_node->num_dependencies = 0;
        new_node->dependents = NULL;
        new_node->pid = -1;

        //add dependencies from command
        add_command_dep(command, new_node);

        dc_node_t last_node = dep_graph_head;
        dc_node_t cur_node  = dep_graph_head;
        //go through the whole graph
        while(cur_node != NULL)
        {
            //if a dep is found, add it
            compare_dc_nodes(cur_node->inputs, new_node->outputs, new_node, cur_node);
            compare_dc_nodes(new_node->inputs, cur_node->outputs, new_node, cur_node);
            last_node = cur_node;
            cur_node = cur_node->next;
        }

        if(last_node == NULL)
            dep_graph_head = new_node;
        else
            last_node->next = new_node;

        last_command = command;
        //printf("%d \n", dep_graph_head->c->type);

    }

    //printf("%d \n", dep_graph_head->c->type);

    //execute commands from the dependency graph

    //Continue to grab the next on the graph
    while(dep_graph_head != NULL)
    {
        dc_node_t cur_node = dep_graph_head;
        while(cur_node != NULL)
        {
          //If there are no dep on it and not running
          if(cur_node->num_dependencies == 0 && cur_node->pid < 1)
          {
            //fork and execute, indicate its pid
            int pid = fork();
            if(pid == -1)
                error(1, errno, "forking error");
            else if( pid == 0)
            {
                execute_command(cur_node->c,0);
                exit(cur_node->c->status);
            }
            else if( pid > 0)
            {
                cur_node->pid = pid;
            }
          }

          cur_node = cur_node->next;
        }

        int status;
        //Get the pid of a finished process
        pid_t fin_pid = waitpid(-1, &status, 0);

        // Use pid to determine who finished and remove them
        dc_node_t prev_node = NULL;
        dc_node_t fin_node = dep_graph_head;
        while(fin_node != NULL)
        {
          // If they're not waiting on anyone
          if(fin_node->pid == fin_pid)
          {
            dep_node_t cur_dep = fin_node->dependents;
            // for all on the list of dependents
            while(cur_dep != NULL)
            {
              dc_node_t freed_node = cur_dep->dep;
              // free that dependent (reduce dependencies)
              freed_node->num_dependencies -= 1;

              cur_dep = cur_dep->next;
            }

            // remove from the list
            if(prev_node == NULL)
              dep_graph_head = fin_node->next;
            else
              prev_node->next = fin_node->next;
            break;
          }

          prev_node = fin_node;
          fin_node = fin_node->next;
        }

    }

    delete_command_stream_t(s);
    return last_command;
}*/
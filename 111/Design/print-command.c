// UCLA CS 111 Lab 1 command printing, for debugging

#include "command.h"
#include "command-internals.h"

#include <stdio.h>
#include <stdlib.h>

static void
command_indented_print (int indent, command_t c)
{
  //printf ("test command_indented print0");
  switch (c->type)
    {
    case IF_COMMAND:
    case WHILE_COMMAND:
    case FOR_COMMAND:
    case UNTIL_COMMAND:
      printf ("%*s%s\n", indent, "",
        (c->type == IF_COMMAND ? "if"
         : c->type == UNTIL_COMMAND ? "until" 
         : c->type == WHILE_COMMAND ? "while" : "for"));
      command_indented_print (indent + 2, c->u.command[0]);
      printf ("\n%*s%s\n", indent, "", c->type == IF_COMMAND ? "then" : "do");
      command_indented_print (indent + 2, c->u.command[1]);
      if (c->type == IF_COMMAND && c->u.command[2])
  {
    printf ("\n%*selse\n", indent, "");
    command_indented_print (indent + 2, c->u.command[2]);
  }
      printf ("\n%*s%s", indent, "", c->type == IF_COMMAND ? "fi" : "done");
      break;
    case AND_COMMAND:
    case SEQUENCE_COMMAND:
    case OR_COMMAND:
    case PIPE_COMMAND:
      {
	command_indented_print (indent + 2 * (c->u.command[0]->type != c->type),
				c->u.command[0]);
	static char const command_label[][3] = { "&&", ";", "||", "|" };
	printf (" \\\n%*s%s\n", indent, "", command_label[c->type]);
 // printf ("test command_indented print1");
	command_indented_print (indent + 2 * (c->u.command[1]->type != c->type),
				c->u.command[1]);
	break;
      }

    case SIMPLE_COMMAND:
      {
	char **w = c->u.word;
	printf ("%*s%s", indent, "", *w);
  //printf ("test command_indented print2");
	while (*++w)
	  printf (" %s", *w);
	break;
      }

    case SUBSHELL_COMMAND:
      printf ("%*s(\n", indent, "");
       // printf ("test command_indented print3");
      command_indented_print (indent + 1, c->u.subshell_command);
      printf ("\n%*s)", indent, "");
      break;
    case NOT_COMMAND:
      printf ("%*s!\n", indent, "");
      command_indented_print (indent + 2, c->u.command[0]);
      break;
    default:
      abort ();
    }

  if (c->input)
    printf ("<%s", c->input);
  //printf ("test command_indented print4");
  if (c->output)
    printf (">%s", c->output);
}

void
print_command (command_t c)
{
  command_indented_print (2, c);
  putchar ('\n');
}

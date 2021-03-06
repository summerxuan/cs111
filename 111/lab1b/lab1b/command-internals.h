// UCLA CS 111 Lab 1 command internals
#include "alloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define STACK_SIZE 20
enum command_type
  {
    AND_COMMAND,         // A && B
    SEQUENCE_COMMAND,    // A ; B
    OR_COMMAND,          // A || B
    PIPE_COMMAND,        // A | B
    SIMPLE_COMMAND,      // a simple command
    SUBSHELL_COMMAND,    // ( A )
  };

// Data associated with a command.
struct command
{
  enum command_type type;

  // Exit status, or -1 if not known (e.g., because it has not exited yet).
  int status;

  // I/O redirections, or 0 if none.
  char *input;
  char *output;

  union
  {
    // for AND_COMMAND, SEQUENCE_COMMAND, OR_COMMAND, PIPE_COMMAND:
    struct command *command[2];

    // for SIMPLE_COMMAND:
    char **word;

    // for SUBSHELL_COMMAND:
    struct command *subshell_command;
  } u;
};

//token
  enum token_type
{
  HEAD,   // used for dummy head of token lists
    SUBSHELL,
    LEFT_DERCTION,
    RIGHT_DERCTION,
    AND,
    OR,
    PIPELINE,
    SEMICOLON,
    WORD
};
typedef struct token* token_t;
struct token
{
  enum token_type t_type;//which indicated the token type
  int line;
  char *text;
  token_t prev;//which linked to the previous token node
  token_t next;//which linked to the next token node
}token;
//typedef struct command *command_t;
typedef struct token_stream *token_stream_t;


struct token_stream
{
  token_t head;
  token_t tail;
  token_stream_t next;
}token_stream;//the linked list of node

struct command_stream
{
  command_t p_command;
  command_stream_t next;//pointer to next command_stream
  command_stream_t prev;
};

//stack
typedef struct Stack
{
  int top;
  command_t data[STACK_SIZE];

}Stack;
typedef struct Stack* Stack_t;

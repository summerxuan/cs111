#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include <stddef.h>
#include "error.h"
#define STACK_SIZE 20

struct command_stream
{
	command_t p_command;
	command_stream_t next;//pointer to next command_stream
	command_stream_t prev;
};
command_stream_t command_stream_head = NULL;
command_stream_t command_stream_tail = NULL;

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
token_t create_token_with_type(enum token_type t_type, char* text, int line)
//this function which initialize the token node
{
	token_t new_token = (token_t)checked_malloc(sizeof(token));
	new_token->t_type = t_type;
	new_token->text = text;
	new_token->line = line;
	return new_token;
}
int isword(char c){
        return isalpha(c) || isdigit(c) || (c == '!') || (c == '%') || (c == '+') || (c == ',')
                        || (c == '-') || (c == '.') || (c == '/') || (c == ':') || (c == '@') || (c == '^')
                        || (c == '_');
}

token_stream_t convert_buffer_to_token_stream(char* buffer, size_t size)
{
//in this function convert buffer to the token stream, which is a linked list
	token_t head_token = create_token_with_type(HEAD, NULL, 0);//which created the dummy token 
  	token_t curr_token = head_token;
  	token_t prev_token = NULL;

  	token_stream_t head_stream = checked_malloc(sizeof(token_stream_t));
  	token_stream_t curr_stream = head_stream;
  	curr_stream->head = head_token;
  	curr_stream->tail = head_token;//now the head and tail both points to the head_token

  	int line = 1;
  	size_t index = 0;
  	char c = *buffer;
  	while(index<size)
  	{  	
  		if(isword(c) )//if c is word
  		{
  			size_t word_length = 5 ;
  			size_t word_position = 0;
  			char *word = (char*)checked_malloc(sizeof(word_length));
  			while(isword(c))
  			{
  				word[word_position] = c;//assign the c to corresponding position
  				if(word_position == word_length)//if reaches the length
  				{
  					word_length*=2;
  					word = checked_grow_alloc(word,&word_length);//resize the word length
  				}
  			word_position++;index++;buffer++;c = *buffer;//update the char and index
  			}
  			token_t now = create_token_with_type(WORD,word,line);
  			curr_token ->next = now;
  			prev_token = curr_token;//assign the previous token to now
  			curr_token = curr_token->next;
  			curr_token->prev = prev_token;
  			prev_token->next = curr_token;
  		}
  		else if(c == '\n')//if c is the new line
  		{
  			line++;
  			if(curr_token->t_type ==LEFT_DERCTION||curr_token->t_type ==RIGHT_DERCTION)
  				//LEFT direction and right direction cannot be followed by new line
  				{
  					error(2, 0, "Line %d: Newline cannot follow redirects.", line);
        			return NULL;
        		}
        	//if the current token type is word or subshell which indicated a new command
        	if(curr_token->t_type==WORD||curr_token->t_type==SUBSHELL)
        		{//create a new token stream
        			curr_stream->next = checked_malloc(sizeof(token_stream));
          			curr_stream = curr_stream->next;
          			curr_stream->head = create_token_with_type(HEAD, NULL, -1);
          			curr_token = curr_stream->head;
      			}
      		//else just treat is as white space
      		index++;buffer++;c = *buffer;
  		}
  		else if(c == ' ' || c =='\t')
  		{
  			index++;buffer++;c = *buffer;
  			//treated it as white space
  		}
  		else if(c == '<')//left direction
  		{
  			curr_token->next = create_token_with_type(LEFT_DERCTION,NULL,line);
			prev_token = curr_token;//assign the previous token to now
  			curr_token = curr_token->next;
  			curr_token->prev = prev_token;
  			prev_token->next = curr_token;
      		buffer++; index++; c = *buffer;
    	}
    	else if (c == '>') // RIGHT REDIRECT
    	{
      		curr_token->next = create_token_with_type(RIGHT_DERCTION, NULL, line);
      		prev_token = curr_token;//assign the previous token to now
  			curr_token = curr_token->next;
  			curr_token->prev = prev_token;
  			prev_token->next = curr_token;
      		buffer++; index++; c = *buffer;
    	}
    	else if (c == ';') // SEQUENCE command
    	{
      		curr_token->next = create_token_with_type(SEMICOLON, NULL, line);
      		prev_token = curr_token;//assign the previous token to now
  			curr_token = curr_token->next;
  			curr_token->prev = prev_token;
  			prev_token->next = curr_token;
      		buffer++; index++; c = *buffer;
    	}
    	else if (c == '&') // and
    	{
    		if(buffer[1]!='&')
    		{
    			error(2, 0, "Line %d: Syntax error. & must be followed by &", line);
        		return NULL;
    		}
    		else if(buffer[1]=='&')
    		{
      			curr_token->next = create_token_with_type(AND, NULL, line);
      			prev_token = curr_token;//assign the previous token to now
  				curr_token = curr_token->next;
  				curr_token->prev = prev_token;
  				prev_token->next = curr_token;
      			buffer++; index++; c = *buffer;
      		}
    	}
    	else if (c == '|') // OR or PIPELINE
    	{
    		if(buffer[1]==' ')
    		{
    			curr_token->next = create_token_with_type(PIPELINE, NULL, line);
      			prev_token = curr_token;//assign the previous token to now
  				curr_token = curr_token->next;
  				curr_token->prev = prev_token;
  				prev_token->next = curr_token;
      			buffer++; index++; c = *buffer;
    		}
    		else if(buffer[1]=='|')
    		{
      			curr_token->next = create_token_with_type(OR, NULL, line);
      			prev_token = curr_token;//assign the previous token to now
  				curr_token = curr_token->next;
  				curr_token->prev = prev_token;
  				prev_token->next = curr_token;
      			buffer++; index++; c = *buffer;
      		}
      	/*	else
    		{
    			error(2, 0, "Line %d: Syntax error. | only can be followed by | or  ", line);
        		return NULL;
    		}*/
    	}
    	if (c == '(') // SUBSHELL
    	{
      		int subshell_line = line;
      		int nested = 1;

      		size_t count = 0;
      		size_t subshell_size = 64;
      		char* subshell = checked_malloc(subshell_size);

      		// grab contents until subshell is closed
      		while (nested > 0)
      		{
        		buffer++; index++; c = *buffer;
        		//to examine the next char
        		if (index == size)
        			{
          				error(2, 0, "Line %d: Syntax error. EOF reached before subshell was closed.", subshell_line);
          				return NULL;
        			}

        		if (c == '\n')
        		{
          			// consume all following whitespace
          			while (buffer[1] == ' ' || buffer[1] == '\t' || buffer[1] == '\n')
          			{
            			if (buffer[1] == '\n')
              			line++;

            		buffer++;
            		index++;
          			}

          			// pass semicolon
          			c = ';';
          			line++;
        		}
        		else if (c == '(') // count for nested subshells
          			nested++;
        		else if (c == ')') // close subshell
        		{
          			nested--;

          		if (nested == 0) // break if outermost subshell is closed
          		{
            		buffer++; index++; c = *buffer; // consume last close parens
            		break;
          		}
        		}

        // load into subshell buffer
        subshell[count] = c;
        count++;

        // expand subshell buffer if necessary
        if (count == subshell_size)
        {
          subshell_size = subshell_size * 2;
          subshell = checked_grow_alloc (subshell, &subshell_size);
        }
      }

      // create subshell token
      curr_token->next = create_token_with_type(SUBSHELL, subshell, subshell_line);
      curr_token = curr_token->next;
    }
    else if (c == ')') // CLOSE PARENS
    {
      error(2, 0, "Line %d: Syntax error. Close parens found without matching open parens.", line);
      return NULL;
    }

  	}
  	return head_stream;
}

//in this function we build command tree in each token_thread


typedef struct Stack
{
	int top;
	command_t data[STACK_SIZE];

}Stack;
typedef struct Stack* stack_t;
Stack* initStack ()
{
	Stack *s = (Stack *) malloc(sizeof (Stack));
	s->top = 0;
	int i = 0;
	while(i<STACK_SIZE)
	{
		s->data[i] = NULL;
		i++;
	}
	return s;
}
command_t top(Stack *s)
{
	return s->data[s->top-1];//return the top command_t 
}
int push(Stack *s, command_t newCommand)
{
  if(s->top == STACK_SIZE-1)
    return -1;
  s->top+=1;
  s->data[s->top] = newCommand;
  return 0;
}
command_t pop(Stack *s)   //pop the top
{
  if (s->top == 0)   //empty stack
  {
    return NULL;
  }
  command_t out = s->data[s->top];
  s->top -= 1;
  return out;
}
int size(Stack *s)
{
  return s->top;
}
int is_empty (Stack *s)
{
  return s->top == 0;
}
int make_command_branch (command_t ops, Stack *s)
{
  if (size(s) < 2)
    return 0;
  command_t rightChild = pop(s);
  command_t leftChild = pop(s);
  //splild two branches
  ops->u.command[0] = leftChild;
  ops->u.command[1] = rightChild;
  //push new command back to stack
  push(s, ops);
  return 1;
}

command_t make_command_tree(token_stream_t t)
{
  token_t sub_head = t->head;//the sub_head is t->head 
  token_t token_now = sub_head;
  stack_t operators = (stack_t)checked_malloc(sizeof(Stack));
  stack_t operands = (stack_t)checked_malloc(sizeof(Stack));//build two stacks to hold operands and operators
  command_t comm;//one command
  command_t prev = NULL;
  int line = token_now->line; 
  while(token_now!=NULL)
  {
    if( !(token_now->t_type == LEFT_DERCTION || token_now->t_type == RIGHT_DERCTION) )
      {
        // make new command
        comm = checked_malloc(sizeof( struct command ));
      }
    switch(token_now->t_type)
    
    {
      case AND:
      comm->type = AND_COMMAND;
      while (  !is_empty(operators) &&( top(operators)->type == PIPE_COMMAND || top(operators)->type == OR_COMMAND || top(operators)->type == AND_COMMAND ))
          {
            command_t pop_ops = pop(operators);//pop until the top ops has smaller presendence than
            if (!make_command_branch(pop_ops, operands))
              {
                error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", line);
                return NULL;
              }
          }
          push(operators,comm);//push ADD command to ops
          break;

        case OR:
      comm->type = OR_COMMAND;
      while (  !is_empty(operators) &&( top(operators)->type == PIPE_COMMAND || top(operators)->type == OR_COMMAND || top(operators)->type == AND_COMMAND ))
          {
            command_t pop_ops = pop(operators);//pop until the top ops has smaller presendence than
            if (!make_command_branch(pop_ops, operands))
              {
                error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", line);
                return NULL;
              }
          }
          push(operators,comm);//push ADD command to ops
          break;

        // push OR to ops
        push(operators,comm);
        break;

        case PIPELINE:
      comm->type = PIPE_COMMAND;
      while (  !is_empty(operators) && top(operators)->type == PIPE_COMMAND )
          {
            command_t pop_ops = pop(operators);//pop until the top ops has smaller presendence than
            if (!make_command_branch(pop_ops, operands))
              {
                error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", line);
                return NULL;
              }
          }
          push(operators,comm);//push PIPE command to ops
          break;

        case SEMICOLON:
          comm->type = SEQUENCE_COMMAND;

        // always pop since SEMICOLON <= all ops
          while (!is_empty(operators))
          {
            command_t pop_ops = pop(operators);
              if (!make_command_branch(pop_ops, operands))
              {
                error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", line);
                return NULL;
              }
          }

        // push SEMICOLON to ops
          push(operators,comm);
          break;

        case WORD:
          comm->type = SIMPLE_COMMAND;
          //seprated by word
          int num_words = 1;
          token_t count = token_now;
          while(count!=NULL&&count->next!=NULL&&count->next->t_type==WORD)//while next is still word
          {
            count = count ->next;
            num_words++;
          }
          comm->u.word[0] = token_now->text;
          int i =1;
          while(i< num_words)
          {
            token_now = token_now->next;
            comm->u.word[i] = token_now->text;
            i++;
            //if(i == num_words)
          }
          comm->u.word[num_words] = NULL;
          push(operators,comm);
          break;
        case SUBSHELL:
        comm->type = SUBSHELL_COMMAND;

        // process subshell command tree
        comm->u.subshell_command = 
        make_command_tree(
          convert_buffer_to_token_stream(token_now->text, strlen(token_now->text))
          );

        // push SUBSHELL tree to operands
        push(operands, comm);
        break;

      case LEFT_DERCTION:
        // check that previous command is a subshell or word
        if (prev== NULL || !(prev->type == SIMPLE_COMMAND || prev->type == SUBSHELL_COMMAND))
        {
          error(2, 0, "Line %d: Syntax error. ONLY words and SUBSHELL can follow redirection", line);
          return NULL;
        }
        else if (prev->output != NULL)
        {
          error(2, 0, "Line %d: Syntax error. Previous command already has output. ", line);
          return NULL;
        }
        else if (prev->input != NULL)
        {
          error(2, 0, "Line %d: Syntax error. Previous command already has input.", line);
          return NULL;
        }

        token_now= token_now->next;
        if (token_now->t_type == WORD) // followed by a word
        {
          //which is valid input
            prev->input = token_now->text;
        }
        else
        {
          error(2, 0, "Line %d: Syntax error. Redirects must be followed by words.", line);
          return NULL;
        }

        // no pushing required
        break;

        case RIGHT_DERCTION:
        // check that previous command is a subshell or word
        if (prev== NULL || !(prev->type == SIMPLE_COMMAND || prev->type == SUBSHELL_COMMAND))
        {
          error(2, 0, "Line %d: Syntax error. ONLY words and SUBSHELL can follow redirection", line);
          return NULL;
        }
        else if (prev->output != NULL)
        {
          error(2, 0, "Line %d: Syntax error. Previous command already has output. ", line);
          return NULL;
        }
        else if (prev->input != NULL)
        {
          error(2, 0, "Line %d: Syntax error. Previous command already has input.", line);
          return NULL;
        }

        token_now= token_now->next;
        if (token_now->t_type == WORD) // followed by a word
        {
          //which is valid output
            prev->output = token_now->text;
        }
        else
        {
          error(2, 0, "Line %d: Syntax error. Redirects must be followed by words.", line);
          return NULL;
        }

        // no pushing required
        break;
        default:
        break;
      };
      prev = comm;
  }

  while(size(operators) > 0)
    {
      command_t pop_ops = pop(operators);

      if (!make_command_branch(pop_ops, operands))
      {
          error(2, 0, "Line %d: Syntax error. Not enough children to create new tree.", line);
          return NULL;
      }
    }

  // check for single root
    if (size(operands) != 1)
    {
      error(2, 0, "Line %d: Syntax error. Tree did not converge into single root.", line);
      return NULL;
    }

    command_t root = pop(operands); // the root should be the final tree left in operands

    return root;
}

command_stream_t init_command_stream(command_t new_command_t)
{
  command_stream_t cmd_stream = (command_stream_t)checked_malloc(sizeof(command_stream_t));
  cmd_stream->p_command = new_command_t;
  cmd_stream->next = NULL;
  cmd_stream->prev =NULL;
  return cmd_stream;
}

void add_command_to_stream(command_t new_command_t )
{
  //if the link list is empty
  if(command_stream_tail == NULL)
  {
    command_stream_tail = init_command_stream(new_command_t);
    command_stream_head = command_stream_tail;
   // command_stream_s->p_command = new_command_t;
    //command_stream_head = command_stream_s;
    //command_stream_tail = command_stream_s;
    //command_stream_head = NULL;
    //command_stream_s->prev = NULL;
  }
  else
  {
    command_stream_t command_stream_new = init_command_stream(new_command_t);
    //command_stream_head->next = command_stream_new;
    command_stream_tail->next = command_stream_new;
    command_stream_new->prev = command_stream_tail;
    command_stream_tail = command_stream_tail->next;
    command_stream_tail->next = NULL;
    //command_stream_new->p_command = new_command_t;
    //command_stream_tail = command_stream_new;
  }
}


command_stream_t make_command_stream (int (*get_next_byte) (void *), void *get_next_byte_arg)
{
  size_t count = 0;
  size_t buffer_size = 1000;
  char* buffer = checked_malloc(buffer_size);
  //create a buffer of charactors to store the readin chars
  // create buffer of input with comments stripped out
  char ch;
  do
  {
    ch = get_next_byte(get_next_byte_arg);

    if (ch == '#') // for comments: ignore everything until '\n' or EOF
    {
      ch = get_next_byte(get_next_byte_arg);
      while(ch != EOF && ch!='\n')
        ch = get_next_byte(get_next_byte_arg);
    }
    //skip all the words until \n
    if (ch!= -1)
    {
      // load into buffer
      buffer[count] = ch;
      count++;

      // expand buffer if necessary
      //if (count == INT_MAX)
      //{
      //  error(1, 0, "Line -1: Input size over INT_MAX.");
      //}
      if (count == buffer_size)
      {
        buffer_size = buffer_size * 2;
        buffer = checked_grow_alloc (buffer, &buffer_size);
      }
    }
  } while(ch != -1);
  token_stream_t return_point = convert_buffer_to_token_stream(buffer,count);
  //build command_tree
  //nuild the first command_tree
  //command_stream_t command_stream_first = init_command_stream ();
  command_t new_command_t = make_command_tree (return_point);
  add_command_to_stream(new_command_t );
  //build the rest command_tree
  while (return_point->next != NULL)
  {
    //make return_point point to next token stream
    token_stream_t temp = return_point->next;
    //free(return_point);
    return_point = temp;
    new_command_t = make_command_tree (return_point);
    add_command_to_stream(new_command_t);
  }
  return command_stream_head;
}
command_t read_command_stream (command_stream_t s)
{
   if (s == NULL || s->p_command == NULL)
    return NULL;

  // grab the current command
  command_t return_command = s->p_command;
  if (s->next != NULL)
    {
    command_stream_t next = s->next;
    s->p_command = s->next->p_command;
    s->next = s->next->next;

    //free(next); // free used node
  }
  else
    s->p_command= NULL;

  return return_command;
}

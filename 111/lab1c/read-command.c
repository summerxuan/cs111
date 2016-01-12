// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include <error.h>
#include "alloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */


//stack functions
Stack* 
initStack ()
{
  Stack *s = (Stack *) checked_malloc(sizeof (Stack));
  s->top = 0;
  int i = 0;
  while(i<STACK_SIZE)
  {
    s->data[i] = NULL;
    i++;
  }
  return s;
}
void 
freeStack(Stack *s)
{
    free (s->data);
    free (s);
}
command_t 
top(Stack *s)
{
  return s->data[s->top-1];//return the top command_t 
}
int 
push(Stack *s, command_t newCommand)
{
  if(s->top == STACK_SIZE-1)
    return -1;
  
  s->data[s->top] = newCommand;
  s->top+=1;
  return 0;
}
command_t 
pop(Stack *s)   //pop the top
{
  if (s->top == 0)   //empty stack
  {
    return NULL;
  }
  s->top -= 1;
  command_t out = s->data[s->top];
  
  return out;
}
int 
size(Stack *s)
{
  return s->top;
}
int 
is_empty (Stack *s)
{
  return s->top == 0;
}

//make command stream functions
int 
make_command_branch (command_t ops, Stack *s)
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

token_t 
create_token_with_type(enum token_type t_type, char* text, int line)
//this function which initialize the token node
{
  token_t new_token = (token_t)checked_malloc(sizeof(token_t));
  new_token->t_type = t_type;
  new_token->text = text;
  new_token->line = line;
  return new_token;
}
int 
isword(char c){
        return isalpha(c) || isdigit(c) || (c == '!') || (c == '%') || (c == '+') || (c == ',')
                        || (c == '-') || (c == '.') || (c == '/') || (c == ':') || (c == '@') || (c == '^')
                        || (c == '_');
}

depend_list_t
initDependlist(char* filename,command_t comm)
{
  depend_list_t new_depend =(depend_list_t) checked_malloc(sizeof(struct depend_list));
  new_depend->filename = filename;
  new_depend->next = NULL;
  new_depend->prev = NULL;
  new_depend->cmd = comm;
  return new_depend;
}
void
add_filename_to_dependlist(char* filename, command_t comm)
{
  
  if(comm->tail_depend== NULL)
  {
    comm->tail_depend = initDependlist(filename,comm);
    comm->head_depend= comm->tail_depend;
  }
  else
  {
    depend_list_t depend_new = initDependlist(filename,comm);
    //command_stream_head->next = command_stream_new;
    comm->tail_depend->next = depend_new;
    depend_new -> prev = comm->tail_depend;
    comm->tail_depend = comm->tail_depend->next;
    comm->tail_depend->next = NULL;
  }
}

outfile_t
add_out_list (char* word, command_t cmd)
{
  if(outfile_tail == NULL)
  {
    outfile_t outfile_new = (outfile_t)checked_malloc(sizeof(outfile_t));
    outfile_tail = outfile_new;
    outfile_head = outfile_tail;
    outfile_new->text = word;
    outfile_new -> cmd = cmd;
    cmd -> outfile_ptr = outfile_new;
    return outfile_new;
  }
  else
  {
    outfile_t outfile_new = (outfile_t)checked_malloc(sizeof(outfile_t));
    outfile_tail->next = outfile_new;
    outfile_new->prev = outfile_tail;
    outfile_tail = outfile_new;
    outfile_tail->next = NULL;
    outfile_new->text = word;
    outfile_new -> cmd = cmd;
    cmd -> outfile_ptr = outfile_new;
    return outfile_new;
  }
}

token_stream_t 
convert_buffer_to_token_stream(char* buffer, size_t size)
{
//in this function convert buffer to the token stream, which is a linked list
  token_t head_token = create_token_with_type(HEAD, NULL, 0);//which created the dummy token 
    token_t curr_token = head_token;
    token_t prev_token = NULL;

    token_stream_t head_stream = (token_stream_t)checked_malloc(sizeof(token_stream_t));
    token_stream_t curr_stream = head_stream;
    curr_stream->head = head_token;
    curr_stream->tail = head_token;//now the head and tail both points to the head_token

    int line = 1;
    size_t index = 0;
    char c = *buffer;
    while(index<size)
    {   
      //printf("index=%d, size = %d",(int)index,(int)size);
      if(isword(c) )//if c is word
      {
        size_t word_length = 5 ;
        size_t word_position = 0;
        char *word = (char*)checked_malloc(sizeof(word_length));
        while(isword(c))
        {
          word[word_position] = c;//assign the c to corresponding position
        //  printf("test convert_buffer_to_token_stream1\n");
       // printf("c=%c \n",c);
          if(word_position == word_length)//if reaches the length
          {
            word_length*=2;
            word = (char*)checked_realloc(word,word_length);//resize the word length
          }
        word_position++;index++;buffer++;c = *buffer;//update the char and index
        
        }
        token_t now = create_token_with_type(WORD,word,line);
        curr_token ->next = now;
        prev_token = curr_token;//assign the previous token to now
        curr_token = curr_token->next;
        curr_token->prev = prev_token;
        prev_token->next = curr_token;
        curr_token->next = NULL;
        
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
            index++;
          if (index == size)
          {
          //  printf("break!" );
            break;
          }
          else
           { buffer++;c = *buffer;}
          if(curr_token->t_type==WORD||curr_token->t_type==SUBSHELL)
            {//create a new token stream
          curr_stream->next = (token_stream_t)checked_malloc(sizeof(token_stream_t));
                curr_stream = curr_stream->next;
                curr_stream->head = create_token_with_type(HEAD, NULL, -1);
                curr_token = curr_stream->head;
            }
          //else just treat is as white space
                 // printf("test convert_buffer_to_token_stream2 \n");
      //printf("c=%c \n",c);
          

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
        curr_token->next = NULL;
       // printf("test convert_buffer_to_token_stream3 \n");
   // printf("c=%c \n",c);
          buffer++; index++; c = *buffer;
      }
      else if (c == '>') // RIGHT REDIRECT
      {
          curr_token->next = create_token_with_type(RIGHT_DERCTION, NULL, line);
          prev_token = curr_token;//assign the previous token to now
        curr_token = curr_token->next;
        curr_token->prev = prev_token;
        prev_token->next = curr_token;
        curr_token->next = NULL;
      //  printf("test convert_buffer_to_token_stream3 \n");
   // printf("c=%c \n",c);
          buffer++; index++; c = *buffer;
      }
      else if (c == ';') // SEQUENCE command
      {
          curr_token->next = create_token_with_type(SEMICOLON, NULL, line);
          prev_token = curr_token;//assign the previous token to now
        curr_token = curr_token->next;
        curr_token->prev = prev_token;
        prev_token->next = curr_token;
        curr_token->next = NULL;
      //  printf("test convert_buffer_to_token_stream3 \n");
   // printf("c=%c \n",c);
          buffer++; index++; c = *buffer;
    
      }
       
      else if (c == '&') // and
      {
    //    printf("test convert_buffer_to_token_stream4 \n");
    //  printf("c=%c \n",c);
        buffer++; //my code 
        index++;
        c=*buffer;
        if(buffer[0]!='&')
        {
          error(2, 0, "Line %d: Syntax error. & must be followed by &", line);
            return NULL;
        }
        else if(buffer[0]=='&')
        {
            curr_token->next = create_token_with_type(AND, NULL, line);
            prev_token = curr_token;//assign the previous token to now
          curr_token = curr_token->next;
          curr_token->prev = prev_token;
          prev_token->next = curr_token;
          curr_token->next = NULL;
      //    printf("test convert_buffer_to_token_stream4 \n");
    //  printf("c=%c \n",c);
            buffer++; index++; c = *buffer;
          }
      
      }
      else if (c == '|') // OR or PIPELINE
      {
   //     printf("test convert_buffer_to_token_stream4 \n");
    // printf("c=%c \n",c);
        buffer++;
        index++;
        c=*buffer;
        
        if(buffer[0]=='|')
        {
            curr_token->next = create_token_with_type(OR, NULL, line);
            prev_token = curr_token;//assign the previous token to now
          curr_token = curr_token->next;
          curr_token->prev = prev_token;
          prev_token->next = curr_token;
          curr_token->next = NULL;
       //   printf("test convert_buffer_to_token_stream5 \n");
   //   printf("c=%c \n",c);
            buffer++; index++; c = *buffer;
          }
          else
        {
          curr_token->next = create_token_with_type(PIPELINE, NULL, line);
            prev_token = curr_token;//assign the previous token to now
          curr_token = curr_token->next;
          curr_token->prev = prev_token;
          prev_token->next = curr_token;
          curr_token->next = NULL;
            //buffer++; index++; c = *buffer;
        }
        /*  else
        {
          error(2, 0, "Line %d: Syntax error. | only can be followed by | or  ", line);
            return NULL;
        }*/
      }
      else if (c == '(') // SUBSHELL
      {
          int subshell_line = line;
          int nested = 1;

          size_t count = 0;
          size_t subshell_size = 64;
          char* subshell = (char*)checked_malloc(subshell_size);

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
// printf("test convert_buffer_to_token_stream6 \n");
 //printf("c=%c \n",c);
        // load into subshell buffer
        subshell[count] = c;
        count++;

        // expand subshell buffer if necessary
        if (count == subshell_size)
        {
          subshell_size = subshell_size * 2;
          subshell = (char*)checked_realloc (subshell, subshell_size);
        }
      //  printf("test convert_buffer_to_token_stream7 \n");
     //   printf("c=%c \n",c);
      }

      // create subshell token
      curr_token->next = create_token_with_type(SUBSHELL, subshell, subshell_line);
     // curr_token = curr_token->next;
      prev_token = curr_token;//assign the previous token to now
          curr_token = curr_token->next;
          curr_token->prev = prev_token;
          prev_token->next = curr_token;
          curr_token->next = NULL;
    }
    else if (c == ')') // CLOSE PARENS
    {
      error(2, 0, "Line %d: Syntax error. Close parens found without matching open parens.", line);
      return NULL;
    }
		else 
	{
		error(2, 0, "Line %d: Syntax error. Unrecognized character in script. ", line);
		return NULL;
	}
    }

  //  printf("return headstream");
    return head_stream;
}

static int 
get_next_byte (FILE *stream)
{
  return getc (stream);
}

command_t 
make_command_tree(token_stream_t t)
{
 
  token_t sub_head = t->head;//the sub_head is t->head 
  token_t token_now = sub_head->next;
  Stack_t operators =initStack();
  Stack_t operands =initStack();//build two stacks to hold operands and operators
  command_t comm;//one command
  command_t prev = NULL;
  int line = token_now->line; 
  int counter=0;
  //if(token_now->next == NULL)
   // printf("Heere\n");
  while(token_now!=NULL)
  {
    counter++;
   // printf("the %d time loop, the type is %d", counter,token_now->t_type);
    if( !(token_now->t_type == LEFT_DERCTION || token_now->t_type == RIGHT_DERCTION) )
      {
        // make new command
        comm = (command_t)checked_malloc(sizeof( struct command ));
       // add_filename_to_dependlist(token_now->text,comm);
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
          token_now = token_now->next;
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
          token_now = token_now->next;
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
          token_now = token_now->next;
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
          token_now = token_now->next;
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
          comm->u.word = (char**)checked_malloc((num_words+1)*sizeof(char**));
          comm->u.word[0] = token_now->text;
          int i =1;
          while(i< num_words)
          {
            token_now = token_now->next;
            comm->u.word[i] = token_now->text;
           // add_filename_to_dependlist(comm->u.word[i],comm);
            i++;
            //if(i == num_words)
          }
          comm->u.word[num_words] = NULL;
          push(operands,comm);
          token_now = token_now->next;
          break;

        case SUBSHELL:
        comm->type = SUBSHELL_COMMAND;

        // process subshell command tree
        token_stream_t subshell_token = convert_buffer_to_token_stream(token_now->text, strlen(token_now->text));
        command_t subshell_command = make_command_tree(subshell_token);
        comm->u.subshell_command = subshell_command;

        // push SUBSHELL tree to operands
        push(operands, comm);
        token_now = token_now->next;
        break;

      case LEFT_DERCTION:
        // check that previous command is a subshell or word
        if (prev== NULL || !(prev->type == SIMPLE_COMMAND || prev->type == SUBSHELL_COMMAND))
        {
         error(2, 0, "Line %d: Syntax error. ONLY words and SUBSHELL can follow redirection", line);
          return NULL;
        }
       /* else if (prev->output != NULL)
        {
          error(2, 0, "Line %d: Syntax error. Previous command already has output. ", line);
          return NULL;
        }*/
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
           // add_filename_to_dependlist(prev->input,comm);
        }
        else
        {
          error(2, 0, "Line %d: Syntax error. Redirects must be followed by words.", line);
          return NULL;
        }
        token_now= token_now->next;
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
        /*else if (prev->input != NULL)
        {
          //error(2, 0, "Line %d: Syntax error. Previous command already has input.", line);
          return NULL;
        }*/

        token_now= token_now->next;
        if (token_now->t_type == WORD) // followed by a word
        {
          //which is valid output
            prev->output = token_now->text;
            //add output word to outfile_list
           // outfile_t new_outfile = add_out_list (prev);
           // prev->outfile_ptr = new_outfile;
        }
        else
        {
          error(2, 0, "Line %d: Syntax error. Redirects must be followed by words.", line);
          return NULL;
        }

        // no pushing required
        token_now= token_now->next;
        break;
        default:
        token_now = token_now->next;
        break;
      };
     prev = comm;
    // printf("the previous command is: %d\n", prev->type);
  }
  //printf("end first while");
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
void 
add_command_to_stream(command_t new_command_t )
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
    command_stream_tail = command_stream_new;
    command_stream_tail->next = NULL;
    //command_stream_new->p_command = new_command_t;
    //command_stream_tail = command_stream_new;
  }
}

void
build_depend_list(command_t comm, token_stream_t token_stream)
{
  //command_stream_t cmd_stream = command_stream_head;

  if(comm == NULL)
    printf("NUll command\n");
  else
      {
       //the sub_head is t->head 
       //token_t token_now = sub_head->next;
        token_t curr_node = token_stream->head->next;
      while(curr_node != NULL)
      {
        if(curr_node->t_type == RIGHT_DERCTION)
         { add_out_list (curr_node->next->text, comm);
          curr_node = curr_node->next;
         }
        else if(curr_node->text != NULL)
        {
          add_filename_to_dependlist(curr_node->text, comm);
        }
        curr_node = curr_node->next;
      }
    //token_stream = token_stream->next;
    //cmd_stream = cmd_stream->next;
  }
}
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  size_t buffer_size=1000;
  char* buffer = (char* )checked_malloc(buffer_size);
  size_t count = 0;
  char ch;

  do
  {
    ch = get_next_byte(get_next_byte_argument);

    if (ch == '#') // for comments: ignore everything until '\n' or EOF
    {
      ch = get_next_byte(get_next_byte_argument);
      while(ch != EOF && ch!='\n')
        ch = get_next_byte(get_next_byte_argument);
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
        buffer = (char*)checked_realloc (buffer, buffer_size);
      }
    }
  } while(ch != -1);

  token_stream_t return_token_stream = convert_buffer_to_token_stream(buffer, count);
  command_t new_command_t = make_command_tree (return_token_stream);
  build_depend_list(new_command_t,return_token_stream);
   // printf("make tree end\n");
  add_command_to_stream(new_command_t );
 // printf("add first command to stream\n");
 // printf("command type : %d\n", new_command_t->type);
  //if(new_command_t == NULL)
   // printf("HAHA");
 // print_command(new_command_t);
    while (return_token_stream->next != NULL)
  {
    //make return_point point to next token stream
    return_token_stream = return_token_stream->next;
    //free(return_point);
    //return_token_stream = temp;
    new_command_t = make_command_tree (return_token_stream);
    build_depend_list(new_command_t,return_token_stream);
    add_command_to_stream(new_command_t);
   // printf("add another command to stream\n");
    //print_command(new_command_t);
  }
//  printf("return to main\n");
 // print_command()
  return command_stream_head;

}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
    if (s == NULL || s->p_command == NULL)
    return NULL;

  // grab the current command
  command_t return_command = s->p_command;
  if (s->next != NULL)
    {
    //command_stream_t next = s->next;
    s->p_command = s->next->p_command;
    s->next = s->next->next;

  }
  else
    s->p_command= NULL;

  return return_command;
  //return 0;

}

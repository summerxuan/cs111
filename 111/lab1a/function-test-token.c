#include "alloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

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



enum token_type
{
	HEAD = 0,   // used for dummy head of token lists
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
	token_t new_token = (token_t)malloc(sizeof(token_t));
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

  	token_stream_t head_stream = (token_stream_t)malloc(sizeof(token_stream_t));
  	token_stream_t curr_stream = head_stream;
  	curr_stream->head = head_token;
  	curr_stream->tail = head_token;//now the head and tail both points to the head_token

  	int line = 1;
  	int index = 0;
  	char c = *buffer;
  	while(index<(int)size)
  	{  	
      printf("index=%d, size = %d",(int)index,(int)size);
  		if(isword(c) )//if c is word
  		{
  			size_t word_length = 5 ;
  			size_t word_position = 0;
  			char *word = (char*)malloc(sizeof(word_length));
  			while(isword(c))
  			{
  				word[word_position] = c;//assign the c to corresponding position
          printf("test convert_buffer_to_token_stream1\n");
        printf("c=%c \n",c);
  				if(word_position == word_length)//if reaches the length
  				{
  					word_length*=2;
  					word = (char*)realloc(word,word_length);//resize the word length
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
  					//error(2, 0, "Line %d: Newline cannot follow redirects.", line);
        			return NULL;
        		}
        	//if the current token type is word or subshell which indicated a new command
            index++;
          if (index == size)
          {
            printf("break!" );
            break;
          }
          else
           { buffer++;c = *buffer;}
        	if(curr_token->t_type==WORD||curr_token->t_type==SUBSHELL)
        		{//create a new token stream
					curr_stream->next = (token_stream_t)malloc(sizeof(token_stream_t));
          			curr_stream = curr_stream->next;
          			curr_stream->head = create_token_with_type(HEAD, NULL, -1);
          			curr_token = curr_stream->head;
      			}
      		//else just treat is as white space
                  printf("test convert_buffer_to_token_stream2 \n");
      printf("c=%c \n",c);
      		

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
        printf("test convert_buffer_to_token_stream3 \n");
    printf("c=%c \n",c);
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
        printf("test convert_buffer_to_token_stream3 \n");
    printf("c=%c \n",c);
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
        printf("test convert_buffer_to_token_stream3 \n");
    printf("c=%c \n",c);
      		buffer++; index++; c = *buffer;
    
    	}
       
    	else if (c == '&') // and
    	{
        printf("test convert_buffer_to_token_stream4 \n");
      printf("c=%c \n",c);
        buffer++; //my code 
        index++;
        c=*buffer;
    		if(buffer[0]!='&')
    		{
    			//error(2, 0, "Line %d: Syntax error. & must be followed by &", line);
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
          printf("test convert_buffer_to_token_stream4 \n");
      printf("c=%c \n",c);
      			buffer++; index++; c = *buffer;
      		}
    	
      }
    	else if (c == '|') // OR or PIPELINE
    	{
        printf("test convert_buffer_to_token_stream4 \n");
      printf("c=%c \n",c);
        buffer++;
        index++;
        c=*buffer;
    		if(buffer[0]==' ')
    		{
    			curr_token->next = create_token_with_type(PIPELINE, NULL, line);
      			prev_token = curr_token;//assign the previous token to now
  				curr_token = curr_token->next;
  				curr_token->prev = prev_token;
  				prev_token->next = curr_token;
          curr_token->next = NULL;
      			//buffer++; index++; c = *buffer;
    		}
    		else if(buffer[0]=='|')
    		{
      			curr_token->next = create_token_with_type(OR, NULL, line);
      			prev_token = curr_token;//assign the previous token to now
  				curr_token = curr_token->next;
  				curr_token->prev = prev_token;
  				prev_token->next = curr_token;
          curr_token->next = NULL;
          printf("test convert_buffer_to_token_stream5 \n");
      printf("c=%c \n",c);
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
      		char* subshell = (char*)malloc(subshell_size);

      		// grab contents until subshell is closed
      		while (nested > 0)
      		{
        		buffer++; index++; c = *buffer;
        		//to examine the next char
        		if (index == size)
        			{
          				//error(2, 0, "Line %d: Syntax error. EOF reached before subshell was closed.", subshell_line);
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
 printf("test convert_buffer_to_token_stream6 \n");
 printf("c=%c \n",c);
        // load into subshell buffer
        subshell[count] = c;
        count++;

        // expand subshell buffer if necessary
        if (count == subshell_size)
        {
          subshell_size = subshell_size * 2;
          subshell = (char*)realloc (subshell, subshell_size);
        }
        printf("test convert_buffer_to_token_stream7 \n");
        printf("c=%c \n",c);
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
      //error(2, 0, "Line %d: Syntax error. Close parens found without matching open parens.", line);
      return NULL;
    }
  	}

    printf("return headstream");
  	return head_stream;
}

static int get_next_byte (FILE *stream)
{
  return getc (stream);
}

main()
{
	char* script_name = "test.sh";
  FILE *script_stream = fopen (script_name, "r");

	int buffer_size=1000;
	char* buffer = (char* )malloc(buffer_size);
	size_t count = 0;
	char ch;

	do
  {
    ch = get_next_byte(script_stream);

    if (ch == '#') // for comments: ignore everything until '\n' or EOF
    {
      ch = get_next_byte(script_stream);
      while(ch != EOF && ch!='\n')
        ch = get_next_byte(script_stream);
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
        buffer = (char*)realloc (buffer, buffer_size);
      }
    }
  } while(ch != -1);

	token_stream_t return_token_stream = convert_buffer_to_token_stream(buffer, count);
  if(return_token_stream==NULL)
    printf("NULL pointer\n");
  else
    {
      if(return_token_stream->head == NULL)
        printf("NULL head\n");
      else
        { 
          printf("head is not null\n" );
          printf("%d\n",return_token_stream->head->t_type);
          printf("%c\n",return_token_stream->head->next->text[0]);
          if(return_token_stream->head->next->next != NULL)
            {
              printf("linked");
              printf("%d\n",return_token_stream->head->next->next->t_type);
            }
        }
    }
  if(return_token_stream->next==NULL)
    printf("NEXT is NULL pointer\n");
  else
    {
      if(return_token_stream->next->head == NULL)
        printf("NEXT NULL head\n");
      else
        { 
          printf("head is not null\n" );
          printf("%d\n",return_token_stream->next->head->t_type);
          printf("%c\n",return_token_stream->next->head->next->text[0]);
          if(return_token_stream->next->head->next->next->next == NULL)
            printf("YEAH");
        }
    }
	printf("main end");
}
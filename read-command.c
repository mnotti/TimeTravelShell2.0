// UCLA CS 111 Lab 1 command reading
#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

//defining struct command_streamas TA specified...
struct commandNode{
  struct command* command; //forms the root of the tree
  struct command* next;
};

typedef struct command_stream{
  struct commandNode* head;
  struct commandNode* tail;
  struct commandNode* cursor;
}command_stream;

//finished TA's given definition

void
print_token_type(token_t t)
{
	switch(t.type)
	{
		case AND:
			printf("AND\n");
			break;
		case OR:
			printf("OR\n");
			break;
		case GREATER_THAN:
			printf("GREATER GREATER_THAN\n");
			break;
		case LESS_THAN:
			printf("LESS THAN\n");
			break;
		case PIPE:
			printf("PIPE\n");
			break;
		case SEMICOLON:
			printf("SEMICOLON\n");
			break;
		case NEWLINE:
		        printf("NEWLINE\n");
			break;
		case LEFT_PARENTHESIS:
			printf("LEFT_PARENTHESIS\n");
			break;
		case RIGHT_PARENTHESIS:
			printf("RIGHT_PARENTHESIS\n");
			break;
		case WORD_TOKEN:
			printf("WORD_TOKEN\n");
			printf("%s \n", t.token_word);
			break;
		case UNKNOWN_TOKEN:
			printf("UNKNOWN_TOKEN\n");
			break;
		default:
			break;
	}
}


//all the stack ish
void
stackPush(stackList* stackPtr, struct token* data)
{
	printf("line num of token being pushed:%i\n", data->line_num);

	//creates the node
	struct stackNode* newNode = (struct stackNode*) malloc(sizeof(struct stackNode));
    if(newNode == NULL)
        printf("problem allocating memory for node\n");
    newNode->tok = data;	
    newNode->next = NULL;
	//base case (Stack is empty)
	if (stackPtr->tail == NULL)
	{
		printf("stack is empty (tail is null)\n");

    	newNode->prev = NULL;

    	stackPtr->head = newNode;
    	stackPtr->tail = newNode;
	}
	//if stack is not empty
	else
	{
		stackPtr->tail->next = newNode;
		newNode->prev = stackPtr->tail;
		stackPtr->tail = newNode;
	}
}

//popfunction
struct token*	
stackPop(stackList* stackPtr)
{
	if (stackPtr->tail == NULL)
		printf("error: tried popping empty stack\n");
	else if (stackPtr->tail->prev == NULL)	
	{
		struct stackNode* temp = stackPtr->tail;
		stackPtr->tail = NULL;
		stackPtr->head = NULL;
		struct token* tempTok = (temp->tok);	
		free(temp);
		return tempTok;	
	}
	else
	{
		struct stackNode* temp = stackPtr->tail;
		stackPtr->tail = stackPtr->tail->prev;
		stackPtr->tail->next = NULL;
		struct token* tempTok = (temp->tok);	
		free(temp);
		return tempTok;
	}
	return NULL;
}

struct token*	
stackTop(stackList* stackPtr)
{
	if (stackPtr->tail == NULL)
		printf("stack is empty\n");
	else
	{
		return (stackPtr->tail->tok);
	}
	return NULL;
}

void
displayDataFromTopOfStack(stackList* stackPtr)
{
	printf("top of stack begins now...\n");
	struct stackNode* it = stackPtr->tail;
	while (it != NULL)
	{
		printf("line number of token = %i\n", it->tok->line_num);	//prints line number to see if token is properly stored in list
		it = it->prev;
	}
	printf("bottom of stack ^^^\n");
}






int
is_valid_word_char(char c)
{
	if (isalpha(c) || isdigit(c))
		return 1;
	switch(c)
	{
		case '!':
		case '%':
		case '+':
		case ',':
		case '-':
		case '.':
		case '/':
		case ':':
		case '@':
		case '^':
		case '_':
			return 1;
		default:
			return 0;
	}
}

/*
enum token_type
check_char_type(char c)
{
	if (isalpha(c) || isdigit(c))
		return WORD_TOKEN;

}
*/

// TODO: Lines not working, easy to implememnt, not sure if we need it though
token_t*
tokenize(char* string, size_t len, size_t *token_array_size)
{
	size_t token_buff_max = 20; // TODO: Change buff size
	size_t token_buff_max_bytes = token_buff_max * sizeof(token_t);
	token_t *token_buff = checked_malloc(token_buff_max_bytes);
	size_t num_tokens = 0;
	size_t pos = 0;
	int line = 0;
	char token_char;
	while (pos < len)
	{
		token_char = string[pos];
		if (is_valid_word_char(token_char))
		{
			int start = pos;
			while (is_valid_word_char(token_char) && pos < len)
			{
			  pos++;
			  token_char = string[pos];
			}
			int token_word_size = pos - start;
			token_buff[num_tokens].token_word = checked_malloc((sizeof(char) * token_word_size) + sizeof(char));
			int count = 0;
				while (start < (int)pos)
			{
			  token_buff[num_tokens].token_word[count] = string[start];
			  count++;
			  start++;
			}
			token_buff[num_tokens].token_word[count] = '\0';
			token_buff[num_tokens].type = WORD_TOKEN;
			token_buff[num_tokens].line_num = line;
			num_tokens++;
		}
		else if (token_char == ' ' || token_char == '\t')
			pos++;
		else
		{
			switch(token_char)
			{
				case '\n':
					token_buff[num_tokens].type = NEWLINE;
					token_buff[num_tokens].token_word = NULL;
					num_tokens++;
					pos++;
					break;
				case ';':
					token_buff[num_tokens].type = SEMICOLON;
					token_buff[num_tokens].token_word = NULL;
					num_tokens++;
					pos++;
					break;
				case '>':
					token_buff[num_tokens].type = GREATER_THAN;
					token_buff[num_tokens].token_word = NULL;
					num_tokens++;
					pos++;
					break;
				case '<':
					token_buff[num_tokens].type = LESS_THAN;
					token_buff[num_tokens].token_word = NULL;
					num_tokens++;
					pos++;
					break;
				case '(':
					token_buff[num_tokens].type = RIGHT_PARENTHESIS;
					token_buff[num_tokens].token_word = NULL;
					num_tokens++;
					pos++;
					break;
				case ')':
					token_buff[num_tokens].type = RIGHT_PARENTHESIS;
					token_buff[num_tokens].token_word = NULL;
					num_tokens++;
					pos++;
					break;
				case '|':
					if (pos+1 >= len)
					{
						token_buff[num_tokens].type = PIPE;
						pos++;
					}
					else
					{
						if (string[pos+1] == '|')
						{
							token_buff[num_tokens].type = OR;
							pos += 2;
						}
						else
						{
							token_buff[num_tokens].type = PIPE;
							pos++;
						}
					}
					token_buff[num_tokens].token_word = NULL;
					num_tokens++;
					break;
				case '&':
					if (pos+1 >= len)
					{
						token_buff[num_tokens].type = UNKNOWN_TOKEN;
						pos++;
					}
					else
					{
						if (string[pos+1] == '&')
						{
							token_buff[num_tokens].type = AND;
							pos += 2;
						}
						else
						{
							token_buff[num_tokens].type = UNKNOWN_TOKEN;
							pos++;
						}
					}
					token_buff[num_tokens].token_word = NULL;
					num_tokens++;
					break;
				default:
					token_buff[num_tokens].type = UNKNOWN_TOKEN;
					num_tokens++;
					pos++;
					break;
			}
		}
		if (num_tokens >= token_buff_max)
		{
		  token_buff = checked_grow_alloc(token_buff, &token_buff_max_bytes);
		  token_buff_max = token_buff_max_bytes/(sizeof(token_t));
		}
	}

	// TODO: Handle if the buffer is empty
	*token_array_size = num_tokens;
	return token_buff;
}


// Get the string buffer from the input
char*
get_string(void* get_next_byte_arguement, int (*get_next_byte) (void *), size_t* buflen)
{
  char c;
  size_t pos = 0;
  size_t blen = 512;
  char* buff =  checked_malloc(blen);

  while (( c = get_next_byte(get_next_byte_arguement)) != EOF) {
	// Reallocate memory if needed
	if (pos >= blen) {
	  buff = checked_grow_alloc(buff, &blen);
	}
	buff[pos] = c;
	pos++;
  }

  //pos -= 1; //CHANGED (MARKUS) ... BUFFER LENGTH WAS 1 TOO LONG // CHANGED (KYLE) ... NO IT WASN'T
  *buflen = pos;
  printf("%zd\n", pos);
  return buff;
  
}


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
			 void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
	 add auxiliary functions and otherwise modify the source code.
	 You can also use external functions defined in the GNU C Library.  */
  
  
  size_t bufflen = 0;
  char * inputString = get_string(get_next_byte_argument, get_next_byte, &bufflen);
  size_t i;

  //allocating stacks
  stackList opStack;
  opStack.head = NULL;
  opStack.tail = NULL;

  stackList comStack;		
  comStack.head = NULL;
  comStack.tail = NULL;

  struct token token1;
  stackPush(&opStack, &token1);	//TODO: so compiler won't complain about unused stack vars
  stackPush(&comStack, &token1);



  //prints input string for reference
  for (i = 0; i < bufflen; i++) {
	printf("%c", inputString[i]);
  }
  printf("\n");

  //  ERROR TESTING FOR TOKENIZING WORDS
  size_t token_array_size;
  token_t *t = tokenize(inputString, bufflen, &token_array_size);

  int j;
  for (j = 0; j < (int)token_array_size; j++)
  {
  	print_token_type(t[j]);
  }
  
  printf("%i",(int)token_array_size);
 	error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;

  if (s) {} //TODO: So the compiler doesn't flip out
}



  		////////////////////////////////////
  		//STACK TESTING AND SAMPLE USAGE
  		/////////////////////////////////////
  		//testing stack
  		/*struct token token1;
  		token1.line_num = 1;

  		struct token token2;
  		token2.line_num = 2;

  		struct token token3;
  		token3.line_num = 3;

  		stackPush(&opStack, &token1);
		stackPush(&opStack, &token2);
		stackPush(&opStack, &token3);

		displayDataFromTopOfStack(&opStack);	//prints line nums from tokens from top to bottom of stack
		struct token* poppedToken = stackPop(&opStack);		//TODO: change to command* or token*
		displayDataFromTopOfStack(&opStack);

		printf("popped token's line = %i\n", poppedToken->line_num);	
		*/
		////////////////////////////////////

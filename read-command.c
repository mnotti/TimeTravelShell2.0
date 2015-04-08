// UCLA CS 111 Lab 1 command reading
#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <ctype.h>

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


//all the stack ish
void
stackPush(stackList* stackPtr, struct token* data)
{
	printf("line num of token being pushed:%i\n", data->line_num);

	//creates the node
	struct stackNode* newNode = (struct stackNode*) malloc(sizeof(struct stackNode));
    if(newNode == NULL)
        printf("problem allocating memory for node\n");
    newNode->tok = data;	//TODO: change to command* or token*
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
struct token*	//TODO: change to command* or token*
stackPop(stackList* stackPtr)
{
	if (stackPtr->tail == NULL)
		printf("error: tried popping empty stack\n");
	else if (stackPtr->tail->prev == NULL)	
	{
		struct stackNode* temp = stackPtr->tail;
		stackPtr->tail = NULL;
		stackPtr->head = NULL;
		struct token* tempTok = (temp->tok);	//TODO: change to command* or token*
		free(temp);
		return tempTok;	//TODO: change to command* or token*
	}
	else
	{
		struct stackNode* temp = stackPtr->tail;
		stackPtr->tail = stackPtr->tail->prev;
		stackPtr->tail->next = NULL;
		struct token* tempTok = (temp->tok);	//TODO: change to command* or token*
		free(temp);
		return tempTok;
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

void
tokenize(char* string, size_t len)
{
  //token_arr = checked_alloc;
	if (*string || len) {} //TODO: FIX
}


// Get the string buffer from the input
char*
get_string (void* get_next_byte_arguement, int (*get_next_byte) (void *), size_t* buflen)
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

  //cycles thru input string
  for (i = 0; i < bufflen; i++) {
    char c = inputString[i];
    printf ("%zd\n", i);
    switch(c)
      {
      case ' ':
	printf("space!\n");
	break;
      case ';':
	printf("semi-colon (END_OF_LINE\n");
	break;
      case '(':
	printf("open-parenthesis (BEGIN_SUBSHELL\n");
	break;
      case ')':
	printf("close-parenthesis (END_SUBSHELL\n");
	break;
      case '<':
	printf("less than (INPUT\n");
	break;
      case '>':
	printf("greater than (OUTPUT\n");
	break;
      case '|':
	if (inputString[i+1] == '|')
	  {
	    printf("double bars(OR_COMMAND)\n");
	    i++;
	  }
	else
	  {
	    printf("PIPE!\n");
	  }
	break;
      case '&':
	if (inputString[i+1] == '&')
	  {
	    printf("double ampersands(AND_COMMAND)\n");
	    i++;
	  }
	break;
	case '\n':
		printf("endline\n");
		break;
      default:
        if (isalpha(c))
	  {
	    // printf("it's a letter\n");
	    int tempBufLen = 12;
	    char *tempBuf = malloc( sizeof(char) * (12) );  //allocate buffer for each individual word made of digits,
	    //letters, and _s
	    int j = 0;
	    while (isalpha(inputString[i]) || isdigit(inputString[i]) || inputString[i] == '_')
	      {
		if (tempBufLen == j)
		  {
		    tempBuf = realloc(tempBuf,  sizeof(char) * (tempBufLen * 2));
		    tempBufLen *= 2;
		  }
		tempBuf[j] = inputString[i];
		j++;
		i++;
	      }
	    int k;
	    for (k = 0; k < tempBufLen; k++)
	      {
		char d = tempBuf[k];
		printf("%c", d);
	      }
	    printf("\n");
	    free(tempBuf);
	    i--;
	  }
	else if (isdigit(c))
	  {
	    printf("it's a digit\n");
	  }
	else
	  {
	    printf("%zd\n", i);
	    printf("something else?\n");//TODO: this prints after the entire array has been traversed (bad access?)
	  }
	break;
      }
  }

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

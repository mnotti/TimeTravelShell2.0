// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <stdio.h>
#include <stdlib.h>

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

//linked list for STACK
struct stackNode
{
    struct command* command;
    struct stackNode *next;
};

//messing with stack functions
void
push(struct stackNode* stackNode, struct command* command)
{
	malloc
}


//reading chars into buffer
char*
get_string (void* get_next_byte_arguement, int (*get_next_byte) (void *), size_t* buflen)
{
  char c;
  size_t pos = 0;
  size_t blen = 512;
  char* buff = malloc(blen);

  while (( c = get_next_byte(get_next_byte_arguement)) != EOF) {
    if (pos >= blen) {
      blen *= 2;
      buff = realloc(buff, blen);
    }
    buff[pos] = c;
    pos++;
  }
  pos -= 1; //CHANGED (MARKUS) ... BUFFER LENGTH WAS 1 TOO LONG
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

  //creating stacks
  struct stackNode* operatorStack;
  operatorStack->command = NULL;
  operatorStack->next = NULL;
  struct stackNode* commandStack;
  commandStack->command = NULL;
  commandStack->next = NULL;




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

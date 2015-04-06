// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <stdio.h>
#include <stdlib.h>

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
  *buflen = pos;
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
  char * string = get_string(get_next_byte_argument, get_next_byte, &bufflen);
  size_t i;
  for (i = 0; i < bufflen; i++) {
    printf("%c", string[i]);
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

// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

void
handle_and_command(command_t c, bool time_travel)
{
	//execute the first command
	execute_command(c->u.command[0], time_travel);
	c->status = WEXITSTATUS(c->u.command[0]->status);

	//if the first statement was succesfully executed
	if (c->status == 0)
	{
		execute_command(c->u.command[1], time_travel);
		c->status = WEXITSTATUS(c->u.command[1]->status);
	}
}


int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, bool time_travel)
{
	switch (c.type){
		case AND_COMMAND:  
			handle_and_command(c, time_travel);
			break;
     
    	case SEQUENCE_COMMAND: 
    		handle_sequence_command(c, time_travel);
    		break;
  
    	case OR_COMMAND: 
    		handle_or_command(c, time_travel);
    		break;
       
     	case PIPE_COMMAND:  
     		handle_pipe_command(c, time_travel);
     		break;
   
     	case SIMPLE_COMMAND:
     		handle_simple_command(c, time_travel);
     		break;

   		case SUBSHELL_COMMAND:  
   			handle_subshell_command(c, time_travel);
   			break;
   		default:
   			printf("Error!! An unidentified command has been passed into execute switch statement!\n");
   			break;
  
	}

  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  error (1, 0, "command execution not yet implemented");
  if (c || time_travel) {} // TODO: So the compiler doesn't flip out
}

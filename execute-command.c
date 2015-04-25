// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <string.h>


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */


void handle_simple_command(command_t c, bool time_travel)
{
	if (time_travel)
	{}

	pid_t pid;
	// child
	if (!(pid = fork()))
	{
		// Handle Input and Output
		int fd;
		if (c->input)
		{
			if ((fd = open(c->input, O_RDONLY, 0777)) < 0)
			{
				fprintf(stderr, "ERROR: Failed to open file for input\n");
				exit(1);
			}
			
			if (dup2(fd, 0) < 0)
			{
				fprintf(stderr, "ERROR: Failed to copy file descriptor for input\n");
				exit(1);
			}

			close(fd);
		}
		if (c->output)
		{
			if ((fd = open(c->output, O_CREAT | O_TRUNC | O_WRONLY, 0777)) < 0)
			{
				fprintf(stderr, "ERROR: Failed to open file for output\n");
				exit(1);
			}
			if (dup2(fd, 1) < 0)
			{
				fprintf(stderr, "ERROR: Failed to copy file descriptor for output\n");
				exit(1);
			}

			close(fd);
		}

		//if command starts with exec, ignore exec
		if (!strcmp(c->u.word[0], "exec"))
		{
			execvp(c->u.word[1], &(c->u.word[1]));
			
		}
		execvp(c->u.word[0], c->u.word);

	}
	// error
	else if (pid == -1)
	{
		fprintf(stderr, "ERROR: Fork Failed\n");
		exit(0); // TODO: Correct exit status?
	}
	// parent
	else 
	{
		int status;
		waitpid(pid, &status, 0);
		c->status = WEXITSTATUS(status);
	}
}

void
handle_and_command(command_t c, bool time_travel)
{
	if (time_travel)
	{}

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

void
handle_or_command(command_t c, bool time_travel)
{
	if (time_travel)
	{}

	execute_command(c->u.command[0], time_travel);
	c->status = WEXITSTATUS(c->u.command[0]->status);
	if (c->status != 0)
	{
		execute_command(c->u.command[1], time_travel);
		c->status = WEXITSTATUS(c->u.command[1]->status);
	}
}

void
handle_subshell_command(command_t c, bool time_travel)
{
	if (c && time_travel)
	{}
	
	execute_command(c->u.subshell_command, time_travel);
	c->status = WEXITSTATUS(c->u.subshell_command->status);

}

void
handle_sequence_command(command_t c, bool time_travel)
{
	if (c && time_travel)//TODO
	{}
	
	execute_command(c->u.command[0], time_travel);
	execute_command(c->u.command[1], time_travel);
	c->status = WEXITSTATUS(c->u.command[1]->status);
}

void
handle_pipe_command(command_t c, bool time_travel)
{
	if (c && time_travel)//TODO
	{}

	int fd[2];
	pipe(fd);
	int pid1;
	int pid2;

	if (!(pid1 = fork()))	//if child
	{
			close(fd[1]);	//close read end
			if ((dup2(fd[0], 0)) < 0)
			{
				fprintf(stderr, "ERROR: Failed to copy file descriptor for output: pipe\n");
				exit(1);
			}
			execute_command(c->u.command[1], time_travel);
			c->status = WEXITSTATUS(c->u.command[1]->status);
			//close(fd[1]);

	}
	else	//parent
	{
		if (!(pid2 = fork()))
		{
			
			close(fd[0]);	//close the write-end of pipe
			if ((dup2(fd[1], 1)) < 0)
				{
					fprintf(stderr, "ERROR: Failed to copy file descriptor for input: pipe\n");
					exit(1);
				}

			execute_command(c->u.command[0], time_travel);
			c->status = WEXITSTATUS(c->u.command[0]->status);
		}
		else
		{
			close(fd[0]);
			close(fd[1]);

			int status;

			waitpid(-1, &status, 0);	
			waitpid(-1, &status, 0);
			c->status = status;

		}
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
	switch (c->type){
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
  //error (1, 0, "command execution not yet implemented");
  if (c || time_travel) {} // TODO: So the compiler doesn't flip out
}
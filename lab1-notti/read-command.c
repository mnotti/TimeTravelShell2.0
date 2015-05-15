// UCLA CS 111 Lab 1 command reading
// MARKUS NOTTI 904-269-231
// KYLE BAKER 604-273-748

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


/*
TODO LIST: 
Handle the case for all functions where the input is empty
Fix newline counter in tokenize function (tells what line each token is on)
Free token array created by tokenize function (can probably do this after we make the token pointer array)
Free token pointer array and each token array created by token ptr array function
Change the buffer size in the tokenize functions to be something more realistic.
	small is good for testing reallocation

a >b < correctly finds the error, but outputs 0: ERROR: TWO OPPERATORS NEXT TO EACH OTHER
	because it gets the input as a >b <; So it works, but its not outputting the right
	line number, not a big deal I don't think.
*/

//GOT A SEG FAULT LIKE WTF?
/*(a&&b ||
	c&&
			d | e && f | g
a && b)*/





//finished TA's given definition

void displayDataFromTopOfStack(stackListOp* stackPtr);
void displayDataFromTopOfStackCom(stackListCom* stackPtr);





// Higher precedence returns a higher number, invalid input returns -1  //DOPE
int
operator_precedence(token_t t)
{
	switch(t.type)
	{
		case PIPE:
			return 2;
		case AND:
		case OR:
			return 1;
		case SEMICOLON:
			return 0;
		case LEFT_PARENTHESIS:
			return -2;	//for easier subshell creation
		default:
			return -1;
	}
}

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
		case TWO_GREATER_THAN:					//NEW
			printf("TWO GREATER THANS\n");
			break;
		case LESS_AND:							//NEW
			printf("LESS AND\n");
			break;
		case GREATER_AND:						//NEW
			printf("GREATER AND\n");
			break;
		case LESS_GREATER:						//NEW
			printf("LESS GREATER\n");
			break;
		case GREATER_OR:						//NEW
			printf("GREATER OR\n");
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
		case COMMENT:
			printf("COMMENT\n");
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
stackPush(stackListOp* stackPtr, token_t* data)
{
	//printf("line num of token being pushed:%i\n", data->line_num);

	//creates the node
	struct stackNodeOp* newNode = (struct stackNodeOp*) malloc(sizeof(struct stackNodeOp));		//TODO: maybe free???
    if(newNode == NULL)
        printf("problem allocating memory for node\n");
    newNode->tok = data;	
    newNode->next = NULL;
	//base case (Stack is empty)
	if (stackPtr->tail == NULL)
	{
		//printf("stack is empty (tail is null)\n");

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
	//printf("result of OPPush\n");
	//displayDataFromTopOfStack(stackPtr);

}

void
stackPushCom(stackListCom* stackPtr, struct command* data)
{
	//creates the node
	struct stackNodeCom* newNode = (struct stackNodeCom*) malloc(sizeof(struct stackNodeCom));
    if(newNode == NULL)
        printf("problem allocating memory for node\n");
    newNode->com = data;	
    newNode->next = NULL;
	//base case (Stack is empty)
	if (stackPtr->tail == NULL)
	{
		//printf("stack is empty (tail is null)\n");

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
	//printf("result of ComPush\n");
	//displayDataFromTopOfStackCom(stackPtr);

}

//popfunction
token_t*	
stackPop(stackListOp* stackPtr)
{
	if (stackPtr->tail == NULL)
		printf("error: tried popping empty stack\n");
	else if (stackPtr->tail->prev == NULL)	
	{
		struct stackNodeOp* temp = stackPtr->tail;
		stackPtr->tail = NULL;
		stackPtr->head = NULL;
		token_t* tempTok = (temp->tok);	
		free(temp);
		//displayDataFromTopOfStack(stackPtr);
		//printf("result of OPPop\n");

		return tempTok;	
	}
	else
	{
		struct stackNodeOp* temp = stackPtr->tail;
		stackPtr->tail = stackPtr->tail->prev;
		stackPtr->tail->next = NULL;
		token_t* tempTok = (temp->tok);	
		free(temp);
		//printf("result of OPPop\n");
		//displayDataFromTopOfStack(stackPtr);

		return tempTok;
	}
	return NULL;
}

//TODO: not tested
struct command*	
stackPopCom(stackListCom* stackPtr)
{
	if (stackPtr->tail == NULL)
		printf("error: tried popping empty stack\n");
	else if (stackPtr->tail->prev == NULL)	
	{
		struct stackNodeCom* temp = stackPtr->tail;
		stackPtr->tail = NULL;
		stackPtr->head = NULL;
		struct command* tempCom = (temp->com);	
		free(temp);
		//printf("result of comPop\n");
		//displayDataFromTopOfStackCom(stackPtr);
		return tempCom;	
	}
	else
	{
		struct stackNodeCom* temp = stackPtr->tail;
		stackPtr->tail = stackPtr->tail->prev;
		stackPtr->tail->next = NULL;
		struct command* tempCom = (temp->com);	
		free(temp);
		//printf("result of comPop\n");
		//displayDataFromTopOfStackCom(stackPtr);


		return tempCom;
	}
	return NULL;
}

token_t*	
stackTop(stackListOp* stackPtr)
{
	if (stackPtr->tail != NULL)
		return (stackPtr->tail->tok);
	
	return NULL;
}

//Todo: TBT
struct command*	
stackTopCom(stackListCom* stackPtr)
{
	if (stackPtr->tail != NULL)
		return (stackPtr->tail->com);
	return NULL;
}

void
displayDataFromTopOfStack(stackListOp* stackPtr)
{
	printf("\n-------------\nOPSTACK TOP\n-------------\n");
	struct stackNodeOp* it = stackPtr->tail;
	while (it != NULL)
	{
		switch(it->tok->type)
		{
			case AND:
				printf("AND\n");
				break;
  			case OR:
  				printf("OR\n");
  				break;
  			case PIPE:
  				printf("PIPE\n");
  				break;
  			case SEMICOLON:
  				printf("SEMICOLON\n");
  				break;
  			case LEFT_PARENTHESIS:
  				printf("LEFT_PARENTHESIS\n");
  				break;
  			default:
  				printf("UNKNOWN\n");
  				break;
		}
		//printf("line number of token = %i\n", it->tok->line_num);	//prints line number to see if token is properly stored in list
		it = it->prev;
	}
	printf("------------------\nOPSTACK BOTTOM ^^^\n-----------------\n");
}

void
displayDataFromTopOfStackCom(stackListCom* stackPtr)
{
	printf("\n-------------\nCOMSTACK TOP\n-------------\n");
	struct stackNodeCom* it = stackPtr->tail;
	while (it != NULL)
	{		
		

		if (it->com->type == SIMPLE_COMMAND)
		{
			printf("words of command = ");
			int j = 0;
			while (it->com->u.word[j][0] != '\0')			//prints all words in the stacked commands
			{
				printf("%s ", it->com->u.word[j]);
				j++;
			}
			printf("\n");
		}
		else if (it->com->type == AND_COMMAND)
			printf("AND_COMMAND\n");
		else if (it->com->type == OR_COMMAND)
			printf("OR_COMMAND\n");
		else if (it->com->type == PIPE_COMMAND)
			printf("PIPE_COMMAND\n");
		else if (it->com->type == SEQUENCE_COMMAND)
			printf("SEMICOLON_COMMAND\n");
		else if (it->com->type == SUBSHELL_COMMAND)
			printf("SUBSHELL_COMMAND\n");
		it = it->prev;
	}
	printf("------------------\nCOMSTACK BOTTOM ^^^\n-----------------\n");
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

/*
OLD VERSION OF THE FUNCTION
void
test_token_ptr_arr(token_t** token_ptr_arr, size_t token_ptr_arr_size)
{
	int i;
	int j;
	for (i = 0; i < (int)token_ptr_arr_size; i++)
	{
		j = 0;
		while (token_ptr_arr[i][j].type != END)
		{
		  print_token_type(token_ptr_arr[i][j]);
		  j++;
		}
		printf("New command tree \n");
	}
}
*/
void 
print_type(token_t t)
{
	switch(t.type)
	{
		case SEMICOLON:
			printf(";");
			break;
		case OR:
			printf("||");
			break;
		case AND:
			printf("&&");
			break;
		case PIPE:
			printf("|");
			break;
		case GREATER_THAN:
			printf(">");
			break;
		case LESS_THAN:
			printf("<");
			break;
		case TWO_GREATER_THAN:
			printf("TWO GREATER THANS\n");
			break;
		case LESS_AND:
			printf("LESS AND\n");
			break;
		case GREATER_AND:
			printf("GREATER AND\n");
			break;
		case LESS_GREATER:
			printf("LESS GREATER\n");
			break;
		case GREATER_OR:
			printf("GREATER OR\n");
			break;
		case LEFT_PARENTHESIS:
			printf("(");
			break;
		case RIGHT_PARENTHESIS:
			printf(")");
			break;
		case WORD_TOKEN:
			printf(" %s ", t.token_word);
			break;
		default:
			printf("I DON'T KNOW WHAT THIS IS!!!!");
			break;
	}
}

void
test_token_ptr_arr(token_t** token_ptr_arr, size_t token_ptr_arr_size)
{
	int i;
	int j;
	for (i = 0; i < (int)token_ptr_arr_size; i++)
	{
		j = 0;
		while (token_ptr_arr[i][j].type != END)
		{
			print_type(token_ptr_arr[i][j]);
			j++;
		}
		printf("\n");
	}
}


token_t**
get_token_arr(token_t *t, size_t token_size, size_t *token_ptr_arr_size)
{
	size_t pos = 0;
	size_t num_token_streams = 0;

	// Allocate room for array of pointers
	size_t max_token_bytes = 20 * sizeof(token_t); // TODO: Change 20 to a bigger number, smaller number to test allocation
	size_t max_ptr_bytes = 20 * sizeof(token_t_ptr); // TODO: Change 20 to a bigger number, smaller number to test allocation

	// Allocate memory for the array of pointers
	token_t_ptr *token_ptr_array = checked_malloc(max_ptr_bytes);;

	// Allocate a token array for the first space in the pointer array.
	token_ptr_array[0] = checked_malloc(max_token_bytes);
	size_t start = 0;
	int newline_count = 0;
	size_t tokens_in_stream = 0;

	// Ignore all the leading newlines
	if (t[0].type == NEWLINE)
	{
		while (t[pos].type == NEWLINE && pos < token_size)
		{
			pos++;
			start++;
		}
	}

	// Begin looping
	while (pos < token_size)
	{
		if (t[pos].type == NEWLINE)
		{
			// Check the thing before the newline
			switch (t[pos-1].type)
			{
				// If its an opperator
				case PIPE:
				case OR:
				case AND:
				case SEMICOLON:
				case LEFT_PARENTHESIS:
					// Count how many spaces to ignore when allocating via newline_count
					while (t[pos].type == NEWLINE && pos < token_size)
					{
						newline_count++;
						pos++;
					}
					if (pos == token_size)
					{
						if (token_ptr_array[num_token_streams][tokens_in_stream-1].type != SEMICOLON)
						{
							if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
								token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
							token_ptr_array[num_token_streams][tokens_in_stream].type = SEMICOLON;
							if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
								token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
							token_ptr_array[num_token_streams][tokens_in_stream+1].type = END;
						}
						else
						{
							if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
								token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
							token_ptr_array[num_token_streams][tokens_in_stream].type = END;
						}
				
						num_token_streams++;
					}
					break;
				// If it ends in anything else
				default:
					// If the next thing is a newline then break off the token stream
					if (pos+1 == token_size || t[pos+1].type == NEWLINE)
					{
						if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
							token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
						token_ptr_array[num_token_streams][tokens_in_stream].type = SEMICOLON;
						if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
							token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
						token_ptr_array[num_token_streams][tokens_in_stream+1].type = END;

						num_token_streams++;
						max_token_bytes = 20 * sizeof(token_t);
						token_ptr_array[num_token_streams] = checked_malloc(max_token_bytes);
						// Ignore the rest of the newlines
						while (t[pos].type == NEWLINE && pos < token_size)
							pos++;
						// If it didn't get to the end of the file, reset to 0
						if (pos < token_size)
							tokens_in_stream = 0;
					}

					// Ignore Newline for alone parenthesis
					else if (pos+1 < token_size && t[pos+1].type == RIGHT_PARENTHESIS)
					{
						newline_count++;
						pos++;
					}
					// Change newline to a ; token
					else
					{
						token_ptr_array[num_token_streams][tokens_in_stream] = t[pos];
						token_ptr_array[num_token_streams][tokens_in_stream].type = SEMICOLON;
						tokens_in_stream++;
						if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
							token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
						pos++;
					}
					break;
			}
		}

		// Ignore if it's a comment
		else if (t[pos].type == COMMENT)
		{
			pos++;
			// Increase token stream size if this token is the last one
			if (pos >= token_size)
				num_token_streams++;
		}

		// If its anything but a newline
		else
		{
			token_ptr_array[num_token_streams][tokens_in_stream] = t[pos];
			tokens_in_stream++;
			pos++;
			if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
				token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
			if(pos == token_size)
			{
				if (token_ptr_array[num_token_streams][tokens_in_stream-1].type != SEMICOLON)
				{
					if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
						token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
					token_ptr_array[num_token_streams][tokens_in_stream].type = SEMICOLON;
					if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
						token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
					token_ptr_array[num_token_streams][tokens_in_stream+1].type = END;
				}
				else
				{
					if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
						token_ptr_array[num_token_streams] = checked_grow_alloc(token_ptr_array[num_token_streams], &max_token_bytes);
					token_ptr_array[num_token_streams][tokens_in_stream].type = END;
				}
				
				num_token_streams++;
			}
		}

		if (num_token_streams * sizeof(token_t_ptr) >= max_ptr_bytes)
			token_ptr_array = checked_grow_alloc(token_ptr_array, &max_ptr_bytes);
	}

/*
	if ((tokens_in_stream+2) * sizeof(token_t) >= max_token_bytes)
		token_ptr_array[num_token_streams-1] = checked_grow_alloc(token_ptr_array[num_token_streams-1], &max_token_bytes);
	if (token_ptr_array[num_token_streams-1][tokens_in_stream].type != SEMICOLON)
	{
		token_ptr_array[num_token_streams-1][tokens_in_stream].type = SEMICOLON;
		token_ptr_array[num_token_streams-1][tokens_in_stream+1].type = END;
	}

	else 
	{
		if (tokens_in_stream * sizeof(token_t) >= max_token_bytes)
			token_ptr_array[num_token_streams-1] = checked_grow_alloc(token_ptr_array[num_token_streams-1], &max_token_bytes);
		token_ptr_array[num_token_streams-1][tokens_in_stream+1].type = END;
	}
*/
	// Check if you need to realloc
	if (num_token_streams * sizeof(token_t_ptr) >= max_ptr_bytes)
	  token_ptr_array = checked_grow_alloc(token_ptr_array, &max_ptr_bytes);

	// Put in the last token stream
	token_ptr_array[num_token_streams] = checked_malloc(sizeof(token_t));
	token_ptr_array[num_token_streams][0].type = END;
	*token_ptr_arr_size = num_token_streams;

	size_t iter;
	for (iter = 0; iter < num_token_streams; iter++)
		is_valid_token_stream(token_ptr_array[iter]);
	return token_ptr_array;
}


/*
 If your shell's input does not fall within the above subset, your implementation
 should output to stderr a syntax error message that starts with the line number 
 and a colon, and should then exit.
*/


void
is_valid_token_stream(token_t *t)
{
	int left_paren = 0;
	int right_paren = 0;
	int paren_error_line = 0;
  // If the first tokenizeen is anything but a word
  if (t[0].type != WORD_TOKEN && t[0].type != LEFT_PARENTHESIS)
  {
    fprintf(stderr, "%i: ERROR: INVALID START TO FILE\n", t[0].line_num);
    exit(1);
  }
  size_t i = 0;
  while (t[i].type != END)
  {
    switch (t[i].type)
    {
      case UNKNOWN_TOKEN:
        fprintf(stderr, "%i: ERROR: UNKNOWN TOKEN\n", t[i].line_num);
        exit(1);
      case LEFT_PARENTHESIS:
      	if (t[i+1].type == END)
      	{
      		fprintf(stderr, "%i: ERROR: INVALID END TO INPUT\n", t[i].line_num);
  				exit(1);
      	}
      	switch(t[i+1].type)
      	{
      		case OR:
      		case AND:
      		case PIPE:
      		case SEMICOLON:
      		case LESS_THAN:
      		case GREATER_THAN:
      		case GREATER_AND:
      		case GREATER_OR:
      		case TWO_GREATER_THAN:
      		case LESS_GREATER:
      		case LESS_AND:
      			fprintf(stderr, "%i: ERROR: INVALID LEFT PARENTHESIS\n", t[i].line_num);
  					exit(1);
      		default:
      			break;
      	}
      	left_paren++;
      	paren_error_line = t[i].line_num;
      	i++;
      	break;
      case RIGHT_PARENTHESIS:
      	if (i == 0)
      	{
      		fprintf(stderr, "%i: ERROR: INVALID START TO INPUT\n", t[i].line_num);
  				exit(1);
      	}
      	switch(t[i-1].type)
      	{
      		case OR:
      		case AND:
      		case PIPE:
      		case SEMICOLON:
      		case LESS_THAN:
      		case GREATER_THAN:
      		case GREATER_AND:
      		case GREATER_OR:
      		case TWO_GREATER_THAN:
      		case LESS_GREATER:
      		case LESS_AND:
      			fprintf(stderr, "%i: ERROR: INVALID RIGHT PARENTHESIS\n", t[i].line_num);
  					exit(1);
      		default:
      			break;
      	}
      	right_paren++;
      	paren_error_line = t[i].line_num;
      	i++;
      	break;
	case OR:
	case AND:
	case PIPE:
	case SEMICOLON:
	case GREATER_THAN:
	case LESS_THAN:
	case GREATER_AND:
	case GREATER_OR:
	case TWO_GREATER_THAN:
	case LESS_GREATER:
  	case LESS_AND:
 	   if (t[i+1].type != END)
        {
        	switch (t[i+1].type)
        	{
        		case OR:
        		case AND:
        		case PIPE:
        		case SEMICOLON:
        		case GREATER_THAN:
        		case LESS_THAN:
        			fprintf(stderr, "%i: ERROR: TWO OPPERATORS NEXT TO EACH OTHER\n", t[i+1].line_num);
        			exit(1);
        		default:
        			break;
        	}
        }
      default:
      	i++;
      	break;
    }
  }
  if (t[i-2].type != WORD_TOKEN && t[i-2].type != SEMICOLON && t[i-2].type != RIGHT_PARENTHESIS)
  {
  	fprintf(stderr, "%i: ERROR: INVALID END TO INPUT\n", t[i-2].line_num);
  	exit(1);
  }
  if (left_paren != right_paren)
  {
  	fprintf(stderr, "%i: ERROR: PARENTHESIS DO NOT MATCH\n", paren_error_line);
  	exit(1);
  }
}


token_t*
tokenize(char* string, size_t len, size_t *token_array_size)
{
	size_t token_buff_max = 20; // TODO: Change buff size
	size_t token_buff_max_bytes = token_buff_max * sizeof(token_t);
	token_t *token_buff = checked_malloc(token_buff_max_bytes);
	size_t num_tokens = 0;
	size_t pos = 0;
	int line = 1;
	char token_char;
	while (pos < len)
	{
		token_char = string[pos];
		if (is_valid_word_char(token_char))
		{
			int start = pos;
			token_buff[num_tokens].type = WORD_TOKEN;
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
			token_buff[num_tokens].line_num = line;
			num_tokens++;
		}
		else if (token_char == ' ' || token_char == '\t')
			pos++;
		else if (token_char == '#')
		{
			while (string[pos] != '\n')
				pos++;
		}
		else
		{
			switch(token_char)
			{
				case '\n':
					token_buff[num_tokens].type = NEWLINE;
					token_buff[num_tokens].token_word = NULL;
					token_buff[num_tokens].line_num = line;
					line++;
					num_tokens++;
					pos++;
					break;
				case ';':
					token_buff[num_tokens].type = SEMICOLON;
					token_buff[num_tokens].token_word = NULL;
					token_buff[num_tokens].line_num = line;
					num_tokens++;
					pos++;
					break;
				case '>':
					if (pos+1 != len)
					{
						switch(string[pos+1])
						{
							case '>':
								token_buff[num_tokens].type = TWO_GREATER_THAN;
								pos++;
								break;
							case '&':
								token_buff[num_tokens].type = GREATER_AND;
								pos++;
								break;
							case '|':
								token_buff[num_tokens].type = GREATER_OR;
								pos++;
								break;
							default:
								token_buff[num_tokens].type = GREATER_THAN;
								break;
						}
					}
					else
						token_buff[num_tokens].type = GREATER_THAN;

					token_buff[num_tokens].token_word = NULL;
					token_buff[num_tokens].line_num = line;
					num_tokens++;
					pos++;
					break;
				case '<':
					if (pos+1 != len)
					{
						switch(string[pos+1])
						{
							case '&':
								token_buff[num_tokens].type = LESS_AND;
								pos++;
								break;
							case '>':
								token_buff[num_tokens].type = LESS_GREATER;
								pos++;
								break;
							default:
								token_buff[num_tokens].type = LESS_THAN;
								break;
						}
					}
					else
						token_buff[num_tokens].type = LESS_THAN;

					token_buff[num_tokens].token_word = NULL;
					token_buff[num_tokens].line_num = line;
					num_tokens++;
					pos++;
					break;
				case '(':
					token_buff[num_tokens].type = LEFT_PARENTHESIS;
					token_buff[num_tokens].token_word = NULL;
					token_buff[num_tokens].line_num = line;
					num_tokens++;
					pos++;
					break;
				case ')':
					token_buff[num_tokens].type = RIGHT_PARENTHESIS;
					token_buff[num_tokens].token_word = NULL;
					token_buff[num_tokens].line_num = line;
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
					token_buff[num_tokens].line_num = line;
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
					token_buff[num_tokens].line_num = line;
					num_tokens++;
					break;
				default:
					token_buff[num_tokens].type = UNKNOWN_TOKEN;
					token_buff[num_tokens].line_num = line;
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

  *buflen = pos;
  //printf("%zd\n", pos);
  return buff;
  
}

command_t
handleTokenBuf(token_t* tok)
{
	//allocating stacks
  stackListOp opStack;
  opStack.head = NULL;
  opStack.tail = NULL;

  stackListCom comStack;		
  comStack.head = NULL;
  comStack.tail = NULL;

  size_t i = 0;
  while(tok[i].type != END)
  {
  	switch(tok[i].type)	//TODO: greater than and less than need to be dealt with l8trrrrr (oh and also comment and newline)
  	{	
  		case AND:
  		case OR:
  		case PIPE:
  		case SEMICOLON:
  		{
  			//check if opStack is empty
 				//check precedence and pop other stuff if necessary before stacking (TODO)
 				//implement safe guards for empty command stacks...and misplaced operators (TODO)
 				while ((stackTop(&opStack) != NULL) //CHANGED THIS MUTHA
 					&& (operator_precedence(*(stackTop(&opStack))) >= 0 ) 
 					&& (operator_precedence(*(stackTop(&opStack)))) >= (operator_precedence((tok[i]))))	//keep popping while operators on stack are gre...
   				{
   					//pop, join with 2 top commands on command struct
   					token_t* opTok = stackPop(&opStack);
   					
   					struct command* command2 = stackPopCom(&comStack);
   					struct command* command1 = stackPopCom(&comStack);

					struct command *newCommand = malloc(sizeof(struct command));		//TODO: maybe free???
  					newCommand->status = tok[i].line_num; 
  					newCommand->u.command[0] = command1;
  					newCommand->u.command[1] = command2;
  					newCommand->status = -1;	//TODO change statussss
  					newCommand->redirect = NO_REDIRECT;
  					
  					//select the type of the new combined command to be created from popped commands and operator
  					switch(opTok->type)
  					{
  						case AND:
  							newCommand->type = AND_COMMAND;         // A && B
  							break;
  						case SEMICOLON:
    						newCommand->type = SEQUENCE_COMMAND;    // A ; B
    						break;
    					case OR:
   							newCommand->type = OR_COMMAND;          // A || B
   							break;
   						case PIPE:
    						newCommand->type = PIPE_COMMAND;        // A | B
    						break;
    					case LESS_THAN:
    					case GREATER_THAN:
    					case NEWLINE:
    					case LEFT_PARENTHESIS:
    					case RIGHT_PARENTHESIS:
    					case WORD_TOKEN:
    					case UNKNOWN_TOKEN:
    					case COMMENT:
    					case END:
    						//printf("some token found in operator that is not supported!!!\n");
    						break;
    					default:
    						break;			//ADD CASE PLS
    				}

    				stackPushCom(&comStack, newCommand);
   					//PSEUDO
   					//pop operator
   					
   					//pop 2 commands
   					//make new command
   					//new command is operator
   					//new command has command[0],[1] = 2 popped commands from command tree
   					//push new command to command stack
   				}
   				stackPush(&opStack, &tok[i]); 	//push operator onto stack and increment i
   				i++;
  		}
  			break;
  		case LEFT_PARENTHESIS:
			stackPush(&opStack, &tok[i]);
			//printf("above prints stack after leftP\n");
  			i++;
  			break;
  		case RIGHT_PARENTHESIS:	//to implement (TODO) 
  								//TODO should account for nested parentheses 
  								//TODO should have a count incrementing for right parentheses encountered
  			while ((stackTop(&opStack) != NULL) 
 					&& (operator_precedence(*(stackTop(&opStack))) != -2 ) )	//keep popping while operators are not NULL or leftP
   				{
   					//pop, join with 2 top commands on command struct
   					token_t* opTok = stackPop(&opStack);
   					
   					struct command* command2 = stackPopCom(&comStack);
   					struct command* command1 = stackPopCom(&comStack);

					struct command *newCommand = malloc(sizeof(struct command));		//TODO: maybe free???
  					//newCommand->status = tok[i].line_num; 
  					newCommand->u.command[0] = command1;
  					newCommand->u.command[1] = command2;
  					newCommand->redirect = NO_REDIRECT;
  					newCommand->status = -1;	//TODO change statussss
  					
  					//select the type of the new combined command to be created from popped commands and operator
  					switch(opTok->type)
  					{
  						case AND:
  							newCommand->type = AND_COMMAND;         // A && B
  							break;
  						case SEMICOLON:
    						newCommand->type = SEQUENCE_COMMAND;    // A ; B
    						break;
    					case OR:
   							newCommand->type = OR_COMMAND;          // A || B
   							break;
   						case PIPE:
    						newCommand->type = PIPE_COMMAND;        // A | B
    						break;
    					case LESS_THAN:
    					case GREATER_THAN:
    					case NEWLINE:
    					case LEFT_PARENTHESIS:
    					case RIGHT_PARENTHESIS:
    					case WORD_TOKEN:
    					case UNKNOWN_TOKEN:
    					case COMMENT:
    					case END:
    						//printf("some token found in operator that is not supported!!!\n");
    						break;
    					default:
    						break; 			// ADD CASE PLS
    				}
    				stackPushCom(&comStack, newCommand);

    			}
    			if (stackTop(&opStack) == NULL)
    			{
    				//printf("ERROR! MISMATCHED PARENTHESES!!!!\n");
    			}
    			else	//if encounters a left parenthesis
    			{
    				token_t* lParenthOp = stackPop(&opStack);		//removes parenthesis from op stack

    				if (lParenthOp)
    				{
    					struct command* command1 = stackPopCom(&comStack);	//command inside of subshell
    					struct command *newCommand = malloc(sizeof(struct command));		//TODO: maybe free???

    					newCommand->type = SUBSHELL_COMMAND;
    					newCommand->u.subshell_command = command1;
  						newCommand->status = -1;
  						newCommand->redirect = NO_REDIRECT;

						//search for greater than/ less than
						if(tok[i+1].type == LESS_THAN)
						  {
						    newCommand->u.subshell_command->input = tok[i+2].token_word;
						    newCommand->redirect = NORMAL_IO_REDIRECT;
						    i+=2;
						  }
						else if(tok[i+1].type == GREATER_THAN)
						  {
						    newCommand->u.subshell_command->output = tok[i+2].token_word;
						    newCommand->redirect = NORMAL_IO_REDIRECT;
						    i+=2;
						  }
		  				else if(tok[i+1].type == GREATER_AND)
		  				{
		  					newCommand->u.subshell_command->output = tok[i+2].token_word;
		  					newCommand->redirect = GREATER_AND_REDIRECT;
		  					i+=2;
		  				}
		  				else if(tok[i+1].type == GREATER_OR)
		  				{
		  					newCommand->u.subshell_command->output = tok[i+2].token_word;
		  					newCommand->redirect = GREATER_OR_REDIRECT;
		  					i+=2;
		  				}  
		   				else if(tok[i+1].type == TWO_GREATER_THAN)
		  				{
		  					newCommand->u.subshell_command->output = tok[i+2].token_word;
		  					newCommand->redirect = TWO_GREATER_THAN_REDIRECT;
		  					i+=2;
		  				}
		  				else if(tok[i+1].type == LESS_AND)
		  				{
		  					newCommand->u.subshell_command->input = tok[i+2].token_word;
		  					newCommand->redirect = LESS_AND_REDIRECT;
		  					i+=2;
		  				}
		  				else if(tok[i+1].type == LESS_GREATER)
		  				{
		  					newCommand->u.subshell_command->input = tok[i+2].token_word;
		  					newCommand->redirect = LESS_GREATER_REDIRECT;
		  					i+=2;
		  				} 								  

						
  						stackPushCom(&comStack, newCommand);
  					}


    			}
  			i++;
  			break;

  		
  		case WORD_TOKEN:
  		{

  			int wordCount = 0;

  			struct command *newCommand = malloc(sizeof(struct command));	//TODO: maybe free???
  			newCommand->status = -1; //TODO: not true, just used for testing purposes
  			newCommand->input = NULL;
  			newCommand->output = NULL;
  			newCommand->type = SIMPLE_COMMAND;
  			newCommand->redirect = NO_REDIRECT;

  			int wrdsAlctd = 4;
  			newCommand->u.word = malloc( sizeof(char*) * (wrdsAlctd) );	//TODO: add reallocation if too big 	//TODO: maybe free???
  			newCommand->u.word[wordCount] = tok[i].token_word;

  			i++;
  			wordCount++;

  			//now cycle thru next tokens until reaching something not a word token
  			while (((tok[i].type == WORD_TOKEN)  || 
  					(tok[i].type == GREATER_THAN) || 
  					(tok[i].type == LESS_THAN)  || 
  					(tok[i].type == LESS_AND)|| 
  					(tok[i].type == LESS_GREATER)|| 
  					(tok[i].type == GREATER_OR)|| 
  					(tok[i].type == GREATER_AND)|| 
  					(tok[i].type == TWO_GREATER_THAN))	)//TODO: account for inputs/outputs/status when creating commands //used to be a condition (i < len)
  			{
  				//check if enough space in words
  				//if necessary, realloc
  				if (wordCount >= wrdsAlctd)
  				{
  					newCommand->u.word = realloc(newCommand->u.word,  sizeof(char*) * (wrdsAlctd + 4));
  					wrdsAlctd += 4;
  				}

  				if (tok[i].type == WORD_TOKEN)
  				{	
  					//add word to current command
  					newCommand->u.word[wordCount] = tok[i].token_word;
  					i++;
  					wordCount++;
  				}
  				else if(tok[i].type == LESS_THAN)
  				{
  					newCommand->input = tok[i+1].token_word;
  					newCommand->redirect = NORMAL_IO_REDIRECT;

  					i+=2;
  				}
  				else if(tok[i].type == GREATER_THAN)
  				{
  					newCommand->output = tok[i+1].token_word;
  					newCommand->redirect = NORMAL_IO_REDIRECT;
  					i+=2;
  				}
  				else if(tok[i].type == GREATER_AND)
  				{
  					newCommand->output = tok[i+1].token_word;
  					newCommand->redirect = GREATER_AND_REDIRECT;
  					i+=2;
  				}
  				else if(tok[i].type == GREATER_OR)
  				{
  					newCommand->output = tok[i+1].token_word;
  					newCommand->redirect = GREATER_OR_REDIRECT;
  					i+=2;
  				}  
   				else if(tok[i].type == TWO_GREATER_THAN)
  				{
  					newCommand->output = tok[i+1].token_word;
  					newCommand->redirect = TWO_GREATER_THAN_REDIRECT;
  					i+=2;
  				}
  				else if(tok[i].type == LESS_AND)
  				{
  					newCommand->input = tok[i+1].token_word;
  					newCommand->redirect = LESS_AND_REDIRECT;
  					i+=2;
  				}
  				else if(tok[i].type == LESS_GREATER)
  				{
  					newCommand->input = tok[i+1].token_word;
  					newCommand->redirect = LESS_GREATER_REDIRECT;
  					i+=2;
  				}  		
  			}
  			newCommand->u.word[wordCount] = '\0';
  			stackPushCom(&comStack, newCommand); 
  			break;
  		}

  		case GREATER_THAN:
  		case LESS_THAN:
  		

  		case COMMENT:
  		case UNKNOWN_TOKEN:
  		case NEWLINE:
  		case END:
  			i++;
  			//printf("encountered the something Unexpected (or the end)\n");
  			break;
  		default:
  			break;			//ADD CASE PLS

  	}
  	
  }
  return (stackPopCom(&comStack));
  //TODO: pop final operator off of the op stack and make a final command to top off the command tree...
  //and then return it so it can be put into the array of command trees to be added to the command stream


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


  if(bufflen) {} //TODO: Fix
  if(i) {}				// TODO: Fix
  if(inputString) {}	//TODO: Fix



  //prints input string for reference
  /*for (i = 0; i < bufflen; i++) {
	printf("%c", inputString[i]);
  }
  printf("\n");*/

  size_t token_array_size;
  token_t *t = tokenize(inputString, bufflen, &token_array_size);



	size_t token_ptr_array_size;
	token_t **token_ptr_array = get_token_arr(t, token_array_size, &token_ptr_array_size);
	if (token_ptr_array_size || token_ptr_array) {} // TODO: Fix
	//printf("%i \n", (int)token_ptr_array_size);
	//test_token_ptr_arr(token_ptr_array, token_ptr_array_size);


	//initialize command_stream
	command_stream_t streamy = malloc(sizeof(struct command_stream));	//TODO: maybe free???
	streamy->head = NULL;
	streamy->tail = NULL;
	streamy->cursor = NULL;

	//loop thru token streams

	size_t k = 0;
	while(k < token_ptr_array_size)
	{
		command_t rootCommand = handleTokenBuf(token_ptr_array[k]);
		//printf("command points to %p\n", rootCommand);

		struct commandNode* newCommandNode = malloc(sizeof(struct commandNode));	//TODO: maybe free???
		newCommandNode->command = rootCommand;
		newCommandNode->next = NULL;

		if (k == 0)
		{
			streamy->head = newCommandNode;
			streamy->cursor = newCommandNode;
			streamy->tail = newCommandNode;
		}
		else
		{
			streamy->tail->next = newCommandNode;
			streamy->tail = newCommandNode;
		}

		k++;
	}



  return streamy;
}



command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */

	if (s->cursor == NULL)
		return NULL;
	else
	{
		struct commandNode* tempCursor = s->cursor;
		s->cursor = s->cursor->next;
		return (tempCursor->command);
	}

  


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


		//MORE TESTS//
		  /*
  //allocating stacks
  stackList opStack;
  opStack.head = NULL;
  opStack.tail = NULL;

  stackList comStack;		
  comStack.head = NULL;
  comStack.tail = NULL;

  struct token token1;
  stackPush(&opStack, &token1);	//TODO: so compiler won't complain about unused stack vars
  stackPush(&comStack, &token1);*/

  //TODO: remove this token array (here for testing purposes)
  /*struct token tokArray[9];
  size_t length = 9;
  tokArray[0].line_num = 0;
  tokArray[0].type = LEFT_PARENTHESIS;
  tokArray[1].line_num = 1;
  tokArray[1].type = WORD_TOKEN;
  tokArray[1].token_word = "apple";
  tokArray[2].line_num = 2;
  tokArray[2].type = RIGHT_PARENTHESIS;
  tokArray[3].line_num = 3;
  tokArray[3].type = WORD_TOKEN;
  tokArray[3].token_word = "berry";

  tokArray[4].line_num = 4;
  tokArray[4].type = WORD_TOKEN;
  tokArray[4].token_word = "bush";

  tokArray[5].line_num = 5;
  tokArray[5].type = WORD_TOKEN;
  tokArray[5].token_word = "cake";

  tokArray[6].line_num = 6;
  tokArray[6].type = WORD_TOKEN;
  tokArray[6].token_word = "decrepit";

  tokArray[7].line_num = 7;
  tokArray[7].type = WORD_TOKEN;
  tokArray[7].token_word = "egregious";

  tokArray[8].line_num = 8;
  tokArray[8].type = WORD_TOKEN;
  tokArray[8].token_word = "flippant";

  handleTokenBuf(tokArray, length);*/

  /*stackListCom comStack;		
  comStack.head = NULL;
  comStack.tail = NULL;
  struct command comArray[5];
  //size_t length = 5;
  comArray[0].status = 0;
  comArray[1].status = 1;
  comArray[2].status = 2;
  comArray[3].status = 3;
  comArray[4].status = 4;

  stackPushCom(&comStack, &comArray[0]);
  displayDataFromTopOfStackCom(&comStack);*/

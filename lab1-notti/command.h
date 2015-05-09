// UCLA CS 111 Lab 1 command interface

#include <stdbool.h>
#include <stddef.h>

typedef struct command *command_t;
typedef struct command_stream *command_stream_t;
typedef struct token token_t;
typedef struct token *token_t_ptr;
typedef struct graphNode graph_node;
typedef struct queueNode queue_node;
typedef struct graphNodeQueue graph_node_queue;
typedef struct dependencyGraph dependency_graph;

typedef struct bstNode bst_node;


/* Create a command stream from GETBYTE and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t read_command_stream (command_stream_t stream);

/* Print a command to stdout, for debugging.  */
void print_command (command_t);

/* Execute a command.  Use "time travel" if the flag is set.  */
void execute_command (command_t, bool);

/* Return the exit status of a command, which must have previously
   been executed.  Wait for the command, if it is not already finished.  */
int command_status (command_t);

// Returns the precidence of a operator, if its not valid it returns -1
int operator_precedence(token_t t);

// Debugging, prints token type
void print_token_type(token_t t);

// Checks if it's a word character
int is_valid_word_char(char c);

// Tokenizes a buffer
token_t* tokenize(char* string, size_t len, size_t *token_array_size);

// Gets a buffer string
char* get_string(void* get_next_byte_arguement, int (*get_next_byte) (void *), size_t* buflen);

// ERROR Tester
void is_valid_token_stream(token_t *t);


/////////////////////////////////////////////////////
//
// EXECUTE COMMANDS
//
////////////////////////////////////////////////////

void handle_and_command(command_t c, bool time_travel);
void handle_or_command(command_t c, bool time_travel);
void handle_sequence_command(command_t c, bool time_travel);
void handle_simple_command(command_t c, bool time_travel);
void handle_subshell_command(command_t c, bool time_travel);
void handle_pipe_command(command_t c, bool time_travel);


////////////////////////////////////////////////////
//
//	QUEUE
//
///////////////////////////////////////////////////

void push_graph_node_queue(graph_node_queue* gnq, graph_node* gn);
void pop_graph_node_queue(graph_node_queue* gnq);
graph_node* top_graph_node_queue(graph_node_queue* gnq);
void destroy_graph_node_queue(graph_node_queue* gnq);
//Testing
void test_queue();
void print_contents_of_queue(graph_node_queue* gnq);
void print_contents_of_graph_node(graph_node* gn);

///////////////////////////////////////////////////
//
//		BST
//
///////////////////////////////////////////////////

void addNewBstNode(bst_node* head, char* word, graph_node* graphNody);	//adds a new BST node (only appropriate when it's not )
graph_node* searchBst(bst_node* head, char* word);	//returns 1 if found, 0 if not found 
//TODO: should retrun the graph node which the found node points to (a node which the searcher is probably dependent upon)
void destroyBst(bst_node* head);
//must make sure the head node gets manually freed outside of destroy BST

///////////////////////////////////////////////////
//
//		EXECUTING AND CREATING GRAPH
//
///////////////////////////////////////////////////

int createAndExecuteGraph(command_stream_t command_stream);
int execute_graph(dependency_graph* DG);
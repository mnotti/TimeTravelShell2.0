// UCLA CS 111 Lab 1 command internals
#include <unistd.h>
#include <sys/types.h>

enum command_type
  {
    AND_COMMAND,         // A && B
    SEQUENCE_COMMAND,    // A ; B
    OR_COMMAND,          // A || B
    PIPE_COMMAND,        // A | B
    SIMPLE_COMMAND,      // a simple command
    SUBSHELL_COMMAND,    // ( A )
  };

enum token_type
{
  AND,
  OR,
  GREATER_THAN,
  LESS_THAN,
  PIPE,
  SEMICOLON,
  NEWLINE,
  LEFT_PARENTHESIS,
  RIGHT_PARENTHESIS,
  WORD_TOKEN,
  COMMENT,
  UNKNOWN_TOKEN,
  END,
};

//added by kyyyyyle
struct token
{
  enum token_type type;
  int line_num;
  char* token_word;
};

//added by markusssssssss
struct stackNodeOp{
  struct token* tok;       
  struct stackNodeOp* next;
  struct stackNodeOp* prev;
};

struct stackNodeCom{
  struct command* com;       
  struct stackNodeCom* next;
  struct stackNodeCom* prev;
};

typedef struct stackListOp{
  struct stackNodeOp* head;
  struct stackNodeOp* tail;
}stackListOp;

typedef struct stackListCom{
  struct stackNodeCom* head;
  struct stackNodeCom* tail;
}stackListCom;


// Data associated with a command.
struct command
{
  enum command_type type;

  // Exit status, or -1 if not known (e.g., because it has not exited yet).
  int status;

  // I/O redirections, or null if none.
  char *input;
  char *output;

  union
  {
    // for AND_COMMAND, SEQUENCE_COMMAND, OR_COMMAND, PIPE_COMMAND:
    struct command *command[2];

    // for SIMPLE_COMMAND:
    char **word;

    // for SUBSHELL_COMMAND:
    struct command *subshell_command;
  } u;
};

struct commandNode{
  struct command* command; //forms the root of the tree
  struct commandNode* next;
};

typedef struct command_stream{
  struct commandNode* head;
  struct commandNode* tail;
  struct commandNode* cursor;
}command_stream;




//////////////////////////////////////////////////
//
//       GRAPH NODE/QUEUE STUFF
//
//////////////////////////////////////////////////

struct graphNode
{
  struct command command_t;
  struct graphNode** before;
  pid_t pid;
};

struct queueNode {
    struct graphNode* gn;
    struct queueNode* next;
};

struct graphNodeQueue {
    struct queueNode* head;
    struct queueNode* tail;
};

struct dependencyGraph {
  struct graphNodeQueue* no_dependencies;
  struct graphNodeQueue* dependencies;
};

////////////////////////////////////////////
//                                        //
//            BST NODE STUFF              //
//                                        //
////////////////////////////////////////////

struct bstNode
{
  struct bstNode* left;
  struct bstNode* right;
  struct graphNode* graphNody;
  char* word;

};


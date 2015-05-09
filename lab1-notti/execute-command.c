// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <string.h>


/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int beforeCount = 0;
int graphNodesToAllocate = 8;




////////////////////////////////////////////////////////////////////////////////
//
//		1C IMPLEMENTATION (PROCESSING COMMAND TREES AND FINDING DEPENDENCIES)
//
////////////////////////////////////////////////////////////////////////////////

void
processCommandAndUpdateGraph(command_t command, graph_node* graphyTemp, bst_node* headOfWriteTree, bst_node* headOfReadTree, dependency_graph* dependencyJones)
{
	
	if(command->type == SIMPLE_COMMAND)
	{
		int i = 1;
		while(command->u.word[i] != '\0')
		{
			if(command->u.word[i][0] != '-')
			{
				graph_node* temp2;
				//search the read/write list for that word...adding if necessary 
				if ((temp2 = searchOppositeList(headOfWriteTree, command->u.word[i])) && (graphyTemp != temp2))
				{
					if (beforeCount >= graphNodesToAllocate)
  					{
  						graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  						graphNodesToAllocate += 4;
  					}
					graphyTemp->before[beforeCount] = temp2;
					beforeCount++;
				}
				if ((temp2 = searchSameList(headOfReadTree, command->u.word[i], graphyTemp)))
				{
					if (beforeCount >= graphNodesToAllocate)
  					{
  						graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  						graphNodesToAllocate += 4;
  					}
					graphyTemp->before[beforeCount] = temp2;
					beforeCount++;
				}
			}
			i++;
		}
		
		graph_node* temp2;
		if(command->input)
		{
			//search the trees for input 
			if ((temp2 = searchOppositeList(headOfWriteTree, command->input)) && (graphyTemp != temp2))
			{
				if (beforeCount >= graphNodesToAllocate)
  				{
  					graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  					graphNodesToAllocate += 4;
  				}
				graphyTemp->before[beforeCount] = temp2;
				beforeCount++;
			}
			if ((temp2 = searchSameList(headOfReadTree, command->input, graphyTemp)))
			{
				if (beforeCount >= graphNodesToAllocate)
  				{
  					graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  					graphNodesToAllocate += 4;
  				}
				graphyTemp->before[beforeCount] = temp2;
				beforeCount++;
			}			
		}
		if(command->output)
		{
			//search the trees for output 
			if ((temp2 = searchOppositeList(headOfReadTree, command->output)) && (graphyTemp != temp2))
			{
				if (beforeCount >= graphNodesToAllocate)
  				{
  					graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  					graphNodesToAllocate += 4;
  				}
				graphyTemp->before[beforeCount] = temp2;
				beforeCount++;
			}
			if ((temp2 = searchSameList(headOfWriteTree, command->output, graphyTemp)))
			{
				if (beforeCount >= graphNodesToAllocate)
  				{
  					graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  					graphNodesToAllocate += 4;
  				}
				graphyTemp->before[beforeCount] = temp2;
				beforeCount++;
			}				
		}
		return;
	}
	else if(command->type == SUBSHELL_COMMAND)
	{
		graph_node* temp2;
		if(command->input)
		{
			//search the trees for input 
			if ((temp2 = searchOppositeList(headOfWriteTree, command->input)) && (graphyTemp != temp2))
			{
				if (beforeCount >= graphNodesToAllocate)
  				{
  					graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  					graphNodesToAllocate += 4;
  				}
				graphyTemp->before[beforeCount] = temp2;
				beforeCount++;
			}
			if ((temp2 = searchSameList(headOfReadTree, command->input, graphyTemp)))
			{
				if (beforeCount >= graphNodesToAllocate)
  				{
  					graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  					graphNodesToAllocate += 4;
  				}
				graphyTemp->before[beforeCount] = temp2;
				beforeCount++;
			}
		}
		if(command->output)
		{
			//search the trees for output 
			if ((temp2 = searchOppositeList(headOfReadTree, command->output)) && (graphyTemp != temp2))
			{
				if (beforeCount >= graphNodesToAllocate)
  				{
  					graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  					graphNodesToAllocate += 4;
  				}
				graphyTemp->before[beforeCount] = temp2;
				beforeCount++;
			}
			if ((temp2 = searchSameList(headOfWriteTree, command->output, graphyTemp)))
			{
				if (beforeCount >= graphNodesToAllocate)
  				{
  					graphyTemp->before = realloc(graphyTemp->before,  sizeof(graph_node*) * (graphNodesToAllocate + 4));
  					graphNodesToAllocate += 4;
  				}
				graphyTemp->before[beforeCount] = temp2;
				beforeCount++;
			}				
		}
		processCommandAndUpdateGraph(command->u.subshell_command, graphyTemp, headOfWriteTree, headOfReadTree, dependencyJones);

	}
	else
	{
		processCommandAndUpdateGraph(command->u.command[0], graphyTemp, headOfWriteTree, headOfReadTree, dependencyJones);
		processCommandAndUpdateGraph(command->u.command[1], graphyTemp, headOfWriteTree, headOfReadTree, dependencyJones);
	}
}

int 
createAndExecuteGraph(command_stream_t command_stream)
{
	int final_status;
	final_status = 0;
	if(command_stream)
	{

	}
	//PSEUDO
/*
	struct graphNodeQueue {
    	struct queueNode* head;
    	struct queueNode* tail;
	};

	struct dependencyGraph {
  		struct graphNodeQueue* no_dependencies;
  		struct graphNodeQueue* dependencies;
};
*/

	//1)
	//INITIALIZE THE DEPENDENCY GRAPH
	
		//1a)
		//INITIALIZE THE DEPENDENCY QUEUE AND THE NON-DEPENDENCY QUEUE

	dependency_graph* dependencyJones = (dependency_graph*) malloc(sizeof(dependency_graph));
	dependencyJones->dependencies = (graph_node_queue*) malloc(sizeof(graph_node_queue));
	dependencyJones->no_dependencies = (graph_node_queue*) malloc(sizeof(graph_node_queue));


	//2)
	//create and read list and a write list by initializing the first nodes with
	//no values
	bst_node* headOfReadTree = (bst_node*) malloc(sizeof(bst_node));
	
	headOfReadTree->left = NULL;
	headOfReadTree->right = NULL;
	headOfReadTree->word = NULL;

	bst_node* headOfWriteTree = (bst_node*) malloc(sizeof(bst_node));
	
	headOfWriteTree->left = NULL;
	headOfWriteTree->right = NULL;
	headOfWriteTree->word = NULL;

	//3)
	//IMPLEMENT THE PROCESS COMMAND RECURSIVE FUNCTION GIVEN BY TUAN FOR EACH OF THE COMMAND TREES IN 
	//COMMAND STREAM

		//3a)
		//MUST CREATE A NEW GRAPH NODE FOR EACH AND ADD THE GRAPH NODE TO EITHER THE DEPENDENCY OR 
		//NON-DEPENDENCY QUEUE AFTER SEARCHING THRU BOTH THE READ AND THE WRITE LISTS FOR INPUTS AND OUTPUTS

	command_t command;
	while ((command = read_command_stream (command_stream)))
    {
    	
    	graph_node* graphyTemp = (graph_node*)malloc(sizeof(graph_node));
    	graphyTemp->before = malloc( sizeof(graph_node*) * (graphNodesToAllocate) );	//TODO: add reallocation if too big 	//TODO: maybe free???
  
    	graphyTemp->command = command;
    	graphyTemp->pid = -1;
    	
    	beforeCount = 0;	//keeps track of the length of the before array in each graph node
    	processCommandAndUpdateGraph(command, graphyTemp, headOfWriteTree, headOfReadTree, dependencyJones);
    	graphyTemp->before[beforeCount] = NULL;

    	if(beforeCount)
    		push_graph_node_queue(dependencyJones->dependencies, graphyTemp);
    	else
    		push_graph_node_queue(dependencyJones->no_dependencies, graphyTemp);
    }

	//4)
	//EXECUTE THE QUEUES
	execute_graph(dependencyJones);

	//then 
	return final_status;
}

int
execute_graph(dependency_graph* DG)
{
	int status = 0;
	size_t to_malloc = DG->no_dependencies->size * DG->dependencies->size;
	pid_t *pidarr = checked_malloc(sizeof(pid_t) * to_malloc);
	//int *statusarr = checked_malloc(sizeof(int) * to_malloc);
	while (DG->dependencies->size != 0)
	{
		// Spawn processes for the runnable (in no_dependencies)
		int i;
		for (i = 0; i < DG->no_dependencies->size; i++)
		{
			pidarr[i] = fork();
			if (pidarr[i] == 0)
			{
				command_t tmp = top_graph_node_queue(DG->no_dependencies)->command;
				pop_graph_node_queue(DG->no_dependencies);
				execute_command(tmp, 1); // TODO: Make it so execute_command has one parameter
				exit(0);	// TODO: Change exit status?
			}
			else if (pidarr[i] < 0)
			{
				fprintf(stderr, "ERROR FORKING\n");				// TODO: Change error?
        		exit(1);
			}
		}

		// Wait for the processes to finish
		int j;
		status = 0;
		for (j = 0; j < i; j++)
		{
			waitpid(pidarr[j], &status, WNOHANG);
		}

		// Move nodes from dependencies to no_dependencies
		int k;
		while (top_graph_node_queue(DG->dependencies) != NULL)
		{
			k = 0;
			while(top_graph_node_queue(DG->dependencies)->before[k] != NULL && top_graph_node_queue(DG->dependencies)->before[k]->pid != -1)
				k++;
			// If the loop terminated because it reached the end, add it to no_dependencies queue
			if (top_graph_node_queue(DG->dependencies)->before[k] == NULL)
			{
				push_graph_node_queue(DG->no_dependencies, top_graph_node_queue(DG->dependencies));
				pop_graph_node_queue(DG->dependencies);
			}
			else
				break;
		}

	}


	int i;
	for (i = 0; i < DG->no_dependencies->size; i++)
	{
		pidarr[i] = fork();
		if (pidarr[i] == 0)
		{
			command_t tmp = top_graph_node_queue(DG->no_dependencies)->command;
			pop_graph_node_queue(DG->no_dependencies);
			execute_command(tmp, 1); // TODO: Make it so execute_command has one parameter
			exit(0);	// TODO: Change exit status?
		}
		else if (pidarr[i] < 0)
		{
			fprintf(stderr, "ERROR FORKING\n");				// TODO: Change error?
    		exit(1);
		}
	}

	// Wait for the processes to finish
	int j;
	status = 0;
	for (j = 0; j < i; j++)
	{
		waitpid(pidarr[j], &status, WNOHANG);
	}
		
	return status;
}

////////////////////////////////////////////////////
//
//		BST IMPLEMENTATION
//
///////////////////////////////////////////////////

//With the BST, you have to initialize it manually with:
	/*
	bstNode* headOfTree = (bstNode*) malloc(sizeof(bstNode));
	headOfTree->left = NULL;
	headOfTree->right = NULL;
	headOfTree->word = NULL;
	*/

graph_node*
searchSameList(bst_node* head, char* word, graph_node* graphNody)
{
	if(head->word == NULL)
	{

		head->graphNody = graphNody;
		head->word = word;
		return NULL;

	}
	else
	{
		bst_node* it = head;
		while (it != NULL)
		{
			int cmpVal = strcmp(word, it->word);
			if (cmpVal < 0)
			{	
				//word < it->word
				if(it->left != NULL)
					it = it->left;
				else
				{
					bst_node* newBstNode = (bst_node*) malloc(sizeof(bst_node));
					newBstNode->left = NULL;
					newBstNode->right = NULL;
					newBstNode->graphNody = graphNody;
					newBstNode->word = word;

					it->left = newBstNode;
					return NULL;
				}
			}
			else if(cmpVal > 0)
			{
				//str1 > it->word
				if(it->right != NULL)
					it = it->right;
				else
				{
					bst_node* newBstNode = (bst_node*) malloc(sizeof(bst_node));
					newBstNode->left = NULL;
					newBstNode->right = NULL;
					newBstNode->graphNody = graphNody;
					newBstNode->word = word;

					it->right = newBstNode;
					return NULL;					
				}
			}
			else
			{
				//str1 == it->word
				if(graphNody != it->graphNody)
				{
					graph_node* tempGraphyNody = it->graphNody;
					it->graphNody = graphNody;
					return tempGraphyNody;
				}
				else 
					return NULL;
			}
		}
	
	}

	return NULL;
}

graph_node*
searchOppositeList(bst_node* head, char* word)
{
	if(head->word == NULL)
	{
		return NULL;
	}
	else
	{
		bst_node* it = head;
		while (it != NULL)
		{
			int cmpVal = strcmp(word, it->word);
			if (cmpVal < 0)
			{	
				//word < it->word
				if(it->left != NULL)
					it = it->left;
				else
				{
					//not found
					return NULL;
				}
			}
			else if(cmpVal > 0)
			{
				//str1 > it->word
				if(it->right != NULL)
					it = it->right;
				else
				{
					//not found
					return NULL;					
				}
			}
			else
			{
				//str1 == it->word
				printf("Found word\n");
				return it->graphNody;
			}
		}
	
	}
	return NULL;
}

void
destroyBst(bst_node* head)
{
	if (head == NULL)
    	return;
    bst_node* it = head;
    //recursively free all nodes
    destroyBst(it->left);
    destroyBst(it->right);

    printf("freeing BST node with word: %s\n", head->word);
    free(head);
    printf("freed\n\n");
}

void
addNewBstNode(bst_node* head, char* word, graph_node* graphNody)
{	
	if(head->word == NULL)
	{

		head->graphNody = graphNody;
		head->word = word;
		return;

	}
	else
	{
		bst_node* it = head;
		while (it != NULL)
		{
			int cmpVal = strcmp(word, it->word);
			if (cmpVal < 0)
			{	
				//word < it->word
				if(it->left != NULL)
					it = it->left;
				else
				{
					bst_node* newBstNode = (bst_node*) malloc(sizeof(bst_node));
					newBstNode->left = NULL;
					newBstNode->right = NULL;
					newBstNode->graphNody = graphNody;
					newBstNode->word = word;

					it->left = newBstNode;
					return;
				}
			}
			else if(cmpVal > 0)
			{
				//str1 > it->word
				if(it->right != NULL)
					it = it->right;
				else
				{
					bst_node* newBstNode = (bst_node*) malloc(sizeof(bst_node));
					newBstNode->left = NULL;
					newBstNode->right = NULL;
					newBstNode->graphNody = graphNody;
					newBstNode->word = word;

					it->right = newBstNode;
					return;					
				}
			}
			else
			{
				//str1 == it->word
				printf("WARNING: Found the same word(shouldn't be adding a newNode if word is already in the BST\n");
					return;
			}
		}
	
	}
}

graph_node* 
searchBst(bst_node* head, char* word)
{
	if(head->word == NULL)
	{
		return NULL;
	}
	else
	{
		bst_node* it = head;
		while (it != NULL)
		{
			int cmpVal = strcmp(word, it->word);
			if (cmpVal < 0)
			{	
				//word < it->word
				if(it->left != NULL)
					it = it->left;
				else
				{
					//not found
					return NULL;
				}
			}
			else if(cmpVal > 0)
			{
				//str1 > it->word
				if(it->right != NULL)
					it = it->right;
				else
				{
					//not found
					return NULL;					
				}
			}
			else
			{
				//str1 == it->word
				printf("Found word\n");
					return it->graphNody;
			}
		}
	
	}
	return NULL;
}
/*
int 
searchBst(bst_node* head, char* word)
{
	if(head->word == NULL)
	{
		return 0;
	}
	else
	{
		bst_node* it = head;
		while (it != NULL)
		{
			int cmpVal = strcmp(word, it->word);
			if (cmpVal < 0)
			{	
				//word < it->word
				if(it->left != NULL)
					it = it->left;
				else
				{
					//not found
					return 0;
				}
			}
			else if(cmpVal > 0)
			{
				//str1 > it->word
				if(it->right != NULL)
					it = it->right;
				else
				{
					//not found
					return 0;					
				}
			}
			else
			{
				//str1 == it->word
				printf("Found word\n");
					return 1;
			}
		}
	
	}
}
*/


////////////////////////////////////////////////////
//
//		QUEUE IMPLEMENTATION
//
///////////////////////////////////////////////////

void
push_graph_node_queue(graph_node_queue* gnq, graph_node* gn) {
    queue_node *newnode = (queue_node*) malloc(sizeof(queue_node));
    newnode->next = NULL;
    newnode->gn = gn;
    if (gnq->head == NULL)
    {
        gnq->head = newnode;
        gnq->tail = newnode;
    }
    else {
        gnq->tail->next = newnode;
        gnq->tail = newnode;
    }
    gnq->size++;
}

void
pop_graph_node_queue(graph_node_queue* gnq)
{
    // Can't pop an empy queue
    if (gnq->head == NULL)
        return;
    
    queue_node* tmp = gnq->head;
    gnq->head = gnq->head->next;
    free(tmp->gn);
    free(tmp);
    // If you deleted the last one, make the tail NULL
    if (gnq->head == NULL)
        gnq->tail = NULL;
    gnq->size--;
}

graph_node*
top_graph_node_queue(graph_node_queue* gnq)
{
    if (gnq->head == NULL)
        return NULL;
    
    return gnq->head->gn;
}

void
destroy_graph_node_queue(graph_node_queue* gnq)
{
    queue_node* curr = gnq->head;
    while (curr != NULL)
    {
        queue_node* tmp = curr;
        free(curr->gn);
        free(curr);
        curr = tmp->next;
        
        if (curr == NULL)
            gnq->tail = NULL;
    }
    gnq->head = NULL;
    gnq->size = 0;
    free(gnq);
}


void
test_queue()
{
	graph_node_queue *q = (graph_node_queue*) malloc(sizeof(graph_node_queue));
    q->head = NULL;
    q->tail = NULL;
    
    printf("Queue Created\n");
    graph_node* gnn1 = (graph_node*) malloc(sizeof(graph_node));
    graph_node* gnn2 = (graph_node*) malloc(sizeof(graph_node));
    graph_node* gnn3 = (graph_node*) malloc(sizeof(graph_node));
    
    gnn1->pid = 1;
    gnn2->pid = 2;
    gnn3->pid = 3;

    printf("PUSH TEST\n");
    
    push_graph_node_queue(q, gnn1);
    print_contents_of_queue(q);

    push_graph_node_queue(q, gnn2);
    print_contents_of_queue(q);

    push_graph_node_queue(q, gnn3);
    print_contents_of_queue(q);

    printf("(3) size = %i \n", q->size);
    
    printf("POP/TOP TEST\n");
    graph_node* gnp;

    gnp = top_graph_node_queue(q);
    printf("TOP\n");
    print_contents_of_graph_node(gnp);

    pop_graph_node_queue(q);
    print_contents_of_queue(q);
	gnp = top_graph_node_queue(q);
	printf("TOP\n");
    print_contents_of_graph_node(gnp);

    printf("(2) size = %i \n", q->size);

    pop_graph_node_queue(q);
    print_contents_of_queue(q);
	gnp = top_graph_node_queue(q);
	printf("TOP\n");
    print_contents_of_graph_node(gnp);

    pop_graph_node_queue(q);
    print_contents_of_queue(q);
	gnp = top_graph_node_queue(q);
	printf("TOP\n");
    print_contents_of_graph_node(gnp);

    printf("(0) size = %i \n", q->size);
    
   	
}

void
print_contents_of_queue(graph_node_queue* gnq)
{
	if (gnq->head == NULL)
	{
    	printf("Head = NULL\n");
    	if (gnq->tail != NULL)
    	{
    		printf("ERROR: HEAD IS NULL BUT TAIL IS NOT\n");
    		return;
    	}
    	printf("Tail = NULL\n");
	}
	else if(gnq->tail == NULL)
	{
		printf("ERROR: TAIL IS NULL WHILE HEAD IS NOT\n");
		return;
	}
    else
    {
    	printf("Head\n-----\n");
    	printf("Address: %p \t pid: %i \n", gnq->head->gn, (int)gnq->head->gn->pid);
    	printf("Tail\n-----\n");
    	printf("Address: %p \t pid: %i \n", gnq->tail->gn, (int)gnq->tail->gn->pid);
    	printf("QUEUE FRONT\n-------------\n");
    	queue_node* tmp = gnq->head;
    	while(tmp != NULL)
    	{
    		printf("Address: %p \t pid: %i \n", tmp->gn, (int)tmp->gn->pid);
    		tmp = tmp->next;
    	}
    	printf("-------------\nQUEUE BOTTOM\n");
    }
}

void print_contents_of_graph_node(graph_node* gn)
{
	if (gn == NULL)
	{
		printf("NULL\n");
		return;
	}

	printf("Address: %p \t pid: %i \n", gn, (int)gn->pid);
}




///////////////////////////////////////
//
//		HANDLING COMMAND TYPES
//
///////////////////////////////////////

void 
handle_simple_command(command_t c, bool time_travel)
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
	pid_t pid;

	if (!(pid = fork()))	//if child
	{
			close(fd[0]);	//close read end
			if ((dup2(fd[1], 1)) < 0)
			{
				fprintf(stderr, "ERROR: Failed to copy file descriptor for output: pipe\n");
				exit(1);
			}
			execute_command(c->u.command[0], time_travel);
			close (fd[1]);
			exit(WEXITSTATUS(c->u.command[0]->status));

	}
	else	//parent
	{
		close(fd[1]);	//close read end
		if ((dup2(fd[0], 0)) < 0)
		{
			fprintf(stderr, "ERROR: Failed to copy file descriptor for output: pipe\n");
			exit(1);
		}
		execute_command(c->u.command[1], time_travel);
		close (fd[1]);
		c->status = WEXITSTATUS(c->u.command[1]->status);
		
		close(fd[0]);
		close(fd[1]);

		int status;
		
		waitpid(pid, &status, 0);	
		c->status = status;
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
#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct Node {
	int pid;
	int run_state;
	char* process_name;
	struct Node *next;
}Node;

Node *head;

Node *new_node(int pid,char* process_name, int run_state);
Node *add_front(Node *list, Node *new);
Node *add_end(Node *list, Node *new);
void remove_node(int pid);
Node *find_node(int pid);
void free_list(Node *head);

#endif

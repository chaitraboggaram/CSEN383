#ifndef _utility_h_
#define _utility_h_

struct node_s {
	struct node_s *next;
	struct node_s *prev;
	void *data;
};

typedef struct node_s node;

struct linked_list_s {
	node *head;
	node *tail;
	int size;
};

typedef struct linked_list_s linked_list;

node *createNode(void *data);
linked_list *createLinkedList();
void addNode(linked_list *ll, void *data);
void removeNode(linked_list *ll, void *data);
void swapNodes(node *a, node *b);
void addAfter(linked_list *ll, node *after_node, void *data);
void sort(linked_list *ll, int (*cmp)(void *data1, void *data2));


// Queue Implementation

typedef struct linked_list_s queue;

queue *createQueue();
void enqueue(queue *q, void *data);
void *dequeue(queue *q);

#endif

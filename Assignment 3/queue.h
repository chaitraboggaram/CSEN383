#ifndef _queue_h_
#define _queue_h_

//Node structure for Queue
struct node_struct {
    struct node_struct *next;
    struct node_struct *prev;
    void *value;
};
typedef struct node_struct Node;

//Queue structure
struct queue_struct {
    Node *head;
    Node *tail;
    int size;
};
typedef struct queue_struct Queue;

//Functions of Queue
Node *createNode(void *value);
Queue *createQueue();
void enqueue(Queue *q, void *value);
void *dequeue(Queue *q);
void sort(Queue *q, int (*cmp)(void *val1, void *val2));
void swapNodes(Node *a, Node *b);

#endif
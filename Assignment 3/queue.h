#ifndef _queue_h_
#define _queue_h_

//Node structure for the Queue
struct node_struct {
    struct node_struct *next;  //Pointer to the next Node in the Queue
    struct node_struct *prev;  //Pointer to the previous Node in the Queue
    void *value;               //Pointer to the value stored in the Node
};
typedef struct node_struct Node;  //Define the Node structure as 'Node'

//Queue structure
struct queue_struct {
    Node *head;  //Pointer to the first Node in the Queue
    Node *tail;  //Pointer to the last Node in the Queue
    int size;    //Size of the Queue
};
typedef struct queue_struct Queue;  //Define the Queue structure as 'Queue'

//Functions of Queue
Node *createNode(void *value);       //Function to create a new Node
Queue *createQueue();                 //Function to create a new Queue
void enqueue(Queue *q, void *value);  //Function to enqueue a value into the Queue
void *dequeue(Queue *q);              //Function to dequeue a value from the Queue
void sort(Queue *q, int (*cmp)(void *val1, void *val2));  //Function to sort the Queue
void swapNodes(Node *a, Node *b);     //Function to swap the values of two Nodes

#endif

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

//Create a new empty queue
Queue *createQueue()
{
    Queue *newQueue = (Queue *)malloc(sizeof(Queue));
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->size = 0;
    return newQueue;
}

//Create a new node with the given value
Node *createNode(void *value)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->value = value;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

//Enqueue a new node with the given value to the end of the queue
void enqueue(Queue *q, void *value)
{
    Node *newNode = createNode(value);

    if (q->tail == NULL)
    {
        q->head = newNode;
    }
    else
    {
        q->tail->next = newNode;
        newNode->prev = q->tail;
    }

    q->tail = newNode;
    q->size++;
}

//Dequeue and return the value of the node from the front of the queue
void *dequeue(Queue *q)
{
    if (q->head == NULL)
    {
        return NULL;
    }

    Node *currentNode = q->head;
    void *value = currentNode->value;

    q->head = currentNode->next;
    if (q->head != NULL)
    {
        q->head->prev = NULL;
    }
    else
    {
        q->tail = NULL;
    }

    q->size--;
    free(currentNode);
    return value;
}

//Function to swap the values of two nodes
void swapNodes(Node *node1, Node *node2)
{
    void *temp = node1->value;
    node1->value = node2->value;
    node2->value = temp;
}

//Function to sort the Queue based on the comparison function provided
void sort(Queue *q, int (*cmp)(void *val1, void *val2))
{
    Node *i = q->head;
    while (i != NULL)
    {
        Node *j = i->next;
        while (j != NULL)
        {
            if ((*cmp)(i->value, j->value) > 0)
            {
                swapNodes(i, j);
            }
            j = j->next;
        }
        i = i->next;
    }
}
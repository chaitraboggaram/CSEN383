#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Implementation of Queue
Node *createNode(void *value)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->value = value;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

Queue *createQueue()
{
    Queue *newQueue = (Queue *)malloc(sizeof(Queue));
    newQueue->head = NULL;
    newQueue->tail = NULL;
    newQueue->size = 0;
    return newQueue;
}

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

void *dequeue(Queue *q)
{
    if (q->head == NULL)
    {
        return NULL;
    }

    Node *curNode = q->head;
    void *value = curNode->value;

    q->head = curNode->next;
    if (q->head != NULL)
    {
        q->head->prev = NULL;
    }
    else
    {
        q->tail = NULL;
    }

    q->size--;
    free(curNode);
    return value;
}

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

void swapNodes(Node *a, Node *b)
{
    void *temp = a->value;
    a->value = b->value;
    b->value = temp;
}

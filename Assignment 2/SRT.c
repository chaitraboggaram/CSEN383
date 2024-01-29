/*
 Group 2
 Advance Operating Systems
 Algorithm: Shortest Remaining Time
 */

#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

process_stat *generateProcessStat(process *proc);

// function to compare time the completion time of processes while sorting 
int compareRemainingTime(void *data1, void *data2)
{
  process_stat *ps1 = (process_stat *) data1;
	process_stat *ps2 = (process_stat *) data2;
	if(((((process *)ps1->proc)->runTime) - (ps1->runTime)) < ((((process *)ps2->proc)->runTime) - (ps2->runTime))) {
		return -1;
	} else {
		return 1;
	}
}

// function to print contents of queue for testing purpose during shortest remaining time to completion 
void printQueueSRT(queue *q)
{
  if (q->head != NULL)
  {
    node *n = q->head;
    printf("Queue Contains:\n");
    while(n != NULL)
    {
      process_stat *ps = n->data;
      process *p = ps->proc;
      printf("Process Id %c Remaining Time %f\n",p->pid,(p->runTime - ps->runTime));
      n = n->next;
    }
  }
  return;
}

// Implementation of shortest remaining time to completion preemptive //
average_stats shortestRemainingTimeP(linked_list *processes)
{
  int t = 0; // quanta

  // creation of a queue of processes
  queue *processQueue = createQueue();
  // creation of linked list for managaing the order of processes in preemeption
  linked_list *ll = createLinkedList();

  node *processPointer = processes->head;
  if(processes->head == NULL) {
		fprintf(stderr,"No Process to schedule\n");
	}

  //while process queue is not empty or time quanta is less than 100
  process_stat *scheduledProcess = NULL;
	printf("\n\n\n==================================================================================================================================\n");
  printf("\nShortest Remaining Time Algorithm:\n");
  printf("Order of Processes in Execution: ");
  while(t<100 || scheduledProcess!=NULL )
  {
    //printf("time %d\n",t);
    if(scheduledProcess!=NULL)
    {
      enqueue(processQueue,scheduledProcess);
      scheduledProcess = NULL;
    }
    //check for incoming new process and enqueue it in the queue
		if(processPointer != NULL) {
			process *newProcess = (process *)(processPointer->data);
			while(processPointer !=NULL && newProcess->arrivalTime <= t) {
				enqueue(processQueue,generateProcessStat(newProcess));
				//sort(processQueue,compareRemainingTime);
				processPointer = processPointer->next;
				if(processPointer!=NULL)
        {
					newProcess = (process *)(processPointer->data);
        }
			}
      // sort all the processes that have arrived based on their remaining running time to completion //
      sort(processQueue,compareRemainingTime);
    }

    //if there is no scheduled process, then check process queue and schedule it //
		if(scheduledProcess == NULL && processQueue->size > 0) {
			scheduledProcess = (process_stat *) dequeue(processQueue);

      // If the process has not started before quanta 100, remove the process from the queue and take the next process in queue for execution //
      while(t>=100 && scheduledProcess->startTime == -1)
      {
        scheduledProcess = (process_stat *) dequeue(processQueue);
      }
		}
    if(scheduledProcess != NULL) {
  			process *proc = scheduledProcess->proc;

  			//add the currently running process to the time chart
  			printf("%c",proc->pid);

  			//update the current processes stat
  			if(scheduledProcess->startTime == -1) {
  				scheduledProcess->startTime = t;
  			}

  			scheduledProcess->runTime++;

        if(scheduledProcess->runTime >= proc->runTime) {
          scheduledProcess->endTime = t;
          addNode(ll,scheduledProcess);
          scheduledProcess = NULL;
        }
      }else {
    			printf("_");
    	}
  		//keep increasing the time
  		t++;
  }
  //Print Process Stat
  return printPolicyStat(ll);
  
}

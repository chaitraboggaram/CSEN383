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

// printQueueSRT is a function to print the contents of the queue during srt
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
//  CRT function helps to compare the completion time of processess during sorting  
int Crt(void *data1, void *data2)
{
  process_stat *ps1 = (process_stat *) data1;
	process_stat *ps2 = (process_stat *) data2;
	if(((((process *)ps1->proc)->runTime) - (ps1->runTime)) < ((((process *)ps2->proc)->runTime) - (ps2->runTime))) {
		return -1;
	} else {
		return 1;
	}
}



// Implementation of shortest remaining time to completion preemptive //
average_stats shortestRemainingTimeP(linked_list *processes)
{
  int timequanta = 0; 

  // creation of a queue of processes
  queue *processQueue = createQueue();
  // creation of linked list for managaing the order of processes in preemeption
  linked_list *ll = createLinkedList();

  node *processPointer = processes->head;
  if(processes->head == NULL) {
		fprintf(stderr,"No Process to schedule\n");
	}

  //runs until  time quanta is less than 100 or process queue is not empty or
  process_stat *scheduledProcess = NULL;
	printf("\n\n\n==================================================================================================================================\n");
  printf("\nShortest Remaining Time Algorithm:\n");
  printf("Order of Processes in Execution: ");
  while(timequanta<100 || scheduledProcess!=NULL )
  {
 
    if(scheduledProcess!=NULL)
    {
      enqueue(processQueue,scheduledProcess);
      scheduledProcess = NULL;
    }
    //check for incoming new process and enqueue it in the queue
		if(processPointer != NULL) {
			process *newProcess = (process *)(processPointer->data);
			while(processPointer !=NULL && newProcess->arrivalTime <= timequanta) {
				enqueue(processQueue,generateProcessStat(newProcess));
				//sort(processQueue,compareRemainingTime);
				processPointer = processPointer->next;
				if(processPointer!=NULL)
        {
					newProcess = (process *)(processPointer->data);
        }
			}
      // Sorting all the processes that have arrived in the queue depending on their remaining running time till completion.
      sort(processQueue,Crt);
    }

    //if scheduled process is null ,it checks process queue and it schedules  
		if(scheduledProcess == NULL && processQueue->size > 0) {
			scheduledProcess = (process_stat *) dequeue(processQueue);

      // process is removed from the queue if it hasn't started beofre time quanta 100 and next process in queue is executed.
      while(timequanta>=100 && scheduledProcess->startTime == -1)
      {
        scheduledProcess = (process_stat *) dequeue(processQueue);
      }
		}
    if(scheduledProcess != NULL) {
  			process *proc = scheduledProcess->proc;

  			// running process is added to chart 
  			printf("%c",proc->pid);

  			//update the current processes stat
  			if(scheduledProcess->startTime == -1) {
  				scheduledProcess->startTime = timequanta;
  			}

  			scheduledProcess->runTime++;

        if(scheduledProcess->runTime >= proc->runTime) {
          scheduledProcess->endTime = timequanta;
          addNode(ll,scheduledProcess);
          scheduledProcess = NULL;
        }
      }else {
    			printf("_");
    	}
  	
  		timequanta++;
  }
  //Print Process Stat
  return printPolicyStat(ll);
  
}
/*
 Group 2
 Advance Operating Systems
 Algorithm: Highest Priority First Non-preemptive
 */

#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

process_stat *generateProcessStat(process *proc);
int comparePriority(void *data1, void *data2);
void printqueue(queue *q);

average_stats highestPriorityFirstNP(linked_list *processes)
{
  int t = 0; // quanta 
  // create a queue of processes.
  queue * processQue1 = createQueue();
  queue * processQue2 = createQueue();
  queue * processQue3 = createQueue();
  queue * processQue4 = createQueue();
  
  
  // create linked list to manage process order in preemeption
  linked_list * linkedlist1 = createLinkedList();
  linked_list * linkedlist2 = createLinkedList();
  linked_list * linkedlist3 = createLinkedList();
  linked_list * linkedlist4 = createLinkedList();
  
  
  node * processPointer = processes->head;
  if(processes->head == NULL) {
		fprintf(stderr,"There is no Process to schedule\n");
	}
	printf("\n\n\n==================================================================================================================================\n");
  printf("\nHighest Priority First Non-preemptive Algorithm:\n");
  printf("Order of Processes in Execution: ");
  
  
 //keep checking while time quanta is less than 100 or the process queue is empty...
 
 
  process_stat * scheduledProcess = NULL;
  while(t<100 || scheduledProcess!=NULL )
  {
         //check for incoming new process and do enqueue based on priority.
		 
		if(processPointer != NULL) {
			process * newProcess = (process *)(processPointer->data);
			
			//keep checking if the process pointer is not null and new process arrival time is less than equal to t...
			while(processPointer !=NULL && newProcess->arrivalTime <= t) {
				
				
        if(newProcess->priority == 1) 
          enqueue(processQue1,generateProcessStat(newProcess));
        if(newProcess->priority == 2) 
          enqueue(processQue2,generateProcessStat(newProcess));
        if(newProcess->priority == 3) 
          enqueue(processQue3,generateProcessStat(newProcess));
        if(newProcess->priority == 4) 
          enqueue(processQue3,generateProcessStat(newProcess));
		
		
				//sort(processQueue,comparePriority);
				processPointer = processPointer->next;
				if(processPointer!=NULL)
        {
					newProcess = (process *)(processPointer->data);
        }
			}
      // sorting arrived processes based on priority.
      //sort(processQueue,comparePriority);
    }
    //printqueue(processQueue);
    //check process queue and schedule it based on priority. //
		if(scheduledProcess == NULL) {
			
			//dequeue processes based on their sizes...
      if (processQue1->size > 0) 
        scheduledProcess = (process_stat *) dequeue(processQue1);
      else if (processQue2->size > 0) 
        scheduledProcess = (process_stat *) dequeue(processQue2);
      else if (processQue3->size > 0) 
        scheduledProcess = (process_stat *) dequeue(processQue3);
      else if (processQue4->size > 0) 
        scheduledProcess = (process_stat *) dequeue(processQue4);
	  
	  
      // If the process has not started before quanta 100, remove the process from the queue and take the next process in queue for execution //
     
	 //if the time > = 100 an startime -1,null the scheduled process.
	 
      if (t>=100 && scheduledProcess->startTime == -1){
        //free(scheduledProcess);
        scheduledProcess = NULL;
        continue;
      }
    }


    if(scheduledProcess != NULL) {
  			process * proc = scheduledProcess->proc;

  			//add  the currently running process to the time chart
  			printf("%c",proc->pid);

  			//updating the  current processes stat
  			if(scheduledProcess->startTime == -1) {
  				scheduledProcess->startTime = t;
  			}

  			scheduledProcess->runTime++;

    

        if(scheduledProcess->runTime >= proc->runTime) {
          scheduledProcess->endTime = t;
		  
		  //add nodes to the linked list based on their priority.
		  
          if(scheduledProcess->proc->priority == 1) 
            addNode(linkedlist1,scheduledProcess);
          else if(scheduledProcess->proc->priority == 2) 
            addNode(linkedlist2,scheduledProcess);
          else if(scheduledProcess->proc->priority == 3) 
            addNode(linkedlist3,scheduledProcess);
          else if(scheduledProcess->proc->priority == 4) 
            addNode(linkedlist4,scheduledProcess);
          //addNode(ll,scheduledProcess);
          scheduledProcess = NULL;
          //free(scheduledProcess);
        }
      }else {
    			printf("_");
  		}
  		//keep increasing the time...
  		t++;
    }
    //Print Process Stat
    average_stats avg1,avg2,avg3,avg4,avg;
    printf("\nFor the  Priority Queue 1");
    avg1 = printPolicyStat(linkedlist1);
    printf("\nFor the  Priority Queue 2");
    avg2 = printPolicyStat(linkedlist2);
    printf("\nFor the  Priority Queue 3");
    avg3 = printPolicyStat(linkedlist3);
    printf("\nFor  the Priority Queue 4");
    avg4 = printPolicyStat(linkedlist4);

//calcultaing average response time ,wait time,turnaround time and throughput.
    avg.avg_response_time = (avg1.avg_response_time + avg2.avg_response_time + avg3.avg_response_time + avg4.avg_response_time)/4 ;
    avg.avg_wait_time = (avg1.avg_wait_time + avg2.avg_wait_time + avg3.avg_wait_time + avg4.avg_wait_time)/4 ;
    avg.avg_turnaround = (avg1.avg_turnaround + avg2.avg_turnaround + avg3.avg_turnaround + avg4.avg_turnaround)/4 ;
    avg.avg_throughput = (avg1.avg_throughput + avg2.avg_throughput + avg3.avg_throughput + avg4.avg_throughput) ;


    printf("\nThe following represent the average times of High Priority First Non-preemptivee for all queues:\n");
    printf("|------------------------------------------------------------|\n");
    printf("| Metric                    | Average Value                  |\n");
    printf("|---------------------------|--------------------------------|\n");
    printf("| Average Response Time     | %.1f                           |\n", avg.avg_response_time);
    printf("| Average Wait Time         | %.1f                           |\n", avg.avg_wait_time);
    printf("| Average Turn Around Time  | %.1f                           |\n", avg.avg_turnaround);
    printf("|------------------------------------------------------------|\n");

    return avg;
}


#include "stat.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

process_stat *generateProcessStat(process *proc);

//Function to compare the priority and arrival time
int priorityComparison(void *data1, void *data2)
{
  process_stat *proc1 = (process_stat *) data1;
	process_stat *proc2 = (process_stat *) data2;
	
	//compare the priorities of both the processes..
	if(((((process *)proc1->proc)->priority)) < ((((process *)proc2->proc)->priority))) {
		return -1;
	}
  else if (((((process *)proc1->proc)->priority)) == ((((process *)proc2->proc)->priority)) &&  (((process *)proc1->proc)->arrivalTime < (((process *)proc2->proc)->arrivalTime)))
  {
    return -1;
  }
  else {
		return 1;
	}
}

// Implementation....
average_stats highestPriorityFirstP(linked_list * procs)
{
  int quanta = 0;
  
  //Creation of 4 Linked Lists for managaing order of processes in preemeption
  linked_list *linkedList1 = createLinkedList();
  linked_list *linkedList2 = createLinkedList();
  linked_list *linkedList3 = createLinkedList();
  linked_list *linkedList4 = createLinkedList();

  //Creation of 4 process queues
  queue *proc_queue1 = createQueue();
  queue *proc_queue2 = createQueue();
  queue *proc_queue3 = createQueue();
  queue *proc_queue4 = createQueue();

  node *proc_pointer = procs->head;
  if(procs->head == NULL) {
		fprintf(stderr,"There is no Process to schedule\n");
	}
	printf("\n\n\n==================================================================================================================================\n");
  printf("\nHighest Priority First Preemptive:\n");
  printf("Order of Processes in Execution: ");

  //keep checking while time quanta is less than 100 or the process queue is empty...
  process_stat *scheduled_proc = NULL;
  while(quanta < 100 || scheduled_proc != NULL )
  {
    if(scheduled_proc!=NULL)
    {
		//enqueue based on priority..
      if(scheduled_proc->proc->priority == 1) 
        enqueue(proc_queue1,scheduled_proc);
      if(scheduled_proc->proc->priority == 2) 
        enqueue(proc_queue2,scheduled_proc);
      if(scheduled_proc->proc->priority == 3) 
        enqueue(proc_queue3,scheduled_proc);
      if(scheduled_proc->proc->priority == 4) 
        enqueue(proc_queue3,scheduled_proc);
    }
    //If new process comes, we enqueue it in the queue
		if(proc_pointer != NULL) {
			process *newProcess = (process *)(proc_pointer->data);
			
			//checking the new process arrival time..
			while(proc_pointer !=NULL && newProcess->arrivalTime <= quanta) {
        if(newProcess->priority == 1) 
          enqueue(proc_queue1,generateProcessStat(newProcess));
        if(newProcess->priority == 2) 
          enqueue(proc_queue2,generateProcessStat(newProcess));
        if(newProcess->priority == 3) 
          enqueue(proc_queue3,generateProcessStat(newProcess));
        if(newProcess->priority == 4) 
          enqueue(proc_queue3,generateProcessStat(newProcess));
		
		    //sort the queues based on priority comparison.
				sort(proc_queue1,priorityComparison);
        sort(proc_queue2,priorityComparison);
        sort(proc_queue3,priorityComparison);
        sort(proc_queue4,priorityComparison);
		
				proc_pointer = proc_pointer->next;
				if(proc_pointer!=NULL)
        {
					newProcess = (process *)(proc_pointer->data);
        }
			}
    }
    //If there are no scheduled processes, we check process queue and schedule it
    if(scheduled_proc == NULL) {
      if (proc_queue1->size > 0) 
        scheduled_proc = (process_stat *) dequeue(proc_queue1);
      else if (proc_queue2->size > 0) 
        scheduled_proc = (process_stat *) dequeue(proc_queue2);
      else if (proc_queue3->size > 0) 
        scheduled_proc = (process_stat *) dequeue(proc_queue3);
      else if (proc_queue4->size > 0) 
        scheduled_proc = (process_stat *) dequeue(proc_queue4);

      //If the process has not started before quanta 100, we remove it from the queue and take the next one in queue for execution//
      if (quanta>=100 && scheduled_proc->startTime == -1){
        scheduled_proc = NULL;
        continue;
      }
		}
    if(scheduled_proc != NULL) {
  			process * proc = scheduled_proc->proc;

  			//Add the currently  running process to  the time chart
  			printf("%c",proc->pid);

  			//Update the current process stat
  			if(scheduled_proc->startTime == -1) {
  				scheduled_proc->startTime = quanta;
  			}

  			scheduled_proc->runTime++;

        if(scheduled_proc->runTime >= proc->runTime) {
          scheduled_proc->endTime = quanta;
		      //adding to the linked list based on priority.
          if(scheduled_proc->proc->priority == 1) 
            addNode(linkedList1,scheduled_proc);
          else if(scheduled_proc->proc->priority == 2) 
            addNode(linkedList2,scheduled_proc);
          else if(scheduled_proc->proc->priority == 3) 
            addNode(linkedList3,scheduled_proc);
          else if(scheduled_proc->proc->priority == 4) 
            addNode(linkedList4,scheduled_proc);
          scheduled_proc = NULL;
        }
      }else {
    			printf("_");
  		}
  		//Increase the Quanta
  		quanta++;
    }
    //Print Process Statistics
    average_stats average1,average2,average3,average4,average;
    printf("\nFor the  Priority Queue 1");
    average1 = printPolicyStat(linkedList1);
    printf("\nFor the Priority Queue 2");
    average2 = printPolicyStat(linkedList2);
    printf("\nFor the  Priority Queue 3");
    average3 = printPolicyStat(linkedList3);
    printf("\nFor  the Priority Queue 4");
    average4 = printPolicyStat(linkedList4);

    average.avg_response_time = (average1.avg_response_time + average2.avg_response_time + average3.avg_response_time + average4.avg_response_time)/4 ;
    average.avg_wait_time = (average1.avg_wait_time + average2.avg_wait_time + average3.avg_wait_time + average4.avg_wait_time)/4 ;
    average.avg_turnaround = (average1.avg_turnaround + average2.avg_turnaround + average3.avg_turnaround + average4.avg_turnaround)/4 ;
    average.avg_throughput = (average1.avg_throughput + average2.avg_throughput + average3.avg_throughput + average4.avg_throughput) ;

    printf("\nThe following represent the average times of Highest Priority First Preemptive for all queues:\n");
    printf("|------------------------------------------------------------|\n");
    printf("| Metric                    | Average Value                  |\n");
    printf("|---------------------------|--------------------------------|\n");
    printf("| Average Response Time     | %.1f                           |\n", average.avg_response_time);
    printf("| Average Wait Time         | %.1f                           |\n", average.avg_wait_time);
    printf("| Average Turn Around Time  | %.1f                           |\n", average.avg_turnaround);
    printf("|------------------------------------------------------------|\n");


    return average;
}

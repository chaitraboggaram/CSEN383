#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

process_stat *createProcessStat(process *proc) {
	process_stat *ps = (process_stat *) malloc(sizeof(process_stat));
	ps->proc = proc;
	ps->waitingTime = 0;
	ps->turnaroundTime = 0;
	ps->responseTime = 0;

	ps->runTime = 0;
	ps->startTime = -1;
	ps->endTime = -1;
	return ps;
}

average_stats firstComeFirstServeNP(linked_list *processes) {
	int t = 0;

	//Creation of  Process Queue
	queue *processQueue = (queue *)createQueue();
	node * procPtr = processes->head;
	if(processes->head == NULL) {
		fprintf(stderr," There is no Process to schedule\n\n");
	}
	
	//keep checking while time quanta is less than 100 or the process queue is empty...
	process_stat * scheduledProcess = NULL;

	linked_list *ll = createLinkedList();
	printf("\n\n\n==================================================================================================================================\n");
	printf("\nFirst-Come First-Serve Algorithm:\n");
	printf("Order of Processes in Execution: ");
	while(t<100 || scheduledProcess!=NULL) {
		//check for incoming new process and do enqueue.
		if(procPtr != NULL) {
			process * newProcess = (process *)(procPtr->data);
			if(newProcess->arrivalTime <= t) {
				enqueue(processQueue,createProcessStat(newProcess));
				procPtr = procPtr->next;
			}
		}

		//check process queue and schedule it if there is no scheduled process now..
		if(scheduledProcess == NULL && processQueue->size > 0) {
			scheduledProcess = (process_stat *) dequeue(processQueue);
		}

        
		if(scheduledProcess != NULL) {
			process * proc = scheduledProcess->proc;

			//add  the currently running process to the time chart
			printf("%c",proc->pid);

			//update current processes stat
			if(scheduledProcess->startTime == -1) {
				scheduledProcess->startTime = t;
			}
			scheduledProcess->runTime++;
    //check if scheduled process run time is greater than the currently running process runtime..
			if(scheduledProcess->runTime >= proc->runTime) {
				scheduledProcess->endTime = t;
				//add the scheduled process.
				addNode(ll,scheduledProcess);
				scheduledProcess = NULL;
			}
		} else {
			printf("_");
		}
		//keep increasing the time.
		t++;
	}

	//Print the Stat of the process.
	return printPolicyStat(ll);
	
}

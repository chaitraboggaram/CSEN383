/*
 Group 2
 Advance Operating Systems
 Algorithm: Shortest Job First
 */

#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

process_stat *generateProcessStat(process* proc);

int compareRunTime(void *data1, void *data2) {
	process_stat *ps1 = (process_stat *) data1;
	process_stat *ps2 = (process_stat *) data2;
	if(((process *)ps1->proc)->runTime < ((process *)ps2->proc)->runTime) {
		return -1;
	} else {
		return 1;
	}
}

average_stats shortestJobFirstNP(linked_list *processes) {
	int t = 0;

	//Creation of Process Queue
	queue *processQueue = (queue *)createQueue();
	node *procPtr = processes->head;
	if(processes->head == NULL) {
		fprintf(stderr,"No Process to schedule\n");
	}
	//keep checking while time quanta is less than 100 or the process queue is empty...
	process_stat *scheduledProcess = NULL;

	linked_list *ll = createLinkedList();
	printf("\n\n\n==================================================================================================================================\n");
	printf("\nShortest Job First Algorithm:\n");
	printf("Order of Processes in Execution: ");
	while(t<100 || scheduledProcess!=NULL) {
		//check for incoming new process and do enqueue.
		if(procPtr != NULL) {
			process *newProcess = (process *)(procPtr->data);
			while(procPtr!=NULL && newProcess->arrivalTime <= t) {
				enqueue(processQueue,generateProcessStat(newProcess));
				sort(processQueue,compareRunTime);
				procPtr = procPtr->next;
				if(procPtr!=NULL)
					newProcess = (process *)(procPtr->data);
			}
		}

		//check process queue and schedule it if there is no scheduled process now..
		if(scheduledProcess == NULL && processQueue->size > 0) {
			scheduledProcess = (process_stat *) dequeue(processQueue);
		}

		if(scheduledProcess != NULL) {
			process * proc = scheduledProcess->proc;

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
		} else {
			printf("_");
		}
		//keep increasing the time
		t++;
	}
	printf("\n");

	//Printing Process Stat
	return printPolicyStat(ll);
	
}

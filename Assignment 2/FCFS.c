/*
 Group 2
 Advance Operating Systems
 Algorithm: First Come First Server
 */

#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

process_stat *generateProcessStat(process *proc) {
    process_stat *newProcessStat = (process_stat *) malloc(sizeof(process_stat));
    newProcessStat->proc = proc;
    newProcessStat->waitingTime = 0;
    newProcessStat->turnaroundTime = 0;
    newProcessStat->responseTime = 0;
    newProcessStat->runTime = 0;
    newProcessStat->startTime = -1;
    newProcessStat->endTime = -1;
    return newProcessStat;
}

average_stats firstComeFirstServeNP(linked_list *processes) {
    int currentTime = 0;

    // Establish the Process Queue
    queue *processQueue = (queue *)createQueue();
    node *procPtr = processes->head;
    
    if (processes->head == NULL) {
        fprintf(stderr,"\nNo procedure exists for scheduling");
    }
    
    // Continue to monitor as long as the currentTime quanta is below 100 or the process queue remains empty
    process_stat *scheduledProcess = NULL;
    linked_list *ll = createLinkedList();
    
    printf("\n\n\n==================================================================================================================================\n");
    printf("\nFirst Come First Serve Algorithm:");
    printf("\nOrder of Processes in Execution: ");
    
    while (currentTime < 100 || scheduledProcess != NULL) {
        // To check incoming new process and enqueue
        if (procPtr != NULL) {
            process * newProcess = (process *)(procPtr->data);
            if (newProcess->arrivalTime <= currentTime) {
                enqueue(processQueue, generateProcessStat(newProcess));
                procPtr = procPtr->next;
            }
        }

        // To check process queue and schedule it if there is no scheduled process now
        if (scheduledProcess == NULL && processQueue->size > 0) {
            scheduledProcess = (process_stat *) dequeue(processQueue);
        }

        if (scheduledProcess != NULL) {
            process *proc = scheduledProcess->proc;

            // To add current running process to the currentTime chart
            printf("%c", proc->pid);

            // To update current processes stat
            if (scheduledProcess->startTime == -1) {
                scheduledProcess->startTime = currentTime;
            }
            
            scheduledProcess->runTime++;
            
            // To check if scheduled process run currentTime is greater than the currently running process runtime
            if (scheduledProcess->runTime >= proc->runTime) {
                scheduledProcess->endTime = currentTime;
                
                // To add scheduled process
                addNode(ll,scheduledProcess);
                scheduledProcess = NULL;
            }
            
        } else {
            printf("_");
        }
        
        // Continue to increase the time
        currentTime++;
    }

    // To print the Stat of the process
    return printPolicyStat(ll);
}


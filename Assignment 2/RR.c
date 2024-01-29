/*
 Group 2
 Advance Operating Systems
 Algorithm: Round Robin
 */

#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

process_stat *generateProcessStat(process *proc);

average_stats roundRobin(linked_list *processes, int time_slice) {
    int current_time = 0;

    // Creation of Process Queue
    queue *process_queue = (queue *)createQueue();
    node *process_pointer = processes->head;
    if (processes->head == NULL) {
        fprintf(stderr, "No Process to schedule\n");
    }
    
    process_stat *scheduled_process = NULL;

    linked_list *time_chart = createLinkedList();
	printf("\n\n\n==================================================================================================================================\n");
    printf("\nRound Robin Algorithm:\n");
    printf("Order of Processes in Execution: ");
    node *current_node = NULL;
    int current_run_time = 0;

    while (current_time < 100 || process_queue->size > 0) {

        // Check for incoming new process and enqueue.
        if (process_pointer != NULL && current_time < 100) {
            process *new_process = (process *)(process_pointer->data);
            while (process_pointer != NULL && new_process->arrivalTime <= current_time) {
                enqueue(process_queue, generateProcessStat(new_process));
                process_pointer = process_pointer->next;
                if (process_pointer != NULL)
                    new_process = (process *)(process_pointer->data);
            }
        }

        // Check process queue and schedule if there is no scheduled process now...
        if (current_node == NULL) {
            current_run_time = 0;
            current_node = process_queue->head;
        } else if (current_run_time == time_slice) {
            current_run_time = 0;
            current_node = current_node->next;
            if (current_node == NULL) {
                current_node = process_queue->head;
            }
        }

        if (current_node != NULL) {
            scheduled_process = (process_stat *)current_node->data;
            process *proc = scheduled_process->proc;

            if (current_time >= 100) {
                if (scheduled_process->startTime == -1) {
                    // Do not start any new process, remove it from process_queue
                    free(scheduled_process);
                    node *next = current_node->next;
                    removeNode(process_queue, current_node->data);
                    current_node = next;
                    current_run_time = 0;
                    continue;
                }
            }
            
            // Add the currently running process to the time chart
            printf("%c", proc->pid);
            current_run_time++;

            // Update the current process stat
            if (scheduled_process->startTime == -1) {
                scheduled_process->startTime = current_time;
            }
            scheduled_process->runTime++;

            if (scheduled_process->runTime >= proc->runTime) {
                scheduled_process->endTime = current_time;
                addNode(time_chart, scheduled_process);
                node *next = current_node->next;
                removeNode(process_queue, current_node->data);
                current_node = next;
                current_run_time = 0;
            }
        } else {
            printf("_");
        }

        // Keep increasing the time
        current_time++;
    }

    return printPolicyStat(time_chart);
}

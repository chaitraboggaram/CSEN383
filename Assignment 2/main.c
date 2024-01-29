#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "process.h"
#include "stat.h"

void dispQueue(queue *q) {
    node *ptr = (node *)q->head;
    printf("<");
    while(ptr != NULL) {
        printf("%c ", ((process *)ptr->data)->pid);
        ptr = ptr->next;
    }
    printf(">\n");
}

void printProcessTable(linked_list *processList) {
    printf("-------------------------------------------------\n");
    printf("Process ID | Arrival Time | Run Time | Priority |\n");
    printf("-------------------------------------------------\n");
    node *ptr = processList->head;
    while(ptr != NULL) {
        process *proc = (process *)ptr->data;
        printf("%10c | %12.1f | %8.1f | %8d |\n", proc->pid, proc->arrivalTime, proc->runTime, proc->priority);
        ptr = ptr->next;
    }
    printf("--------------------------------------------------\n");
    printf("Total No. of Processes : %d\n", processList->size);
    printf("--------------------------------------------------\n");
}

void printAlgorithmStats(const char *algorithmName, average_stats *stats) {
    printf("\n");
    printf("------------------------------------------------------------------------------------------------\n");
    printf("|                         %-69s |\n", algorithmName);
    printf("|---------------------------|---------------------------|-------------------|-------------------|\n");
    printf("| %-25s | %-25s | %-17s| %-17s|\n", "Average Response Time", "Average Wait Time", "Average Turnaround", "Average Throughput");
    printf("|---------------------------|---------------------------|-------------------|-------------------|\n");
    printf("| %-25.1f | %-25.1f | %-17.1f | %-17.1f |\n", stats->avg_response_time, stats->avg_wait_time, stats->avg_turnaround, stats->avg_throughput);
    printf("|---------------------------|---------------------------|-------------------|-------------------|\n");
    printf("\n");
}


int main(int argc, char **argv) {
    int ctr = 0;
    average_stats fcfs[6], sjf[6], srf[6], rr[6], hpfp[6], hpfnp[6], final[6];
    int time_slice = 5;

    while (ctr < 5) {
        // Generate Processes
        printf("\n*********************************************************************************\n");
        printf("\t\t\t\tRound %d\n", ctr + 1);
        printf("*********************************************************************************\n");

        linked_list *processList = generateProcesses(52);
        printProcessTable(processList);

        // First Come First Serve
        fcfs[ctr] = firstComeFirstServeNP(processList);
        // Shortest Job First
        sjf[ctr] = shortestJobFirstNP(processList);
        // Shortest Remaining Time First
        srf[ctr] = shortestRemainingTimeP(processList);
        // Round Robin
        if (argc > 1) {
            time_slice = atoi(argv[1]);
        }
        printf("Round Robin Time Slice: %d\n", time_slice);
        rr[ctr] = roundRobin(processList, time_slice);
        // Highest Priority First Preemptive
        hpfp[ctr] = highestPriorityFirstP(processList);
        // Highest Priority First Non-Preemptive
        hpfnp[ctr] = highestPriorityFirstNP(processList);
        ctr++;
    }

    for (int i = 0; i < 6; i++) {
        final[i].avg_response_time = 0;
        final[i].avg_wait_time = 0;
        final[i].avg_turnaround = 0;
        final[i].avg_throughput = 0;
    }

    // Total of all average values
    for (int i = 0; i < 5; i++) {
        final[0].avg_response_time += fcfs[i].avg_response_time;
        final[1].avg_response_time += sjf[i].avg_response_time;
        final[2].avg_response_time += srf[i].avg_response_time;
        final[3].avg_response_time += rr[i].avg_response_time;
        final[4].avg_response_time += hpfp[i].avg_response_time;
        final[5].avg_response_time += hpfnp[i].avg_response_time;

        final[0].avg_wait_time += fcfs[i].avg_wait_time;
        final[1].avg_wait_time += sjf[i].avg_wait_time;
        final[2].avg_wait_time += srf[i].avg_wait_time;
        final[3].avg_wait_time += rr[i].avg_wait_time;
        final[4].avg_wait_time += hpfp[i].avg_wait_time;
        final[5].avg_wait_time += hpfnp[i].avg_wait_time;

        final[0].avg_turnaround += fcfs[i].avg_turnaround;
        final[1].avg_turnaround += sjf[i].avg_turnaround;
        final[2].avg_turnaround += srf[i].avg_turnaround;
        final[3].avg_turnaround += rr[i].avg_turnaround;
        final[4].avg_turnaround += hpfp[i].avg_turnaround;
        final[5].avg_turnaround += hpfnp[i].avg_turnaround;

        final[0].avg_throughput += fcfs[i].avg_throughput;
        final[1].avg_throughput += sjf[i].avg_throughput;
        final[2].avg_throughput += srf[i].avg_throughput;
        final[3].avg_throughput += rr[i].avg_throughput;
        final[4].avg_throughput += hpfp[i].avg_throughput;
        final[5].avg_throughput += hpfnp[i].avg_throughput;
    }

    // Average for all the algorithms
    for (int i = 0; i < 6; i++) {
        final[i].avg_response_time /= 5;
        final[i].avg_wait_time /= 5;
        final[i].avg_turnaround /= 5;
        final[i].avg_throughput /= 5;
    }

    printf("\n\n\n");
    printf("============================================================================================================================\n\n");
    printf("Below are the average metrics obtained from 5 runs for each algorithm:\n");
    printf("\n");

    printAlgorithmStats("First-Come First-Served (FCFS) [non-preemptive]", &final[0]);
    printAlgorithmStats("Shortest Job First (SJF) [non-preemptive]", &final[1]);
    printAlgorithmStats("Shortest Remaining Time (SRT) [preemptive]", &final[2]);
    printAlgorithmStats("Round Robin (RR) [preemptive]", &final[3]);
    printAlgorithmStats("Highest Priority First (HPF) [preemptive]", &final[4]);
    printAlgorithmStats("Highest Priority First (HPF) [non-preemptive]", &final[5]);

    return 0;
}

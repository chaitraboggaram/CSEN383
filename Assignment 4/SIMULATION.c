#include "PAGE.h"

int main(int arg1, char* arg2[]) {
    // To set simulator timestamp
    int timestamp = 0;

    // To set paging options
    int *PGCoptn = malloc(sizeof(int) * 4);
    PGCoptn[0] = 5;
    PGCoptn[1] = 11;
    PGCoptn[2] = 17;
    PGCoptn[3] = 31;

    // To set hit and miss counters
    int hitCounter = 0;
    int missesCounter = 0;

    // The pointer to the Page
    page *pagePtr;

    // The function pointer to hold the selected page replacement algorithm
    void (*pageReplacementAlgo)(LIST_OF_PAGES*);

    // To check command line arguments for the selected algorithm
    if (arg1 != 2) {
        printf("\nPage Replacement Algorithms are: FCFS, LRU, LFU, MFU, or Random\n");
        return -1;
    }

    // To determine which function to call using the given algorithm name
    if (strcmp(arg2[1], "FCFS") == 0) {
        pageReplacementAlgo = FCFS_FUNCTION;
    } else if (strcmp(arg2[1], "LRU") == 0) {
        pageReplacementAlgo = LRU_FUNCTION;
    } else if (strcmp(arg2[1], "LFU") == 0) {
        pageReplacementAlgo = LFU_FUNCTION;
    } else if (strcmp(arg2[1], "MFU") == 0) {
        pageReplacementAlgo = MFU_FUNCTION;
    } else if (strcmp(arg2[1], "Random") == 0) {
        pageReplacementAlgo = R_FUNCTION;
    } else {
        printf("************************************************************************************\n");
        printf("\nThese are the options to run for simulation: FCFS, LRU, LFU, MFU, or Random\n");
        printf("************************************************************************************\n");
        return -1;
    }

    int swappingInProcess = 0;

    // To seed the random number generator
    srand(0);

    int i;
    for (i = 0; i < 5; i++) {
        printf("********************* Running Simulator %d*********************\n", i + 1);
        LIST_OF_PAGES listOfPages;
        InitializePageList(&listOfPages);
        process Q[TOTAL_PROCESSES];

        int i;
        for (i = 0; i < TOTAL_PROCESSES; i++) {
            // To initialize process attributes
            Q[i].pid = i;
            Q[i].PGECNTER = PGCoptn[rand() % 4];
            Q[i].TIMEARR = rand() % 60;
            // Maximum process duration
            Q[i].DURATION = rand() % PROCESS_DURATION;
            // All processes will begin with Page 0
            Q[i].PGCRR = 0;
        }

        // To sort the process queue by arrival time
        qsort(Q, TOTAL_PROCESSES, sizeof(process), CompareArrivalTime);

        // Start of the process queue
        int index = 0;
        for (timestamp = 0; timestamp < TOTAL_DURATION; timestamp++) {
            // To look for new processes at the start of every second
            while (index < TOTAL_PROCESSES && Q[index].TIMEARR <= timestamp) {
                // To check at least four pages
                if (PageWhoAreFree(&listOfPages, 4)) {
                    // If it's present, bring it into memory
                    page *p = PAGEfrreeeg(&listOfPages);
                    p->pid = Q[index].pid;
                    p->PAGE_NUMBER = Q[index].PGCRR;
                    p->FTBOUGHT = 1.0 * timestamp;
                    p->CNTER = 1;
                    p->LONE = timestamp;
                    printf("\nPage -> %d for Process ID -> %d brought in at -> %f\n", Q[index].PGCRR, Q[index].pid, p->FTBOUGHT);
                    swappingInProcess++;
                    index++;
                } else
                    break;
            }
            
            int i;
            for (i = 0; i < 10; i++) {
                int j;
                for (j = 0; j < index; j++) if (Q[j].DURATION > 0) {
                    Q[j].PGCRR = PGNUMNXT(Q[j].PGCRR, Q[j].PGECNTER);
                    if (MemInPages(&listOfPages, Q[j].pid, Q[j].PGCRR)) {
                        pagePtr = IDOfPageFree(&listOfPages, Q[j].pid, Q[j].PGCRR);
                        if (pagePtr == NULL) {
                            printf("There is a bug, got null: pid %d page:: %d\n", Q[j].pid, Q[j].PGCRR);
                            return -1;
                        }
                        pagePtr->CNTER++;
                        pagePtr->LONE = timestamp;
                        hitCounter++;
                        continue;
                    }
                    // If we are here, that means we referred a page that is not in memory
                    // Bring that page in
                    page *pageeeg = PAGEfrreeeg(&listOfPages);
                    if (!pageeeg) {
                        printf("\n************************************************************************************");
                        printf("\nMemory is full, the Page list:");
                        DisplayPages(&listOfPages);
                        pageReplacementAlgo(&listOfPages);
                        DisplayPages(&listOfPages);
                        pageeeg = PAGEfrreeeg(&listOfPages);
                    }
                    pageeeg->pid = Q[j].pid;
                    pageeeg->PAGE_NUMBER = Q[j].PGCRR;
                    pageeeg->FTBOUGHT = timestamp + (0.1 * i);
                    pageeeg->LONE = timestamp + (0.1 * i);
                    pageeeg->CNTER = 0;
                    printf("\nPage -> %d for process %d brought in at %f\n", Q[j].PGCRR, Q[j].pid, pageeeg->FTBOUGHT);
                    swappingInProcess++;
                    missesCounter++;
                }
            }
            
            int j;
            for (j = 0; j < index; j++) if (Q[j].DURATION > 0) {
                Q[j].DURATION--;
                if (Q[j].DURATION == 0) {
                    printf("\n************************************************************************************");
                    printf("\nProcess id %d is done. Memory is getting free", Q[j].pid);
                    printf("\n************************************************************************************");
                    MEMRFree(&listOfPages, Q[j].pid);
                }
            }
            // Sleep for 0.9 ms (900 milliseconds)
            usleep(900);
        }
    }

    // To calculate the hit-miss ratio
    float hitMissRatio = (float) hitCounter / (hitCounter + missesCounter);

    // To print the output results
    printf("\nAverage number of processes that were successfully swapped in: %d", (swappingInProcess / 5));
    printf("\nThe Hit Ratio: %.6f\n", hitMissRatio);
}

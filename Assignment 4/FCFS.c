/* Implementation of FCFS Algorithm */

#include "PAGE.h"

void FCFS_FUNCTION(LIST_OF_PAGES* plFCFS) {
    // To initialize a pointer to the start of the page list
    page* PITFCF = plFCFS->HDL;

    // To initialize a pointer to the page to be evicted as the first page in the list
    page* PAGE_FCFS_EVICTION = plFCFS->HDL;

    // To loop through the pages to find the one with the earliest arrival time
    while (PITFCF) {
        if (PITFCF->FTBOUGHT < PAGE_FCFS_EVICTION->FTBOUGHT) {
            PAGE_FCFS_EVICTION = PITFCF;
        }

        // To move to the next page in the list
        PITFCF = PITFCF->next;
    }

    // If debugging is enabled, to print details about the evicted page
    if (DEBUG == 1) printf("\nEVICTED ONES -> p[%03d] c:%02d l:%02f", PAGE_FCFS_EVICTION->pid, PAGE_FCFS_EVICTION->CNTER, PAGE_FCFS_EVICTION->LONE);

    // To mark the evicted page as not in use
    PAGE_FCFS_EVICTION->pid = -1;
    PAGE_FCFS_EVICTION->PAGE_NUMBER = -1;
}

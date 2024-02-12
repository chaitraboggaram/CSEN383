/* Implementation of LFU Algorithm */

#include "PAGE.h"

void LFU_FUNCTION(LIST_OF_PAGES* pLFUl) {
    // To initialize a pointer to the start of the page list
    page* FITLFU = pLFUl->HDL;

    // To initialize a pointer to the page to be evicted as the first page in the list
    page* PG_LFUEVICTN = pLFUl->HDL;

    // To initialize the minimum reference count with the count of the first page
    int min = FITLFU->CNTER;

    // To loop through the pages to find the one with the least frequent usage
    while(FITLFU) {
        if(FITLFU->CNTER < min){
            // To update the page to be evicted
            PG_LFUEVICTN = FITLFU;

            // To update the minimum reference count
            min = FITLFU->CNTER;
        }

        // To move to the next page in the list
        FITLFU = FITLFU->next;
    }

    // If debugging is enabled, print details about the evicted page
    if (DEBUG == 1) printf("EVICTED ONES -> p[%03d] c:%02d l:%02f\n", PG_LFUEVICTN->pid, PG_LFUEVICTN->CNTER, PG_LFUEVICTN->LONE);

    // To mark the evicted page as not in use
    PG_LFUEVICTN->pid = -1;
    PG_LFUEVICTN->PAGE_NUMBER = -1;
}

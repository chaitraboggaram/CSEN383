/* Implementation of MFU Algorithm */

#include "PAGE.h"

void MFU_FUNCTION(LIST_OF_PAGES* PMFUL) {
    // To initialize a pointer to the start of the page list
    page* FITMFU = PMFUL->HDL;
    // To initialize a pointer to the page to be evicted as the first page in the list
    page* PGMFUEVC = PMFUL->HDL;
    // To initialize the maximum count with the count of the first page
    int max = FITMFU->CNTER;

    // To loop through the pages to find the most frequently used page
    while(FITMFU) {
        if(FITMFU->CNTER > max){
            // To update the page to be evicted
            PGMFUEVC = FITMFU;
            // To update the maximum count
            max = FITMFU->CNTER;
        }
        // To move to the next page in the list
        FITMFU = FITMFU->next;
    }

    // If debugging is enabled, print details about the evicted page
    if (DEBUG == 1) printf("EVICTED -> p[%03d] c:%02d l:%02f\n", PGMFUEVC->pid, PGMFUEVC->CNTER, PGMFUEVC->LONE);

    // To mark the evicted page as not in use
    PGMFUEVC->pid = -1;
    PGMFUEVC->PAGE_NUMBER = -1;
}

/* Implementation of LRU Algorithm */

#include "PAGE.h"

void LRU_FUNCTION(LIST_OF_PAGES* PLLRU) {
    // To initialize a pointer to the start of the page list
    page* FITLRU = PLLRU->HDL;

    // To initialize a pointer to the page to be evicted as the first page in the list
    page* LRU_PGEVVTON = PLLRU->HDL;

    // To initialize the LRU timestamp with the timestamp of the first page
    int lru = FITLRU->LONE;

    // To loop through the pages to find the one with the least recently used timestamp
    while (FITLRU) {
        if (FITLRU->LONE < lru){
            // To update the page to be evicted
            LRU_PGEVVTON = FITLRU;

            // To update the LRU timestamp
            lru = FITLRU->LONE;
        }
        // To move to the next page in the list
        FITLRU = FITLRU->next;
    }

    // If debugging is enabled, print details about the evicted page
    if (DEBUG == 1) printf("EVICTED ONES -> p[%03d] c:%02d l:%02f\n", LRU_PGEVVTON->pid, LRU_PGEVVTON->CNTER, LRU_PGEVVTON->LONE);

    // To mark the evicted page as not in use
    LRU_PGEVVTON->pid = -1;
    LRU_PGEVVTON->PAGE_NUMBER = -1;
}

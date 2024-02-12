/* Implementation of Random Page Replacement Algorithm */

#include "PAGE.h"

void R_FUNCTION(LIST_OF_PAGES* PRM) {
    // To initialize a pointer to the first page in the page list
    page* FITR = PRM->HDL;
    // To initialize a pointer to track the page for eviction
    page* PGRNDMEVC = PRM->HDL;
    // The counter to keep track of the current page number
    int CNTER = 0;
    // To generate a random number 'r' within the range of total pages
    int r = rand() % TOTAL_PAGE;

    // To loop through the pages in the list
    while (FITR) {
        // To check if the page is in use and if the counter is less than the random number 'r'
        if (FITR->pid > 0 && CNTER < r) {
            // To update the page to be evicted
            PGRNDMEVC = FITR;
        }
        // To move to the next page
        FITR = FITR->next;
        // Increment the counter
        CNTER++;
    }

    if (DEBUG == 1) {
        printf("EVICTED ONES -> p[%03d] c:%02d l:%02f\n", PGRNDMEVC->pid, PGRNDMEVC->CNTER, PGRNDMEVC->LONE);
    }

    // To mark the page as not in use
    PGRNDMEVC->pid = -1;
    // To reset the page number
    PGRNDMEVC->PAGE_NUMBER = -1;
}

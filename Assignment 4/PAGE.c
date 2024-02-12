#include "PAGE.h"

// To initialize the list of pages with free pages
void InitializePageList(LIST_OF_PAGES* listOfPages) {
    listOfPages->HDL = malloc(sizeof(page));
    page* it = listOfPages->HDL;
    int i;
    for(i = 0; i < TOTAL_PAGE; i++) {
        it->pid = -1;
        it->PAGE_NUMBER = -1;
        it->next = NULL;
        if(i < 99) {
            it->next = malloc(sizeof(page));
            it = it->next;
        }
    }
}

// To print all pages in the list
void DisplayPages(LIST_OF_PAGES* listOfPages) {
    page* it = listOfPages->HDL;
    int cnt = 0;
    while(it) {
        printf(it->pid > 0 ? "|*| p[%03d] c:%02d l:%02f |*|" : "|*                  |",it->pid, it->CNTER, it->LONE);
        cnt++;
        if((cnt % 10) == 0) printf("\n");
        it = it->next;
    }
    printf("\n");
}

// To check if there are enough free pages
int PageWhoAreFree(LIST_OF_PAGES* listOfPages, int CNTER) {
    page* it = listOfPages->HDL;
    while(it) {
        // To check if the page is not being used by any process
        if(it->pid == -1) {
            CNTER--;
        }
        if(!CNTER) return 1;
        it = it->next;
    }
    return 0;
}

// To check if a specific page is in memory
int MemInPages(LIST_OF_PAGES* listOfPages, int pid, int PAGE_NUMBER) {
    page* it = listOfPages->HDL;
    while(it) {
        if(it->pid == pid && it->PAGE_NUMBER == PAGE_NUMBER) return 1;
        it = it->next;
    }
    return 0;
}

// To find a free page
page* PAGEfrreeeg(LIST_OF_PAGES* listOfPages) {
    page* it = listOfPages->HDL;
    while(it) {
        if(it->pid == -1) return it;
        it = it->next;
    }
    return NULL;
}

// To free memory occupied by a process
void MEMRFree(LIST_OF_PAGES* listOfPages, int pid) {
    page* it = listOfPages->HDL;
    while(it) {
        if(it->pid == pid) {
            it->pid = -1;
            it->PAGE_NUMBER = -1;
        }
        it = it->next;
    }
}

// To calculate the next page number for a process
int PGNUMNXT(int curr_page_no, int max_page_size) {
    int x = rand() % 10;
    if(x < 7) {
        x = curr_page_no + (rand() % 3) - 1;
        if(x < 0) x = 0;
    } else {
        x = rand() % max_page_size;
        while(abs(x - curr_page_no) <= 1) x = rand() % max_page_size;
    }
    if(x < 0) x = 0;
    if(x >= 100) x = max_page_size - 1;
    return x;
}

// To find a page with specific ID
page* IDOfPageFree(LIST_OF_PAGES* listOfPages, int pid, int PAGE_NUMBER) {
    page* it = listOfPages->HDL;
    while(it) {
        if(it->pid == pid && it->PAGE_NUMBER == PAGE_NUMBER) return it;
        it = it->next;
    }
    return NULL;
}

// To compare arrival times for sorting
int CompareArrivalTime(const void* a, const void* b) {
    return ((process*)a)->TIMEARR - ((process*)b)->TIMEARR;
}

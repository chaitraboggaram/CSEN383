#ifndef PAGING_H
#define PAGING_H

#define DEBUG 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_PROCESSES 150
#define TOTAL_DURATION 60
#define PROCESS_DURATION 7
#define TOTAL_PAGE 100

extern int SIMULATION_CLOCK;
extern int *PAGING_OPTIONS;

typedef struct {
    int pid, PGECNTER, TIMEARR, DURATION, PGCRR;
} process;

typedef struct page {
    int pid;
    int PAGE_NUMBER;
    struct page* next;
    float FTBOUGHT;
    float LONE;
    int CNTER;
} page;

typedef struct {
    page* HDL;
} LIST_OF_PAGES;

// Method to check which pages are free
int PageWhoAreFree(LIST_OF_PAGES*,int);
// Method to check existing pages in memory
int MemInPages(LIST_OF_PAGES*,int,int);

page* PAGEfrreeeg(LIST_OF_PAGES*);
// To free memory
void MEMRFree(LIST_OF_PAGES*,int);
// To initialize all pages
void InitializePageList(LIST_OF_PAGES*);
/// To display all pages
void DisplayPages(LIST_OF_PAGES*);

// To get all pages number
int PGNUMNXT(int,int);

// To compare between arrival time
int CompareArrivalTime(const void* ,const void*);
// Pagesid who are free
page* IDOfPageFree(LIST_OF_PAGES*,int,int);

void FCFS_FUNCTION(LIST_OF_PAGES*);
void LRU_FUNCTION(LIST_OF_PAGES*);
void LFU_FUNCTION(LIST_OF_PAGES*);
void MFU_FUNCTION(LIST_OF_PAGES*);
void R_FUNCTION(LIST_OF_PAGES*);

#endif

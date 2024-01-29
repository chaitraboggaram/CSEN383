#ifndef _stat_h_
#define _stat_h_
#include "process.h"

enum State {UNUSED, EMBRYO, SLEEPING, RUNNING, ZOMBIE};

struct process_stat_s {
	process * proc;
	float waitingTime;
	float turnaroundTime;
	float responseTime;
	float startTime;
	float endTime;
	float runTime;
	enum State state;
};

typedef struct process_stat_s process_stat;

struct stat_s {
	linked_list * processQueue;
	linked_list * time_chart;
	float avg_turnaround_time;
	float avg_waiting_time;
	float avg_response_time;
	float throughput;
	int total_quanta;
};

typedef struct stat_s scheduling_stat;

struct average_stats_s{
	float avg_response_time;
	float avg_wait_time;
	float avg_turnaround;
	float avg_throughput;
};

typedef struct average_stats_s average_stats;

average_stats firstComeFirstServeNP(linked_list * processes);
average_stats shortestJobFirstNP(linked_list * processes);
average_stats shortestRemainingTimeP(linked_list * processes);
average_stats roundRobin(linked_list * processes,int time_slice);
average_stats highestPriorityFirstP(linked_list * processes);
average_stats highestPriorityFirstNP(linked_list * processes);
average_stats highest_priority_first_p_aging(linked_list * processes);
average_stats highest_priority_first_np_aging(linked_list * processes);

// To print policy stat
average_stats printPolicyStat(linked_list * ll);

#endif

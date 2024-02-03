#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "queue.h"

#define SELLER_H 1
#define SELLER_M 3
#define SELLER_L 6
#define TOTAL_SELLERS 10
#define CONCERT_ROW 10
#define CONCERT_COL 10
#define sim_duration 60

//Struct for seller
typedef struct seller {
    char sellerNum;
    char sellerType;
    Queue *sellerQueue;
} Seller;

//Struct for customer
typedef struct customer {
    char customerNum;
    int arrivalTime;
    int responseTime;
    int turnaroundTime;
} Customer;

//Variables for threading
pthread_t sellerTID[TOTAL_SELLERS];
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t condMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadCreatedMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_waiting_for_clock_tick_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reservationMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadCompletionMutex = PTHREAD_MUTEX_INITIALIZER;

//Program logic variables
int N = 0;
int current_time_slice;
char seats_matrix[CONCERT_ROW][CONCERT_COL][5]; //This is a 10x10 matrix of strings with length 5
int responseTimeForH;
int responseTimeForM;
int responseTimeForL;
int turnaroundTimeForH;
int turnaroundTimeForM;
int turnaroundTimeForL;

int threadCount = 0;
int threads_waiting_for_clock_tick = 0;
int activeThread = 0;
int verbose = 0;

//This function will print the customer values in the queue
void printQueue(Queue *q)
{
    Node *ptr = q->head;
    while(ptr != NULL)
    {
        Customer *c = (Customer *)ptr->value;
        printf("(%d,%d)", c->customerNum, c->arrivalTime);
        ptr = ptr->next;
    }
}

int compareByArrivalTime(void *value1, void *value2)
{
    Customer *c1 = (Customer *)value1;
    Customer *c2 = (Customer *)value2;
    if(c1->arrivalTime < c2->arrivalTime)
        return -1;
    else if(c1->arrivalTime == c2->arrivalTime)
        return 0;
    else
        return 1;
}

Queue *generateCustomerQueue(int N)
{
    Queue *customerQueue = createQueue();
    char customerNum = 0;
    while(N--)
    {
        Customer *c = (Customer *)malloc(sizeof(Customer));
        c->customerNum = customerNum;
        c->arrivalTime = rand() % sim_duration;
        enqueue(customerQueue, c);
        customerNum++;
    }
    sort(customerQueue, compareByArrivalTime);
    Node *ptr = customerQueue->head;
    customerNum = 0;
    while(ptr != NULL)
    {
        customerNum++;
        Customer *c = (Customer *) ptr->value;
        c->customerNum = customerNum;
        ptr = ptr->next;
    }
    return customerQueue;
}

//This function will find the next available seat depending on seller type
int findAvailableSeat(char sellerType)
{
    int seatIdx = -1;
    if(sellerType == 'H')
    {
        for(int r_idx = 0; r_idx < CONCERT_ROW; r_idx++)
        {
            for(int c_idx = 0; c_idx < CONCERT_COL; c_idx++)
            {
                if(strcmp(seats_matrix[r_idx][c_idx],"-") == 0)
                {
                    seatIdx = r_idx * CONCERT_COL + c_idx;
                    return seatIdx;
                }
            }
        }
    }
    else if(sellerType == 'M')
    {
        int mid = CONCERT_ROW / 2;
        int rowJump = 0;
        while(1)
        {
            int r_idx = mid + rowJump;
            if(r_idx < CONCERT_ROW)
            {
                for(int c_idx = 0; c_idx < CONCERT_COL; c_idx++)
                {
                    if(strcmp(seats_matrix[r_idx][c_idx],"-") == 0)
                    {
                        seatIdx = r_idx * CONCERT_COL + c_idx;
                        return seatIdx;
                    }
                }
            }
            r_idx = mid - rowJump;
            if(r_idx >= 0)
            {
                for(int c_idx = 0; c_idx < CONCERT_COL; c_idx++)
                {
                    if(strcmp(seats_matrix[r_idx][c_idx],"-") == 0)
                    {
                        seatIdx = r_idx * CONCERT_COL + c_idx;
                        return seatIdx;
                    }
                }
            }
            if(mid + rowJump >= CONCERT_ROW && mid - rowJump < 0)
                break;
            rowJump++;
        }
    }
    else if(sellerType == 'L')
    {
        for(int r_idx = CONCERT_ROW - 1; r_idx >= 0; r_idx--)
        {
            for(int c_idx = CONCERT_COL - 1; c_idx >= 0; c_idx--)
            {
                if(strcmp(seats_matrix[r_idx][c_idx],"-") == 0)
                {
                    seatIdx = r_idx * CONCERT_COL + c_idx;
                    return seatIdx;
                }
            }
        }
    }
    return -1;
}

//Seller thread to serve one time slice (1 min)
void *sell(void *seller)
{
    Seller *args = (Seller *) seller;
    Queue *customerQueue = args->sellerQueue;
    Queue *sellerQueue = createQueue();
    char sellerType = args->sellerType;
    int sellerNum = args->sellerNum + 1;

    pthread_mutex_lock(&threadCreatedMutex);
    threadCount--;
    activeThread++;
    pthread_mutex_unlock(&threadCreatedMutex);

    Customer *c = NULL;
    int randomWaitTime = 0;

    while(current_time_slice < sim_duration)
    {
        pthread_mutex_lock(&condMutex);
        if(verbose)
            printf("00:%02d %c%02d waiting for next clock tick\n", current_time_slice, sellerType, sellerNum);
        
        pthread_mutex_lock(&thread_waiting_for_clock_tick_mutex);
        threads_waiting_for_clock_tick++;
        pthread_mutex_unlock(&thread_waiting_for_clock_tick_mutex);

        pthread_cond_wait(&cond, &condMutex);
        if(verbose)
            printf("00:%02d %c%02d received clock tick\n", current_time_slice, sellerType, sellerNum);
        pthread_mutex_unlock(&condMutex);

        //Sell
        if(current_time_slice == sim_duration) break;

        //New customers arrived
        while(customerQueue->size > 0 && ((Customer *)customerQueue->head->value)->arrivalTime <= current_time_slice)
        {
            Customer *temp = (Customer *) dequeue (customerQueue);
            enqueue(sellerQueue, temp);
            printf("| 00:%02d |    %c%d       | Customer %c%d%02d arrived                  |               |                 |\n", current_time_slice, sellerType, sellerNum, sellerType, sellerNum, temp->customerNum);
        }

        //Serve the next customer
        if(c == NULL && sellerQueue->size > 0)
        {
            c = (Customer *) dequeue(sellerQueue);
            c->responseTime = current_time_slice - c->arrivalTime;

            printf("| 00:%02d |    %c%d       | Serving Customer %c%d%02d                  |      %8d |                 |\n",
                    current_time_slice, sellerType, sellerNum, sellerType, sellerNum, c->customerNum, c->responseTime);
            if(sellerType == 'H')
            {
                randomWaitTime = (rand() % 2) + 1;
                responseTimeForH = responseTimeForH + c->responseTime;
            }
            else if(sellerType == 'M')
            {
                randomWaitTime = (rand() % 3) + 2;
                responseTimeForM = responseTimeForM + c->responseTime;
            }
            else if(sellerType == 'L')
            {
                randomWaitTime = (rand() % 4) + 4;
                responseTimeForL = responseTimeForL + c->responseTime;
            }
        }
        //Customer is being served
        if(c != NULL)
        {
            if(randomWaitTime == 0)
            {
                pthread_mutex_lock(&reservationMutex);

                //Find the next available seat
                int seatIdx = findAvailableSeat(sellerType);
                if(seatIdx == -1)
                {
                    printf("| 00:%02d |    %c%d       | Customer %c%d%02d has been told tickets are sold out.   |\n", 
                            current_time_slice, sellerType, sellerNum, sellerType, sellerNum, c->customerNum);
                }
                else
                {
                    int r_idx = seatIdx / CONCERT_COL;
                    int c_idx = seatIdx % CONCERT_COL;
                    c->turnaroundTime = c->turnaroundTime + current_time_slice;
                    sprintf(seats_matrix[r_idx][c_idx], "%c%d%02d", sellerType, sellerNum, c->customerNum);
                    printf("| 00:%02d |    %c%d       | Customer %c%d%02d is assigned seat %d       |             %d | %13d   |\n",
                            current_time_slice, sellerType, sellerNum, sellerType, sellerNum, c->customerNum, r_idx, c_idx, c->turnaroundTime);
                    
                    if(sellerType == 'H')
                    {
                        turnaroundTimeForH = turnaroundTimeForH + c->turnaroundTime;
                    }
                    else if(sellerType == 'M')
                    {
                        turnaroundTimeForM = turnaroundTimeForM + c->turnaroundTime;
                    }
                    else if(sellerType == 'L')
                    {
                        turnaroundTimeForL = turnaroundTimeForL + c->turnaroundTime;
                    }
                }
                pthread_mutex_unlock(&reservationMutex);
                c = NULL;
            }
            else randomWaitTime--;
        }
    }

    while(c != NULL || sellerQueue->size > 0)
    {
        if(c == NULL)
            c = (Customer *) dequeue(sellerQueue);
        printf("| 00:%02d |    %c%d       | Tickets Sold Out. Customer %c%d%02d Leaves |               |                 |\n",
        current_time_slice, sellerType, sellerNum, sellerType, sellerNum, c->customerNum);
        c = NULL;
    }
    pthread_mutex_lock(&threadCreatedMutex);
    activeThread--;
    pthread_mutex_unlock(&threadCreatedMutex);
    return NULL;
}

//This function creates all the seller threads
void createSellerThreads(pthread_t *thread, char sellerType, int numOfSellers)
{
    for(int t = 0; t < numOfSellers; t++)
    {
        Seller *createdSeller = (Seller *)malloc(sizeof(Seller));
        createdSeller->sellerNum = t;
        createdSeller->sellerType = sellerType;
        createdSeller->sellerQueue = generateCustomerQueue(N);

        pthread_mutex_lock(&threadCreatedMutex);
        threadCount++;
        pthread_mutex_unlock(&threadCreatedMutex);
        if(verbose)
            printf("Creating thread %c%02d\n", sellerType, t);
        pthread_create(thread+t, NULL, &sell, createdSeller);
    }
}

void wakeup_all_seller_threads()
{
    pthread_mutex_lock(&condMutex);
    if(verbose)
		printf("| 00:%02d | Main Thread Broadcasting Clock Tick\n", current_time_slice);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&condMutex);
}

//This function will check if all threads have 
//finished their jobs in the current time slice
void waitForThread()
{
    while(1)
    {
        pthread_mutex_lock(&thread_waiting_for_clock_tick_mutex);
        if(threads_waiting_for_clock_tick == activeThread)
        {
            threads_waiting_for_clock_tick = 0;
            pthread_mutex_unlock(&thread_waiting_for_clock_tick_mutex);
            break;
        }
        pthread_mutex_unlock(&thread_waiting_for_clock_tick_mutex);
    }
}

int main(int argc, char **argv)
{
    if(argc == 2)
    {
        N = atoi(argv[1]);
    }

    //Mark all seats as empty("-") at first
    for(int r = 0; r < CONCERT_ROW; r++)
    {
        for(int c = 0; c < CONCERT_COL; c++)
        {
            strncpy(seats_matrix[r][c],"-",1);
        }
    }

    //Create the threads
    createSellerThreads(sellerTID, 'H', SELLER_H);
    createSellerThreads(sellerTID + SELLER_H, 'M', SELLER_M);
    createSellerThreads(sellerTID + SELLER_H + SELLER_M, 'L', SELLER_L);

    //Wait for threads to initialize and wait for clock tick
    while(1)
    {
        pthread_mutex_lock(&threadCreatedMutex);
        if(threadCount == 0)
        {
            pthread_mutex_unlock(&threadCreatedMutex);
            break;
        }
        pthread_mutex_unlock(&threadCreatedMutex);
    }

    //Simulate each time slice as 1 iteration
    printf("\nStarting the Thread Simulation Process");
	printf("\n--------------------------------------------------------------------------------------------------\n");
	printf("| Time  | Seller Name |                 Activity               | Response Time | Turnaround Time |");
	printf("\n--------------------------------------------------------------------------------------------------\n");
	threads_waiting_for_clock_tick = 0;
	wakeup_all_seller_threads(); //For first tick

    do {
		//Wake up all thread
		waitForThread();
		current_time_slice = current_time_slice + 1;
		wakeup_all_seller_threads();
		//Wait for thread completion
	} while(current_time_slice < sim_duration);

    //Wakeup all threads so that none of them keep waiting for clock tick in limbo
    wakeup_all_seller_threads();

    //This is a loop to wait for all threads to finish executing
    while(activeThread);

    printf("\nThread Simulation Successfully Completed\n\n");
	//Display concert chart
	printf("\n\n");
	printf("Seat Claimed Summary");
	printf("\n=======================================================================================================================================\n\n");

	int h_customers = 0;
    int m_customers = 0;
    int l_customers = 0;
	for(int r = 0; r < CONCERT_ROW; r++) {
		for(int c = 0; c < CONCERT_COL; c++) {
			if(c!=0)
				printf("\t");
			printf("%5s", seats_matrix[r][c]);
			if(seats_matrix[r][c][0] == 'H') h_customers++;
			if(seats_matrix[r][c][0] == 'M') m_customers++;
			if(seats_matrix[r][c][0] == 'L') l_customers++;
		}
		printf("\n");
	}

	printf("\n\n=======================================================================================================================================");
	printf("\nMulti-threaded Ticket Sellers");
	printf("\nInput N = %02d\n",N);
	printf("=======================================================================================================================================\n\n");
	
	printf("---------------------------------------------------------------\n");
	printf("|%3c | Number of Customers | Sold Seats | Returned | Throughput|\n",' ');
	printf("----------------------------------------------------------------\n");
	printf("|%3c | %19d | %10d | %8d | %.2f      |\n",'H', SELLER_H * N, h_customers, (SELLER_H * N) - h_customers, (h_customers / 60.0));
	printf("|%3c | %19d | %10d | %8d | %.2f      |\n",'M', SELLER_M * N, m_customers, (SELLER_M * N) - m_customers, (m_customers / 60.0));
	printf("|%3c | %19d | %10d | %8d | %.2f      |\n",'L', SELLER_L * N, l_customers, (SELLER_L * N) - l_customers, (l_customers / 60.0));
	printf(" ---------------------------------------------------------------\n");
	printf("\n");

	printf("-------------------------------------------------\n");
	printf("|%3c   | Avg Response Time | Avg Turnaround time|\n",' ');
	printf("-------------------------------------------------\n");
	printf("| %3c  | %3f          | %.2f 		|\n",'H', responseTimeForH / (N * 1.0), turnaroundTimeForH / (N * 1.0));
	printf("| %3c  | %3f          | %.2f 		|\n",'M', responseTimeForM / (3.0 * N), turnaroundTimeForM / (3.0 * N));
	printf("| %3c  | %3f          | %.2f 		|\n",'L', responseTimeForL / (6.0 * N), turnaroundTimeForL / (6.0 * N));
	printf(" ------------------------------------------------\n");
	return 0;
}
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
#define CONCERT_COLUMN 10
#define DURATION 60

//Structure for customer
typedef struct customer {
    char customerNumber;
    int arrivalTime;
    int responseTime;
    int turnaroundTime;
} Customer;

//Structure for seller
typedef struct seller {
    char sellerNumber;
    char sellerType;
    Queue *sellerQueue;
} Seller;

//Threading-related variables
pthread_t sellerThreadID[TOTAL_SELLERS];
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t condMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadCreatedMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadWaitingForClockTickMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reservationMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadCompletionMutex = PTHREAD_MUTEX_INITIALIZER;

//Variables governing program logic
int N = 0;
int currentTimeSlice;
char seats_matrix[CONCERT_ROW][CONCERT_COLUMN][5];
int responseTimeForH;
int responseTimeForM;
int responseTimeForL;
int turnaroundTimeForH;
int turnaroundTimeForM;
int turnaroundTimeForL;
int threadCount = 0;
int threadsWaitingForClockTick = 0;
int activeThread = 0;
int flag = 0;

//Compare function for sorting customers by arrival time
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
    char customerNumber = 0;
    while(N--)
    {
        Customer *c = (Customer *)malloc(sizeof(Customer));
        c->customerNumber = customerNumber;
        c->arrivalTime = rand() % DURATION;
        enqueue(customerQueue, c);
        customerNumber++;
    }
    sort(customerQueue, compareByArrivalTime);
    Node *pointer = customerQueue->head;
    customerNumber = 0;
    while(pointer != NULL)
    {
        customerNumber++;
        Customer *c = (Customer *) pointer->value;
        c->customerNumber = customerNumber;
        pointer = pointer->next;
    }
    return customerQueue;
}

//This function locates the next available seat based on the seller type
int findAvailableSeat(char sellerType)
{
    int seatID = -1;
    if(sellerType == 'H')
    {
        for(int rowID = 0; rowID < CONCERT_ROW; rowID++)
        {
            for(int columnID = 0; columnID < CONCERT_COLUMN; columnID++)
            {
                if(strcmp(seats_matrix[rowID][columnID],"-") == 0)
                {
                    seatID = rowID * CONCERT_COLUMN + columnID;
                    return seatID;
                }
            }
        }
    }
    else if(sellerType == 'L')
    {
        for(int rowID = CONCERT_ROW - 1; rowID >= 0; rowID--)
        {
            for(int columnID = CONCERT_COLUMN - 1; columnID >= 0; columnID--)
            {
                if(strcmp(seats_matrix[rowID][columnID],"-") == 0)
                {
                    seatID = rowID * CONCERT_COLUMN + columnID;
                    return seatID;
                }
            }
        }
    }
    else if(sellerType == 'M')
    {
        int mid = CONCERT_ROW / 2;
        int skip = 0;
        while(1)
        {
            int rowID = mid + skip;
            if(rowID < CONCERT_ROW)
            {
                for(int columnID = 0; columnID < CONCERT_COLUMN; columnID++)
                {
                    if(strcmp(seats_matrix[rowID][columnID],"-") == 0)
                    {
                        seatID = rowID * CONCERT_COLUMN + columnID;
                        return seatID;
                    }
                }
            }
            rowID = mid - skip;
            if(rowID >= 0)
            {
                for(int columnID = 0; columnID < CONCERT_COLUMN; columnID++)
                {
                    if(strcmp(seats_matrix[rowID][columnID],"-") == 0)
                    {
                        seatID = rowID * CONCERT_COLUMN + columnID;
                        return seatID;
                    }
                }
            }
            if(mid + skip >= CONCERT_ROW && mid - skip < 0)
                break;
            skip++;
        }
    }
    return -1;
}

//Seller thread dedicated to handling one time slice (1 minute)
void *sell(void *seller)
{
    Seller *arguments = (Seller *) seller;
    Queue *customerQueue = arguments->sellerQueue;
    Queue *sellerQueue = createQueue();
    char sellerType = arguments->sellerType;
    int sellerNumber = arguments->sellerNumber + 1;

    pthread_mutex_lock(&threadCreatedMutex);
    threadCount--;
    activeThread++;
    pthread_mutex_unlock(&threadCreatedMutex);

    Customer *c = NULL;
    int randomWaitTime = 0;

    while(currentTimeSlice < DURATION)
    {
        pthread_mutex_lock(&condMutex);
        if(flag)
            printf("00:%02d %c%02d waiting for next clock tick\n", currentTimeSlice, sellerType, sellerNumber);
        
        pthread_mutex_lock(&threadWaitingForClockTickMutex);
        threadsWaitingForClockTick++;
        pthread_mutex_unlock(&threadWaitingForClockTickMutex);

        pthread_cond_wait(&cond, &condMutex);
        if(flag)
            printf("00:%02d %c%02d received clock tick\n", currentTimeSlice, sellerType, sellerNumber);
        pthread_mutex_unlock(&condMutex);

        //Process the sale operation
        if(currentTimeSlice == DURATION) break;

        //Handle the arrival of new customers
        while(customerQueue->size > 0 && ((Customer *)customerQueue->head->value)->arrivalTime <= currentTimeSlice)
        {
            Customer *temp = (Customer *) dequeue (customerQueue);
            enqueue(sellerQueue, temp);
            printf("| 00:%02d |    %c%d       | Customer %c%d%02d arrived                  |               |                 |\n", currentTimeSlice, sellerType, sellerNumber, sellerType, sellerNumber, temp->customerNumber);
        }

        //Process the service for the next customer
        if(c == NULL && sellerQueue->size > 0)
        {
            c = (Customer *) dequeue(sellerQueue);
            c->responseTime = currentTimeSlice - c->arrivalTime;

            printf("| 00:%02d |    %c%d       | Serving Customer %c%d%02d                  |      %8d |                 |\n",
                    currentTimeSlice, sellerType, sellerNumber, sellerType, sellerNumber, c->customerNumber, c->responseTime);
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

        //Customer is currently being served
        if(c != NULL)
        {
            if(randomWaitTime == 0)
            {
                pthread_mutex_lock(&reservationMutex);

                //Locate the next available seat
                int seatID = findAvailableSeat(sellerType);
                if(seatID == -1)
                {
                    printf("| 00:%02d |    %c%d       | Customer %c%d%02d has been told tickets are sold out.   |\n", 
                            currentTimeSlice, sellerType, sellerNumber, sellerType, sellerNumber, c->customerNumber);
                }
                else
                {
                    int rowID = seatID / CONCERT_COLUMN;
                    int columnID = seatID % CONCERT_COLUMN;
                    c->turnaroundTime = c->turnaroundTime + currentTimeSlice;
                    sprintf(seats_matrix[rowID][columnID], "%c%d%02d", sellerType, sellerNumber, c->customerNumber);
                    printf("| 00:%02d |    %c%d       | Customer %c%d%02d is assigned seat %d       |             %d | %13d   |\n",
                            currentTimeSlice, sellerType, sellerNumber, sellerType, sellerNumber, c->customerNumber, rowID, columnID, c->turnaroundTime);
                    
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
        currentTimeSlice, sellerType, sellerNumber, sellerType, sellerNumber, c->customerNumber);
        c = NULL;
    }
    pthread_mutex_lock(&threadCreatedMutex);
    activeThread--;
    pthread_mutex_unlock(&threadCreatedMutex);
    return NULL;
}

//Function to spawn threads for all sellers
void createSellerThreads(pthread_t *thread, char sellerType, int numOfSellers)
{
    for(int t = 0; t < numOfSellers; t++)
    {
        Seller *createdSeller = (Seller *)malloc(sizeof(Seller));
        createdSeller->sellerNumber = t;
        createdSeller->sellerType = sellerType;
        createdSeller->sellerQueue = generateCustomerQueue(N);

        pthread_mutex_lock(&threadCreatedMutex);
        threadCount++;
        pthread_mutex_unlock(&threadCreatedMutex);
        if(flag)
            printf("Creating thread %c%02d\n", sellerType, t);
        pthread_create(thread+t, NULL, &sell, createdSeller);
    }
}

//Function to wake up all the seller threads
void wakeup_all_seller_threads()
{
    pthread_mutex_lock(&condMutex);
    if(flag)
		printf("| 00:%02d | Main Thread Broadcasting Clock Tick\n", currentTimeSlice);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&condMutex);
}

//Function to ensure completion of all threads for the current time slice
void waitForThread()
{
    while(1)
    {
        pthread_mutex_lock(&threadWaitingForClockTickMutex);
        if(threadsWaitingForClockTick == activeThread)
        {
            threadsWaitingForClockTick = 0;
            pthread_mutex_unlock(&threadWaitingForClockTickMutex);
            break;
        }
        pthread_mutex_unlock(&threadWaitingForClockTickMutex);
    }
}

//Function to print the queue values
void printQueue(Queue *q)
{
    Node *pointer = q->head;
    while(pointer != NULL)
    {
        Customer *c = (Customer *)pointer->value;
        printf("(%d,%d)", c->customerNumber, c->arrivalTime);
        pointer = pointer->next;
    }
}

int main(int argc, char **argv)
{
    if(argc == 2)
    {
        N = atoi(argv[1]);
    }

    //Initialize all seats as vacant ("-") initially
    for(int r = 0; r < CONCERT_ROW; r++)
    {
        for(int c = 0; c < CONCERT_COLUMN; c++)
        {
            strncpy(seats_matrix[r][c],"-",1);
        }
    }

    //Spawn the threads
    createSellerThreads(sellerThreadID, 'H', SELLER_H);
    createSellerThreads(sellerThreadID + SELLER_H, 'M', SELLER_M);
    createSellerThreads(sellerThreadID + SELLER_H + SELLER_M, 'L', SELLER_L);

    //Wait for threads to be set up and ready, anticipating the clock tick
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

    //Consider each time slice as one iteration in the simulation
    printf("\nStarting the Thread Simulation Process");
	printf("\n--------------------------------------------------------------------------------------------------\n");
	printf("| Time  | Seller Name |                 Activity               | Response Time | Turnaround Time |");
	printf("\n--------------------------------------------------------------------------------------------------\n");
	threadsWaitingForClockTick = 0;
	wakeup_all_seller_threads();

    do {
		waitForThread();
		currentTimeSlice = currentTimeSlice + 1;
		wakeup_all_seller_threads();
	} while(currentTimeSlice < DURATION);

    //This loop is designed to wait until all threads complete their execution
    wakeup_all_seller_threads();

    //This is an iteration to await the completion of all concurrently executing threads
    while(activeThread);

    //Display seats chart
    printf("--------------------------------------------------------------------------------------------------\n");
    printf("\nThread Simulation Successfully Completed\n\n");
	printf("\n\n");
	printf("\n===============================================================================================================\n");
	printf("                                        Seat Claim Summary");
	printf("\n===============================================================================================================\n\n");

	int h_customers = 0;
    int m_customers = 0;
    int l_customers = 0;
	for(int r = 0; r < CONCERT_ROW; r++) {
		for(int c = 0; c < CONCERT_COLUMN; c++) {
			if(c!=0)
				printf("\t");
			printf("%5s", seats_matrix[r][c]);
			if(seats_matrix[r][c][0] == 'H') h_customers++;
			if(seats_matrix[r][c][0] == 'M') m_customers++;
			if(seats_matrix[r][c][0] == 'L') l_customers++;
		}
		printf("\n");
	}

	printf("\n\n===============================================================================================================");
	printf("\n                                 Multi-threading with %02d Ticket Sellers\n",N);
	printf("===============================================================================================================\n\n");
	
	printf("---------------------------------------------------------------------------------------------------------------\n");
	printf("| Seller | Number of Customers | Sold Seats | Returned | Throughput | Avg Response Time | Avg Turnaround time |\n");
	printf("---------------------------------------------------------------------------------------------------------------\n");
	printf("| %3c    | %19d | %10d | %8d | %.2f       | %3f          | %.2f               |\n",'H', SELLER_H * N, h_customers, (SELLER_H * N) - h_customers, (h_customers / 60.0), responseTimeForH / (N * 1.0), turnaroundTimeForH / (N * 1.0));
	printf("| %3c    | %19d | %10d | %8d | %.2f       | %3f          | %.2f               |\n",'M', SELLER_M * N, m_customers, (SELLER_M * N) - m_customers, (m_customers / 60.0), responseTimeForM / (3.0 * N), turnaroundTimeForM / (3.0 * N));
	printf("| %3c    | %19d | %10d | %8d | %.2f       | %3f          | %.2f               |\n",'L', SELLER_L * N, l_customers, (SELLER_L * N) - l_customers, (l_customers / 60.0), responseTimeForL / (6.0 * N), turnaroundTimeForL / (6.0 * N));
	printf(" --------------------------------------------------------------------------------------------------------------\n");
	printf("\n");
	return 0;
}
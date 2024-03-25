#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1
#define NUM_PIPES 5
#define PIPE_DURATION 30

fd_set set_fds, active_fds;
FILE *log_file;
char read_buffer[BUFFER_SIZE];
char input_buffer[BUFFER_SIZE-10];
int pipes[NUM_PIPES][2];
int is_timeout = 0;
int msg_counter = 1;
struct timeval start_timeval;
struct itimerval count_down_timer;
time_t start_time;

// Function to read from a pipe
void Rfp(int *pipe_descriptor, int end_of_pipe, int pipe_index)
{
    if (!is_timeout)
    {
        close(pipe_descriptor[WRITE_END]); // Close the write end of the pipe

        struct timeval current_time;
        gettimeofday(&current_time, NULL); // Get the current time

        // Calculate the time elapsed since start
        float elapsed_time = (float)((current_time.tv_sec - start_timeval.tv_sec) + 
                            ((current_time.tv_usec - start_timeval.tv_usec) / 1000000.));

        // Read from the pipe end into the buffer
        read(end_of_pipe, read_buffer, BUFFER_SIZE);
        if (pipe_index == 4)
        {
            fprintf(log_file, "%6.3f: %s", elapsed_time, read_buffer);
        }
        else
        {
            fprintf(log_file, "%6.3f: %s\n", elapsed_time, read_buffer);
        }
    }
}

// Function to write into a pipe
void Wtp(int *pipe_descriptor)
{
    if (!is_timeout)
    {
        close(pipe_descriptor[READ_END]); // Close the read end of the pipe
        write(pipe_descriptor[WRITE_END], read_buffer, BUFFER_SIZE); // Write into the pipe
    }
}

// Handler for the interrupt signal
void alarm_handler(int sig)
{
    assert(sig == SIGALRM);
    is_timeout = 1;
    exit(0);
}

int main()
{
    log_file = fopen("output.txt", "w"); // Open the log file

    time(&start_time); // Record the start time
    count_down_timer.it_value.tv_sec = PIPE_DURATION; // Set the timeout duration
    count_down_timer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &count_down_timer, NULL);
    gettimeofday(&start_timeval, NULL);
    signal(SIGALRM, alarm_handler); // Set the alarm signal handler

    srand(time(NULL));
    int random_seed;

    FD_ZERO(&active_fds);

    int i, pipe_selected;
    pid_t pid;

    // Create child processes and their associated pipes
    for (i = 0; i < NUM_PIPES; i++)
    {
        random_seed = rand();
        if (pipe(pipes[i]) == -1)
        {
            perror("Pipe creation failed");
            exit(1);
        }

        FD_SET(pipes[i][READ_END], &active_fds);

        pid = fork(); // Fork a child process
        if (pid == -1)
        {
            perror("Fork failed");
            exit(1);
        }

        if (pid == 0) // In child process
        {
            fflush(stdout);
            srand(random_seed); // Ensure different seed for each child
            break;
        }
    }

    // Main loop
    while (!is_timeout)
    {
        // Parent Process
        if (pid > 0)
        {
            set_fds = active_fds;

            pipe_selected = select(FD_SETSIZE, &set_fds, NULL, NULL, NULL);
            if (pipe_selected < 0)
            {
                perror("Select error");
                exit(1);
            }
            else if (pipe_selected == 0)
            {
                // This block should not be reached as timeout is NULL
                perror("No data to read");
            }
            else
            {
                for (i = 0; i < NUM_PIPES; i++)
                {
                    if (FD_ISSET(pipes[i][READ_END], &set_fds))
                    {
                        Rfp(pipes[i], pipes[i][READ_END], i);
                    }
                }
            }
        }
        // Child Process
        else
        { 
            if (i == 4)
            {
                printf("Input for Child 5:: ");
                fgets(input_buffer, BUFFER_SIZE, stdin);
                snprintf(read_buffer, BUFFER_SIZE, "Child 5: %s", input_buffer);
                Wtp(pipes[i]);
            }
            else
            {
                snprintf(read_buffer, BUFFER_SIZE, "Child %d Message %d", i+1, msg_counter++);
                Wtp(pipes[i]);
                sleep(rand() % 3); // Random sleep duration (0, 1, or 2 seconds)
            }
        }
    }
    fclose(log_file); // Close the log file
    exit(0);
}

CC = gcc
CFLAGS = -Wall -Wextra

SRCS = utility.c process.c FCFS.c HPFNP.c HPFP.c RR.c SJF.c SRT.c main.c
OBJS = $(SRCS:.c=.o)
EXEC = main

.PHONY: all clean run

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: clean all
	./$(EXEC) $(RR_TIME_SLICE)

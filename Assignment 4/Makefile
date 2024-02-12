CC = gcc
CFLAGS = -Wall -Wextra

SRCS = SIMULATION.c PAGE.c FCFS.c LFU.c MFU.c LRU.c R.c
OBJS = $(SRCS:.c=.o)
EXEC = SIMULATION

.PHONY: all clean run

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: clean all
	./$(EXEC) FCFS
	./$(EXEC) LRU
	./$(EXEC) Random
	./$(EXEC) MFU
	./$(EXEC) LFU

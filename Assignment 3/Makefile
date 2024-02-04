CC = gcc
CFLAGS = -Wall -Wextra -pthread
SRCS = tickets.c queue.c
OBJS = $(SRCS:.c=.o)
TARGET = p3
DEFAULT_ARG = 10

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) $(arg)

.DEFAULT_GOAL := all
arg ?= $(DEFAULT_ARG)

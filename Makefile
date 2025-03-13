CC = gcc
CFLAGS = -Wall -Wextra -g3
EXEC = myshell
SRCS = main.c parsing.c execution.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.h)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

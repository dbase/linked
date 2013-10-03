CC	?= gcc

CFLAGS	= -Isrc/ -g -O0
LFLAGS	=
WARN	= -Wall -Wextra -Werror

SRC	= src/linked.c test/test.c
DEPS	= src/linked.h

NAME	= linked

.PHONY: test

test: $(SRC) $(DEPS)
	$(CC) $(WARN) $(CFLAGS) $(LFLAGS) $(SRC) -o $(NAME)

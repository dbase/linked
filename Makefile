CC	?= gcc

CFLAGS	= -Isrc/ -g -O0
LFLAGS	=
WARN	= -Wall -Wextra -Werror

SRC	= src/linked.c tests/test.c
DEPS	= src/linked.h

NAME	= test

test: $(SRC) $(DEPS)
	$(CC) $(WARN) $(CFLAGS) $(LFLAGS) $(SRC) -o $(NAME)

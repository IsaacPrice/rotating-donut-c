CC = gcc
CFLAGS_DEBUG = -g -Wall -Wextra -pedantic
CFLAGS_RELEASE = -O3 -march=native -Wall -Wextra -pedantic
LDFLAGS = -lm

SRC = $(wildcard *.c)

all: release

debug:
	$(CC) $(CFLAGS_DEBUG) -o debug.out $(SRC) $(LDFLAGS)

release:
	$(CC) $(CFLAGS_RELEASE) -o release.out $(SRC) $(LDFLAGS)

clean:
	rm -f *.out


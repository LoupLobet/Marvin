CC=cc

CFLAGS=-Wall -pedantic -std=c99
# OSX
LNCURSES=-lncurses
# Linux uncomment
# LNCURSES=-lncursesw
LDFLAGS=$(LNCURSES) -lm

PROG=marvin
OBJS=\
	buffer.o\
	rune.o\
	main.o

all: clean $(PROG)

clean:
	rm -f *.o

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $<

# utf/
rune.o: utf/rune.c
	$(CC) -c $^ $(CFLAGS)

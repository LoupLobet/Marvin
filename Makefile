CC=cc

LIBS=-L/opt/homebrew/lib
INCS=-I/opt/homebrew/include

CFLAGS=-Wall -pedantic -std=c99 $(INCS) -D_POSIX_C_SOURCE=200809L
LDFLAGS=$(LIBS) -lm -lSDL2 -lSDL2_ttf

PROG=marvin
OBJS=\
	buffer.o\
	draw.o\
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

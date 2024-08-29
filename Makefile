CC = gcc
CFLAGS = -std=c11 -Wall -Werror -D_DEFAULT_SOURCE -g -O0
LDFLAGS = -lm

SRCDIR = $(shell basename $(shell pwd))
DESTDIR ?= 
PREFIX ?= /usr

SRC0 =  src/synth.c src/main.c
OBJ0 = $(SRC0:%.c=%.c.o)
EXE0 = dicsnd

all: $(EXE0)

$(EXE0): $(OBJ0)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ0) $(EXE0)

install:
	cp $(EXE0) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXE0)

dist:
	cd .. && tar cvzf $(SRCDIR).tgz ./$(SRCDIR)


CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror -Wno-unused-function -g
LDFLAGS=-lm -lasound

PROJECT=$(shell basename $$(pwd))
VERSION=1.0
TARBALL=$(PROJECT)-$(VERSION).tar.xz

SOURCE1=\
	dicsnd.c\
	sample.c

OBJECT1=$(SOURCE1:%.c=%.c.o)
TARGET1=sample

SOURCE2=\
	dicsnd.c\
	play.c

OBJECT2=$(SOURCE2:%.c=%.c.o)
TARGET2=play

TARGETS=\
	$(TARGET1)\
	$(TARGET2)

.PHONY: all clean dist
all: $(TARGETS)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET1): $(OBJECT1)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TARGET2): $(OBJECT2)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@echo -n "Cleaning project ${PROJECT}... "
	@rm -f *.c.o *.bak $(TARGETS) && echo "done!" || echo "failed!"

dist: clean
	@echo "Building distribution: ${PROJECT}"
	@( cd .. && tar cv ./$(PROJECT) | xz -9 > $(TARBALL) ) && echo "done!" || echo "failed!"


#
# Makefile
#

CC = gcc
# -Wall
CFLAGS = -I.

OBJDIR = build/objects/
BINDIR = build/bin/

all: build

build: image/image.c loader/loader.c main.c
	mkdir -p $(OBJDIR) $(BINDIR)
	$(CC) $(CFLAGS) -o $(OBJDIR)/image.o -c image/image.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/loader.o -c loader/loader.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/main.o -c main.c
	$(CC) $(CFLAGS) -o $(BINDIR)/med $(OBJDIR)/image.o $(OBJDIR)/loader.o $(OBJDIR)/main.o

clean:
	rm -rf build

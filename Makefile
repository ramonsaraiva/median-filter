#
# Makefile
#

CC = gcc
# -Wall
CFLAGS = -I. -pthread

OBJDIR = build/objects/
BINDIR = build/bin/

all: $(BINDIR)/med

build:
	mkdir -p $(OBJDIR) $(BINDIR)

$(OBJDIR)/image.o: image/image.c build
	$(CC) $(CFLAGS) -o $(OBJDIR)/image.o -c image/image.c

$(OBJDIR)/loader.o: loader/loader.c build
	$(CC) $(CFLAGS) -o $(OBJDIR)/loader.o -c loader/loader.c

$(OBJDIR)/main.o: main.c build
	$(CC) $(CFLAGS) -o $(OBJDIR)/main.o -c main.c

$(BINDIR)/med: $(OBJDIR)/image.o $(OBJDIR)/loader.o $(OBJDIR)/main.o
	$(CC) $(CFLAGS) -o $(BINDIR)/med $(OBJDIR)/image.o $(OBJDIR)/loader.o $(OBJDIR)/main.o

clean:
	rm -rf build

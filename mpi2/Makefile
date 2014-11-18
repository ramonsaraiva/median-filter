#
# Makefile
#

CC = mpicc
# -Wall
CFLAGS = -I.

OBJDIR = build/objects/
BINDIR = build/bin/

all: $(BINDIR)/med $(BINDIR)/child

build:
	mkdir -p $(OBJDIR) $(BINDIR)

$(OBJDIR)/image.o: image/image.c build
	$(CC) $(CFLAGS) -o $(OBJDIR)/image.o -c image/image.c

$(OBJDIR)/loader.o: loader/loader.c build
	$(CC) $(CFLAGS) -o $(OBJDIR)/loader.o -c loader/loader.c

$(OBJDIR)/exectime.o: exectime/exectime.c build
	$(CC) $(CFLAGS) -o $(OBJDIR)/exectime.o -c exectime/exectime.c

$(OBJDIR)/main.o: main.c build
	$(CC) $(CFLAGS) -o $(OBJDIR)/main.o -c main.c

$(OBJDIR)/child.o: child.c build
	$(CC) $(CFLAGS) -o $(OBJDIR)/child.o -c child.c

$(BINDIR)/med: $(OBJDIR)/image.o $(OBJDIR)/loader.o $(OBJDIR)/exectime.o $(OBJDIR)/main.o
	$(CC) $(CFLAGS) -o $(BINDIR)/med $(OBJDIR)/image.o $(OBJDIR)/loader.o $(OBJDIR)/exectime.o $(OBJDIR)/main.o

$(BINDIR)/child: $(OBJDIR)/image.o $(OBJDIR)/loader.o $(OBJDIR)/exectime.o $(OBJDIR)/child.o
	$(CC) $(CFLAGS) -o $(BINDIR)/child $(OBJDIR)/image.o $(OBJDIR)/loader.o $(OBJDIR)/exectime.o $(OBJDIR)/child.o

clean:
	rm -rf build

CC=gcc
CFLAGS=-g -Wall

BINDIR = ./bin
OBJDIR = ./build

all: gzep

gzep: main.c $(OBJDIR)/hashtable.o $(OBJDIR)/heap.o $(OBJDIR)/stack.o $(OBJDIR)/huffman.o
	$(CC) $(CFLAGS) -o ./bin/$@ $^;

$(OBJDIR)/%.o: ./core/%.impl.c
	$(CC) $(CFLAGS) -c -o $@ $^;

clean:
	rm -r $(OBJDIR)/* $(BINDIR)/*
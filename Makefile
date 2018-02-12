CC = gcc
CFLAGS = -Wall -pedantic -g
MAIN = tree
OBJS = tree.o
all : $(MAIN)

$(MAIN) : $(OBJS) tree.h
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

tree.o : tree.c tree.h
	$(CC) $(CFLAGS) -c tree.c

clean :
	rm *.o $(MAIN) core

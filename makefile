IDIR     = inc
SDIR     = src
BDIR     = bin

BIN	 = $(BDIR)/pricer
CC	 = g++
CFLAGS	 = -std=c++14 -c -Wall -I$(IDIR)

OBJS	 = main.o pricer.o
SOURCE	 = main.cpp pricer.cpp
DEPS     = pricer.hpp

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(BIN) 

main.o: $(SDIR)/main.cpp
	$(CC) $(CFLAGS) $(SDIR)/main.cpp 

pricer.o: $(SDIR)/pricer.cpp
	$(CC) $(CFLAGS) $(SDIR)/pricer.cpp 

.PHONY: clean

clean:
	rm -f $(OBJS) $(BIN)

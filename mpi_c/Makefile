SHELL:=/bin/bash
CFLAGS = -Wall -Werror -lm
CC = mpicc
OBJS = juliaset.o cmdl.o grid.o

all: juliaset


juliaset: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -rf *.out *.o juliaset *.dat

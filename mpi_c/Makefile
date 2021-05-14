SHELL:=/bin/bash
CFLAGS = -Wall -Werror
CC = mpicc
OBJS = juliaset.o cmdl.o

all: juliaset

juliaset: $(OBJS)
	module purge 
	module load cpu 
	module load aocc 
	module load openmpi/4.0.4 
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -rf *.out *.o juliaset

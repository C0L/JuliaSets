SHELL:=/bin/bash
CFLAGS = -Wall -Werror
#CC = mpicc
OBJS = juliaset.o cmdl.o

all: mods juliaset


mods:
	module purge 
	module load cpu 
	module load aocc 
	module load openmpi/4.0.4

juliaset: $(OBJS)
#	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -rf *.out *.o juliaset

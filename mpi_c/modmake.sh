#!/bin/bash

module purge 
module load cpu 
module load gcc
module load openmpi/4.0.4
#module load aocc 
#module load openmpi/4.0.4I

make juliaset

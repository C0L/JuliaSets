#!/bin/bash

module purge 
module load cpu 
module load aocc 
module load openmpi/4.0.4

make juliaset

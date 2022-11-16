Instituto Superior Técnico

Master's Degree in Computer Science and Engineering

Parallel and Distributed Computing 2021/2022

# Parallel programing on shared-memory and distributed-memory systems.

## Authors

**Group 11**

81120 Filipe Sousa

92513 Mafalda Ferreira

93114 Lucas Raimundo

## Project Description

The purpose of this class project is to give students hands-on experience in parallel programming on both shared-memory and distributed-memory systems, using OpenMP and MPI, respectively. For this assignment you are to write a sequential and two parallel implementations of a program to simulate an ecosystem with two species: the Iberian fox (Vulpes vulpes) and the Iberian rabbit (Oryctolagus cuniculus). The simulation takes place on a square grid containing cells. At the start, some of the cells are occupied by either a rabbit, a fox, or a rock, the rest are empty. The simulation consists of computing how the population evolves over discrete time steps (generations) according to certain rules described next.

## Part 1 - Serial implementation

The program should take ten command line parameters, all positive integers:

    foxes-rabbits <# generations> <M> <N> <# rocks> <# rabbits> <rabbit breeding> <# foxes> <fox breeding> <fox starvation> <seed>

## Part 2 - OpenMP implementation

OpenMP (Open Multi-Processing) is an application programming interface (API) that supports multi-platform shared-memory multiprocessing programming.

Setting of the OMP_NUM_THREADS environment variable:

    export OMP_NUM_THREADS = <num_threads>

## Part 3 - MPI implementation

Open MPI is a Message Passing Interface (MPI) library project combining technologies and resources from several other projectsOpen MPI is a Message Passing Interface (MPI) library project combining technologies and resources from several other projects


Compile a program using MPI:

    mpicc -g -o foxes-rabbits foxes-rabbits.c
    
Run program using flag -n with p processors. Arguments come after program name:
    
    mpirun -n <p> foxes-rabbits <arguments>
    
Running the program on RNL's where istTWXYZ is your Fenix id.

    ssh istTWXYZ@cluster.rnl.tecnico.ulisboa.
    
    cd $CLUSTER_HOME
    
    srun -n <number of nodes> <executable> <args>

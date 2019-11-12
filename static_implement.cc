#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <mpi.h>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include "TMPI.hpp"
int main(int argc, char** argv){
    //suppose size == 10;
    MPI_Init(&argc, &argv);  
    int rank, size;  
    std::string tif(argv[1]);
    std::string csv(argv[2]);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    ThunderStormMpi t(tif, csv, size, rank);
    for (int i=rank; i<10; i+=size){
        t.GenCSV(i);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0) {
        t.ReduceCSV();
    }
    MPI_Finalize();
    return 0;
}
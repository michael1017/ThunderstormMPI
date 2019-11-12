#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <mpi.h>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <pthread.h>
#include "TMPI.hpp"
using namespace std;

int total_jobs = 10;
atomic_int current_jobID;
int MPI_size;
int MPI_rank;

void* job_distribute(void* _arg) {
	
    char recv_buf;
    int send_buf;
    MPI_Status status;

    send_buf = current_jobID++;
    while (send_buf < total_jobs) {
        MPI_Recv(&recv_buf, 1, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&send_buf, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        send_buf = current_jobID++;
    }
    
    int finished_proc = 1;	// the finished processed
    send_buf = -1;
	while (finished_proc < MPI_size) {
		MPI_Recv(&recv_buf, 1, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Send(&send_buf, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
        finished_proc++;
	}
    pthread_exit(NULL);
}

int main(int argc, char** argv){
    //suppose size == 10;
    MPI_Init(&argc, &argv);  
    int job_id;
    string tif(argv[1]);
    string csv(argv[2]);
    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);
    ThunderStormMpi t(tif, csv, MPI_size, MPI_rank);

    MPI_Barrier(MPI_COMM_WORLD);

    if (MPI_rank == 0) {
        // create a thread to distribute jobs
        pthread_t thread;
	    pthread_create(&thread, 0, job_distribute, NULL);
        MPI_Barrier(MPI_COMM_WORLD);
        int job_id;
        int exec_status;
        while (current_jobID < total_jobs) {
            job_id = current_jobID++;
            
            t.GenCSV(job_id);
            
        }
        pthread_join(thread, NULL);
        cout << "Rank " << MPI_rank << " end processing" << endl;
        // merge data

        cout << "Rank " << MPI_rank << " start to merge data" << endl;
        t.ReduceCSV();
        cout << "Finish merging" << endl;
    }
    else {
        int job_id;
        char send_buf;
        int exec_status;
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Send(&send_buf, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        while (job_id != -1) {
            t.GenCSV(job_id);

            MPI_Send(&send_buf, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        cout << "Rank " << MPI_rank << " end" << endl;
    }
    MPI_Finalize();
    return 0;
}


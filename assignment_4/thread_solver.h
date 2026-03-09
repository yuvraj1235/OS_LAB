#ifndef THREAD_SOLVER_H
#define THREAD_SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "matrix_math.h"
#include "logger.h"

typedef struct
{
    int start_col;       
    int end_col;         
    Matrix *A_orig;      
    double *B;           
    double det_A;        
    double *solutions;   
    int thread_id;       // Added to make logs easier to read
} ThreadData;

void* worker_thread(void* arg)
{
    ThreadData *data = (ThreadData*)arg;

    log_msg("[Thread %d] Started processing columns %d to %d.\n", 
            data->thread_id, data->start_col, data->end_col - 1);

    for(int i=data->start_col;i<data->end_col;i++)
    {
        Matrix *local_A = copy_matrix(data->A_orig);
        replace_column(local_A, data->B, i);
        
        double det_Ai = fast_determinant(local_A);
        data->solutions[i] = det_Ai / data->det_A;
        
        free_matrix(local_A);

        // Log progress every 50 columns to avoid massive file sizes
        if((i + 1) % 50 == 0 || i == data->end_col - 1)
            log_msg("[Thread %d] Finished column %d.\n", data->thread_id, i);
    }
    
    log_msg("[Thread %d] Completed workload.\n", data->thread_id);
    pthread_exit(NULL);
}

double solve_system_pthreads(Matrix *A, double *B, int N, int cores, double det_A, double *solutions)
{
    pthread_t threads[cores];
    ThreadData t_data[cores];
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int chunk = (N + cores - 1) / cores;
    
    for(int p=0;p<cores;p++)
    {
        t_data[p].start_col = p * chunk;
        t_data[p].end_col = (p + 1) * chunk;
        if(t_data[p].end_col > N) t_data[p].end_col = N;
        
        t_data[p].A_orig = A;
        t_data[p].B = B;
        t_data[p].det_A = det_A;
        t_data[p].solutions = solutions;
        t_data[p].thread_id = p; // Assign a clean ID (0, 1, 2...)

        if(t_data[p].start_col < N)
            pthread_create(&threads[p], NULL, worker_thread, (void*)&t_data[p]);
    }

    for (int p = 0; p < cores; p++)
    {
        if (t_data[p].start_col < N)
            pthread_join(threads[p], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

#endif
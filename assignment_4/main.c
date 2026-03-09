#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "logger.h"
#include "matrix_math.h"

int main()
{
    // 1. Start the Logger immediately
    init_logger("cramer_execution_log.txt");
    
    log_msg("==========================================\n");
    log_msg(" GAUSSIAN ELIMINATION SOLVER \n");
    log_msg("==========================================\n");
    
    int N;
    // We still use standard printf for the user prompt since it requires keyboard input
    printf("Enter number of equations (N) [e.g., 1000]: ");
    if (scanf("%d", &N) != 1 || N <= 0) N = 300;
    
    log_msg("[User Input] Matrix Size set to N = %d\n", N);
    
    srand(time(NULL));
    Matrix *A = create_matrix(N);
    double *B = malloc(N * sizeof(double));
    double *solutions = malloc(N * sizeof(double));
    
    log_msg("[Init] Generating random coefficients...\n");
    fill_random_system(A, B);

    log_msg("[Math] Solving system using Gaussian elimination...\n");
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    solve_gaussian(A, B, solutions);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    log_msg("------------------------------------------\n");
    log_msg("[Success] Calculation Complete!\n");
    log_msg("[Performance] Total Time Elapsed: %.4f seconds\n", time_taken);
    
    log_msg("[Results] Preview of Solutions (First 10):\n");
    int preview = (N < 10) ? N : 10;
    for(int i = 0; i < preview; i++) {
        log_msg(" x[%d] = %f\n", i, solutions[i]);
    }

    free(solutions); 
    free(B); 
    free_matrix(A);
    
    log_msg("[System] Shutting down normally.\n");
    close_logger();
    
    return 0;
}
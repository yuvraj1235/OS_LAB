#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#define CORES 8

typedef struct { double log_val; int sign; } LogDet;

LogDet calculate_log_det(int n, double **matrix) {
    double **a = malloc(n * sizeof(double *));
    for(int i=0; i<n; i++) {
        a[i] = malloc(n * sizeof(double));
        for(int j=0; j<n; j++) a[i][j] = matrix[i][j];
    }
    double log_sum = 0; int sign = 1;
    for (int i = 0; i < n; i++) {
        int pivot = i;
        for (int j = i + 1; j < n; j++)
            if (fabs(a[j][i]) > fabs(a[pivot][i])) pivot = j;
        
        double *temp = a[i]; a[i] = a[pivot]; a[pivot] = temp;
        if (pivot != i) sign *= -1;
        if (fabs(a[i][i]) < 1e-18) { sign = 0; break; }
        
        if (a[i][i] < 0) sign *= -1;
        log_sum += log(fabs(a[i][i]));

        for (int j = i + 1; j < n; j++) {
            double factor = a[j][i] / a[i][i];
            for (int k = i + 1; k < n; k++) a[j][k] -= factor * a[i][k];
        }
    }
    for(int i=0; i<n; i++) free(a[i]); free(a);
    return (LogDet){log_sum, sign};
}

int main() {
    int n;
    printf("FASTsolve (File-based) - Enter N: ");
    scanf("%d", &n);

    double **A = malloc(n * sizeof(double *));
    double *B = malloc(n * sizeof(double));
    double *results = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) A[i] = malloc(n * sizeof(double));

    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        B[i] = (rand() % 10) + 1;
        for (int j = 0; j < n; j++) A[i][j] = (rand() % 10) + 1;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    LogDet D = calculate_log_det(n, A);
    if (D.sign == 0) { printf("Singular Matrix!\n"); return 1; }

    for (int p = 0; p < CORES; p++) {
        if (fork() == 0) {
            char filename[20];
            sprintf(filename, "part_%d.txt", p);
            FILE *f = fopen(filename, "w");
            
            for (int j = p; j < n; j += CORES) {
                double **Ai = malloc(n * sizeof(double *));
                for(int r=0; r<n; r++) {
                    Ai[r] = malloc(n * sizeof(double));
                    for(int c=0; c<n; c++) Ai[r][c] = (c == j) ? B[r] : A[r][c];
                }
                LogDet Di = calculate_log_det(n, Ai);
                double xi = (double)(Di.sign * D.sign) * exp(Di.log_val - D.log_val);
                fprintf(f, "%d %f\n", j, xi); 
                for(int r=0; r<n; r++) free(Ai[r]); free(Ai);
            }
            fclose(f);
            exit(0);
        }
    }

    while (wait(NULL) > 0);

    for (int p = 0; p < CORES; p++) {
        char filename[20];
        sprintf(filename, "part_%d.txt", p);
        FILE *f = fopen(filename, "r");
        int idx; double val;
        while (fscanf(f, "%d %lf", &idx, &val) != EOF) {
            results[idx] = val;
        }
        fclose(f);
        remove(filename);
    }

    gettimeofday(&end, NULL);
    printf("\nSolutions : \n");
    for(int i=0; i<n; i++) printf("\nx%d = %.2f", i+1, results[i]);
    printf("\n\nExecution Time: %ld ms\n", (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);
    
    return 0;
}
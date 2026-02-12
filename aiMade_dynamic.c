#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

// Struct to hold log-determinant and sign
typedef struct { double l_val; int sign; } det_t;

// O(N^3) Determinant function using Log-arithmetic
det_t get_det(int n, double **matrix) {
    double **a = malloc(n * sizeof(double *));
    for(int i = 0; i < n; i++) {
        a[i] = malloc(n * sizeof(double));
        for(int j = 0; j < n; j++)
            a[i][j] = matrix[i][j];
    }

    double sum = 0;
    int s = 1;

    for (int i = 0; i < n; i++) {
        int p = i;
        for (int j = i + 1; j < n; j++)
            if (fabs(a[j][i]) > fabs(a[p][i])) p = j;

        double *tmp = a[i];
        a[i] = a[p];
        a[p] = tmp;

        if (p != i) s *= -1;
        if (fabs(a[i][i]) < 1e-18) { s = 0; break; }

        s *= (a[i][i] > 0) ? 1 : -1;
        sum += log(fabs(a[i][i]));

        for (int j = i + 1; j < n; j++) {
            double f = a[j][i] / a[i][i];
            for (int k = i + 1; k < n; k++)
                a[j][k] -= f * a[i][k];
        }
    }

    for(int i = 0; i < n; i++) free(a[i]);
    free(a);

    return (det_t){sum, s};
}

int main() {
    FILE *fp = fopen("input_small.txt", "r");
    if (!fp) {
        perror("File open failed");
        return 1;
    }

    int n;
    fscanf(fp, "%d", &n);

    double **A = malloc(n * sizeof(double *));
    double *B = malloc(n * sizeof(double));
    double *X = malloc(n * sizeof(double));

    for (int i = 0; i < n; i++) {
        A[i] = malloc(n * sizeof(double));
        for (int j = 0; j < n; j++)
            fscanf(fp, "%lf", &A[i][j]);
    }

    for (int i = 0; i < n; i++)
        fscanf(fp, "%lf", &B[i]);

    fclose(fp);

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    // Main determinant
    det_t D = get_det(n, A);
    if (D.sign == 0) {
        printf("No unique solution.\n");
        return 1;
    }

    // Parallel Cramer's Rule
    for (int p = 0; p < 8; p++) {
        if (fork() == 0) {
            char fn[20];
            sprintf(fn, "res_%d.txt", p);
            FILE *out = fopen(fn, "w");

            for (int i = p; i < n; i += 8) {
                double **Ai = malloc(n * sizeof(double *));
                for (int r = 0; r < n; r++) {
                    Ai[r] = malloc(n * sizeof(double));
                    for (int c = 0; c < n; c++)
                        Ai[r][c] = (c == i) ? B[r] : A[r][c];
                }

                det_t Di = get_det(n, Ai);
                double xi = (double)(Di.sign * D.sign) *
                            exp(Di.l_val - D.l_val);

                fprintf(out, "%d %lf\n", i, xi);

                for (int r = 0; r < n; r++) free(Ai[r]);
                free(Ai);
            }
            fclose(out);
            exit(0);
        }
    }

    while (wait(NULL) > 0);

    // Collect results
    for (int p = 0; p < 8; p++) {
        char fn[20];
        sprintf(fn, "res_%d.txt", p);
        FILE *in = fopen(fn, "r");

        int idx;
        double val;
        while (fscanf(in, "%d %lf", &idx, &val) != EOF)
            X[idx] = val;

        fclose(in);
        remove(fn);
    }

    gettimeofday(&t2, NULL);

    printf("\n--- SYSTEM SOLVED ---\n");
    for (int i = 0; i < n; i++)
        printf("x[%d] = %.6f\n", i + 1, X[i]);

    printf("Total Time: %f sec\n",
           (t2.tv_sec - t1.tv_sec) +
           (t2.tv_usec - t1.tv_usec) / 1e6);

    return 0;
}


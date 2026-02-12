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





 gettimeofday(&t2, NULL);

    printf("\n--- SYSTEM SOLVED ---\n");
    for (int i = 0; i < n; i++)
        printf("x[%d] = %.6f\n", i + 1, X[i]);

    printf("Total Time: %f sec\n",
           (t2.tv_sec - t1.tv_sec) +
           (t2.tv_usec - t1.tv_usec) / 1e6);

    return 0;
}

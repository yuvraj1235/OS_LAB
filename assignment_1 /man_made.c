#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h> 

int isPrime(int num) {
    if (num < 2) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    for (int i = 3; i*i <= num; i+=2) {
        if (num % i == 0) return 0;
    }
    return 1;
}

void mergeFiles(int n) {
    int fd_out = open("human/prime.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    for (int i = 0; i < n; i++) {
        char filename[64];
        sprintf(filename, "temp_%d.txt", i);
        int fd_in = open(filename, O_RDONLY);
        if (fd_in < 0) continue;
        char buffer[4096];
        ssize_t bytes;
        while ((bytes = read(fd_in, buffer, sizeof(buffer))) > 0) 
            write(fd_out, buffer, bytes);
        close(fd_in);
        unlink(filename);
    }
    close(fd_out);
}

double run_test(int rl, int rh, int n) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int range_size = (rh - rl + 1) / n;
    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            int low = rl + i * range_size;
            int high = (i == n - 1) ? rh : low + range_size - 1;
            char filename[64];
            sprintf(filename, "temp_%d.txt", i);
            int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            char buf[64];
            for (int x = low; x <= high; x++) {
                if (isPrime(x)) {
                    int len = sprintf(buf, "%d\n", x);
                    write(fd, buf, len);
                }
            }
            close(fd);
            exit(0);
        }
    }
    for (int i = 0; i < n; i++) wait(NULL);
    mergeFiles(n);
    clock_gettime(CLOCK_MONOTONIC, &end);

    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    mkdir("human", 0777);

    int ranges[][2] = {{1000, 10000}, {50000, 100000}, {100000, 200000}};
    int num_ranges = 3;
    int max_cores = sysconf(_SC_NPROCESSORS_ONLN);

    FILE *csv = fopen("human/results.csv", "w");
    fprintf(csv, "Range,Processes,Time\n");

    for (int i = 0; i < num_ranges; i++) {
        int rl = ranges[i][0];
        int rh = ranges[i][1];
        printf("Testing Range: %d to %d\n", rl, rh);

        for (int n = 1; n <= max_cores; n++) {
            double time_taken = run_test(rl, rh, n);
            fprintf(csv, "%d-%d,%d,%lf\n", rl, rh, n, time_taken);
            printf("  n=%d: %lf sec\n", n, time_taken);
        }
    }

    fclose(csv);
    printf("\nAll data stored in 'human' folder.\n");
    return 0;
}
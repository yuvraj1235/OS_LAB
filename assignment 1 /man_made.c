#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return 0;
    return 1;
}

int main() {
    int rl = 1000, rh = 10000;
    int n;  // number of child processes

    int cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Logical processors: %d\n", cores);

    for (n = 1; n <= cores; n++) {
        int fd = open("prime.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        int range = (rh - rl + 1) / n;

        for (int i = 0; i < n; i++) {
            pid_t pid = fork();

            if (pid == 0) {
                int start_range = rl + i * range;
                int end_range = (i == n - 1) ? rh : start_range + range - 1;

                char buffer[32];
                for (int num = start_range; num <= end_range; num++) {
                    if (is_prime(num)) {
                        int len = snprintf(buffer, sizeof(buffer), "%d\n", num);
                        write(fd, buffer, len);
                    }
                }
                close(fd);
                exit(0);
            }
        }

        for (int i = 0; i < n; i++)
            wait(NULL);

        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_taken =
            (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("Processes: %d | Time: %lf seconds\n", n, time_taken);
        close(fd);
    }
    return 0;
}

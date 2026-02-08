#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // fork()
#include <sys/wait.h>  // wait()
#include <fcntl.h>     // open(), write()
#include <time.h>      // clock_gettime

// Function to check if a number is prime
int isPrime(int num) {
    if (num < 2) return 0;       // 0 and 1 are not prime
    if (num == 2) return 1;      // 2 is prime
    if (num % 2 == 0) return 0;  // even numbers > 2 are not prime
    for (int i = 3; i*i <= num; i+=2) { // check only odd numbers
        if (num % i == 0) return 0;
    }
    return 1; // prime
}

int main() {
    int rl, rh;
    printf("Enter lower and upper range: ");
    scanf("%d %d", &rl, &rh);

    int cores = sysconf(_SC_NPROCESSORS_ONLN); // get number of logical cores
    printf("Logical cores available: %d\n", cores);

    // Open CSV file to store execution times
    FILE *csv = fopen("timing_manual.csv", "w");
    fprintf(csv, "Processes,Time\n");

    // Try 1 to cores number of child processes
    for (int n = 1; n <= cores; n++) {

        // Open prime.txt to store prime numbers
        int fd = open("prime.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start); // start time

        int range = (rh - rl + 1) / n; // divide range into n parts

        for (int i = 0; i < n; i++) {
            pid_t pid = fork();
            if (pid == 0) { // child process
                int low = rl + i*range;
                int high = (i == n-1) ? rh : low + range - 1;

                char buf[64];
                for (int x = low; x <= high; x++) {
                    if (isPrime(x)) {
                        int len = sprintf(buf, "%d\n", x);
                        write(fd, buf, len); // write to file
                    }
                }
                close(fd); // close file
                exit(0);   // child exits
            }
        }

        // parent waits for all children to finish
        for (int i = 0; i < n; i++)
            wait(NULL);

        clock_gettime(CLOCK_MONOTONIC, &end); // end time

        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9;
        fprintf(csv, "%d,%lf\n", n, time_taken);
        printf("Processes = %d, Time = %lf sec\n", n, time_taken);

        close(fd);
    }

    fclose(csv);

    return 0;
}
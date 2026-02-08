#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

// Function to check if a number is prime
int isPrime(int num) {
    if (num < 2) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    for (int i = 3; i*i <= num; i+=2) {
        if (num % i == 0) return 0;
    }
    return 1;
}

// Function to merge temporary files into prime.txt
void mergeFiles(int n) {
    int fd_out = open("prime.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd_out < 0) {
        perror("Error opening prime.txt");
        return;
    }
    
    for (int i = 0; i < n; i++) {
        char filename[64];
        sprintf(filename, "temp_primes_%d.txt", i);
        
        int fd_in = open(filename, O_RDONLY);
        if (fd_in < 0) continue;
        
        char buffer[4096];
        ssize_t bytes;
        while ((bytes = read(fd_in, buffer, sizeof(buffer))) > 0) {
            write(fd_out, buffer, bytes);
        }
        
        close(fd_in);
        unlink(filename); // delete temp file
    }
    
    close(fd_out);
}

int main() {
    int rl, rh;
    printf("Enter lower and upper range: ");
    scanf("%d %d", &rl, &rh);

    int cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Logical cores available: %d\n", cores);

    // IMPORTANT: Open CSV file ONCE and write header ONCE
    FILE *csv = fopen("timing_manual.csv", "w");
    if (!csv) {
        perror("Error opening CSV");
        return 1;
    }
    fprintf(csv, "Processes,Time\n");  // Write header ONLY ONCE
    fclose(csv);  // Close it

    // Test with 1 to cores number of processes
    for (int n = 1; n <= cores; n++) {
        
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        int range = (rh - rl + 1) / n;

        // Create n child processes
        for (int i = 0; i < n; i++) {
            pid_t pid = fork();
            
            if (pid < 0) {
                perror("Fork failed");
                exit(1);
            }
            else if (pid == 0) { // CHILD PROCESS
                int low = rl + i * range;
                int high = (i == n-1) ? rh : low + range - 1;

                // Each child writes to its OWN temporary file
                char filename[64];
                sprintf(filename, "temp_primes_%d.txt", i);
                
                int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Child: error opening file");
                    exit(1);
                }

                char buf[64];
                for (int x = low; x <= high; x++) {
                    if (isPrime(x)) {
                        int len = sprintf(buf, "%d\n", x);
                        write(fd, buf, len);
                    }
                }
                
                close(fd);
                exit(0); // child terminates
            }
            // PARENT continues to fork next child
        }

        // PARENT: Wait for all children to complete
        for (int i = 0; i < n; i++) {
            wait(NULL);
        }

        // PARENT: Merge all temporary files
        mergeFiles(n);

        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_taken = (end.tv_sec - start.tv_sec) + 
                           (end.tv_nsec - start.tv_nsec) / 1e9;
        
        // Append to CSV (open in append mode)
        csv = fopen("timing_manual.csv", "a");
        if (csv) {
            fprintf(csv, "%d,%lf\n", n, time_taken);
            fclose(csv);
        }
        
        printf("Processes = %d, Time = %lf sec\n", n, time_taken);
    }

    printf("\nResults saved to timing_manual.csv and prime.txt\n");

    return 0;
}
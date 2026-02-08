#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <signal.h>

int is_prime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

// Function to find primes in a given range and write to a temporary file
void find_primes_in_range(int start, int end, int process_id) {
    char filename[50];
    sprintf(filename, "temp_primes_%d.txt", process_id);
    
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            fprintf(fp, "%d\n", i);
        }
    }
    
    fclose(fp);
}

// Function to merge all temporary files into prime.txt
void merge_files(int num_processes) {
    FILE *output = fopen("prime.txt", "w");
    if (output == NULL) {
        perror("Error opening output file");
        return;
    }
    
    for (int i = 0; i < num_processes; i++) {
        char filename[50];
        sprintf(filename, "temp_primes_%d.txt", i);
        
        FILE *input = fopen(filename, "r");
        if (input == NULL) continue;
        
        int prime;
        while (fscanf(input, "%d", &prime) == 1) {
            fprintf(output, "%d\n", prime);
        }
        
        fclose(input);
        remove(filename); // Delete temporary file
    }
    
    fclose(output);
}

// Function to get current time in microseconds
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Function to get number of logical processors
int get_logical_processors() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

// Main function to compute primes using parallel processing
double compute_primes_parallel(int rl, int rh, int num_processes) {
    double start_time = get_time();
    
    int range_size = (rh - rl + 1) / num_processes;
    pid_t pids[num_processes];
    
    // Create child processes
    for (int i = 0; i < num_processes; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Fork failed");
            exit(1);
        }
        else if (pids[i] == 0) {
            // Child process
            int start = rl + i * range_size;
            int end = (i == num_processes - 1) ? rh : (start + range_size - 1);
            
            find_primes_in_range(start, end, i);
            exit(0);
        }
    }
    
    // Parent process waits for all children
    for (int i = 0; i < num_processes; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    // Merge all temporary files
    merge_files(num_processes);
    
    double end_time = get_time();
    return end_time - start_time;
}

int main(int argc, char *argv[]) {
    int rl, rh, num_processes;
    
    if (argc == 4) {
        rl = atoi(argv[1]);
        rh = atoi(argv[2]);
        num_processes = atoi(argv[3]);
        
        printf("Finding primes in range [%d, %d] using %d processes...\n", 
               rl, rh, num_processes);
        
        double exec_time = compute_primes_parallel(rl, rh, num_processes);
        
        printf("Execution time: %.6f seconds\n", exec_time);
        printf("Primes saved to prime.txt\n");
    }
    else if (argc == 3) {
        // Performance testing mode
        rl = atoi(argv[1]);
        rh = atoi(argv[2]);
        
        int max_processors = get_logical_processors();
        printf("System has %d logical processors\n", max_processors);
        printf("Testing range [%d, %d]\n\n", rl, rh);
        
        printf("n,execution_time(s)\n");
        
        // Test with varying number of processes
        for (int n = 1; n <= max_processors * 2; n++) {
            double exec_time = compute_primes_parallel(rl, rh, n);
            printf("%d,%.6f\n", n, exec_time);
            fflush(stdout);
        }
    }
    else {
        printf("Usage:\n");
        printf("  %s <rl> <rh> <n>  - Find primes with n processes\n", argv[0]);
        printf("  %s <rl> <rh>      - Performance testing mode\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s 1000 10000 4\n", argv[0]);
        printf("  %s 1000 10000\n", argv[0]);
        return 1;
    }
    
    return 0;
}
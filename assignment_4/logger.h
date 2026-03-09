#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

FILE *log_fp = NULL;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_logger(const char *filename)
{
    log_fp = fopen(filename, "w");
    if(!log_fp)
    {
        printf("Critical Error: Failed to open log file %s!\n", filename);
        exit(1);
    }
}

void close_logger()
{
    if (log_fp) fclose(log_fp);
}

// Custom logging function that behaves exactly like printf
void log_msg(const char *format, ...)
{
    if(!log_fp) return;
    
    // 1. Get the current Timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", t);

    // 2. Lock the Mutex so threads don't interrupt each other
    pthread_mutex_lock(&log_mutex);
    
    // 3. Write Timestamp and Message to File
    fprintf(log_fp, "[%s] ", time_str);
    va_list args;
    va_start(args, format);
    vfprintf(log_fp, format, args);
    va_end(args);
    fflush(log_fp); // Force save to disk immediately

    // 4. (Optional) Also print to Terminal so you aren't staring at a blank screen
    printf("[%s] ", time_str);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    // 5. Unlock Mutex
    pthread_mutex_unlock(&log_mutex);
}

#endif
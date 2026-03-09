#ifndef SYS_UTILS_H
#define SYS_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include <sys/sysctl.h>

int get_cpu_cores() {
    int cores;
    size_t len = sizeof(cores);
    if (sysctlbyname("hw.ncpu", &cores, &len, NULL, 0) != 0) {
        log_msg("[Warning] sysctl failed. Defaulting to 1 core.\n");
        return 1;
    }
    return (cores > 0) ? cores : 1;
}

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 1000
#define MAX_LINE 1024

typedef struct {
    char user[50];
    int pid;
    char cpu[10];
    char mem[10];
    char vsz[20];
    char rss[20];
    char tty[20];
    char stat[10];
    char start[20];
    char time[20];
    char command[256];
} PSAux;

typedef struct {
    char uid[50];
    int pid;
    int ppid;
    int lwp;
    char c[10];
    char nlwp[10];
    char sz[20];
    char rss[20];
    char psr[10];
    char stime[20];
    char tty[20];
    char time[20];
    char cmd[256];
} PSeLf;

int main() {

    system("ps aux > x1.txt");
    system("ps -eLf > x2.txt");
    
    FILE *file1 = fopen("x1.txt", "r");
    FILE *file2 = fopen("x2.txt", "r");
    FILE *merged = fopen("merged.txt", "w");
    
    if (file1 == NULL || file2 == NULL || merged == NULL) {
        printf("Error opening files!\n");
        return 1;
    }
    
    PSAux aux_data[MAX_PROCESSES];
    PSeLf elf_data[MAX_PROCESSES];
    int aux_count = 0;
    int elf_count = 0;
    char buffer[MAX_LINE];
    fgets(buffer, MAX_LINE, file1);
    while (fgets(buffer, MAX_LINE, file1) != NULL && aux_count < MAX_PROCESSES) {
        sscanf(buffer, "%s %d %s %s %s %s %s %s %s %s %[^\n]",
               aux_data[aux_count].user,
               &aux_data[aux_count].pid,
               aux_data[aux_count].cpu,
               aux_data[aux_count].mem,
               aux_data[aux_count].vsz,
               aux_data[aux_count].rss,
               aux_data[aux_count].tty,
               aux_data[aux_count].stat,
               aux_data[aux_count].start,
               aux_data[aux_count].time,
               aux_data[aux_count].command);
        aux_count++;
    }
    
    fgets(buffer, MAX_LINE, file2);
    while (fgets(buffer, MAX_LINE, file2) != NULL && elf_count < MAX_PROCESSES) {
        sscanf(buffer, "%s %d %d %d %s %s %s %s %s %s %s %s %[^\n]",
               elf_data[elf_count].uid,
               &elf_data[elf_count].pid,
               &elf_data[elf_count].ppid,
               &elf_data[elf_count].lwp,
               elf_data[elf_count].c,
               elf_data[elf_count].nlwp,
               elf_data[elf_count].sz,
               elf_data[elf_count].rss,
               elf_data[elf_count].psr,
               elf_data[elf_count].stime,
               elf_data[elf_count].tty,
               elf_data[elf_count].time,
               elf_data[elf_count].cmd);
        elf_count++;
    }
    
    fprintf(merged, "%-10s %-6s %-6s %-6s %-4s %-6s %-7s %-7s %-8s %-8s %-8s %-8s %-6s %-12s %-6s %-12s %-s\n",
            "UID", "PID", "PPID", "LWP", "C", "NLWP", "%CPU", "%MEM", "VSZ", "RSS", "TTY", 
            "STAT", "START", "TIME", "PSR", "STIME", "COMMAND");
    
    for (int i = 0; i < elf_count; i++) {
        for (int j = 0; j < aux_count; j++) {
            if (elf_data[i].pid == aux_data[j].pid) {
                fprintf(merged, "%-10s %-6d %-6d %-6d %-4s %-6s %-7s %-7s %-8s %-8s %-8s %-8s %-6s %-12s %-6s %-12s %s\n",
                        elf_data[i].uid,
                        elf_data[i].pid,
                        elf_data[i].ppid,
                        elf_data[i].lwp,
                        elf_data[i].c,
                        elf_data[i].nlwp,
                        aux_data[j].cpu,
                        aux_data[j].mem,
                        aux_data[j].vsz,
                        elf_data[i].rss,
                        aux_data[j].tty,
                        aux_data[j].stat,
                        aux_data[j].start,
                        aux_data[j].time,
                        elf_data[i].psr,
                        elf_data[i].stime,
                        aux_data[j].command);
                break;
            }
        }
    }
    
    fclose(file1);
    fclose(file2);
    fclose(merged);
    
    printf("Merged output:\n");
    printf("==================================================\n");
    system("cat merged.txt");
    
    return 0;
}
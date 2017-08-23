#include "health.h"

inline unsigned int get_proc_mem (FILE *fd, int offset = 15) {
    char name[64] = {0};
    char line_buff[512] = {0}; 
    int vmrss;
    for (int i = 0; i < offset - 1; i++) {
        fgets(line_buff, sizeof(line_buff), fd);
    }

    fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d", name, &vmrss);
    return vmrss;
}

inline int get_proc_thread (FILE *fd, int offset = 24) {
    char name[64] = {0};
    char line_buff[512] = {0}; 
    int vmrss;
    for (int i = 0; i < offset - 1; i++) {
        fgets(line_buff, sizeof(line_buff), fd);
    }

    fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d", name, &vmrss);
    return vmrss;
}

char *get_items(const char *buffer, int ie) {
    char *p = (char *)buffer;
    int len = strlen(buffer);
    int count = 0;
    if (1 == ie || ie < 1) {
        return p;
    }

    for (int i = 0; i < len; i++) {
        if (' ' == *p) {
            ++count;
            if (count == ie - 1) {
                ++p;
                break;
            }
        }

        ++p;
    }

    return p;
}

inline unsigned int get_cpu_process_occupy (pid_t pid) {
    char file[64] = {0};
    process_cpu_occupy_t t;

    FILE *fd;
    char line_buff[1024] = {0};
    sprintf(file, "/proc/%d/stat", pid);
    fd = fopen (file, "r");
    fgets (line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%u", &t.pid);
    char *q = get_items(line_buff, 14);
    sscanf(q, "%u %u %u %u", &t.utime, &t.stime, &t.cutime, &t.cstime);
    fclose(fd);
    return (t.utime + t.stime + t.cutime + t.cstime);
}

inline unsigned int get_cpu_total_occupy () {
    FILE *fd;
    char buff[1024] = {0};
    total_cpu_occupy_t t;

    fd = fopen ("/proc/stat", "r"); 
    fgets (buff, sizeof(buff), fd);
    char name[16];
    sscanf (buff, "%s %u %u %u %u", name, &t.user, &t.nice,&t.system, &t.idle);

    fclose(fd);
    return (t.user + t.nice + t.system + t.idle);
    
}

inline double get_proc_cpu (pid_t pid) {
    unsigned int totalcputime1, totalcputime2;
    unsigned int procputime1, procputime2;

    totalcputime1 = get_cpu_total_occupy();
    procputime1 = get_cpu_process_occupy(pid);
    usleep(200000);
    totalcputime2 = get_cpu_total_occupy();
    procputime2 = get_cpu_process_occupy(pid);

    double cpu = 0.0;
    if (totalcputime2 - totalcputime1) {
        cpu = 100.0 * (procputime2 - procputime1)/(totalcputime2 - totalcputime1);
    }
    return cpu;
}

Health UpdateHeadlth (pid_t pid) {
    char file_name[64] = {0};
    FILE *fd;
    char line_buff[512] = {0};
    sprintf(file_name, "/proc/%d/status", pid);
    fd = fopen(file_name, "r");
    if (nullptr == fd) {
        Health UnHealth(0, 0, 0);
        return UnHealth;
    }

    uint32_t mem = get_proc_mem(fd);
    int threadnum = get_proc_thread(fd);
    double cpu = get_proc_cpu(pid);
    Health result(mem, threadnum, cpu);
    fclose(fd);
    return result;
}


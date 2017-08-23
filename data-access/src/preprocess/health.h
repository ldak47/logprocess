#ifndef DATAACCESS_HEALTH_H
#define DATAACCESS_HEALTH_H
#include <unistd.h>
#include <stdio.h>
#include <tuple>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

//mem(RSS), threadnum, cpu
using Health = std::tuple<unsigned int, int, double>;
Health UpdateHeadlth (pid_t pid);

typedef struct {
    pid_t pid;
    unsigned int utime; //run in user space
    unsigned int stime; //run in kernel space
    unsigned int cutime;//all in user space
    unsigned int cstime;//all in kernel space
} process_cpu_occupy_t;

typedef struct {
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
} total_cpu_occupy_t;

#endif

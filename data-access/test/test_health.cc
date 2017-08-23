#include "gtest/gtest.h"
#include "glog/logging.h"
#include <unistd.h>

#define VMRSS_LINE 15

uint32_t get_proc_mem (pid_t pid) {
    char file_name[64] = {0};
    FILE *fd;
    char line_buff[512] = {0};
    sprintf(file_name, "/proc/%d/status", pid);
    fd = fopen(file_name, "r");
    if (nullptr == fd) {
       return 0; 
    }

    char name[64] = {0};
    int vmrss;
    for (int i = 0; i < VMRSS_LINE - 1; i++) {
        fgets(line_buff, sizeof(line_buff), fd);
    }

    fgets(line_buff, sizeof(line_buff), fd);
    sscanf(line_buff, "%s %d", name, &vmrss);
    fclose(fd);
    return vmrss;
}

TEST (test, health) {
    pid_t pid = getpid();
    LOG(INFO) << get_proc_mem(13064);
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
 * Copyright (C) 2014 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#define LOG_TAG "PowerHAL"

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cutils/log.h>
#include <cutils/properties.h>
#include <errno.h>

static const char* PROPERTY_NON_INTERACTIVE_PROC = "sys.power_hal.niproc";

#define PROCESS_NAME_LEN 32

struct process_pid_t {
    char name[PROCESS_NAME_LEN];
    pid_t pid;
};

static struct process_pid_t s_pids[] = {
    { "/system/bin/mediaserver", -1 },
    { "", -1 }
};

/* Set a property so that init can write the PID to correct files */
static void notify_pid(pid_t pid)
{
    int fd;
    int ret;
    char str[10];

    ret = snprintf(str, sizeof(str), "%d", pid);
    if (ret > 0) {
        ret = property_set(PROPERTY_NON_INTERACTIVE_PROC, str);
        if (ret < 0) {
            ALOGV("cannot set property\n");
        }
    }
}

void cgroup_walk_proc_for_processes()
{
    DIR *d;
    struct dirent *de;
    int pid;
    char cmdline[128];
    char process_name[PROCESS_NAME_LEN];
    int fd;
    struct process_pid_t *p;
    int cnt = 0, total_cnt = 0;

    d = opendir("/proc");
    if (d == 0) {
        ALOGV("helper cannot open /proc\n");
        return;
    }

    /* should look for # of processes */
    p = &s_pids[0];
    while (p->name[0] != '\0') {
        if (p->pid == -1) {
            total_cnt++;
        }
        p++;
    };

    /* walk through /proc */
    /* part of code is from /system/core/toolbox/ps.c */
    while ((de = readdir(d)) != 0){
        /* only care about numbered directories */
        if (isdigit(de->d_name[0])){
            pid = atoi(de->d_name);

            snprintf(cmdline, sizeof(cmdline), "/proc/%d/cmdline", pid);

            /* match process name */
            fd = open(cmdline, O_RDONLY);
            if (fd >= 0) {
                if (read(fd, process_name, sizeof(process_name)) > 0) {
                    p = &s_pids[0];
                    while (p->name[0] != '\0') {
                        if ((p->pid == -1) &&
                            (strncmp(process_name, p->name, sizeof(process_name)) == 0)) {

                            p->pid = pid;
                            cnt++;

                            ALOGV("Found %s (%d)\n", p->name, p->pid);
                            notify_pid(p->pid);
                        }
                        p++;
                    }
                }

                close(fd);
            }
        }

        /* found enough */
        if (cnt >= total_cnt) {
            break;
        }
    }

    closedir(d);

    ALOGV("helper found %d processes\n", cnt);
}

int main()
{
    /* find all interesting processes */
    cgroup_walk_proc_for_processes();

    return 0;
}

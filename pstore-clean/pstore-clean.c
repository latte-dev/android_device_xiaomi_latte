/*
 * Copyright (C) 2015 Intel Corporation
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

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mount.h>
#include <linux/fs.h>
#include <cutils/config_utils.h>
#include <cutils/fs.h>
#include <ctype.h>

#define LOG_TAG "pstore-clean"
#include <cutils/log.h>
#define MNT "/dev/pstore"
#define DST "/data/dontpanic/apanic_console"
#define BUFFER_SIZE 4096
#define PSTORE_CLEAN_VENDOR_CONFIG_FILE "/vendor/etc/pstore-clean.conf"

/* This tool copies and clean kernel dumps from the pstore filesystem
 * according to its configuration file (see above). Otherwise that
 * data will accumulate in pstore backend (NVRAM, EFI variable, RAM,
 * ...) and eventually fill storage. This behavior can be customized
 * as all storage backends and file types do not have same
 * constraints.
 */

typedef enum {
    DEFAULT,
    KEEP,
    MOVE
} pstore_action;

static inline int file_exists(const char *filename)
{
    struct stat info;

    return (stat(filename, &info) == 0);
}

static int filecopy(const char *srcfile, int dstfd)
{
    int srcfd, read_ret, write_ret;
    char buffer[BUFFER_SIZE];

    srcfd = open(srcfile, O_RDONLY);
    if (srcfd == -1) {
        ALOGE("Open %s error: %s\n", srcfile, strerror(errno));
        return -1;
    }

    do {
        memset(buffer, 0, BUFFER_SIZE);
        read_ret = read(srcfd, buffer, BUFFER_SIZE);
        if (read_ret == -1) {
            ALOGE("Read %s error: %s\n", srcfile, strerror(errno));
            close(srcfd);
            return -1;
        }

        write_ret = write(dstfd, buffer, read_ret);
        if(write_ret == -1) {
            ALOGE("Write error: %s\n", strerror(errno));
            close(srcfd);
            return -1;
        }
    } while(read_ret > 0 && write_ret > 0);

    close(srcfd);
    return 0;
}

static int dir_not_empty(char* path)
{
    DIR* dir;
    if ((dir = opendir(path)) == NULL) {
        ALOGE("Opendir %s failed (%s)", MNT, strerror(errno));
        return -1;
    }

    struct dirent* dent = NULL;
    while ((dent = readdir(dir)) != NULL) {
        if ((strcmp(dent->d_name, ".") != 0) && (strcmp(dent->d_name, "..") != 0)) {
            closedir(dir);
            return 1; //dir is not empty
        }
    }
    closedir(dir);
    return 0; //dir is empty
}

static int is_fs_mounted(char* path)
{
    FILE* proc_mounts = NULL;
    char part [PAGE_SIZE];
    int is_mounted = 0;

    if ((proc_mounts = fopen("/proc/mounts", "r")) != NULL) {
        while (fgets(part, PAGE_SIZE, proc_mounts) != NULL) {
            if (strstr(part, path) != NULL) {
                is_mounted = 1;
                break;
            }
        }
        fclose(proc_mounts);
    }
    return is_mounted;
}

static size_t getbasefilename(char *filename, char* basefilename,
                              size_t maxlen)
{
    char separator = '-';
    size_t filename_len = maxlen, i;
    size_t lastoccurance = 0;

    for (i = 0; i < maxlen; i++) {
        if (filename[i] == separator) {
            lastoccurance = i;
        }
        basefilename[i] = filename[i];

        if (filename[i] == '\0') {
            filename_len = i;
            break;
        }
    }

    if (lastoccurance) {
        if (isdigit(filename[lastoccurance + 1])) {
            basefilename[lastoccurance] = '\0';
            filename_len = lastoccurance;
        }
    }

    return filename_len;
}

static int perform_rule_check(cnode *root, char *filename,
                              pstore_action *action,
                              const char **dstpath)
{
    const char *action_str = NULL;
    cnode *node = NULL;

    if (!root || !filename || !action) {
        return -1;
    }

    if (!(node = config_find(root, filename))) {
        ALOGI("no rule for %s", filename);
        return -1;
    }

    if (!(action_str = config_str(node, "action", NULL))) {
        ALOGE("Could not retrieve action for rule: %s", node->name);
        return -1;
    }

    if (!strcmp(action_str, "move")) {
        if (!(*dstpath = config_str(node, "destination", NULL))) {
            *dstpath = "/dev/null";
        }

        *action = MOVE;
    } else if (!strcmp(action_str, "keep")) {
        *action = KEEP;
    } else {
        *action = DEFAULT;
    }

    return 0;
}

int main()
{
    int n = 0;
    int status;
    int dstfd = -1, fd = -1;
    struct dirent **namelist = NULL;
    char srcpath[PATH_MAX];
    const char *dstpath;
    char destpath[PATH_MAX];
    char basefile[FILENAME_MAX];
    int namelist_len = 0;
    pstore_action action;

    umask(0027);

    struct dirent* dent = NULL;

    cnode *rulesroot = NULL;
    cnode *root = config_node("", "");

    if (root) {
        config_load_file(root, PSTORE_CLEAN_VENDOR_CONFIG_FILE);

        if (!(rulesroot = config_find(root, "pstore_rules"))) {
            ALOGI("pstore_rules not specified in config file");
        }
    }

    if (!is_fs_mounted(MNT)) {
        ALOGE("Pstore fs isn't mounted. Exiting.");
        goto error1;
    }
    status = dir_not_empty(MNT);
    if (status == -1) {
        ALOGE("Opendir %s failed (%s)", MNT, strerror(errno));
        goto error2;
    }
    else if (status == 0) {
        ALOGI("Pstore is clean.");
        goto done;
    }

    ALOGW("Kernel pstore crash dump found, copying to " DST "\n");

    if (file_exists(DST) && unlink(DST) != 0) {
        ALOGE("Remove %s failed (%s)", DST, strerror(errno));
        goto error2;
    }

    if ((namelist_len = scandir(MNT, &namelist, NULL, alphasort)) < 1) {
        ALOGE("Failed to list " MNT " (%s)", strerror(errno));
        goto error2;
    }

    for (n = namelist_len - 1; n >= 0; n--) {
        dent = namelist[n];
        if ((strcmp(dent->d_name, ".") == 0) || (strcmp(dent->d_name, "..") == 0))
            continue;

        getbasefilename(dent->d_name, basefile, FILENAME_MAX);
        if (perform_rule_check(rulesroot, basefile, &action, &dstpath)) {
            action = DEFAULT;
        }

        snprintf(srcpath, sizeof(srcpath), "%s/%s", MNT, dent->d_name);
        switch (action) {
        case MOVE:
            snprintf(destpath, sizeof(destpath), "%s/%s", dstpath, dent->d_name);
            if ((fd = open(destpath, O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0) {
                ALOGE("Failed to open %s (%s)", destpath, strerror(errno));
            }
            else if (filecopy(srcpath, fd) == -1) {
                ALOGE("Could not move %s to %s on rule: %s (%s)",
                      srcpath, dstpath, dent->d_name, strerror(errno));
                close(fd);
            }
            else {
                close(fd);
            }

            if (unlink(srcpath) != 0) {
                ALOGE("Remove %s failed (%s)", srcpath, strerror(errno));
                goto error3;
            }
            break;
        case KEEP:
            /* nothing to do for now, as the file is supposed to remain
               in pstore and be excluded from apanic_console */
            break;
        default:
            /* default rules, fallback */
            if (dstfd == -1 &&
                (dstfd = open(DST, O_CREAT | O_APPEND | O_WRONLY, 0666)) < 0) {
                ALOGE("Failed to open " DST " (%s)", strerror(errno));
                goto error3;
            }

            snprintf(destpath, sizeof(destpath), "------ %s ------\n", dent->d_name);
            write(dstfd, destpath, strlen(destpath));
            if (filecopy(srcpath, dstfd) == -1) {
                ALOGE("Copy file %s failed (%s)", srcpath, strerror(errno));
            }
            if (unlink(srcpath) != 0) {
                ALOGE("Remove %s failed (%s)", srcpath, strerror(errno));
                goto error3;
            }
        }
    }

done:
    if (umount(MNT)) {
        ALOGE("Umount %s failed (%s)", MNT, strerror(errno));
        goto error1;
    }

    if (root)
        config_free(root);

    if (rmdir(MNT))
        ALOGE("Remove dir %s failed (%s)", MNT, strerror(errno));

    return 0;

error3:
    for (n = 0; n < namelist_len; n++) {
        free(namelist[n]);
    }
    free(namelist);

error2:
    if (dstfd >= 0)
        close(dstfd);

    if (umount(MNT))
        ALOGE("Umount %s failed (%s)", MNT, strerror(errno));

error1:
    if (root)
        config_free(root);

    if (rmdir(MNT))
        ALOGE("Remove dir %s failed (%s)", MNT, strerror(errno));

    ALOGE("Clean pstore failed.\n");
    return -1;
}

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> //stat(), struct stat
#include <dirent.h>  //opendir(), readdir(), closedir()
#include <pwd.h>  //chuyen UID sang ten nguoi dung: getpwuid()
#include <time.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <linux/stat.h>
#include <linux/limits.h>



//statx syscall wrapper
int statx(int dirfd, const char *path, int flags, unsigned int mask, struct statx *statxbuf) {
    return syscall(SYS_statx, dirfd, path, flags, mask, statxbuf);
}

void print_info(const char *name) {
    struct stat st;
    if (stat(name, &st) == -1) {
        perror("stat");
        return;
    }

    //dung statx de lay thoi gian tao
    struct statx stx;
    int has_birth = 0;
    if (statx(AT_FDCWD, name, AT_STATX_SYNC_AS_STAT, STATX_BTIME, &stx) == 0) {
        if (stx.stx_mask & STATX_BTIME)
            has_birth = 1;
    }

    //type
    const char *type = S_ISDIR(st.st_mode) ? "Folder" :
                       S_ISREG(st.st_mode) ? "File" : "Other";

    // owner
    struct passwd *pw = getpwuid(st.st_uid);
    const char *owner = pw ? pw->pw_name : "Unknown";

    // format time
    char buf_created[64] = "N/A";
    char buf_modified[64];
    if (has_birth) {
        struct tm bt = *localtime(&stx.stx_btime.tv_sec);
        strftime(buf_created, sizeof(buf_created), "%F %T", &bt);
    }
    struct tm mt = *localtime(&st.st_mtime);
    strftime(buf_modified, sizeof(buf_modified), "%F %T", &mt);

    printf("%-25s | %-6s | %-10s | Created: %-19s | Modified: %s\n",
           name, type, owner, buf_created, buf_modified);
}

int main(int argc, char *argv[]) {
    const char *dirpath = (argc > 1) ? argv[1] : ".";
    DIR *d = opendir(dirpath);
    if (!d) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    printf("Listing in \"%s\":\n", dirpath);
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        //build path 
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dirpath, ent->d_name);
        print_info(path);
    }
    closedir(d);
    return EXIT_SUCCESS;
}


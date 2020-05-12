#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <limits.h>
#include <math.h>
#include <dirent.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

long int pathSize = 0;
int fileCount = 0, folderCount = 0;

int isDirectory(const char *path);
char* displaySize(long int size);
int ftwFileCount(const char* file, const struct stat *ptr, int flags);
long int countFileSize();

void resetColor() {
    printf("\033[0m");
}

int main(int argc, char const *argv[]) {
    char* _targetPath = malloc(sizeof(char) * PATH_MAX);
    char _canonicalPath[PATH_MAX];

    printf("\033[0;33m########### \033[1;33mShamil\033[0;33m — Find the size of everything. ############\n");
    printf("\033[0;36m###################### MADE BY \033[1;36mBILLVOG\033[0;36m #######################\n\n");
    resetColor();

    if ((argc > 1 && strcmp(argv[1], "?") == 0) || argc == 1) {
        printf("Usage: shamil <path>\n");
        return 0;
    }
    else if (argc > 1) {
        strcpy(_targetPath, argv[1]);

        realpath(_targetPath, _canonicalPath);
    }

    if (isDirectory(_canonicalPath) != 0) {
        DIR *dir = opendir(_canonicalPath);
        if (dir == NULL) {
            printf("\033[1;31mError\033[0;31m: Directory doesn't exist\n");
            resetColor();
            return -1;
        }

        if (ftw(_canonicalPath, ftwFileCount, 100) == 0) {}
        else {
            printf("\033[1;31mError\033[0;31m: Unknown error occured\n");
            resetColor();
            return -1;
        }
    }
    else {
        pathSize = countFileSize(_canonicalPath, &fileCount);
        if (pathSize == -1)
            return -1;
    }

    folderCount > 0 ? --folderCount : folderCount;
    printf("\033[1;0mPath: %s\nSize: %s\nFolder(s): %d, File(s): %d\n", _canonicalPath, displaySize(pathSize), folderCount, fileCount);
    resetColor();

    return 0;
}

int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;

   return S_ISDIR(statbuf.st_mode);
}

long int countFileSize(const char* path, int *count) {
    FILE *fPointer = fopen(path, "r");
    (*count)++;

    if (fPointer == NULL) {
        printf("\033[1;31mError\033[0;31m: File doesn't exist\n");
        resetColor();
        return -1;
    }

    fseek(fPointer, 0L, SEEK_END);
    return ftell(fPointer);
}

int ftwFileCount(const char* file, const struct stat *ptr, int flags) {
    if (!(isDirectory(file) != 0)) {
        long int size = countFileSize(file, &fileCount);

        if (size == -1) {
            printf("\033[1;31mError\033[0;31m: File (%s) doesn't exist\n", file);
            resetColor();
            return -1;
        }

        pathSize += size;
        return 0;
    }

    folderCount++;
    return 0;
}

char* displaySize(long int size) {
    char *returnStr = malloc(sizeof(char) * 100);

    if (size >= 1099511627776) {
        sprintf(returnStr, "%.1f %s", (float)size / (1099511627776), "TB");
    }
    else if (size >= 1073741824) {
        sprintf(returnStr, "%.1f %s", (float)size / (1073741824), "GB");
    }
    else if (size >= 1048576) {
        sprintf(returnStr, "%.1f %s", (float)size / (1048576), "MB");
    }
    else if (size >= 1024) {
        sprintf(returnStr, "%.1f %s", (float)size / (1024), "KB");
    }
    else {
        sprintf(returnStr, "%.1f %s", (float)size, "Bytes");
    }

    return returnStr;
}

/*
Copyright (c) 2020 BILLVOG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#define isDebugging argc > 2 && strcmp(argv[2], "-db") == 0
#define _version "1.0"

long int pathSize = 0;
int fileCount = 0, folderCount = 0;

// Finish time
clock_t begin;
clock_t end;

int isDirectory(const char *path);
char* displaySize(long int size);
int ftwFileCount(const char* file, const struct stat *ptr, int flags);
long int countFileSize();
void resetColor();

int main(int argc, char const *argv[]) {
    if (isDebugging) {
        begin = clock();
    }

    char* _targetPath = malloc(sizeof(char) * PATH_MAX);
    char _canonicalPath[PATH_MAX];

    printf("\033[0;33m############## \033[1;33mShamil\033[0;33m — Find the size of everything. ###############\n");
    printf("\033[0;36m######################### MADE BY \033[1;36mBILLVOG\033[0;36m ##########################\n\n");
    resetColor();

    if ((argc > 1 && strcmp(argv[1], "?") == 0) || argc == 1) {
        printf("Shamil (%s) is a program to easily calculate the size of anything \nin your computer. Find the source-code here: https://github.com/billvog/shamil\n\n", _version);
        printf("Usage: shamil <path> [-db (Calculate execution time)]\n");
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
            return -1;
        }
    }
    else {
        pathSize = countFileSize(_canonicalPath, &fileCount);
        if (pathSize == -1) {
            printf("\033[1;31mError\033[0;31m: Unknown error occured while trying to open:\033[0m %s\n", _canonicalPath);
            return -1;
        }
    }

    folderCount > 0 ? --folderCount : folderCount;
    printf("\033[1;0mPath: %s\nSize: %s\nFolder(s): %d, File(s): %d\n", _canonicalPath, displaySize(pathSize), folderCount, fileCount);
    resetColor();

    if (isDebugging) {
        double time_spent = 0.0;
        end = clock();
        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
        printf("\n\033[1;33mFinished:\033[0;36m %fs\n", time_spent);
    }

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

    if (fPointer == NULL) return -1;

    fseek(fPointer, 0L, SEEK_END);
    return ftell(fPointer);
}

int ftwFileCount(const char* file, const struct stat *ptr, int flags) {
    if (!(isDirectory(file) != 0)) {
        long int size = countFileSize(file, &fileCount);

        if (size == -1) {
            printf("\033[1;31mError\033[0;31m: Unknown error occured while trying to open:\033[0m %s\n", file);
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
        sprintf(returnStr, "%ld %s", size, "Bytes");
    }

    return returnStr;
}

void resetColor() {
    printf("\033[0m");
}

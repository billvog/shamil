#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include "colors.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#define isDebugging findIn(argc, argv, "-db") == 0
#define _version "1.0"

long int pathSize = 0;
int fileCount = 0, folderCount = 0;
char** exceptions;
int exCount = 0;

clock_t begin;
clock_t end;

int isDirectory(const char *path);
int findIn(int count, char** array, char* item);
int ftwFileCount(const char* file, const struct stat *ptr, int flags);
long int countFileSize();
char* displaySize(long int size);

int main(int argc, char** argv) {
    if (isDebugging) {
        begin = clock();
    }

    char _canonicalPath[PATH_MAX];
    char *current_dir = malloc(PATH_MAX);
    getcwd(current_dir, PATH_MAX);
    exceptions = malloc(10 * sizeof(char*));

    printf("%s############## %sShamil%s — Find the size of everything. ###############\n", ANSI_COLOR_YELLOW, ANSI_COLOR_BOLD_YELLOW, ANSI_COLOR_YELLOW);
    printf("%s######################### MADE BY %sBILLVOG%s ##########################%s\n\n", ANSI_COLOR_CYAN, ANSI_COLOR_BOLD_CYAN, ANSI_COLOR_CYAN, ANSI_COLOR_RESET);

    if ((argc > 1 && strcmp(argv[1], "--help") == 0) || argc == 1) {
        printf("Shamil (%s) is a program to easily calculate the size of anything \nin your computer. Find the source-code here: https://github.com/billvog/shamil\n\n", _version);
        printf("Usage: shamil <path> [-db (Calculate execution time)] [/\"file\" (to add exceptions)]\n");
        return 0;
    }
    else if (argc > 1) {
        realpath(argv[1], _canonicalPath);

        for (int i = 2; i < argc; i++)
            if (argv[i][0] == '/') {
                exceptions[exCount] = malloc(sizeof(char) * 120);
                sprintf(exceptions[exCount], "%s/%s", current_dir, argv[i] + 1);
                exCount++;
            }
    }

    if (isDirectory(_canonicalPath) == 1) {
        DIR *dir = opendir(_canonicalPath);
        if (dir == NULL) {
            printf("%sError%s: Directory doesn't exist%s\n", ANSI_COLOR_BOLD_RED, ANSI_COLOR_RED, ANSI_COLOR_RESET);
            return -1;
        }

        if (ftw(_canonicalPath, ftwFileCount, 100) != 0) { return -1; }
    }
    else {
        pathSize = countFileSize(_canonicalPath, &fileCount);
        if (pathSize == -1) {
            printf("%sError%s: Unknown error occured while trying to open:%s %s\n", ANSI_COLOR_BOLD_RED, ANSI_COLOR_RED, ANSI_COLOR_RESET, _canonicalPath);
            return -1;
        }
    }

    folderCount > 0 ? --folderCount : folderCount;
    printf("%sPath: %s\nSize: %s\nFolder(s): %d, File(s): %d\n", ANSI_COLOR_RESET, _canonicalPath, displaySize(pathSize), folderCount, fileCount);

    if (isDebugging) {
        double time_spent = 0.0;
        end = clock();
        time_spent += ((double)(end - begin) / CLOCKS_PER_SEC) * 1000;
        printf("\n%sFinished:%s %.3f ms\n", ANSI_COLOR_BOLD_YELLOW, ANSI_COLOR_MAGENTA, time_spent);
    }

    return 0;
}

int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;

   return S_ISDIR(statbuf.st_mode);
}

int findIn(int count, char** array, char* item) {
    for (int i = 0; i < count; i++) {
        if (strcmp(array[i], item) == 0)
            return 0;
    }

    return -1;
}

int ftwFileCount(const char* file, const struct stat *ptr, int flags) {
    char formattedFile[PATH_MAX];
    strcpy(formattedFile, file);
    if (isDirectory(formattedFile) == 1) {
        strcat(formattedFile, "/");
    }

    for (int i = 0; i < exCount; i++)
        if (strstr(formattedFile, exceptions[i]) != NULL)
            return 0;

    if (isDirectory(formattedFile) != 1) {
        long int size = countFileSize(formattedFile, &fileCount);

        if (size == -1) {
            printf("%sError%s: Unknown error occured while trying to open:%s %s\n", ANSI_COLOR_BOLD_RED, ANSI_COLOR_RED, ANSI_COLOR_RESET, formattedFile);
            return -1;
        }

        pathSize += size;
        return 0;
    }

    folderCount++;
    return 0;
}

long int countFileSize(const char* path, int *count) {
    FILE *fPointer = fopen(path, "r");
    (*count)++;

    if (fPointer == NULL) return -1;

    fseek(fPointer, 0L, SEEK_END);
    return ftell(fPointer);
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

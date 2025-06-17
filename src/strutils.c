#include "../include/strutils.h"
#include <string.h>
#include <stdlib.h>
#include "../libs/raylib.h"

char *path = NULL;

char *AddPath(const char *inStr)
{
    char *retval = (char*) malloc(
        sizeof(char) * (strlen(inStr) + 1 + strlen(path))
    );

    //copies path and inStr into retval
    for(int i = 0; i < strlen(path); i++) retval[i] = path[i];
    for(int i = 0; i < strlen(inStr); i++) retval[i + strlen(path)] = inStr[i];
    retval[strlen(inStr) + strlen(path)] = 0;

    return retval;
}

void AddPathReuse(const char *inStr, char **outStr)
{
    if(*outStr != NULL)free(*outStr);
    *outStr = AddPath(inStr);
}

void SetPathToDir(char *pathToExec)
{
    const char* temp = GetDirectoryPath(pathToExec);

    //mallocs new data for path
    if(path != NULL)free(path);
    path = (char*) malloc(sizeof(char) * strlen(temp) + 1);

    for(int i = 0; i < strlen(temp); i++) path[i] = temp[i];
    path[strlen(temp)] = 0;

}

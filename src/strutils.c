#include "../include/strutils.h"
#include <string.h>
#include <stdlib.h>

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
    path = pathToExec;

    //I know this is a bit cursed
    for(int i = strlen(pathToExec) - 1; i-- > 0;)
    {
        if(path[i] == '/')
        {
           path[i] = 0;
           return;
        }
    }
}

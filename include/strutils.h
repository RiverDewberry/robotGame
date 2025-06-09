#ifndef STRUTILS_H
#define STRUTILS_H

//The path to the file that is executed.
extern char *path;

//added path to the start of inStr and returns a pointer to that new string
// WARNING: path must be set prior to use
char *AddPath(const char *inStr);

//same as add path, but outputs to a var. Also frees outStr if it is not null
void AddPathReuse(const char *instr, char **outStr);

//sets path to the dir that the executable file is in.
void SetPathToDir(char *pathToExec);
#endif

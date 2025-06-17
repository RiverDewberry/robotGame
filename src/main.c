#include "../include/screens.h"
#include "../include/strutils.h"
#include <stdio.h>
#include "../libs/raylib.h"

int main(int argc, char **argv)
{
    //sets path
    SetPathToDir(*argv);

    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "project");

    //does what it says
    SetTargetFPS(30);

    SelectLogicBoard(NULL);

    //does what it says
    CloseWindow();

}

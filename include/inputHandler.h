#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H
#include "../libs/raylib.h"
#include <stdint.h>
#include <stddef.h>

//tracks how much the user clicks and drags
typedef struct DragTracker
{
    //moves vector by mouse drag. if null, does nothing.
    // NOTE: is used for output
    Vector2 *center;

    //bounds movement by this box
    Rectangle bounds;

    //camera used to translate movement on screen to movement in the world
    //if NULL, no camera is used
    Camera2D *camera;

    //scales drag by this value
    float scale;

    //the position of center when the mouse is clicked
    Vector2 posClicked;

    //the position of the mouse each time the tracker is updated
    Vector2 mouseChange;
} DragTracker;

//modes for text input
typedef enum TextInputMode {
    //0123456789
    NUMBERS = 1,

    //abcdefghijklmnopqrstuvwxyz
    LETTERS = 2,

    //_-
    DASHES = 4,

    //' '
    SPACE = 8,
} textInputMode;

//tracks text input
typedef struct TextInput {
    //text is output to here
    char *textOutput;

    //max input length;
    int maxLen;

    //the current index of first empty char in textOutput
    int curIndex;

    //mode of input
    int inputMode;

    //where the input is at
    Rectangle bounds;

    //if the input was clicked
    char wasClicked;

    //if the mouse is on the input
    char mouseOnInput;

    //if the input was selected
    char isSelected;
} TextInput;

//just a button
typedef struct Button {
    //where the button is at
    Rectangle bounds;

    //if the button was clicked
    char wasClicked;

    //if the mouse is on the button
    char mouseOnInput;

    //if the button was selected
    char isSelected;
} Button;

//a list of elements that allows the user to select one elem
typedef struct SelectionList {
    //where the first elem is at
    Rectangle bounds;

    //the offset of each element
    Vector2 offset;

    //which elem was clicked
    char elemClicked;

    //scales the collision detection
    float scale;

    //which elem the mouse is over
    char elemMouseOn;

    //which elem was selected
    char elemSelected;

    //the amount of elements
    int elemNums;

    //the amount of elems displayed
    int elemsShown;

    //the first elem that is shown
    int firstElemShown;

    //the amount that the list has been scrolled.
    //  NOTE: this value gets multiplied by scale when updating
    float scroll;

} SelectionList;

//updates the DragTracker
void UpdateDragTracker(DragTracker *dt);

//ensures that the tracker is within the bounding box
void BoundDragTracker(DragTracker *dt);

//uses mouse zoom to update zoom of camera within min and max vals at a speed
void ZoomCamera(Camera2D *c, float min, float max, float speed);

//gets how far a drag tracker has been draged in the x direction in screenspace
float GetDragX(DragTracker dt);

//gets how far a drag tracker has been draged in the y direction in screenspace
float GetDragY(DragTracker dt);

//gets how far a drag tracker has been draged in the x direction in worldspace
// NOTE: needs a non-null camera
float GetWorldDragX(DragTracker dt);

//gets how far a drag tracker has been draged in the y direction in worldspace
// NOTE: needs a non-null camera
float GetWorldDragY(DragTracker dt);

//updates the state of the text input
void UpdateTextInput(TextInput *ti);

//gets a number form a string.
// WARNING: does not check if str is valid
int GetNumFromStr(const char *str);

//sets a string to a num.
// NOTE: str must have enough mem allocated.
void SetStrToNum(char *str, int num);

//updates a button
void UpdateButton(Button *bn);

//updates a button that is scaled by a factor
void UpdateButtonScaled(Button *bn, float scale);

//updates a selectionList.
// NOTE: does not update scroll
// NOTE: this also assumes that elements do not overlap
void UpdateSelectionList(SelectionList *sl);

//scrolls the list. NOTE: scroll should not be scaled
void ScrollSelectionList(SelectionList *sl, float scroll);

#endif

#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H
#include <raylib.h>
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

//updates the DragTracker
void updateDragTracker(DragTracker *dt);

//ensures that the tracker is within the bounding box
void boundDragTracker(DragTracker *dt);

//uses mouse zoom to update zoom of camera within min and max vals at a speed
void zoomCamera(Camera2D *c, float min, float max, float speed);

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

#endif

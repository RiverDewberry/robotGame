#include "../include/inputHandler.h"
#include <raylib.h>

void updateDragTracker(DragTracker *dt)
{

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        dt->mouseChange = GetMousePosition();
        if(dt->center == NULL)return;

        dt->posClicked = *dt->center;
    }

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if(dt->center == NULL)return;
        Vector2 t = GetMousePosition();

        *dt->center = dt->posClicked;
        if(dt->camera == NULL)
        {
            dt->center->x += (t.x - dt->mouseChange.x) * dt->scale;
            dt->center->y += (t.y - dt->mouseChange.y) * dt->scale;
        } else
        {
            dt->center->x += 
                ((t.x - dt->mouseChange.x) * dt->scale) / dt->camera->zoom;
            dt->center->y += 
                ((t.y - dt->mouseChange.y) * dt->scale) / dt->camera->zoom;
        }

        boundDragTracker(dt);
    }
}

float GetDragX(DragTracker dt)
{
    return (GetMouseX() - dt.mouseChange.x) * dt.scale;
}

float GetDragY(DragTracker dt)
{
    return (GetMouseY() - dt.mouseChange.y) * dt.scale;
}

float GetWorldDragX(DragTracker dt)
{
    return ((GetMouseX() - dt.mouseChange.x) * dt.scale) / dt.camera->zoom;
}

float GetWorldDragY(DragTracker dt)
{
    return ((GetMouseY() - dt.mouseChange.y) * dt.scale) / dt.camera->zoom;
}

void boundDragTracker(DragTracker *dt)
{
        if(dt->center == NULL)return;

        if(dt->bounds.x > dt->center->x)dt->center->x = dt->bounds.x;
        if(dt->bounds.y > dt->center->y)dt->center->y = dt->bounds.y;

        if((dt->bounds.x + dt->bounds.width) < dt->center->x)
            dt->center->x = dt->bounds.x + dt->bounds.width;

        if((dt->bounds.y + dt->bounds.height) < dt->center->y)
            dt->center->y = dt->bounds.y + dt->bounds.height;
}

void zoomCamera(Camera2D *c, float min, float max, float speed)
{
    c->zoom += GetMouseWheelMove() * speed * c->zoom;
    if(c->zoom < min)c->zoom = min;
    if(c->zoom > max)c->zoom = max;
}

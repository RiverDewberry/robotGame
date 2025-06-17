#include "../include/inputHandler.h"
#include "../libs/raylib.h"

void UpdateDragTracker(DragTracker *dt)
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

        BoundDragTracker(dt);
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

void BoundDragTracker(DragTracker *dt)
{
        if(dt->center == NULL)return;

        if(dt->bounds.x > dt->center->x)dt->center->x = dt->bounds.x;
        if(dt->bounds.y > dt->center->y)dt->center->y = dt->bounds.y;

        if((dt->bounds.x + dt->bounds.width) < dt->center->x)
            dt->center->x = dt->bounds.x + dt->bounds.width;

        if((dt->bounds.y + dt->bounds.height) < dt->center->y)
            dt->center->y = dt->bounds.y + dt->bounds.height;
}

void ZoomCamera(Camera2D *c, float min, float max, float speed)
{
    c->zoom += GetMouseWheelMove() * speed * c->zoom;
    if(c->zoom < min)c->zoom = min;
    if(c->zoom > max)c->zoom = max;
}

void UpdateTextInput(TextInput *ti)
{
    ti->mouseOnInput = CheckCollisionPointRec(
        GetMousePosition(),
        ti->bounds
    );

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        ti->wasClicked = ti->mouseOnInput;

    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        ti->isSelected = ti->wasClicked && ti->mouseOnInput;

    if(ti->isSelected)ti->isSelected = ti->mouseOnInput;

    if(ti->isSelected)
    {
        if(IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE))
        {
            if(ti->curIndex > 0)
            {
                ti->curIndex--;
                ti->textOutput[ti->curIndex] = 0;
            }
        }
        else
        {
            int temp;
            while(
                ((temp = GetCharPressed()) != 0) &&
                (ti->curIndex < ti->maxLen)
            )
            {
                if(temp > 0xff)break;

                char valid = 0;

                if(
                    ((temp >= 'A') && (temp <= 'Z')) ||
                    ((temp >= 'a') && (temp <= 'z'))
                )
                {
                    if ((ti->inputMode & LETTERS) != 0)
                        valid = 1;
                    else break;
                }

                if((temp >= '0') && (temp <= '9'))
                {
                    if((ti->inputMode & NUMBERS) != 0)
                        valid = 1;
                    else break;
                }

                if((temp == '-') || (temp == '_'))
                {
                    if((ti->inputMode & DASHES) != 0)
                        valid = 1;
                    else break;
                }

                if(temp == ' ')
                {
                    if((ti->inputMode & SPACE) != 0)
                        valid = 1;
                    else break;
                }

                if(valid == 1)
                {
                    ti->textOutput[ti->curIndex] = temp & 0xff;
                    ti->curIndex++;
                }
            }
        }
    }
}

int GetNumFromStr(const char *str)
{
    int ret = 0;

    for(int i = 0; str[i] != 0; i++)
    {
        ret *= 10;
        ret += str[i] - '0';
    }

    return ret;
}

void SetStrToNum(char *str, int num)
{
    int i = 0;

    for(int j = num; j > 0; i++, j /= 10);

    str[i] = 0;
    i--;

    for(; num > 0; i--, num /= 10)
    {
        str[i] = '0' + (num % 10);
    }
}

void UpdateButton(Button *bn)
{
    bn->mouseOnInput = CheckCollisionPointRec(
        GetMousePosition(),
        bn->bounds
    );

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        bn->wasClicked = bn->mouseOnInput;

    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        bn->isSelected = bn->wasClicked && bn->mouseOnInput;

    if(bn->isSelected)bn->isSelected = bn->mouseOnInput;
}

void UpdateButtonScaled(Button *bn, float scale)
{
    bn->mouseOnInput = CheckCollisionPointRec(
        GetMousePosition(),
        (Rectangle) {
            bn->bounds.x * scale,
            bn->bounds.y * scale,
            bn->bounds.width * scale,
            bn->bounds.height * scale
        }
    );

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        bn->wasClicked = bn->mouseOnInput;

    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        bn->isSelected = bn->wasClicked && bn->mouseOnInput;

    if(bn->isSelected)bn->isSelected = bn->mouseOnInput;
}

void UpdateSelectionList(SelectionList *sl)
{
    int maxElem = sl->firstElemShown + sl->elemsShown;

    if(maxElem > sl->elemNums) maxElem = sl->elemNums;

    for(
        int i = sl->firstElemShown;
        i < sl->firstElemShown + sl->elemsShown;
        i++
    )
    {
        if(CheckCollisionPointRec(
            GetMousePosition(),
            (Rectangle) {
                (
                    sl->bounds.x + sl->offset.x * (i - sl->firstElemShown)
                ) * sl->scale,
                (
                    sl->bounds.y + sl->offset.y * (i - sl->firstElemShown)
                ) * sl->scale,
                sl->bounds.width * sl->scale,
                sl->bounds.height * sl->scale
            }
        ))
        {
            if(i >= maxElem)
            {
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    sl->elemSelected = -1;

                continue;
            }

            sl->elemMouseOn = i;

            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                sl->elemClicked = i;

            if(
                IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                (sl->elemMouseOn == sl->elemClicked)
            )
            {
                if(sl->elemSelected == i)
                    sl->elemSelected = -1;
                else
                    sl->elemSelected = i;
            }

            return;
        }
    }

    sl->elemMouseOn = -1;
}

void ScrollSelectionList(SelectionList *sl, float scroll)
{
    sl->scroll -= scroll;

    if(
        (sl->scroll > sl->offset.y) &&
        ((sl->firstElemShown + sl->elemsShown) < sl->elemNums)
    )
    {
        sl->firstElemShown++;
        if((sl->firstElemShown + sl->elemsShown) >= sl->elemNums)
            sl->firstElemShown = sl->elemNums - sl->elemsShown;
        sl->scroll -= sl->offset.y;
    }

    if(sl->scroll < (sl->offset.y * -1.0f))
    {
        sl->firstElemShown--;
        if(sl->firstElemShown >= -1)sl->firstElemShown = 0;
        sl->scroll += sl->offset.y;
    }
}

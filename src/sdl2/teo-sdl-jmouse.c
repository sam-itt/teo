#include <SDL.h>

#include "teo.h"
#include "defs.h"

#include "media/mouse.h"
#include "sdl2/teo-sdl-mouse.h"

#define MOUSE_SPEED 1;

int jmouse_vertical_axis = 3;
int jmouse_horizontal_axis = 2;
int jmouse_button_left = 4;
int jmouse_button_right = 5;

static Sint8 jmouse_vx = 0;
static Sint8 jmouse_vy = 0;




void teoSDL_JMouseAccelerate(SDL_JoyAxisEvent *event)
{
/*    printf("Joystick %d axis %d value changed to: %d\n", 
           event->which, 
           event->axis,
           event->value);*/
    Sint16 factor;

    /* Analogue axis go from -32768 to 32767. 
     * i.e for a horizontal axis:
     * -32768 -> 0: left
     *  0: center
     *  0-32767: right
     * is the stick is pushed past halfway in a
     * direction we apply a speedup of twice
     * */
    factor = (abs(event->value) < 16383) ? 1 : 2; 

    if(event->axis == jmouse_vertical_axis){
        if(event->value < 0){
            jmouse_vy = -1 * factor*MOUSE_SPEED
        }else if(event->value > 0){
            jmouse_vy = factor*MOUSE_SPEED
        }else{
            jmouse_vy = 0;
        }
    }

    if(event->axis == jmouse_horizontal_axis){
        if(event->value < 0){
            jmouse_vx = -1 * factor*MOUSE_SPEED
        }else if(event->value > 0){
            jmouse_vx = factor*MOUSE_SPEED
        }else{
            jmouse_vx = 0;
        }
    }
}


void teoSDL_JMouseMove(void)
{
    int cx,cy;

    if(jmouse_vx == 0 && jmouse_vy == 0)
        return;
    
    mouse_GetPosition(&cx, &cy);
    cx += jmouse_vx;
    cy += jmouse_vy;

    mouse_Motion(cx, cy);
}

void teoSDL_JMouseButton(SDL_JoyButtonEvent *event)
{
    SDL_MouseButtonEvent mb_event;
    Uint32 type;

    if(event->state == SDL_PRESSED)

    type = (event->state == SDL_PRESSED) ? 
           SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;

    mb_event = (SDL_MouseButtonEvent) {
        type,
        event->timestamp,
        0, /*windowID*/
        0, /*which mouse*/
        0, /*button*/
        event->state, /*state*/
        1, /*clicks*/
        0, /*x*/
        0, /*y*/
    };

    if(event->button == jmouse_button_left){
        mb_event.button = SDL_BUTTON_LEFT;
    }else if(event->button == jmouse_button_right){
        mb_event.button = SDL_BUTTON_RIGHT;
    }

    teoSDL_MouseButton(&mb_event);
}


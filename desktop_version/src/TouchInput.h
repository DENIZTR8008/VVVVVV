#ifndef TOUCHINPUT_H
#define TOUCHINPUT_H

#include <SDL.h>

void TouchInput_Init(void);
void TouchInput_HandleEvent(const SDL_Event& evt, int screenW, int screenH);
void TouchInput_Render(SDL_Renderer* renderer, int screenW, int screenH);

#endif

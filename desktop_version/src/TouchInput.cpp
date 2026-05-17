#include "TouchInput.h"
#include <SDL.h>
#include <cstring>

struct TouchZone {
    float x, y, w, h;
    SDL_Keycode keycode;    // SDL_Keycode для keymap
    bool active;
    SDL_FingerID fingerId;
    Uint8 r, g, b;
};

static TouchZone zones[] = {
    { 0.04f, 0.70f, 0.18f, 0.24f, SDLK_LEFT,  false, 0, 200, 200, 255 },
    { 0.24f, 0.70f, 0.18f, 0.24f, SDLK_RIGHT, false, 0, 200, 200, 255 },
    { 0.58f, 0.70f, 0.18f, 0.24f, SDLK_SPACE, false, 0, 255, 200, 100 },
    { 0.78f, 0.70f, 0.18f, 0.24f, SDLK_m,     false, 0, 150, 255, 150 },
};

static const int NUM_ZONES = sizeof(zones) / sizeof(zones[0]);

void TouchInput_Init(void) {
    // zones инициализированы статически
}

// Пушим SDL событие в очередь, чтобы KeyPoll::Poll() его обработал
static void PushKeyEvent(Uint32 type, SDL_Keycode keycode) {
    SDL_Event evt;
    SDL_memset(&evt, 0, sizeof(evt));
    evt.type = type;
    evt.key.keysym.sym = keycode;
    evt.key.keysym.scancode = SDL_GetScancodeFromKey(keycode);
    evt.key.keysym.mod = 0;
    evt.key.repeat = 0;
    evt.key.windowID = 0;
    SDL_PushEvent(&evt);
}

void TouchInput_HandleEvent(const SDL_Event& evt, int screenW, int screenH) {
    if (evt.type != SDL_FINGERDOWN && evt.type != SDL_FINGERUP && evt.type != SDL_FINGERMOTION)
        return;

    float fx = evt.tfinger.x;
    float fy = evt.tfinger.y;
    SDL_FingerID fid = evt.tfinger.fingerId;

    for (int i = 0; i < NUM_ZONES; ++i) {
        TouchZone& z = zones[i];
        bool inside = (fx >= z.x && fx <= z.x + z.w && fy >= z.y && fy <= z.y + z.h);

        if (evt.type == SDL_FINGERDOWN && inside && !z.active) {
            z.active = true;
            z.fingerId = fid;
            PushKeyEvent(SDL_KEYDOWN, z.keycode);
        }
        else if (evt.type == SDL_FINGERUP && z.active && z.fingerId == fid) {
            z.active = false;
            PushKeyEvent(SDL_KEYUP, z.keycode);
        }
        else if (evt.type == SDL_FINGERMOTION && z.active && z.fingerId == fid && !inside) {
            z.active = false;
            PushKeyEvent(SDL_KEYUP, z.keycode);
        }
    }
}

void TouchInput_Render(SDL_Renderer* renderer, int screenW, int screenH) {
    for (int i = 0; i < NUM_ZONES; ++i) {
        TouchZone& z = zones[i];
        SDL_Rect rect;
        rect.x = (int)(z.x * screenW);
        rect.y = (int)(z.y * screenH);
        rect.w = (int)(z.w * screenW);
        rect.h = (int)(z.h * screenH);

        Uint8 alpha = z.active ? 180 : 80;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, z.r, z.g, z.b, alpha);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, z.r, z.g, z.b, 200);
        SDL_RenderDrawRect(renderer, &rect);
    }
}

#include "TouchInput.h"
#include <SDL.h>
#include <cstring>

struct TouchZone {
    float x, y, w, h;       // Relative coordinates (0.0-1.0)
    SDL_Scancode key;
    bool active;
    SDL_FingerID fingerId;
    const char* label;
    Uint8 r, g, b;
};

static TouchZone zones[] = {
    // Left bottom: D-Pad
    { 0.04f, 0.70f, 0.18f, 0.24f, SDL_SCANCODE_LEFT,  false, 0, "<",  200, 200, 255 },
    { 0.24f, 0.70f, 0.18f, 0.24f, SDL_SCANCODE_RIGHT, false, 0, ">",  200, 200, 255 },

    // Right bottom: Action + Map
    { 0.58f, 0.70f, 0.18f, 0.24f, SDL_SCANCODE_SPACE, false, 0, "FLIP", 255, 200, 100 },
    { 0.78f, 0.70f, 0.18f, 0.24f, SDL_SCANCODE_TAB,   false, 0, "MAP",  150, 255, 150 },
};

static const int NUM_ZONES = sizeof(zones) / sizeof(zones[0]);

void TouchInput_Init(void) {
    // zones are statically initialized, nothing to do here
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
            SDL_SendKeyboardKey(SDL_PRESSED, z.key);
        }
        else if (evt.type == SDL_FINGERUP && z.active && z.fingerId == fid) {
            z.active = false;
            SDL_SendKeyboardKey(SDL_RELEASED, z.key);
        }
        else if (evt.type == SDL_FINGERMOTION && z.active && z.fingerId == fid && !inside) {
            z.active = false;
            SDL_SendKeyboardKey(SDL_RELEASED, z.key);
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

#include "TouchInput.h"
#include <SDL.h>
#include <map>

// Зональное управление как в VVVVVV-CE
// Левая половина: LEFT/RIGHT (зависит от текущего движения)
// Правая половина: FLIP (V)
// Верхний левый угол: ENTER (действие/меню)
// Верхний правый угол: ESCAPE (назад/меню)

static std::map<SDL_FingerID, SDL_Keycode> finger_buttons;
static float orig_x = 0.0f;
static int delayed_left_time = -10;
static int delayed_right_time = -10;

// Для эмуляции fakekey (Enter/Escape)
static SDL_Keycode fakekey = SDLK_UNKNOWN;
static int fakekeytimer = -1;

void TouchInput_Init(void) {
    finger_buttons.clear();
    delayed_left_time = -10;
    delayed_right_time = -10;
    fakekey = SDLK_UNKNOWN;
    fakekeytimer = -1;
}

// Прямое обновление keymap (как в CE)
// Нужен доступ к keymap из KeyPoll — будем использовать SDL_PushEvent
// Но правильнее — модифицировать KeyPoll.h чтобы keymap был доступен
// Пока используем SDL_PushEvent с SDL_KEYDOWN/UP

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

    float fx = evt.tfinger.x;  // 0.0 - 1.0
    float fy = evt.tfinger.y;
    SDL_FingerID fid = evt.tfinger.fingerId;

    // Абсолютные координаты для проверки углов (игра 320x240 логически)
    float absx = fx * 320.0f;
    float absy = fy * 240.0f;

    if (evt.type == SDL_FINGERDOWN) {
        // Углы: Enter и Escape (как в CE)
        if (absx < 30 && absy < 30) {
            // Верхний левый угол = ENTER
            if (fakekeytimer > 0) {
                PushKeyEvent(SDL_KEYUP, fakekey);
            }
            fakekey = SDLK_RETURN;
            fakekeytimer = 6;
            PushKeyEvent(SDL_KEYDOWN, SDLK_RETURN);
            return;
        }
        else if (absx > 290 && absy < 30) {
            // Верхний правый угол = ESCAPE
            if (fakekeytimer > 0) {
                PushKeyEvent(SDL_KEYUP, fakekey);
            }
            fakekey = SDLK_ESCAPE;
            fakekeytimer = 6;
            PushKeyEvent(SDL_KEYDOWN, SDLK_ESCAPE);
            return;
        }

        // Основное управление
        if (fx < 0.5f) {
            // ЛЕВАЯ половина экрана
            // Если уже идём вправо — это FLIP
            // Иначе — LEFT
            // (Проверяем через delayed_right_time как в CE)
            bool going_right = (delayed_right_time > -3);

            if (going_right) {
                // Flip!
                PushKeyEvent(SDL_KEYDOWN, SDLK_v);
                finger_buttons[fid] = SDLK_v;
                delayed_right_time = 0;
            } else {
                // Влево
                PushKeyEvent(SDL_KEYDOWN, SDLK_LEFT);
                finger_buttons[fid] = SDLK_LEFT;
                delayed_left_time = 0;
            }
        } else {
            // ПРАВАЯ половина экрана
            // Если уже идём влево — это FLIP
            // Иначе — RIGHT
            bool going_left = (delayed_left_time > -3);

            if (going_left) {
                // Flip!
                PushKeyEvent(SDL_KEYDOWN, SDLK_v);
                finger_buttons[fid] = SDLK_v;
                delayed_left_time = 0;
            } else {
                // Вправо
                PushKeyEvent(SDL_KEYDOWN, SDLK_RIGHT);
                finger_buttons[fid] = SDLK_RIGHT;
                delayed_right_time = 0;
            }
        }
    }
    else if (evt.type == SDL_FINGERUP) {
        // Отпускаем кнопку
        auto iter = finger_buttons.find(fid);
        if (iter != finger_buttons.end()) {
            PushKeyEvent(SDL_KEYUP, iter->second);

            if (iter->second == SDLK_LEFT) {
                delayed_left_time = -10;  // Задержка перед следующим нажатием
            } else if (iter->second == SDLK_RIGHT) {
                delayed_right_time = -10;
            }

            finger_buttons.erase(iter);
        }

        // Отпускаем fakekey
        if (fakekeytimer > 0) {
            PushKeyEvent(SDL_KEYUP, fakekey);
            fakekeytimer = -1;
        }
    }
    else if (evt.type == SDL_FINGERMOTION) {
        // Для свайпа можно добавить логику, но в CE это holdinput
        // Пока ничего не делаем при движении
    }
}

void TouchInput_Render(SDL_Renderer* renderer, int screenW, int screenH) {
    // Полупрозрачные зоны (как в CE — невидимые, но для отладки можно включить)
    // В CE зоны невидимые, но я добавлю лёгкую индикацию

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Левая зона (LEFT/RIGHT) — полупрозрачный синий
    SDL_Rect leftZone = { 0, (int)(screenH * 0.3f), (int)(screenW * 0.5f), (int)(screenH * 0.7f) };
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 30);
    SDL_RenderFillRect(renderer, &leftZone);
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 80);
    SDL_RenderDrawRect(renderer, &leftZone);

    // Правая зона (FLIP) — полупрозрачный оранжевый
    SDL_Rect rightZone = { (int)(screenW * 0.5f), (int)(screenH * 0.3f), (int)(screenW * 0.5f), (int)(screenH * 0.7f) };
    SDL_SetRenderDrawColor(renderer, 255, 150, 50, 30);
    SDL_RenderFillRect(renderer, &rightZone);
    SDL_SetRenderDrawColor(renderer, 255, 150, 50, 80);
    SDL_RenderDrawRect(renderer, &rightZone);

    // Угловые кнопки
    // Enter (левый верх)
    SDL_Rect enterBtn = { 10, 10, 60, 40 };
    SDL_SetRenderDrawColor(renderer, 200, 255, 200, 60);
    SDL_RenderFillRect(renderer, &enterBtn);
    SDL_SetRenderDrawColor(renderer, 200, 255, 200, 120);
    SDL_RenderDrawRect(renderer, &enterBtn);

    // Escape (правый верх)
    SDL_Rect escBtn = { screenW - 70, 10, 60, 40 };
    SDL_SetRenderDrawColor(renderer, 255, 200, 200, 60);
    SDL_RenderFillRect(renderer, &escBtn);
    SDL_SetRenderDrawColor(renderer, 255, 200, 200, 120);
    SDL_RenderDrawRect(renderer, &escBtn);
}

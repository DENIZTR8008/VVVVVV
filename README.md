# VVVVVV Android Touch Controls — Integrated (FINAL)

Теперь touch управление ВСТРОЕНО прямо в KeyPoll.cpp.
Никаких отдельных модулей, всё в одном месте.

## Как работает

- Переменные `finger_buttons`, `delayed_left_time`, `delayed_right_time`, `fakekey` — в начале KeyPoll.cpp
- `ProcessTouchEvent()` — обрабатывает SDL_FINGERDOWN/UP прямо внутри `while(SDL_PollEvent)`
- Прямое обновление `key.keymap[SDLK_*]` — без посредников
- Рендер зон встроен в `Screen::RenderPresent()`

## Управление (как в CE)

| Зона | Действие |
|------|----------|
| Левая половина экрана | LEFT / RIGHT |
| Правая половина экрана | FLIP (V) |
| Верхний левый угол | ENTER |
| Верхний правый угол | ESCAPE |

## Установка

1. Распакуй ZIP поверх форка (с заменой)
2. GitHub Desktop → Commit → Push
3. Собери: ./gradlew assembleDebug

# VVVVVV Android Touch Controls v2 — Пропатченные файлы

ИСПРАВЛЕНА ПРОБЛЕМА: теперь используется SDL_PushEvent вместо SDL_SendKeyboardKey.
KeyPoll::Poll() корректно обрабатывает touch через стандартную SDL очередь событий.

## Быстрая установка

1. Распакуй ZIP поверх своего форка VVVVVV (с заменой файлов)
2. GitHub Desktop → Commit → Push
3. data.zip в desktop_version/VVVVVV-android/app/src/main/assets/
4. Собери: ./gradlew assembleDebug

## Кнопки
- [ < ] влево (SDLK_LEFT)
- [ > ] вправо (SDLK_RIGHT)
- [FLIP] флип (SDLK_SPACE)
- [MAP] карта (SDLK_m)

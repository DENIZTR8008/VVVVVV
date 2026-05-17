# VVVVVV Android Touch Controls — Пропатченные файлы

Этот архив содержит уже изменённые файлы. Просто распакуй его поверх своего форка VVVVVV.

## Быстрая установка

1. Открой папку со своим форком VVVVVV (где лежит .git и desktop_version)
2. Распакуй этот ZIP прямо туда (с заменой файлов)
3. Открой GitHub Desktop — ты увидишь 5 изменённых файлов
4. Напиши Summary: `Add Android touch controls` и нажми Commit + Push

## Что изменено

| Файл | Изменение |
|------|-----------|
| `desktop_version/src/TouchInput.h` | Новый — заголовок модуля |
| `desktop_version/src/TouchInput.cpp` | Новый — 4 виртуальные кнопки |
| `desktop_version/src/KeyPoll.cpp` | Пропатчен — обработка тач-событий |
| `desktop_version/src/Screen.cpp` | Пропатчен — отрисовка кнопок |
| `desktop_version/CMakeLists.txt` | Пропатчен — добавлен TouchInput.cpp в сборку |

## Кнопки на экране

```
┌─────────────────────────────┐
│                             │
│         ИГРА                 │
│                             │
│  [ < ] [ > ]    [FLIP] [MAP]│
│  влево вправо   флип   карта│
└─────────────────────────────┘
```

## Сборка

Не забудь положить `data.zip` в:
```
desktop_version/VVVVVV-android/app/src/main/assets/data.zip
```

Затем:
```bash
cd desktop_version/VVVVVV-android
./gradlew assembleDebug
```

## GitHub Actions

Зайди на github.com/ТВОЙ_НИК/VVVVVV → Actions → Android CI → Run workflow

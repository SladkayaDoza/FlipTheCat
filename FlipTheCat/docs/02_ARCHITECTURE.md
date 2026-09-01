# FlipTheCat — архитектура (после реструктуризации)

> «Как было» описано в [`01_CURRENT_STRUCTURE.md`](01_CURRENT_STRUCTURE.md).
> Как добавить свой модуль — в [`03_ADDING_MODULES.md`](03_ADDING_MODULES.md).
> Список исправлений и идей — в [`04_IMPROVEMENTS.md`](04_IMPROVEMENTS.md).

## 1. Идея

Прошивка осталась «однопоточным конечным автоматом на блокирующих экранах»
(так проще всего для устройства с одним пользователем и одним экраном), но:

- код разложен по слоям (`config / core / hal / ui / apps / games`);
- верхнеуровневые меню теперь **данные, а не код** — таблица `MenuItem[]`;
- пины и константы собраны в один `config/`;
- заголовки защищены `#pragma once`, устранены дубли определений и ODR-ловушки;
- исправлены зависания, гонки, утечки и просадки производительности
  (полный список — в `04_IMPROVEMENTS.md`).

## 2. Дерево каталогов

```
src/
├── main.cpp                 // setup()/loop(), функции CC1101, экраны RC/Raw/Freq/Brute, таблицы меню
├── config/
│   ├── pins.h               // ЕДИНАЯ карта выводов (все #define пинов + LEDC/ADC)
│   └── build_config.h       // версия, флаги, макросы логирования FTC_LOG/FTC_LOGLN
├── core/
│   ├── global_vars.h/.cpp   // глобальное состояние (радио, бипер, настройки, JSON-документы)
│   └── menu.h/.cpp          // MenuItem + универсальный драйвер runMenu()  ← реестр модулей
├── hal/                     // абстракция железа
│   ├── display.h            // extern GyverOLED oled
│   ├── button.h             // extern VirtButton up/down/.../top_left/top_right
│   ├── tick.h/.cpp          // tk(): опрос кнопок с АЦП + авто-выключение бипера
│   ├── buzzer.h/.cpp        // бипер (LEDC ch1)
│   ├── adc.h/.cpp           // getVolltage(): напряжение батареи (с кешем)
│   ├── gpioPins.h/.cpp      // класс OUTPIN + объекты pin1/pin2/pin3 (LEDC ch2/3/4)
│   ├── radio.h/.cpp         // запись/воспроизведение raw-сигналов
│   └── fileSystem.h/.cpp    // save/readJsonToFile (SPIFFS, прямой стрим)
├── ui/                      // переиспользуемые элементы интерфейса
│   ├── pointer.h/.cpp       // курсоры > / <
│   ├── tools.h/.cpp         // setName() (клавиатура), setNumber(), confirmReWrite()
│   ├── view.h/.cpp          // viewer(): визуализация raw-сигнала
│   ├── logo.h/.cpp          // заставка
│   └── nothing.h/.cpp       // заглушка-пустышка
├── apps/                    // экраны-функции верхнего уровня
│   ├── GPIO.h/.cpp          // управление выходными пинами
│   ├── settings.h/.cpp      // настройки + экран RxBW
│   ├── bluetooth.h/.cpp     // BLE-клавиатура/мышь
│   ├── wifi_app.h/.cpp      // Wi-Fi: сканер / монитор каналов / сниффер / станции (только приём)
│   ├── temp.h/.cpp          // ИК-термометр MLX90614/GY-906 (I2C 0x5A)
│   └── games.h/.cpp         // подменю игр (таблица MenuItem)
└── games/                   // движки игр
    ├── game2048.h
    ├── gamecat.h
    ├── tiles.h
    ├── game_space_impact.h
    └── si_engine.h / si_types.h / si_data.h / si_game_objects.h
```

Все подпапки добавлены в путь поиска заголовков через `build_flags = -I ...`
в `platformio.ini`, поэтому прежний стиль `#include <foo.h>` работает без
изменений вне зависимости от того, в какой папке лежит файл.

## 3. Слои и зависимости

```
main.cpp ──▶ core (menu, global_vars) ──▶ hal ──▶ железо/драйверы
   │            ▲                           ▲
   └─▶ apps ────┘        ui ────────────────┘
              apps ─▶ ui, hal, core
              games ─▶ hal, ui, core
```

- **config** — не зависит ни от чего (только `#define`).
- **hal** — тонкая обёртка над железом и драйверами (CC1101, OLED, SPIFFS, LEDC).
- **core** — общее состояние (`global_vars`) и драйвер меню (`menu`).
- **ui** — ввод/вывод, не привязанный к конкретной функции (клавиатура, курсоры).
- **apps** — конкретные функции устройства; каждая экспортирует функцию-экран.
- **games** — самодостаточные игровые движки.

## 4. Модель выполнения

`loop() → mainn() → runMenu(mainMenu, …, allowBack=false)` — корневой цикл,
который никогда не выходит. Выбор пункта вызывает функцию-экран; та крутит свой
блокирующий `while(1)`, опрашивая кнопки через `tk()`, перерисовывая OLED только
по флагу, и возвращается по кнопке back.

```
loop()
 └─ mainn()
     └─ runMenu(mainMenu)                 // рисует таблицу, диспетчеризует
         ├─ rcLay()          (main.cpp)
         ├─ rawLay()         (main.cpp) ── rawMenu(), viewer()
         ├─ detectSignal()   (main.cpp)
         ├─ miBruteForce()   (main.cpp) ── rc12bitBruteForce()
         ├─ bluetoothLay()   (main.cpp) ── runMenu(bluetoothMenu) ── selectKeyboard()/selectMouse()
         ├─ wifiLay()        (apps/wifi_app.cpp) ── runMenu(wifiMenu) ── Scanner/Ch Monitor/Sniffer/Stations
         ├─ tempLay()        (apps/temp.cpp) ── MLX90614/GY-906 I2C-термометр
         ├─ gpioLay()        (apps/GPIO.cpp)
         ├─ settings()       (apps/settings.cpp) ── settingsRawBW()
         └─ games()          (apps/games.cpp)     ── runMenu(gamesMenu) ── 2048/Cat/Space Impact
```

## 5. Реестр модулей (`core/menu.h`)

```cpp
struct MenuItem {
  const char* name;   // подпись в меню
  void (*run)();       // функция-экран
};

void runMenu(const MenuItem* items, uint8_t count,
             bool showVoltage = false, bool allowBack = true);
```

`runMenu()` — единый драйвер списочного меню: рисует пункты, двигает курсор в
границах `count` (поэтому курсор не заезжает на пустые строки, как было раньше),
по OK вызывает `items[pointer].run()`, по back выходит (кроме корневого меню).

Три таблицы:
- `mainMenu[]` (`main.cpp`) — 8 функций устройства;
- `bluetoothMenu[]` (`main.cpp`) — Keyboard/Mouse;
- `gamesMenu[]` (`apps/games.cpp`) — 2048/Cat/Space Impact.

Списки RC и Raw (`rcLay`/`rawLay`) НЕ используют `runMenu`, потому что рисуют
динамические имена из JSON и поддерживают постраничную прокрутку — это отдельные
экраны, а не фиксированные таблицы.

## 6. Хранилище (SPIFFS + ArduinoJson v7)

Без изменений по смыслу (см. `01_CURRENT_STRUCTURE.md` §4), но:

- `saveJsonToFile()` теперь стримит прямо в файл, без промежуточной `String`;
- на старте контейнеры создаются надёжно: `.as<JsonObject>()`, а если пусто —
  `.to<JsonObject>()` (раньше на «чистом» устройстве все сохранения были no-op);
- ключ конфигурации переименован из кириллического `externalССModule` в
  ASCII `externalCCModule`.

> ⚠️ После обновления прошивки, если ранее была включена настройка «External CC
> module», её нужно один раз включить заново (старый кириллический ключ больше
> не читается). Значение по умолчанию — Internal.

## 7. Карта выводов

Полностью описана в [`config/pins.h`](../src/config/pins.h). Кратко:

| Назначение | Вывод |
|-----------|-------|
| OLED I²C | SDA=21, SCL=22 |
| Кнопки-джойстик (АЦП) | 34 |
| top_left / top_right | 0 / 16 |
| LED / бипер | 26 / 27 (LEDC ch1) |
| Батарея (АЦП) | 35 |
| CC1101 | SCK18/MISO19/MOSI23, CS=5(внутр)/13(внешн), GDO0=4 |
| Выходы GPIO | 32/33/25 (LEDC ch2/3/4) |

## 8. Сборка

```bash
pio run                 # сборка
pio run -t upload       # прошивка
pio device monitor      # монитор порта (115200)
```

Отладочный вывод в Serial по умолчанию выключен. Чтобы включить — раскомментируйте
`#define FTC_DEBUG_SERIAL` в [`config/build_config.h`](../src/config/build_config.h).

> ⚠️ **Объём после добавления Wi-Fi.** Библиотека Wi-Fi тяжёлая: Flash вырос до
> ~87% раздела приложения (1.25 МБ), RAM до ~21.6%. Ещё помещается (~168 КБ
> запаса), но это основной потребитель flash. Если места перестанет хватать,
> нужна своя таблица разделов с бо́льшим app-разделом (без потери SPIFFS, где
> лежат `*.json`/`*.sav`).

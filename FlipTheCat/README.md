# FlipTheCat

Многофункциональный инструмент для работы с сигналами на базе **ESP32**
(в духе Flipper Zero): суб-ГГц радио (CC1101), Bluetooth (BLE), управление GPIO,
анализатор частот и набор игр. Фреймворк — **Arduino**, сборка — **PlatformIO**.

> ⚠️ Инструмент предназначен для авторизованного тестирования, исследований и
> обучения на **собственном оборудовании**. Ответственность за использование
> несёт владелец устройства.

## Возможности

- **RC Custom** — запись и передача кодов RCSwitch (data + bits).
- **RawData Read** — запись/воспроизведение «сырых» суб-ГГц сигналов + просмотрщик.
- **Frequency Analyzer** — поиск пиковой частоты по RSSI.
- **RC 12bit bruteForce** — перебор 12-битных кодов.
- **Bluetooth** — BLE-клавиатура (мышь — задел).
- **WiFi** — пассивный анализ эфира: сканер точек доступа, монитор загруженности
  каналов, сниффер кадров (promiscuous), список станций в эфире. Только приём,
  без активных атак.
- **Temp** — ИК-термометр MLX90614 (модуль GY-906) по I2C: температура объекта и
  окружения в °C, калибровка‑смещение, пресеты эмиссивности по типу поверхности.
- **GPIO** — управление выходными пинами (ШИМ).
- **Settings** — частота, бипер, выбор CC-модуля (внутр./внешн.), полоса RxBW.
- **Games** — 2048, Cat, Space Impact.

## Быстрый старт

```bash
pio run                 # сборка
pio run -t upload       # прошивка ESP32 (esp32dev)
pio device monitor      # монитор порта, 115200
```

Отладочный вывод в Serial по умолчанию выключен — включается
`#define FTC_DEBUG_SERIAL` в `src/config/build_config.h`.

## Структура

```
src/
├── main.cpp        точка входа, экраны RC/Raw/Freq/Brute, таблицы меню
├── config/         pins.h (распиновка), build_config.h (флаги/версия)
├── core/           global_vars (состояние), menu (реестр модулей runMenu)
├── hal/            железо: display, button, tick, buzzer, adc, gpioPins, radio, fileSystem
├── ui/             pointer, tools (клавиатура/число/подтверждение), view, logo, nothing
├── apps/           GPIO, settings, bluetooth, wifi_app, temp, games
└── games/          game2048, gamecat, tiles, space impact (game_space_impact + si_*)
lib/                вендоренные библиотеки (точные версии — источник истины)
docs/               документация (см. ниже)
```

## Документация

| Документ | О чём |
|----------|-------|
| [docs/01_CURRENT_STRUCTURE.md](docs/01_CURRENT_STRUCTURE.md) | Структура «как было» (до реструктуризации) |
| [docs/02_ARCHITECTURE.md](docs/02_ARCHITECTURE.md) | Актуальная архитектура, слои, поток управления |
| [docs/03_ADDING_MODULES.md](docs/03_ADDING_MODULES.md) | Как добавить свой модуль (реестр меню) |
| [docs/04_IMPROVEMENTS.md](docs/04_IMPROVEMENTS.md) | Аудит: исправления, что не трогали и почему, идеи развития |

## Железо (кратко)

OLED SSH1106 128×64 (I²C 21/22), CC1101 (VSPI 18/19/23, CS 5/13, GDO0 4),
кнопки-джойстик на АЦП 34, доп. кнопки 0/16, LED 26, бипер 27, батарея 35,
выходы GPIO 32/33/25. Полная карта — в [`src/config/pins.h`](src/config/pins.h).

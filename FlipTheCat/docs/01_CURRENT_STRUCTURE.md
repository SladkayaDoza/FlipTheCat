# FlipTheCat — структура проекта (состояние «как есть»)

> Этот документ описывает архитектуру проекта **до** реструктуризации.
> Новая архитектура описана в [`02_ARCHITECTURE.md`](02_ARCHITECTURE.md),
> список найденных проблем и исправлений — в [`04_IMPROVEMENTS.md`](04_IMPROVEMENTS.md).

## 1. Что это

**FlipTheCat** — многофункциональный инструмент для работы с радиосигналами
на базе **ESP32**, вдохновлённый Flipper Zero. Устройство собрано на фреймворке
**Arduino** и собирается через **PlatformIO**.

Основные возможности (пункты главного меню):

| # | Пункт меню | Файл реализации | Назначение |
|---|------------|-----------------|------------|
| 0 | `RC Custom` | `main.cpp: rcLay()` | Запись/передача кодов RCSwitch (data + bits) |
| 1 | `RawData Read` | `main.cpp: rawLay()` | Запись/воспроизведение «сырых» radio-сигналов (тайминги) |
| 2 | `Frequency Analyzer` | `main.cpp: detectSignal()` | Поиск пиковой частоты по RSSI |
| 3 | `RC 12bit bruteForce` | `main.cpp: rc12bitBruteForce()` | Перебор 12-битных кодов |
| 4 | `BLUETOOTH` | `bluetooth.cpp` | BLE-клавиатура (мышь — заглушка) |
| 5 | `GPIO` | `GPIO.cpp: gpioLay()` | Управление 3 выходными пинами (ШИМ) |
| 6 | `Settings` | `settings.cpp: settings()` | Частота, бипер, CC-модуль, RxBW |
| 7 | `Games` | `games.cpp: games()` | 2048, Cat (сокобан), Space Impact |

## 2. Аппаратная часть и распиновка

| Периферия | Интерфейс / пины | Примечание |
|-----------|------------------|------------|
| OLED-дисплей `SSH1106 128×64` | I²C (SDA=21, SCL=22), `Wire.setClock(800000)` | Библиотека GyverOLED |
| Радиомодуль `CC1101` | VSPI: SCK=18, MISO=19, MOSI=23, CS=5 (внутр.) / 13 (внешн.) | Драйвер SmartRC-CC1101 |
| CC1101 GDO0 / линия данных RC | `RCPin = GPIO4` | Приём/передача RCSwitch и raw |
| Кнопки-джойстик (резистивная лестница) | ADC на `GPIO34` | up/right/back/down/ok/left по порогам |
| Кнопка `top_left` | `GPIO0` (`INPUT_PULLUP`) | В комментарии указан альт. пин 17 |
| Кнопка `top_right` | `GPIO16` (`INPUT_PULLUP`) | |
| Светодиод | `GPIO26` | Индикация приёма/передачи |
| Бипер | `GPIO27`, LEDC-канал 1 | |
| Замер напряжения батареи | ADC на `GPIO35` | Делитель R1=50.61k, R2=76.4k |
| Пользовательские выходы GPIO | pin1=`GPIO32`, pin2=`GPIO33`, pin3=`GPIO25` | ШИМ на pin1 через LEDC-канал 0 |

### Пороги АЦП для кнопок (`tick.cpp`)

Все кнопки-джойстик считываются с одного вывода `GPIO34` по диапазонам:

```
up    : 0    < a < 1800
right : 1800 < a < 2060
back  : 2060 < a < 2360
down  : 2360 < a < 2850
ok    : 2850 < a < 3623
left  : 3623 < a < 4096
```

## 3. Сборка и зависимости

`platformio.ini`:

```ini
[env:esp32dev]
platform      = espressif32
board         = esp32dev
framework     = arduino
monitor_speed = 115200
upload_speed  = 921600
```

Внешние библиотеки (вендорятся в `lib/`, версии из baseline-сборки):

| Библиотека | Версия | Для чего |
|------------|--------|----------|
| ArduinoJson | 7.0.3 | Хранение конфигов и сигналов в JSON |
| EncButton | 3.5.2 | `VirtButton` — виртуальные кнопки/дебаунс |
| ESP32 BLE Keyboard | 0.3.2 | BLE HID-клавиатура |
| GyverIO | 1.3.4 | Вспомогательный I/O |
| GyverOLED | 1.6.1 | Драйвер OLED |
| NimBLE-Arduino | 1.4.1 | BLE-стек |
| SmartRC-CC1101-Driver-Lib | 2.5.7 | Драйвер CC1101 |
| rc-switch | 2.6.4 | Кодирование/декодирование RC-протоколов |

Файловая система — **SPIFFS** (`SPIFFS.begin(true)` — форматирует при первом старте).

**Baseline-метрики сборки:** RAM 16.4% (53 880 B), Flash 56.6% (742 237 B).

## 4. Модель данных (SPIFFS + ArduinoJson)

В памяти живут три глобальных `JsonDocument` (объявлены в `main.cpp`), каждый
загружается из своего файла при старте и **целиком** переписывается при любом изменении:

| Документ | Файл | Корневой объект | Содержимое |
|----------|------|-----------------|------------|
| `docConfigFile` | `/config.json` | `config` → `objectConfigFile` | `pik`, `pikHz`, `externalССModule` |
| `doc` | `/raw.json` | `sensorData` → `objectDoc` | Метаданные raw-сигналов по индексу-ключу |
| `docRC` | `/rc.json` | `data` → `objectRC` | RC-коды: `name`, `frequency`, `data`, `bytes` |

Ключами объектов служат **строки индексов** (`"0"`, `"1"`, …), формируемые через
`String(pointer)`. Сами отсчёты raw-сигнала хранятся в **отдельных файлах**
`/rawN.json` (поле `RAW` — массив знаковых длительностей в мкс), на которые
ссылается `objectDoc[i]["RawData"]`.

> ⚠️ Ключ `externalССModule` содержит **кириллические** символы «СС» (U+0421),
> а не латинские «CC». Это работает, но легко приводит к ошибкам при ручном
> редактировании.

## 5. Пофайловая карта `src/`

### Ядро и точка входа
- **`main.cpp`** (926 строк) — «монолит»: `setup()`, `loop()`, главное меню
  `mainn()`, а также все экраны RC/Raw/BruteForce/Frequency/Bluetooth и функции
  CC1101 (`setupRx/setupTx/func_trans/func_reciv/…`). Форвард-декларации и
  массивы пунктов меню (`main_lay[8]`, `blue_lay[8]`) заданы прямо здесь.
- **`global_vars.h` / `global_vars.cpp`** — глобальное состояние: указатели
  режимов, частоты, RSSI, флаги, `extern`-объявления JSON-документов. Часть
  `#define`-ов и `extern`-объявлений **продублирована** в обоих файлах.
- **`tick.cpp` / `tick.h`** — `tk()`: опрос всех кнопок из АЦП + авто-выключение
  бипера. Вызывается в каждой итерации всех UI-циклов.
- **`button.h`** — `extern VirtButton up/down/left/right/ok/back/top_left/top_right`.
- **`display.h`** — `extern GyverOLED<SSH1106_128x64> oled`.

### Радио
- **`radio.cpp` / `radio.h`** — `RecordSignal()` (запись raw), `tryRecordSignalToBuffer()`
  (низкоуровневый захват переходов), `sendSamples()` (воспроизведение).
- Функции CC1101 (`setupRx/setupTx/func_trans/func_custom_translate/func_reciv/
  detectSignal/rc12bitBruteForce`) — внутри `main.cpp`.

### Файловая система и просмотр
- **`fileSystem.cpp` / `fileSystem.h`** — `saveJsonToFile()`, `readJsonFromFile()`.
- **`view.cpp` / `view.h`** — `viewer()` и `drawSignal()`: визуализация raw-сигнала
  с масштабированием и прокруткой.

### Ввод/вывод и утилиты
- **`tools.cpp` / `tools.h`** — экранная клавиатура `setName()`, ввод числа
  `setNumber()`, подтверждение `confirmReWrite()`, `addElementToFront()`.
- **`pointer.cpp` / `pointer.h`** — отрисовка курсоров `>` / `<`.
- **`buzzer.cpp` / `buzzer.h`** — `setupBuzzer()`, `beep()` (LEDC-канал 1).
- **`adc.cpp` / `adc.h`** — `getVolltage()` (64 замера АЦП с усреднением).
- **`logo.cpp` / `logo.h`** — заставка.
- **`nothing.cpp` / `nothing.h`** — пустышка-заглушка для незанятых пунктов меню.

### GPIO
- **`GPIO.cpp` / `GPIO.h`** — экран управления пинами.
- **`gpioPins.h`** — класс `OUTPIN` **и создание трёх объектов** `pin1/pin2/pin3`
  прямо в заголовке (см. риски ODR в отчёте).

### Настройки и Bluetooth
- **`settings.cpp` / `settings.h`** — экран настроек + `settingsRawBW()`, который
  запускает **отдельную FreeRTOS-задачу** на ядре 1 для отрисовки водопада RxBW.
- **`bluetooth.cpp` / `bluetooth.h`** — `selectKeyboard()` (рабочая BLE-клавиатура),
  `selectMouse()` (закомментирована).

### Игры
- **`games.cpp` / `games.h`** — меню игр и запуск экземпляров.
- **`game2048.h`** — класс `Game2048` (вся логика в заголовке).
- **`gamecat.h`** — класс `GameCat` (сокобан, логика в заголовке).
- **`tiles.h`** — битмапы 8×8 для GameCat.
- **Space Impact** (порт): `game_space_impact.h` (класс-обёртка + рендер в OLED),
  `si_engine.h` (графика/выстрелы/враги/сцена/сейвы), `si_types.h` (типы),
  `si_data.h` (уровни/враги/шрифт), `si_game_objects.h` (спрайты). Функции и
  массивы объявлены `static` прямо в заголовках.

## 6. Поток управления

```
setup()
  ├─ adcAttachPin(34), Wire@800kHz, oled.init(), drawLogo()
  ├─ pinMode кнопок/LED, Serial, SPIFFS.begin(true)
  ├─ readJsonFromFile ×3  (/raw.json, /rc.json, /config.json)
  ├─ инициализация бипера (если pik)
  └─ CC1101: setSpiPin → Init → setMHZ → SetRx → goSleep
loop() → mainn()
  └─ while(1): tk(); отрисовка меню при displayUpdate;
       ok.click() → switch(pointer) → соответствующий экран
```

Каждый экран — это **свой блокирующий `while(1)`-цикл**, который:
1. вызывает `tk()` для опроса кнопок,
2. перерисовывает OLED только по флагу `displayUpdate`/`updDisplay`,
3. выходит по `back.click()`/`back.hold()`.

Обмена сообщениями/событий нет — навигация построена на вложенных вызовах
функций-экранов и локальных флагах перерисовки.

## 7. Как сейчас добавляется новый модуль

Чтобы добавить пункт в главное меню, сегодня нужно вручную:
1. добавить строку в массив `main_lay[8]` в `main.cpp`;
2. написать форвард-декларацию функции экрана в `main.cpp`;
3. добавить `case` в `switch(pointer)` внутри `mainn()`;
4. реализовать функцию экрана (обычно новый `.cpp` + `.h`);
5. подключить заголовок в список `#include` в `main.cpp`.

Это ручная связка в четырёх местах одного файла — главный источник хрупкости
и основной объект реструктуризации (см. [`03_ADDING_MODULES.md`](03_ADDING_MODULES.md)).

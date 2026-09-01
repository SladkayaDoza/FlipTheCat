# Как добавить свой модуль

Этот документ показывает, как расширять FlipTheCat. Благодаря реестру меню
(`core/menu.h`) добавление функции в главное меню — это **один файл и одна строка
в таблице**, а не правка switch/массивов/forward-деклараций в четырёх местах.

## Что такое «модуль»

Модуль — это функция-экран с сигнатурой:

```cpp
void myModule();
```

Она блокирующая: внутри крутится свой цикл `while (1)`, который:
1. вызывает `tk()` для опроса кнопок;
2. перерисовывает OLED только когда что-то изменилось (по флагу);
3. выходит (`return`/`break`) по кнопке **back**.

Управление возвращается в `runMenu()`, который перерисует меню.

## Шаг 1. Создать файл модуля

Положите новый экран в `src/apps/` (для функций устройства) или в подходящий слой.
Пример — `src/apps/hello.cpp`:

```cpp
#include <Arduino.h>
#include <display.h>   // oled
#include <button.h>    // up/down/ok/back/...
#include <tick.h>      // tk()
#include "hello.h"

void helloScreen() {
  bool redraw = true;
  while (1) {
    tk();                       // 1) опрос кнопок

    if (redraw) {               // 2) перерисовка по флагу
      redraw = false;
      oled.clear();
      oled.home();
      oled.setCursor(14, 0);
      oled.print("Hello, FlipTheCat!");
      oled.update();
    }

    if (back.click() || back.hold()) return;  // 3) выход по back
  }
}
```

И заголовок `src/apps/hello.h`:

```cpp
#pragma once
void helloScreen();
```

> Каталоги `src/apps`, `src/hal`, `src/ui`, `src/core`, `src/config`, `src/games`
> уже в пути поиска заголовков (см. `platformio.ini`), поэтому `#include "hello.h"`
> и `#include <display.h>` работают из любого места.

## Шаг 2. Зарегистрировать пункт в меню

Откройте `src/main.cpp`, подключите заголовок и добавьте строку в таблицу
`mainMenu[]`:

```cpp
#include <hello.h>            // рядом с другими include

static const MenuItem mainMenu[] = {
  {"RC Custom", rcLay},
  // ... существующие пункты ...
  {"Games", games},
  {"Hello", helloScreen},     // ← новая строка. Всё.
};
```

`mainMenuCount` вычисляется автоматически (`sizeof(...) / sizeof(...)`), границы
навигации подстроятся сами. Пересоберите: `pio run`.

## Шаблоны экранов

### Список с курсором (готовый драйвер)

Если у вас фиксированный набор подпунктов — не пишите цикл вручную, используйте
`runMenu()`:

```cpp
#include <menu.h>

static void optA() { /* ... */ }
static void optB() { /* ... */ }

static const MenuItem subMenu[] = {
  {"Option A", optA},
  {"Option B", optB},
};

void myModule() {
  runMenu(subMenu, sizeof(subMenu) / sizeof(subMenu[0]));  // back — выход
}
```

### Ввод имени / числа / подтверждения (готовые виджеты из `ui/tools.h`)

```cpp
#include <tools.h>

String name = setName("Enter name");           // экранная клавиатура
int freq    = setNumber("Frequency", 433);      // ввод числа
if (confirmReWrite()) { /* пользователь подтвердил */ }
```

## Работа с железом

| Нужно | Подключить | Использовать |
|-------|-----------|--------------|
| Экран | `<display.h>` | `oled.clear()/print()/update()` |
| Кнопки | `<button.h>` + `<tick.h>` | `tk();` затем `up.click()`, `ok.hold()`, `left.step()`… |
| Курсор в меню | `<pointer.h>` | `printPointer(pos)` |
| Пины (номера) | `<pins.h>` | `LED_PIN`, `RC_PIN`, `GPIO_OUT_PIN_1`… |
| Радио CC1101 | `<global_vars.h>` + `<ELECHOUSE_CC1101_SRC_DRV.h>` | `setupRx()/setupTx()`, `ELECHOUSE_cc1101.*` |
| Хранилище | `<fileSystem.h>` | `readJsonFromFile()`, `saveJsonToFile()` |
| Бипер | `<buzzer.h>` | `beep()` |
| Батарея | `<adc.h>` | `getVolltage()` |
| Логи (debug) | `<build_config.h>` | `FTC_LOG(x)`, `FTC_LOGLN(x)` |

## Правила, чтобы не сломать устройство

1. **Всегда `#pragma once`** в каждом заголовке.
2. **Не определяйте объекты/переменные в заголовках.** В `.h` — только объявления
   (`extern`, прототипы, `class {}` без объектов). Определения — в `.cpp`.
   (Именно из-за нарушения этого правила `gpioPins.h` был ODR-миной.)
3. **В блокирующих циклах всегда есть выход по back** и вызов `tk()` каждую итерацию.
4. **Не считывайте АЦП/долгие операции в каждом кадре** — кешируйте (пример:
   `getVolltage()` пересчитывается раз в секунду).
5. **Не крутите бесконечный цикл без условия выхода/паузы** — иначе watchdog
   перезагрузит ESP32 (см. исправления в `04_IMPROVEMENTS.md`).
6. **Пины — только через `config/pins.h`.** Не вводите новые `#define` пинов в модулях.
7. **JSON:** для новых данных используйте отдельный `JsonDocument`/файл, а после
   загрузки проверяйте `isNull()` и при необходимости создавайте контейнер `.to<JsonObject>()`.
8. **LEDC-каналы:** бипер занимает канал 1 (таймер 0), выходные пины — каналы 2/3/4.
   Для новой ШИМ берите свободный канал на свободном таймере.

## Пример: добавить пункт в подменю Bluetooth или Games

То же самое, но правьте таблицу `bluetoothMenu[]` в `main.cpp` или `gamesMenu[]`
в `apps/games.cpp` — по одной строке `{"Название", функция}`.

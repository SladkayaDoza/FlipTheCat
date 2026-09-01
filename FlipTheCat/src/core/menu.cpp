#include "menu.h"
#include "build_config.h"
#include <display.h>
#include <button.h>
#include <tick.h>
#include <pointer.h>
#include <adc.h>

void runMenu(const MenuItem* items, uint8_t count, bool showVoltage, bool allowBack) {
  if (items == nullptr || count == 0) return;

  uint8_t pointer = 0;
  bool displayUpdate = true;
  tk();

  while (1) {
    tk();

    if (displayUpdate) {
      displayUpdate = false;
      oled.clear();
      oled.home();
      // Постраничный вывод: если пунктов больше UI_MENU_ROWS, список прокручивается.
      uint8_t page = (pointer / UI_MENU_ROWS) * UI_MENU_ROWS;
      for (uint8_t row = 0; row < UI_MENU_ROWS; row++) {
        uint8_t idx = page + row;
        if (idx >= count) break;
        oled.setCursor(14, row);
        oled.print(items[idx].name);
      }
      if (showVoltage) {
        oled.setCursor(100, 0);
        oled.print(getVolltage());
      }
      printPointer(pointer);
      oled.update();
    }

    if (up.click() || up.step()) {
      pointer = constrain(pointer - 1, 0, count - 1);
      displayUpdate = true;
    }
    if (down.click() || down.step()) {
      pointer = constrain(pointer + 1, 0, count - 1);
      displayUpdate = true;
    }
    if (allowBack && (back.click() || back.hold())) return;
    if (ok.click() || ok.hold()) {
      if (items[pointer].run) items[pointer].run();
      displayUpdate = true;
    }
  }
}

#include <Arduino.h>
#include <global_vars.h>
#include <display.h>
#include <tick.h>
#include <button.h>
#include <fileSystem.h>

void drawSignal(float cof, JsonArray arr, int size, float flyx, uint32_t allTime);

void viewer(String j) { // Ряжанка с яблоком, Злагода
  bool displayUpdate = 1;
  float cof = 100; // Коэффициент масштабирования
  float flyx = 1; // Смещение по X
  JsonDocument rawSignal1;
  readJsonFromFile(objectDoc[j]["RawData"].as<const char*>(), rawSignal1);
  int size = rawSignal1["RAW"].as<JsonArray>().size();
  JsonArray arr = rawSignal1["RAW"].as<JsonArray>();

  uint32_t allTime = 0;  // Подсчет всего времени сигнала
  for (int i = 0; i < size; i++) {
    allTime += abs(arr[i].as<int>());
  }

  while (1) {
    static uint8_t pointer = 0;
    tk();
    if (displayUpdate != 0) {
      displayUpdate = 0;
      oled.clear();
      oled.home();
      drawSignal(cof, rawSignal1["RAW"].as<JsonArray>(), size, flyx, allTime);  // Отрисовка сигнала
      oled.update();
    }

    // Управление масштабированием
    if (up.click() or up.step()) {
      cof = constrain(cof -= cof/10, 0, 100);
      displayUpdate = 1;
    }
    if (down.click() or down.step()) {
      cof = constrain(cof += cof/10, 0, 100);
      displayUpdate = 1;
    }

    // Управление перемещением по X
    if (left.click() or left.step()) {
      flyx = constrain(flyx -= cof/20, 0, 100);
      displayUpdate = 1;
    }
    if (right.click() or right.step()) {
      flyx = constrain(flyx += cof/20, 0, 100);
      displayUpdate = 1;
    }

    // Кнопка "Назад"
    if (back.click()) return;
  }
}

void drawSignal(float cof, JsonArray arr, int size, float flyx, uint32_t allTime) {

  uint32_t percent = allTime * (cof/100);  // Определение зоны для отображения
  uint32_t sdvig = (allTime - percent) * (flyx/100);  // Перемещение зоны в доступной области
  uint8_t y = 16;  // Установка высокого уровня по умолчанию
  int curx = 0;

  // Вывод параметров отладки
  oled.setCursor(0, 0);
  oled.print(allTime);  // Длительность всего сигнала
  oled.print("  ");
  oled.print(percent);  // длительность помещаемая на екране
  oled.print("  ");
  oled.print(sdvig);  // Пропущеное время до отрисовки
  
  // отрисовка сигнала
  for (int i = 0; i < size; i++) {
    y = 48;  // Параметр уровня сигнала низкий
    if (arr[i] > 0) y = 16;  // Изменяем параметр сигнала если он высокий

    if (curx + abs(arr[i].as<int>()) > sdvig && curx < sdvig + percent) {
      oled.fastLineH(y, map(curx - sdvig, 0, percent, 0, 127), map(curx + abs(arr[i].as<int>()) - sdvig, 0, percent, 0, 127));  // рисование горизонтальной линии (отображение уровня сигнала (y = 16 -> высокий, y = 48 -> низкий))
      oled.fastLineV(map(curx + abs(arr[i].as<int>()) - sdvig, 0, percent, 0, 127), 16, 48);  // Удобное для глаз отделение высокого сигнала от низкого
    }
    curx += abs(arr[i].as<int>());  // Добавляем растояние уже пройденного сигнала
  }
}

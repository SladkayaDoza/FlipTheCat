void viewer(String j) { // Ряжанка с яблоком, Злагода
  bool displayUpdate = 1;
  float cof = 100; // Коэффициент масштабирования
  int centerX = 64; // Координата X центра экрана
  int flyx = 0; // Смещение по X
  JsonDocument rawSignal1;
  readJsonFromFile(objectDoc[j]["RawData"].as<const char*>(), rawSignal1);
  int size = rawSignal1["RAW"].as<JsonArray>().size();

  while (1) {
    static uint8_t pointer = 0;
    tk();
    if (displayUpdate != 0) {
      displayUpdate = 0;
      oled.clear();
      oled.home();
      drawSignal(cof, rawSignal1["RAW"].as<JsonArray>(), size, flyx, centerX);
      oled.update();
    }

    // Управление масштабированием
    if (up.click() or up.step()) {
      cof -= 10;
      displayUpdate = 1;
    }
    if (down.click() or down.step()) {
      cof += 10;
      displayUpdate = 1;
    }

    // Управление перемещением по X
    if (left.click() or left.step()) {
      flyx = constrain(flyx - 10 / 10, 0, 99999);
      displayUpdate = 1;
    }
    if (right.click() or right.step()) {
      flyx = constrain(flyx + 10 / 10, 0, 99999);
      displayUpdate = 1;
    }

    // Кнопка "Назад"
    if (back.click()) return;
  }
}

void drawSignal(float cof, JsonArray arr, int size, int flyx, int centerX) {
  int curx = -flyx + centerX; // Вычисление координаты X с учетом смещения и центра экрана
  int bufx = 0;
  bool dot = 0;
  oled.setCursor(0, 0);
  oled.print((int)(curx + (float)(arr[1].as<int>()) / cof));
  oled.print("  ");
  oled.print(size);
  // for (int i = 0; i < size; i++) {
  //   if (arr[i] > 0) {
  //     bufx += arr[i].as<int>() / cof;
  //     dot = 0;
  //   } else {
  //     bufx -= arr[i].as<int>() / cof;
  //     dot = 1;
  //   }
    
  //   if (bufx > centerX - 64 + flyx) {
  //     oled.fastLineH(32 * dot + 16, curx, bufx - flyx);
  //     curx = bufx - flyx;
  //     oled.fastLineV(curx, 16, 48);
  //   }
  // }

  for (int i = 0; i < size; i++) {
    if (curx > 128) return;
    if (arr[i] > 0) {
      oled.fastLineH(16, curx, curx + (int)(arr[i].as<int>()) / cof);
      curx += (float)((arr[i].as<int>()) / cof);
      oled.fastLineV(curx, 16, 48);
    } else {
      oled.fastLineH(48, curx, curx - (int)(arr[i].as<int>()) / cof);
      curx -= (float)((arr[i].as<int>()) / cof);
      oled.fastLineV(curx, 16, 48);
    }
  }
}

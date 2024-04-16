void viewer(String j) {
  bool displayUpdate = 1;
  JsonDocument rawSignal1;
  readJsonFromFile(objectDoc[j]["RawData"].as<const char*>(), rawSignal1); // Текст null
  int size = sizeof(rawSignal1["RAW"].as<JsonArray>()) / sizeof(rawSignal1["RAW"].as<JsonArray>()[0]);
  
  while(1) {
    int cof = 100;
    static uint8_t pointer = 0;
    tk();
    if (displayUpdate != 0) {
      displayUpdate = 0;
      oled.clear();
      oled.home();
      drawSignal(cof, rawSignal1["RAW"].as<JsonArray>(), size);
      oled.update();
    }

    if (up.click() or up.step()) {
      cof+=10;
      displayUpdate = 1;
    }
    if (down.click() or down.step()) {
      cof-=10;
      displayUpdate = 1;
    }
    if (left.click() or left.step()) {
      ;;
    }
    if (right.click() or right.click()) {
      ;;
    }
    if (back.click()) return;
  }
}

void drawSignal(uint16_t cof, JsonArray arr, int size) {
  int curx = 0;
  oled.setCursor(0, 0);
  oled.print(11);
  ooed.print(arr[1].as<int>());
  for (int i = 0; i < size; i++) {
    if (arr[i] > 0) {
      oled.fastLineH(16, curx, curx + arr[i].as<int>()*(1/cof));
      curx += + arr[i].as<int>()*(1/cof);
    } else {
      oled.fastLineH(48, curx, curx - arr[i].as<int>()*(1/cof));
      curx -= arr[i].as<int>()*(1/cof);
    }
  }
}
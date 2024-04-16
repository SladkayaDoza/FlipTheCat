void viewer(String j) {
  bool displayUpdate = 1;
  float cof = 100;
  JsonDocument rawSignal1;
  readJsonFromFile(objectDoc[j]["RawData"].as<const char*>(), rawSignal1); // Текст null
  int size = rawSignal1["RAW"].as<JsonArray>().size();;
  
  while(1) {
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

void drawSignal(float cof, JsonArray arr, int size) {
  int curx = 0;
  oled.setCursor(0, 0);
  oled.print((int)(curx + (float)(arr[1].as<int>())/cof));
  oled.print(size);
  for (int i = 0; i < size; i++) {
    if (curx > 128) return;
    if (arr[i] > 0) {
      oled.fastLineH(16, curx, (int)(curx + (float)(arr[i].as<int>())/cof));
      curx += (float)((arr[i].as<int>())/cof);
    } else {
      oled.fastLineH(48, curx, (int)(curx - (float)(arr[i].as<int>())/cof));
      curx -= (float)((arr[i].as<int>())/cof);
    }
  }
}
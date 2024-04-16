void viewer(String j) {  // Ряжанка с яблоком, Злагода
  bool displayUpdate = 1;
  float cof = 100;
  int flyx = 0;
  JsonDocument rawSignal1;
  readJsonFromFile(objectDoc[j]["RawData"].as<const char*>(), rawSignal1);
  int size = rawSignal1["RAW"].as<JsonArray>().size();;
  
  while(1) {
    static uint8_t pointer = 0;
    tk();
    if (displayUpdate != 0) {
      displayUpdate = 0;
      oled.clear();
      oled.home();
      drawSignal(cof, rawSignal1["RAW"].as<JsonArray>(), size, flyx);
      oled.update();
    }

    if (up.click() or up.step()) {
      cof-=10;
      displayUpdate = 1;
    }
    if (down.click() or down.step()) {
      cof+=10;
      displayUpdate = 1;
    }
    if (left.click() or left.step()) {
      flyx = constrain(flyx-cof/10, 0, 99999);
      displayUpdate = 1;
    }
    if (right.click() or right.step()) {
      flyx = constrain(flyx+cof/10, 0, 99999);
      displayUpdate = 1;
    }
    if (back.click()) return;
  }
}

void drawSignal(float cof, JsonArray arr, int size, int flyx) {
  int curx = -flyx;
  oled.setCursor(0, 0);
  oled.print((int)(curx + (float)(arr[1].as<int>())/cof));
  oled.print(size);
  for (int i = 0; i < size; i++) {
    if (curx > 128) return;
    if (arr[i] > 0) {
      oled.fastLineH(16, curx, curx + (int)(arr[i].as<int>())/cof);
      curx += (float)((arr[i].as<int>())/cof);
      oled.fastLineV(curx, 16, 48);
    } else {
      oled.fastLineH(48, curx, curx - (int)(arr[i].as<int>())/cof);
      curx -= (float)((arr[i].as<int>())/cof);
      oled.fastLineV(curx, 16, 48);
    }
  }
}
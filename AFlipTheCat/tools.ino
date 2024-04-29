// Keyboard
char* symbols = "abcdefghijklmnopqrstuvwxyz!_,. <>";

String setName(char* title) {
  static uint8_t pointerX = 0;
  static uint8_t pointerY = 0;
  String outValue = "";

  bool updOled = true;
  tk();
  while (1) {
    tk();
    if (updOled) {
      updOled = false;
      oled.clear();
      oled.home();
      oled.print(title);
      oled.print(": ");
      oled.print(outValue);
      byte col = 0;
      while (col <= 3) {
        oled.setCursor(0, col * 2 + 2);
        byte stroke = 0;
        while (stroke <= 10) {
          oled.print(symbols[col * 11 + stroke]);
          oled.print(" ");
          stroke++;
        }
        col++;
      }

      // Cursor
      oled.setCursor(pointerX * 6 * 2, pointerY * 2 + 3);
      oled.print("^");

      oled.update();
    }

    if (ok.click()) {
      bool systemCode = false;
      if (pointerY == 2 && pointerX == 10) {
        return outValue;
      }
      if (pointerY == 2 && pointerX == 9) {
        systemCode = true;
        outValue = outValue.substring(0, outValue.length() - 1);
      }
      if (!systemCode) {
        outValue += symbols[pointerY * 11 + pointerX];
      }
      updOled = true;
    }
    if (back.click()) {
      // if (defa != "0") {
      //   return String(defa);
      // }
      return String("tmp - ") + random(100000, 999999);
    }
    if (up.click() or up.step()) {
      pointerY = constrain(pointerY - 1, 0, 2);
      updOled = true;
    }
    if (down.click() or down.step()) {
      pointerY = constrain(pointerY + 1, 0, 2);
      updOled = true;
    }
    if (left.click() or left.step()) {
      if (pointerX == 0) pointerX = 10;
      else pointerX = constrain(pointerX - 1, 0, 10);
      updOled = true;
    }
    if (right.click() or right.step()) {
      if (pointerX == 10) pointerX = 0;
      else pointerX = constrain(pointerX + 1, 0, 10);
      updOled = true;
    }
  }
}

int countNumber(int cnt) {
  if (cnt == 0) return 1;
  int n = 0;
  while (cnt > 0) {
    cnt /= 10;
    n++;
  }
  return n;
}

int setNumber(char* title, int count) {
  static uint8_t pointer = 0;
  tk();
  bool updOled = true;
  int outCnt = count;
  int n = countNumber(outCnt);

  while (1) {
    tk();
    if (updOled) {
      updOled = false;
      oled.clear();
      oled.setCursor(0, 0);
      oled.print(title);
      oled.print(": ");

      oled.setCursor(0, 4);
      oled.setScale(2);
      oled.print(outCnt);
      oled.setCursor(pointer * 12, 2);
      oled.print("_");
      oled.setScale(1);

      oled.update();
    }
    if (ok.click()) {
      return outCnt;
    }
    if (back.click()) {
      return count;
    }
    if (up.click() or up.step()) {
      if (outCnt == 0) {
        outCnt++;
        n = countNumber(outCnt);
        updOled = true;
      } else {
        outCnt += pow(10, n - pointer - 1);
        n = countNumber(outCnt);
        updOled = true;
      }
    }
    if (down.click() or down.step()) {
      outCnt -= pow(10, n - pointer - 1);
      n = countNumber(outCnt);
      updOled = true;
    }
    if (left.click()) {
      pointer = constrain(pointer - 1, 0, n - 1);
      updOled = true;
    }
    if (right.click()) {
      pointer = constrain(pointer + 1, 0, n - 1);
      updOled = true;
    }
  }
}
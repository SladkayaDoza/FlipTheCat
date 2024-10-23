#include <Arduino.h>
#include "game2048.h"
#include "gamecat.h"
#include <pointer.h>
#include <tick.h>
#include <global_vars.h>


// GAMES
Game2048 game;
GameCat gameCat;

const char* game_lay[8] = {
  "2048",
  "Cat",
  "",
  "",
  "",
  "",
  "",
  ""
};

void games() {
  bool updDisplay = 1;
  tk();
  while (1) {
    static int pointer = 0;
    tk();
    if (updDisplay) {
      updDisplay = 0;
      oled.clear();
      oled.home();
      for (uint8_t i = 0; i < 8; i++) {
        oled.setCursor(14, i);
        oled.print(game_lay[i]);
      }

      printPointer(pointer);
      oled.update();
    }

    if (up.click() or up.step()) {
      pointer = constrain(pointer - 1, 0, ITEMS - 1);
      updDisplay = 1;
    }
    if (down.click() or down.step()) {
      pointer = constrain(pointer + 1, 0, ITEMS - 1);
      updDisplay = 1;
    }
    if (back.click() or back.hold()) break;

    if (ok.click() or ok.hold()) {
      switch (pointer) {
        case 0: game.init(); game.run(); break;
        case 1: gameCat.init(); gameCat.run(); break;
      }
      updDisplay = 1;
    }
  }
}
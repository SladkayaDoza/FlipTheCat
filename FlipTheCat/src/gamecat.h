#pragma once
#include <Arduino.h>
#include <display.h>
#include <tick.h>
#include <button.h>
#include <tiles.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define boardSize 4

class GameCat {
public:
  GameCat();
  void init();
  void run();

private:
  int board[boardSize][boardSize];
  int score;
  bool updDisplay;
  bool out;
  uint8_t userX = 0;
  uint8_t userY = 0;

  void resetGame();
  void printBoard();
  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();
};

GameCat::GameCat(): score(0), updDisplay(true), out(false) {}

void GameCat::resetGame() {
  score = 0;
  updDisplay = true;
  userX = 0;
  userY = 0;
}

void GameCat::init() {
  resetGame();
}

void GameCat::run() {
  while (1) {
    tk();
    if (updDisplay) {
      updDisplay = false;
      oled.clear();
      oled.home();
      printBoard();
      oled.update();
    }

    if (down.click() || down.step()) {
      moveDown();
      updDisplay = true;
    }
    if (up.click() || up.step()) {
      moveUp();
      updDisplay = true;
    }
    if (right.click() || right.step()) {
      moveRight();
      updDisplay = true;
    }
    if (left.click() || left.step()) {
      moveLeft();
      updDisplay = true;
    }
    if (back.click() || back.hold()) {
      out = true;
      break;
    }
  }

  if (!out) {
    delay(2500);
  }

  score = 5;
  updDisplay = true;
  while (true) {
    tk();
    if (updDisplay) {
      updDisplay = false;
      oled.clear();
      oled.home();
      oled.print("Game Over!");
      oled.setCursor(0, 1);
      oled.print("Score: ");
      oled.print(score);
      oled.update();
    }
    if (back.click() || back.hold()) break;
    if (ok.click() || ok.hold()) {
      resetGame();
      run();
      return;
    }
  }
}

void GameCat::moveLeft() {
  userX = constrain(userX - 1, 0, 15);
}

void GameCat::moveRight() {
  userX = constrain(userX + 1, 0, 15);
}

void GameCat::moveUp() {
  userY = constrain(userY - 1, 0, 7);
}

void GameCat::moveDown() {
  userY = constrain(userY + 1, 0, 7);
}

void GameCat::printBoard() {
  oled.drawBitmap(userX*8, userY*8, pix_cat_8x8, 8, 8);
}
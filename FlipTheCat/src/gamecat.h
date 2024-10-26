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
  int score;
  bool updDisplay;
  bool out;
  uint16_t board[16][8];
  uint8_t userX = 0;
  uint8_t userY = 0;
  uint8_t boxX = 2;
  uint8_t boxY = 2;

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
  boxX = 2;
  boxY = 2;
  
  // Reset the board array
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 8; j++) {
      board[i][j] = 0;
    }
  }
  // Create 5 random points for walls on the board
  for (int i = 0; i < 15; i++) {
    int wallX, wallY;
    do {
      wallX = random(0, 16);
      wallY = random(0, 8);
    } while (board[wallX][wallY] != 0 || (wallX == userX && wallY == userY) || (wallX == boxX && wallY == boxY));
    board[wallX][wallY] = 1; // 1 represents a wall
  }
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
  int newUserX = constrain(userX - 1, 0, 15);
  if (board[userY][newUserX] != 1) {  // Check if the new position is not a wall
    if (newUserX == boxX && userY == boxY) {
      if (boxX > 0 && board[boxY][boxX - 1] != 1) {  // Check if the box can be moved and not into a wall
        boxX -= 1;
        userX = newUserX;
      }
    } else {
      userX = newUserX;
    }
  }
}

void GameCat::moveRight() {
  int newUserX = constrain(userX + 1, 0, 15);
  if (board[userY][newUserX] != 1) {  // Check if the new position is not a wall
    if (newUserX == boxX && userY == boxY) {
      if (boxX < 15 && board[boxY][boxX + 1] != 1) {  // Check if the box can be moved and not into a wall
        boxX += 1;
        userX = newUserX;
      }
    } else {
      userX = newUserX;
    }
  }
}

void GameCat::moveUp() {
  int newUserY = constrain(userY - 1, 0, 7);
  if (board[newUserY][userX] != 1) {  // Check if the new position is not a wall
    if (newUserY == boxY && userX == boxX) {
      if (boxY > 0 && board[boxY - 1][boxX] != 1) {  // Check if the box can be moved and not into a wall
        boxY -= 1;
        userY = newUserY;
      }
    } else {
      userY = newUserY;
    }
  }
}

void GameCat::moveDown() {
  int newUserY = constrain(userY + 1, 0, 7);
  if (board[newUserY][userX] != 1) {  // Check if the new position is not a wall
    if (newUserY == boxY && userX == boxX) {
      if (boxY < 7 && board[boxY + 1][boxX] != 1) {  // Check if the box can be moved and not into a wall
        boxY += 1;
        userY = newUserY;
      }
    } else {
      userY = newUserY;
    }
  }
}

void GameCat::printBoard() {
  oled.drawBitmap(userX*8, userY*8, pix_cat_8x8, 8, 8);
  oled.drawBitmap(boxX*8, boxY*8, pix_box_8x8, 8, 8);
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 16; x++) {
      if (board[y][x] == 1) {
        oled.drawBitmap(x*8, y*8, pix_wall_8x8, 8, 8);
      }
    }
  }
}
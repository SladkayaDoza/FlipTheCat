#pragma once
#include <Arduino.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define boardSize 4

extern void tk();

class Game2048 {
public:
  Game2048();
  void init();
  void run();
  void draw();
  void changeDirection(int newDirection);

private:
  int board[boardSize][boardSize];
  int score;
  bool updDisplay;
  bool out;

  void resetGame();
  void update();
  void printBoard();
  int countTiles();
  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();
  bool canMove();
  void addRandomTile();
  void merge(int row[boardSize]);
};

Game2048::Game2048():
  score(0), updDisplay(true), out(false) {
  memset(board, 0, sizeof(board));
}

void Game2048::init() {
  resetGame();
  addRandomTile();
  addRandomTile();
}

void Game2048::run() {
  while (canMove()) {
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
      addRandomTile();
      updDisplay = true;
    }
    if (up.click() || up.step()) {
      moveUp();
      addRandomTile();
      updDisplay = true;
    }
    if (right.click() || right.step()) {
      moveRight();
      addRandomTile();
      updDisplay = true;
    }
    if (left.click() || left.step()) {
      moveLeft();
      addRandomTile();
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

  score = countTiles();
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

void Game2048::resetGame() {
  memset(board, 0, sizeof(board));
  score = 0;
  updDisplay = true;
  out = false;
  addRandomTile();
  addRandomTile();
}

void Game2048::printBoard() {
  for (int i = 0; i < boardSize; ++i) {
    for (int j = 0; j < boardSize; ++j) {
      oled.setCursor(j * 20, i * 2);
      if (board[i][j]) {
        oled.print(board[i][j]);
      }
    }
  }
}

int Game2048::countTiles() {
  int count = 0;
  for (int i = 0; i < boardSize; ++i) {
    for (int j = 0; j < boardSize; ++j) {
      if (board[i][j] != 0) {
        count += board[i][j];
      }
    }
  }
  return count;
}

void Game2048::moveLeft() {
  for (int i = 0; i < boardSize; ++i) {
    merge(board[i]);
  }
}

void Game2048::moveRight() {
  for (int i = 0; i < boardSize; ++i) {
    int reversedRow[boardSize];
    for (int j = 0; j < boardSize; ++j) {
      reversedRow[j] = board[i][boardSize - j - 1];
    }
    merge(reversedRow);
    for (int j = 0; j < boardSize; ++j) {
      board[i][j] = reversedRow[boardSize - j - 1];
    }
  }
}

void Game2048::moveUp() {
  for (int j = 0; j < boardSize; ++j) {
    int column[boardSize];
    for (int i = 0; i < boardSize; ++i) {
      column[i] = board[i][j];
    }
    merge(column);
    for (int i = 0; i < boardSize; ++i) {
      board[i][j] = column[i];
    }
  }
}

void Game2048::moveDown() {
  for (int j = 0; j < boardSize; ++j) {
    int reversedColumn[boardSize];
    for (int i = 0; i < boardSize; ++i) {
      reversedColumn[i] = board[boardSize - i - 1][j];
    }
    merge(reversedColumn);
    for (int i = 0; i < boardSize; ++i) {
      board[i][j] = reversedColumn[boardSize - i - 1];
    }
  }
}

bool Game2048::canMove() {
  for (int i = 0; i < boardSize; ++i) {
    for (int j = 0; j < boardSize; ++j) {
      if (board[i][j] == 0) {
        return true;  // Есть пустые ячейки, можно продолжить ходить
      }
      if (i > 0 && board[i][j] == board[i - 1][j]) {
        return true;  // Есть возможность объединения элементов по вертикали
      }
      if (j > 0 && board[i][j] == board[i][j - 1]) {
        return true;  // Есть возможность объединения элементов по горизонтали
      }
    }
  }
  return false;  // Нельзя сделать ни одного хода
}

void Game2048::addRandomTile() {
  int emptyTiles[boardSize * boardSize][2];
  int emptyCount = 0;

  for (int i = 0; i < boardSize; ++i) {
    for (int j = 0; j < boardSize; ++j) {
      if (board[i][j] == 0) {
        emptyTiles[emptyCount][0] = i;
        emptyTiles[emptyCount][1] = j;
        emptyCount++;
      }
    }
  }

  if (emptyCount > 0) {
    int randomIndex = random(emptyCount);
    int randomValue = random(1) > 0.5 ? 2 : 4;
    board[emptyTiles[randomIndex][0]][emptyTiles[randomIndex][1]] = randomValue;
  }
}

void Game2048::merge(int row[boardSize]) {
  int mergedRow[boardSize] = { 0 };
  int index = 0;

  for (int i = 0; i < boardSize; ++i) {
    int value = row[i];
    if (value != 0) {
      if (mergedRow[index] == 0) {
        mergedRow[index] = value;
      } else if (mergedRow[index] == value) {
        mergedRow[index] *= 2;
        index += 1;
      } else {
        index += 1;
        mergedRow[index] = value;
      }
    }
  }

  memcpy(row, mergedRow, boardSize * sizeof(int));
}
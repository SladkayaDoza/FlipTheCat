char* game_lay[8] = {
  "2048",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
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
        case 0: game2048(); break;
      }
      updDisplay = 1;
    }
  }
}

void printBoard(int board[boardSize][boardSize]) {
  for (int i = 0; i < boardSize; ++i) {
    for (int j = 0; j < boardSize; ++j) {
      oled.setCursor(j * 20, i * 2);
      if (board[i][j]) {
        oled.print(board[i][j]);
      }
    }
    // Serial.println();
  }
}

void game2048() {
  bool updDisplay = 1;
  bool out = 0;
  int score = 0;
  tk();
  int board[boardSize][boardSize] = {
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 }
  };
  addRandomTile(board);
  addRandomTile(board);

  while (canMove(board)) {
    tk();
    if (updDisplay) {
      updDisplay = 0;
      oled.clear();
      oled.home();
      printBoard(board);
      oled.update();
    }

    if (down.click() or down.step()) {
      moveDown(board);
      addRandomTile(board);
      updDisplay = 1;
    }
    if (up.click() or up.step()) {
      moveUp(board);
      addRandomTile(board);
      updDisplay = 1;
    }
    if (right.click() or right.step()) {
      moveRight(board);
      addRandomTile(board);
      updDisplay = 1;
    }
    if (left.click() or left.step()) {
      moveLeft(board);
      addRandomTile(board);
      updDisplay = 1;
    }
    if (back.click() or back.hold()) {
      out = 1;
      break;
    }
  }
  if (!out) {
    delay(2500);
  }

  score = countTiles(board);
  updDisplay = 1;
  while (1) {
    tk();
    if (updDisplay) {
      updDisplay = 0;
      oled.clear();
      oled.home();
      oled.print("Game Over!");
      oled.setCursor(0, 1);
      oled.print("Score: ");
      oled.print(score);
      oled.update();
    }
    if (back.click() or back.hold()) break;
    if (ok.click() or ok.hold()) {
      game2048();
      return;
    }
  }
}

int countTiles(int board[boardSize][boardSize]) {
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

void moveLeft(int board[boardSize][boardSize]) {
  for (int i = 0; i < boardSize; ++i) {
    merge(board[i]);
  }
}

void moveRight(int board[boardSize][boardSize]) {
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

void moveUp(int board[boardSize][boardSize]) {
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

void moveDown(int board[boardSize][boardSize]) {
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

bool canMove(int board[boardSize][boardSize]) {
  for (int i = 0; i < boardSize; ++i) {
    for (int j = 0; j < boardSize; ++j) {
      if (board[i][j] == 0) {
        return true;  // Есть пустые ячейки, можно продолжить ходить
      }
      if (i < boardSize - 1 && board[i][j] == board[i + 1][j]) {
        return true;  // Есть возможность объединения элементов по вертикали
      }
      if (j < boardSize - 1 && board[i][j] == board[i][j + 1]) {
        return true;  // Есть возможность объединения элементов по горизонтали
      }
    }
  }
  return false;  // Нельзя сделать ни одного хода
}

void addRandomTile(int board[boardSize][boardSize]) {
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

void merge(int row[boardSize]) {
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
#include <Arduino.h>
#include "game2048.h"
#include "gamecat.h"
#include "game_space_impact.h"
#include <menu.h>

// GAMES
Game2048 game;
GameCat gameCat;
GameSpaceImpact gameSpaceImpact;

// Обёртки init()+run() под сигнатуру MenuItem::run (void()).
static void run2048()        { game.init(); game.run(); }
static void runCat()         { gameCat.init(); gameCat.run(); }
static void runSpaceImpact() { gameSpaceImpact.init(); gameSpaceImpact.run(); }

static const MenuItem gamesMenu[] = {
  {"2048", run2048},
  {"Cat", runCat},
  {"Space Impact", runSpaceImpact},
};
static const uint8_t gamesMenuCount = sizeof(gamesMenu) / sizeof(gamesMenu[0]);

void games() {
  // Курсор ограничен реальным числом игр (3) — раньше можно было встать
  // на 5 пустых строк, где OK ничего не делал.
  runMenu(gamesMenu, gamesMenuCount);
}

#pragma once
#include <Arduino.h>
#include <display.h>
#include <tick.h>
#include <button.h>
#include "si_engine.h"

#define SI_OLED_OFFSET_X 22
#define SI_OLED_OFFSET_Y 8

class GameSpaceImpact {
public:
    void init() {
        SI_UncompressFont();
        SI_UncompressObjects();
        memset(TopScores, 0, sizeof(TopScores));
        SavedLevel = 0;
        SI_ReadSavedLevel(&SavedLevel);
        SI_ReadTopScore(TopScores);
        LevelCount = 0;
        const Uint8 *ll = si_getLevelData(0);
        while (ll) { ++LevelCount; ll = si_getLevelData(LevelCount); }
    }

    void run() {
        Uint8 running = 1;
        Uint8 IntroPhase = 12;
        Uint8 FrameHold = 3;
        SI_PlayerObject Player;
        Sint8 Level = SI_MENU_SCREEN_MAIN;
        Uint8 MenuItem = 1;
        Uint8 PlayerShootTimer = 0;
        SI_Shot *Shots = NULL;
        SI_EnemyList *Enemies = NULL;
        Uint8 AnimPulse = 0;
        SI_Scenery *Scene = NULL;
        Uint8 MoveScene = 1;
        #ifdef LEGACY_TOP_SCORE
        Uint8 TimeInScores = 0;
        #endif
        unsigned long lastFrame = 0;
        const unsigned long frameInterval = 1000 / SI_FRAMERATE;

        memset(PixelMap, 0, sizeof(PixelMap));

        while (running) {
            // === POLL BUTTONS AS FAST AS POSSIBLE ===
            tk();

            // Discrete events (menus, bonus weapon, ESC)
            if (back.hold()) {
                if (IntroPhase) { IntroPhase = 0; }
                else if (Level == SI_MENU_SCREEN_PAUSE) { Level = SI_MENU_SCREEN_MAIN; }
                else if (Level == SI_MENU_SCREEN_HIGH_SCORE || Level == LevelCount) { Level = SI_MENU_SCREEN_MAIN; }
                else if (Level == SI_MENU_SCREEN_MAIN) { running = 0; }
                else if (Level >= 0) {
                    SavedLevel = Level; SI_SaveLevel(Level);
                    #ifdef SI_PAUSE
                    Level = SI_MENU_SCREEN_PAUSE; MenuItem = 1;
                    #else
                    Level = SI_MENU_SCREEN_MAIN; MenuItem = 1;
                    #endif
                }
            }
            if (top_right.click()) {
                if (Level >= 0 && Level < LevelCount && Player.Bonus) {
                    SI_AddShot(&Shots, SI_NewVec2(Player.Pos.x+9, Player.Weapon==SI_Wall?5:Player.Pos.y+2), Player.Weapon==SI_Beam?0:2, 1, Player.Weapon);
                    --Player.Bonus;
                }
            }

            // Menu navigation — click-based (discrete)
            if (IntroPhase) {
                if (up.click() || down.click() || left.click() || right.click() || ok.click()) IntroPhase = 0;
            }
            #ifdef SI_PAUSE
            else if (Level == SI_MENU_SCREEN_PAUSE) {
                if (ok.click()) Level = MenuItem == 1 ? SavedLevel : SI_MENU_SCREEN_MAIN;
                if (up.click() || down.click()) MenuItem = 3 - MenuItem;
            }
            #endif
            else if (Level == SI_MENU_SCREEN_HIGH_SCORE || Level == LevelCount) {
                // No action, back.hold() handles exit
            }
            else if (Level == SI_MENU_SCREEN_MAIN) {
                if (up.click()) MenuItem = MenuItem == 1 ? (SavedLevel ? 3 : 2) : (MenuItem - 1);
                if (down.click()) MenuItem = MenuItem % (SavedLevel ? 3 : 2) + 1;
                if (ok.click()) {
                    Uint8 mi = MenuItem;
                    if (SavedLevel == 0) mi++;
                    if (mi == 3) {
                        Level = SI_MENU_SCREEN_HIGH_SCORE;
                        #ifdef LEGACY_TOP_SCORE
                        TimeInScores = 0;
                        #endif
                        if (SavedLevel == 0) MenuItem = 2;
                    } else {
                        if (mi == 1) Level = SavedLevel; else Level = 0;
                        SI_EmptyShotList(&Shots);
                        Player.Lives = 3; Player.Score = 0; Player.Bonus = 3; Player.Weapon = SI_Missile;
                        Player.Pos = SI_NewVec2(3, 20); Player.Protection = 50; PlayerShootTimer = 0;
                        SI_LevelSpawner(&Enemies, Level);
                        SI_EmptyScenery(&Scene); MoveScene = 1;
                    }
                }
            }

            // === FRAME TICK (only at framerate intervals) ===
            unsigned long now = millis();
            if (now - lastFrame < frameInterval) { delay(1); continue; }  // отдаём CPU и кормим watchdog между кадрами
            lastFrame = now;

            // Read movement state from pressing() — continuous hold
            Uint8 PlayerUp = up.pressing();
            Uint8 PlayerDown = down.pressing();
            Uint8 PlayerLeft = left.pressing();
            Uint8 PlayerRight = right.pressing();
            Uint8 PlayerShooting = top_left.pressing();

            // === FRAME RENDERING ===
            memset(PixelMap, 0, sizeof(PixelMap));

            if (IntroPhase) {
                SI_DrawObject(PixelMap, SI_GetObject(SI_gSpace), SI_NewVec2(8, 12 - IntroPhase));
                SI_DrawObject(PixelMap, SI_GetObject(SI_gImpact), SI_NewVec2(4, 24 + IntroPhase));
                SI_DrawOutlinedObject(PixelMap, SI_GetObject(SI_gIntro), SI_NewVec2(56 - IntroPhase * 4, 20));
                if (FrameHold) { --FrameHold; if (!FrameHold) FrameHold = --IntroPhase == 1 ? SI_FRAMERATE : 2; }
            } else if (Level == LevelCount) {
                char ScoreText[12];  // хватает на любой unsigned int (раньше [6] мог переполниться от битого сейва) itoa(Player.Score, ScoreText, 10);
                SI_DrawText(PixelMap, "Game over\nYour score:", SI_NewVec2(1, 1), 9);
                SI_DrawText(PixelMap, ScoreText, SI_NewVec2(1, 19), 0);
            #ifdef SI_PAUSE
            } else if (Level == SI_MENU_SCREEN_PAUSE) {
                SI_DrawSmallNumber(PixelMap, 8, 1, SI_NewVec2(57, 0)); SI_DrawObject(PixelMap, SI_GetObject(SI_gShot), SI_NewVec2(61, 2));
                SI_DrawSmallNumber(PixelMap, 2, 1, SI_NewVec2(65, 0)); SI_DrawObject(PixelMap, SI_GetObject(SI_gShot), SI_NewVec2(69, 2));
                SI_DrawSmallNumber(PixelMap, 1, 1, SI_NewVec2(73, 0)); SI_DrawObject(PixelMap, SI_GetObject(SI_gShot), SI_NewVec2(77, 2));
                SI_DrawSmallNumber(PixelMap, SI_gNum0 + MenuItem, 1, SI_NewVec2(81, 0));
                SI_DrawText(PixelMap, "Continue\nExit", SI_NewVec2(1, 7), 11);
                SI_InvertScreenPart(PixelMap, SI_NewVec2(0, MenuItem * 11 - 5), SI_NewVec2(76, MenuItem * 11 + 5));
                SI_DrawText(PixelMap, "Select", SI_NewVec2(24, 40), 0);
                SI_DrawScrollBar(PixelMap, (MenuItem - 1) * 100);
            #endif
            } else if (Level == SI_MENU_SCREEN_HIGH_SCORE) {
                #ifdef LEGACY_TOP_SCORE
                char ScoreText[12];  // хватает на любой unsigned int (раньше [6] мог переполниться от битого сейва) itoa(TopScores[0], ScoreText, 10);
                SI_DrawText(PixelMap, "Top score:", SI_NewVec2(1, 1), 0);
                SI_DrawText(PixelMap, ScoreText, SI_NewVec2(1, 11), 0);
                for (int i = 0; i < 4; ++i) for (int j = 0; j < 6; ++j) {
                    SI_Vec2 Pos = SI_NewVec2(64 + i * 5, 1 + j * 4);
                    if (TimeInScores / 3 - (4 - i) - j >= 0 && TimeInScores / 3 - (4 - i) - j <= 1)
                        SI_DrawObject(PixelMap, SI_GetObject(TimeInScores / 3 - (4 - i) - j ? SI_gDotFull : SI_gDotEmpty), Pos);
                    else if (TimeInScores / 3 - (4 - i) - j > 1) {
                        const Uint8 OneSign[24] = {0,0,1,0,0,1,1,0,1,1,1,0,0,1,1,0,0,1,1,0,1,1,1,1};
                        if (OneSign[j * 4 + i]) SI_DrawObject(PixelMap, SI_GetObject(TimeInScores < 45 ? SI_gDotEmpty : SI_gDotFull), Pos);
                    }
                }
                if (++TimeInScores == 54) TimeInScores = 35;
                #else
                SI_Vec2 Pos = {3, 3};
                for (int i = 0; i < SI_SCORE_COUNT; i++) {
                    if (i == 5) Pos = SI_NewVec2(47, 3);
                    SI_DrawSmallNumber(PixelMap, i + 1, i == 9 ? 2 : 1, Pos);
                    SI_DrawObject(PixelMap, SI_GetObject(SI_gShot), SI_NewVec2(Pos.x + 4, Pos.y + 2));
                    SI_DrawSmallNumber(PixelMap, TopScores[i], 5, SI_NewVec2(Pos.x + 24, Pos.y));
                    Pos.y += 9;
                }
                #endif
            } else if (Level == SI_MENU_SCREEN_MAIN) {
                SI_DrawSmallNumber(PixelMap, 8, 1, SI_NewVec2(65, 0)); SI_DrawObject(PixelMap, SI_GetObject(SI_gShot), SI_NewVec2(69, 2));
                SI_DrawSmallNumber(PixelMap, 2, 1, SI_NewVec2(73, 0)); SI_DrawObject(PixelMap, SI_GetObject(SI_gShot), SI_NewVec2(77, 2));
                SI_DrawSmallNumber(PixelMap, SI_gNum0 + MenuItem, 1, SI_NewVec2(81, 0));
                SI_DrawText(PixelMap, SavedLevel ? "Continue\nNew game\nTop score" : "New game\nTop score", SI_NewVec2(1, 7), 11);
                SI_InvertScreenPart(PixelMap, SI_NewVec2(0, MenuItem * 11 - 5), SI_NewVec2(76, MenuItem * 11 + 5));
                SI_DrawText(PixelMap, "Select", SI_NewVec2(24, 40), 0);
                SI_DrawScrollBar(PixelMap, (MenuItem - 1) * (SavedLevel ? 50 : 100));
            } else {
                // === GAMEPLAY ===
                Uint8 NonInverseLevel = Level < 4 || 5 < Level;
                Sint16 BarTop = NonInverseLevel ? 0 : 43;
                Uint8 StartLives = Player.Lives;
                #ifdef ZEROTH_LIFE
                for (int i = 0; i < Player.Lives - 1; ++i)
                #else
                for (int i = 0; i < Player.Lives; ++i)
                #endif
                    SI_DrawObject(PixelMap, SI_GetObject(SI_gLife), SI_NewVec2(i * 6, BarTop));
                SI_DrawObject(PixelMap, SI_GetObject((SI_Graphics)(SI_gLife + Player.Weapon)), SI_NewVec2(33, BarTop));
                SI_DrawSmallNumber(PixelMap, Player.Bonus, 2, SI_NewVec2(43, BarTop));
                SI_DrawSmallNumber(PixelMap, Player.Score, 5, SI_NewVec2(71, BarTop));
                SI_ShotListTick(&Shots, PixelMap, &Player);
                if (Enemies) {
                    if (PlayerLeft && Player.Pos.x > (Player.Protection ? 2 : 0)) --Player.Pos.x;
                    if (PlayerRight && Player.Pos.x < 74) ++Player.Pos.x;
                    if (PlayerUp && Player.Pos.y > NonInverseLevel * 5 + (Player.Protection ? 2 : 0)) --Player.Pos.y;
                    if (PlayerDown && Player.Pos.y < 36 + NonInverseLevel * 5 - (Player.Protection ? 2 : 0)) ++Player.Pos.y;
                } else {
                    SI_EmptyShotList(&Shots);
                    if (Player.Pos.x > 84) {
                        Player.Pos = SI_NewVec2(3, 20); PlayerShootTimer = 0;
                        SI_LevelSpawner(&Enemies, ++Level);
                        if (Level == LevelCount) { SI_PlaceTopScore(TopScores, Player.Score); SavedLevel = 0; SI_SaveLevel(SavedLevel); }
                        else SI_EmptyScenery(&Scene);
                        MoveScene = 1;
                    } else {
                        Sint16 OutPos = NonInverseLevel ? 10 : 31;
                        if (Player.Pos.y < OutPos) ++Player.Pos.y;
                        else if (Player.Pos.y > OutPos) --Player.Pos.y;
                        else Player.Pos.x += 3;
                    }
                }
                SI_DrawObject(PixelMap, SI_GetObject(Player.Protection ? SI_G_PROTECTION_A1 + (Player.Protection / 2) % 2 : SI_G_PLAYER),
                    Player.Protection ? SI_NewVec2(Player.Pos.x - 2, Player.Pos.y - 2) : Player.Pos);
                if (PlayerShootTimer) PlayerShootTimer--;
                if (PlayerShooting && PlayerShootTimer == 0) {
                    SI_AddShot(&Shots, SI_NewVec2(Player.Pos.x + 9, Player.Pos.y + 3), 2, 1, SI_Standard);
                    PlayerShootTimer = 4;
                }
                AnimPulse = 1 - AnimPulse;
                SI_EnemyListTick(&Enemies, &Player, PixelMap, &Shots, AnimPulse, &MoveScene);
                SI_HandleScenery(&Scene, PixelMap, MoveScene, &Player, Level);
                if (Level == 0 || !NonInverseLevel) SI_InvertScreen(PixelMap);
                if (Player.Protection) Player.Protection--;
                if (Player.Lives == 0) { SI_PlaceTopScore(TopScores, Player.Score); Level = LevelCount; SavedLevel = 0; SI_SaveLevel(SavedLevel); }
                else if (Player.Lives != StartLives) { Player.Pos = SI_NewVec2(3, 20); Player.Protection = 50; }
            }

            renderToOled();
        }

        // Cleanup
        SI_EmptyEnemyList(&Enemies);
        SI_EmptyScenery(&Scene);
        SI_EmptyShotList(&Shots);
        SI_FreeDynamicGraphics();
        SI_FreeDynamicEnemies();
    }

private:
    Uint8 PixelMap[84 * 48];
    Uint8 SavedLevel = 0;
    Uint8 LevelCount = 0;
    unsigned int TopScores[SI_SCORE_COUNT];

    void renderToOled() {
        oled.clear();
        oled.rect(SI_OLED_OFFSET_X - 2, SI_OLED_OFFSET_Y - 2, SI_OLED_OFFSET_X + 84 + 1, SI_OLED_OFFSET_Y + 48 + 1, OLED_STROKE);
        for (int i = 0; i < 84 * 48; i++) {
            if (PixelMap[i]) oled.dot(SI_OLED_OFFSET_X + (i % 84), SI_OLED_OFFSET_Y + (i / 84));
        }
        oled.update();
    }
};

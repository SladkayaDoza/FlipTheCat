#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// SDL-compatible type aliases (from sdlint.h)
typedef int8_t Sint8;
typedef int16_t Sint16;
typedef int32_t Sint32;
typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;

// Config defines
#define SI_FRAMERATE 20
#define BONUS_COLLIDER
#define LEGACY_TOP_SCORE
#define SI_PAUSE
#define SI_SCORE_COUNT 10

#define SI_MENU_SCREEN_MAIN -1
#define SI_MENU_SCREEN_HIGH_SCORE -2
#define SI_MENU_SCREEN_PAUSE -3

// Weapon types
typedef enum {
    SI_Standard = 0,
    SI_Missile = 1,
    SI_Beam = 2,
    SI_Wall = 3
} SI_WeaponKind;

// Graphics object IDs
typedef enum {
    SI_gNum0 = 0, SI_gNum1, SI_gNum2, SI_gNum3, SI_gNum4, SI_gNum5, SI_gNum6, SI_gNum7, SI_gNum8, SI_gNum9,
    SI_gSpace, SI_gIntro, SI_gImpact, SI_gScrollMark, SI_gDotEmpty, SI_gDotFull,
    SI_gLife,
    SI_gMissileIcon,
    SI_gBeamIcon,
    SI_gWallIcon,
    SI_gShot,
    SI_gExplosionA1, SI_gExplosionA2
} SI_Graphics;

#define SI_G_PROTECTION_A1 (256 + 250)
#define SI_G_PROTECTION_A2 (256 + 251)
#define SI_G_MISSILE (256 + 252)
#define SI_G_BEAM (256 + 253)
#define SI_G_WALL (256 + 254)
#define SI_G_PLAYER (256 + 255)

// Vec2
typedef struct SI_Vec2 {
    Sint16 x, y;
} SI_Vec2;

// Object
typedef struct SI_Object {
    SI_Vec2 Size;
    Uint8 *Samples;
} SI_Object;

// Player
typedef struct SI_PlayerObject {
    SI_Vec2 Pos;
    Uint8 Lives;
    Uint16 Score;
    Uint8 Bonus;
    SI_WeaponKind Weapon;
    Uint8 Protection;
} SI_PlayerObject;

// Enemy definition
typedef struct SI_Enemy {
    Uint16 Model;
    SI_Vec2 Size;
    Uint8 AnimCount;
    Sint8 Lives;
    Uint8 Floats;
    Uint8 ShotTime;
    Uint8 MoveUp;
    Uint8 MoveDown;
    Uint8 MoveAnyway;
    SI_Vec2 MovesBetween;
} SI_Enemy;

// Enemy linked list
typedef struct SI_EnemyList {
    SI_Vec2 Pos;
    SI_Enemy Type;
    Uint8 AnimState;
    Sint8 Lives;
    Sint8 MoveDir;
    Uint8 Cooldown;
    struct SI_EnemyList *Next;
} SI_EnemyList, *SI_EnemyListStart;

// Shot linked list
typedef struct SI_Shot {
    SI_Vec2 Pos;
    Sint8 v;
    Uint8 FromPlayer;
    SI_WeaponKind Kind;
    Uint8 Damage;
    SI_Vec2 Size;
    struct SI_Shot *Next;
} SI_Shot, *SI_ShotList;

// Scenery linked list
typedef struct SI_Scenery {
    Uint16 Model;
    SI_Vec2 Pos;
    struct SI_Scenery *Next;
} SI_Scenery, *SI_SceneryList;

// Scenery data per level
typedef struct SI_SceneryData {
    Uint16 FirstObject;
    Uint8 Objects;
    Uint8 Upper;
} SI_SceneryData;

// Helpers
static inline SI_Vec2 SI_NewVec2(Sint16 x, Sint16 y) {
    SI_Vec2 ret;
    ret.x = x;
    ret.y = y;
    return ret;
}

static inline SI_Object SI_NewObject(SI_Vec2 Size, Uint8* Samples) {
    SI_Object ret;
    ret.Size = Size;
    ret.Samples = Samples;
    return ret;
}

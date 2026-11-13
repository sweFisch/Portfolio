// ruleTile
#pragma once

#include "SDL3/SDL_render.h"
// #include "levels.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <cstddef>
#include <cstdint>

struct LevelData;

struct Walls{
  int xMax = 10;
  int yMax = 10;
  bool* data;// [xMax * yMax]; // bool array of what is wall
  int* renderData;//[xMax * yMax]; // contains calculated tiledata for sprites to use
};


void InitializeWalls();
void ProcessWallData(uint16_t* cells, LevelData* level);
void UpdateRuleTile();
void DrawRuleTile(SDL_Renderer* renderer, Sprite* spriteBuffer, Walls* wallsStruct);


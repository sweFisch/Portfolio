// camera.h
//
#pragma once

#include "levels.h"

struct Camera
{
  float camera_x;
  float camera_y;
};


namespace camera {
  // spcify a position on the game board and get back the acual position in the game window
  void GridToWorld(float* x, float* y, const LevelData* lvl);
  // any point in space and finds what cell this would belong to on the game board - can return positions outside the game board
  void WorldToGrid(float x_world, float y_world, int* x, int* y, const LevelData* lvl);
  // Checks if position is inside the game board
  bool GetIsPointInsideGrid(float x, float y, const LevelData* lvl);

};

// camera.cpp
//
#include "camera.h"
#include "common.h"

// Checks if point is inside map
bool camera::GetIsPointInsideGrid(float x, float y, const LevelData *lvl)
{
  int x_grid;
  int y_grid;
  WorldToGrid(x,y,&x_grid, &y_grid, lvl);
  return x_grid >= 0 && y_grid >= 0 && x_grid < lvl->w && y_grid < lvl->h;
}

// Translate a game board position x,y  to camera pixel position
void camera::GridToWorld(float *x, float *y, const LevelData *lvl)
{
  // we dereference our float pointer to write to its values *x , *y
  *x *= TILE_SIZE_PX_SCALED; // shifting our coordinate into pixel space
  // *x += TILE_SIZE_PX_SCALED; // new
  *x += SCREEN_WIDTH / 2.0f; // moving it to the center of the screen
  *x -= lvl->w * TILE_SIZE_PX_SCALED / 2.0f; // moving it back halft the game world width so everything is in the center of the camera

  *y *= TILE_SIZE_PX_SCALED;
  // *y += TILE_SIZE_PX_SCALED; // new
  *y += SCREEN_HEIGHT / 2.0f;
  *y -= lvl->h * TILE_SIZE_PX_SCALED / 2.0f;
}


void camera::WorldToGrid(float x_world, float y_world, int *x, int *y, const LevelData *lvl)
{
  // Dereference pointer to access data  and set it to the incoming floats
  *x = x_world;
  *y = y_world;

  // Do the reverse from GridToWorld - translating the screen pixel to a coordinate in game world
  *x += lvl->w * TILE_SIZE_PX_SCALED / 2.0f;
  *x -= SCREEN_WIDTH / 2.0f;
  // *x -= TILE_SIZE_PX_SCALED; // new
  *x /= TILE_SIZE_PX_SCALED;
  
  *y += lvl->h * TILE_SIZE_PX_SCALED / 2.0f;
  *y -= SCREEN_HEIGHT / 2.0f;
  // *y -= TILE_SIZE_PX_SCALED; // new
  *y /= TILE_SIZE_PX_SCALED;
}

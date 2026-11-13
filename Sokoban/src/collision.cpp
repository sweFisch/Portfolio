#include "collision.h"


bool CheckCollisionInsideBounds(SDL_FRect bounds, float x, float y)
{
  // Check inside bounds. return false if any bound check fails
  if (x > bounds.x + bounds.w) return false;
  if (x < bounds.x) return false;
  if (y > bounds.y + bounds.h) return false;
  if (y < bounds.y) return false;
  return true;
}

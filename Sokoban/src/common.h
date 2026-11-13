#pragma once

// helper file containing memory conversion
// Macros for conversion, will replace the defined strings with the text after 'space'
// macros are used as a preprocessor before compiling
#include <cstddef>
#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

constexpr int FPS = 60;
constexpr double FRAME_TIME_MS = (1.0/FPS)*1000;

// constexpr is a value that is not ment to change
// and we know the value at compile time
constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(15);
constexpr size_t AUDIO_MEMORY_ALLOWANCE = MEGABYTES(5); // has to be smaller than game memory as this is a subarena of game
// constexpr size_t GAME_MEMORY_IMAGES= MEGABYTES(4); // IMAGE_ARENA_SIZE defined in main 


static const char STOP_CHAR = '\0';

inline bool IsStringEmpty(const char* str)
{
  return str == nullptr || str[0] == STOP_CHAR;
}


const int SCREEN_WIDTH   = 1920; // 650
const int SCREEN_HEIGHT  = 1080; //400
const int UPSCALE_FACTOR = 4;   // 4 now with new tileset

// Tilemap pixel size
const int TILE_SIZE_PX_RAW = 16;
const int TILE_SIZE_PX_SCALED   = TILE_SIZE_PX_RAW * UPSCALE_FACTOR; // 16 * UPSCALE_FACTOR - depends on the tileset

const float MOVE_SPEED = 8.0f; // (old 6.0f) used for lerp animations

const float UNDO_REPEAT_TIME = 0.15f;
const float KEY_REPEAT_TIME = (1.0f/MOVE_SPEED) + 0.10f; // (1/MOVE_SPEED) * 1.15f - in his code 

// flatIndex is the input index, width is the x count in cell collums, x, y gets modified by pointer reference
// int x = i % level_data->w; // finds the x coord - using modulo on the width gives ut the remainder
// int y = i / level_data->w; // finds the y coord - dividing by width gets how many rows down we are - int rounding down helps us here
inline void Expand1Dto2D(int flatIndex, int width, int* x, int* y)
{
  *x = flatIndex % width;
  *y = flatIndex / width;
}

inline void Expand1Dto2D(int flatIndex, int width, float* x, float* y)
{
  *x = (float)(flatIndex % width);
  *y = (float)(flatIndex / width);
}

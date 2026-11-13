// credits.h

#pragma once

#include "fontLibrary.h"



struct Input;
struct GameData;
struct SDL_Renderer;
struct Sprite;

namespace Memory{
  struct Arena;
}

struct CreditsTextRow{
  const char* text;
  int char_count;
};



struct Credits
{
  bool initialized;
  const char* text;
  // static const int rowsCount = 5;
  // CreditsTextRow creditTextRow[rowsCount];
  float scrollSpeed;
  FontAtlas* font;
  float yPosOffset;
  float* time_in_scene;
  const uint64_t* ticks_total;
  int total_nr_charcters;
  float show_chars_count;
};

void InitializeCredits(Credits* credits, Sprite* spriteBuffer, FontAtlas* font, Memory::Arena* arena_main, const uint64_t* ticks_total, float* time_in_scene);
void UpdateCredits(GameData* data);
void DrawCredits(Credits* credits, SDL_Renderer* renderer, Sprite* spriteBuffert, Input* input);

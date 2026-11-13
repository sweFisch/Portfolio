#pragma once
// gameState.h - struct with all variables in the game

#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_rect.h"
#include "arena.h"
#include "audioSystem.h"
#include "command.h"
#include "entity.h"
#include "fontLibrary.h"
#include "imgui/imgui_internal.h"
#include "levels.h"
#include "input.h"
#include "camera.h"
#include "spriteLibrary.h"
#include "levelEditor.h"
#include "tilesetLibrary.h"
#include "mainmenu.h"
#include "credits.h"
#include <cstdint>

enum class SCENE_TYPES : uint8_t{
  NONE,
  TITLESCREEN,
  MAINMENU,
  GAME,
  CREDITS,
  TEST_RULETILE
};

// Holding level data, commandbuffer  and inputbuffer
struct Gameplay{
  CommandBuffer* commandBuffer;
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;
  // float undo_timer; // ska den här finnas - TODO
  
  // Ring buffer for input key buffering - used for animation - allocated in main
  // using the variables below with a % modulo operator to loop in the buffer
  Position* input_buffer; // entity.h position struct-  Array
  int input_buffer_capacity; // how many positions to store- size of Array
  int input_buffer_write_count; // running tally of the total inputs feed to the array - will grow larger than the array
  int input_buffer_read_count; // read_count will lag behind write count on fast key presses
  // front = input_buffer_read_count % input_buffer_capacity
  
  bool initialized;

  // For active player selection - allowing only one player to move during keypresses
  int activePlayerIndex;
  Entity** activePlayerBuffer;
};





// struct MainMenu - its own file mainmenu.cpp / .h
// {
  
// };


struct TitleScreen{
  
};

// struct Credits{
  
// };

struct Scenes{
  Gameplay gameplay;
  MainMenu mainMenu;
  TitleScreen titleScreen;
  Credits credits;
};


struct Transition {
  enum States {
    Inactive,
    FadeTo,
    FadeFrom
  };
  States state;
  float fade_time_elapsed;
  float fade_time_duration = 1.0f;
};

struct GameData {
  SCENE_TYPES scene_current;
  SCENE_TYPES scene_previous;
  Scenes scenes;
  Transition transition;
  float time_in_scene; // used for timing animations in scenes
  EditorData editor_data;
  AudioSystem audio; // our FMOD implementation
  uint64_t* ticks_total; // all ticks since game start
  bool running;
  // bool edit_level; // TODO check if it should still be here or be moved !? not on page 320 ish during refactor
  
  // --- INPUT DATA - KEYBOARD ---
  Input input; // a struct from input.h - keeping track of keypresses and time for each key
  Sprite* spriteBuffer; // holds all our sprite data - paths ect
  Tileset* tilesetBuffer; // array of Tilesets ->  TILESETS type (enum), and walkableBuffer (bool array)
  
  Memory::Arena* arena_main;  
  Memory::Arena* arena_input; // memory arena for keypress data 
  Memory::Arena* arena_levels;
  Memory::Arena* arena_entities;
  Memory::Arena* arena_images;
  Memory::Arena* arena_commands;
  Memory::Arena* arena_scratch;

  FontAtlas font; // for text rendering
  
  Camera camera;
  float cam_target_x; // TODO remove - update loop game.cpp
  float cam_target_y; // TODO remove - update loop
  bool cam_lock_to_player;

  ImGuiContext* imGui_context;
  const float* dt; // pointer to our deltatime
  float* dt_scaler; // or scaler for slowing down or speeding up the game

  
};


// inline makes it so this function will be the same for all files that implement it. Alternative we could make it in a gameState.cpp file instead
// but this is just a small helper function and we dont feel the need to create a cpp file for that
// Else we would get a compilation error as the same function is in multiple places in the code. (as .h files just gets copied at the top of the document)
inline LevelData* GetCurrentLevel(Gameplay* game){
  return &game->levels[game->currentLevelIndex]; // Its important to return a pointer, else we return a copy of it and not the acual level data
}

#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "command.h"
#include "entity.h"
#include "gameState.h"
#include "levels.h"


// extern C will ensure that the names wont get mangled when exporting to an DLL
// These are still function declarations
// (https://en.wikipedia.org/wiki/Name_mangling)
// __declspec(dllexport) is the tag microsoft expects
// for exposing dll function names that are sure to be the same
extern "C" {
  __declspec(dllexport) void Initialize(GameData* data, SDL_Renderer* renderer, SDL_Window* window);
  __declspec(dllexport) bool HandleEvents(GameData* data, SDL_Event event);
  __declspec(dllexport) void Draw(GameData* data, SDL_Renderer* renderer);
  __declspec(dllexport) void Update(GameData* data, float dt);
  __declspec(dllexport) void OnQuit(GameData* data, SDL_Renderer* renderer);

  // functions that are ment to only run inside our dll not needing linking ect to the exe
  // encapsulation

  // Uses the direction we try to move and the level data
  // bool TryMove(Entity* mover, LevelData* level, int xDir, int yDir); - old
  bool TryMove(Entity* mover, LevelData* level, CommandBuffer* cmd_buffer ,int xDir, int yDir, int strength, int health);
  void ChangeScene(GameData* data, SCENE_TYPES new_scene);
  
}




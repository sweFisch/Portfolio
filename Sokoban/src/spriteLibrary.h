// spriteLibrary.h
//

#pragma once
#include "SDL3/SDL_render.h"
#include "entity.h"


enum class SPRITE_ID{
  FALLBACK,
  ROCK,
  DEMON,
  MEDUSA,
  MEDUSA_ROTATE,
  MEDUSA_IDLE_LEFT,
  MEDUSA_IDLE_FRONT,
  MEDUSA_IDLE_BACK,
  GOLEM,
  GHOST,
  SIREN,
  DROPSHADOW,
  titlescreen_background,
  selection_marker,
  black_1x1,
  dungeon_tileset,
  Goal,
  Menu_Horizon,
  Menu_Cloud_Back,
  Menu_Cloud_Front,
  Menu_Middle,
  Menu_Front,
  Button_Basic,
  Lava,
  Ice_0,
  Ice_1,
  Ice_2,
  Puddle,
  WALL_RULE_TILESET,
  GROUND_TILESET,
  PLAYER,
  GROUND_EDITOR, // editor sprite
  WALL_EDITOR,    // editor sprite
  WALL_BG,
  Lava2,
  Box_Wood,
  HEADBANG,
  FIREBALL
};


struct Sprite{
  SDL_Texture* texture;
  int width;
  int height;
  int pivot_x;
  int pivot_y;
  int sprite_count_x;
  int sprite_count_y;
  int framerate;
};


const int NOT_SET = -1;

// Struct for maching id with a path
struct SpriteDataEntry {
  SPRITE_ID id;
  const char* path;
  int pivot_x = NOT_SET;
  int pivot_y = NOT_SET;
  int tileset_cell_count_x = NOT_SET;// possible automation of reading this from our tileset - TODO
  int tileset_cell_count_y = NOT_SET;
  int framerate = NOT_SET;
};


struct SpriteRenderInfo
{
  Sprite* sprite;
  int frame;
  bool flipped_x;

  // Default constructor - no input
  SpriteRenderInfo(){
    this->sprite = nullptr;
    this->frame = 0;
    this->flipped_x = false;
  }

  SpriteRenderInfo(int frame, Sprite* sprite){
    this->sprite = sprite;
    this->frame = frame;
    this->flipped_x = false;
  }

  
  SpriteRenderInfo(int frame, Sprite* sprite, bool flipped_x){
    this->sprite = sprite;
    this->frame = frame;
    this->flipped_x = flipped_x;
  }
  
  // An implicit conversion constructor
  SpriteRenderInfo(Sprite* sprite){
    this->sprite = sprite;
    this->frame = 0;
    this->flipped_x = false;
  }
};

inline int GetSpriteCount(Sprite* sprite)
{
  if(sprite->sprite_count_x == NOT_SET) return 1;
  if(sprite->sprite_count_y == NOT_SET) return 1;
  return sprite->sprite_count_x * sprite->sprite_count_y;
}

// Functions

Sprite* GetSprite(SPRITE_ID sprite_id, Sprite* spriteBuffer);
Sprite* GetSpriteFromID(ENTITY_ID id, Sprite* spriteBuffer); // spriteBuffer is in GameData
SpriteRenderInfo GetSprite_FromEntityState(Entity* entity, Sprite* spriteBuffer, const uint64_t* ticks_total); // returns a SpriteRenderInfo now - p 365

namespace AssetManagement
{
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer);
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer);
}

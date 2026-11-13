// spriteLibrary.cpp
//

#include "spriteLibrary.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "entity.h"
#include <cassert>
#include <cmath>
#include "common.h"

const char* FALLBACK_PATH = "assets/sprites/fallback.png";

// static makes the array scoped to only exist in this file , const makes it read only
static const SpriteDataEntry all_sprite_data[] = {
  {SPRITE_ID::FALLBACK, FALLBACK_PATH             , 8, 8   }, // fallback pivot placed in the center
  {SPRITE_ID::DEMON,    "assets/sprites/demon.png" , 12, 24 }, // showcase a not placed pivot - TODO change
  {SPRITE_ID::ROCK,     "assets/sprites/rock.png", 10, 20     },
  {SPRITE_ID::MEDUSA,   "assets/sprites/medusa_idle_front.png" ,12 ,24 },
  {SPRITE_ID::MEDUSA_ROTATE, "assets/sprites/medusa_rotate.png", 12, 24, 8, 1}, // medusa animaiton tile sheet
  // {SPRITE_ID::MEDUSA_IDLE_LEFT,   "assets/sprites/medusa_idle_left.png"  , 12, 24, 4, 1, 8 },
  {
    .id = SPRITE_ID::MEDUSA_IDLE_LEFT,
    .path = "assets/sprites/medusa_idle_left.png",
    .pivot_x = 12,
    .pivot_y = 24,
    .tileset_cell_count_x = 4,
    .tileset_cell_count_y = 1,
    .framerate = 8
  }, // other way of assigning a struct
  {SPRITE_ID::MEDUSA_IDLE_FRONT,   "assets/sprites/medusa_idle_front.png", 12, 24, 4, 1, 8 },
  {SPRITE_ID::MEDUSA_IDLE_BACK,   "assets/sprites/medusa_idle_back.png"  , 12, 24, 4, 1, 8 },
  {SPRITE_ID::GHOST,    "assets/sprites/ghost.png"  ,12,24 },
  {SPRITE_ID::GOLEM,    "assets/sprites/golem.png"  ,12,24 },
  {SPRITE_ID::SIREN,    "assets/sprites/siren.png"  ,12,24 },
  {SPRITE_ID::DROPSHADOW, "assets/sprites/dropshadow.png", 8, 8 },
  {SPRITE_ID::titlescreen_background, "assets/sprites/titlescreen.png"},
  {SPRITE_ID::selection_marker, "assets/sprites/selection_marker.png", 9, 9},
  {SPRITE_ID::black_1x1, "assets/sprites/black_1x1.png", 0,0},
  {SPRITE_ID::dungeon_tileset, "assets/sprites/hell_of_a_time_dungeon_tileset.png",0 ,0, 9, 9},
  {SPRITE_ID::Goal, "assets/sprites/goal.png", 8, 8, 8, 1},
  {SPRITE_ID::Menu_Horizon, "assets/sprites/mainmenu_background.png"},
  {SPRITE_ID::Menu_Cloud_Back, "assets/sprites/mainmenu_cloud_back.png"},
  {SPRITE_ID::Menu_Cloud_Front, "assets/sprites/mainmenu_cloud_front.png"},
  {SPRITE_ID::Menu_Middle, "assets/sprites/mainmenu_middle.png"},
  {SPRITE_ID::Menu_Front, "assets/sprites/mainmenu_front.png"},
  {SPRITE_ID::Button_Basic, "assets/sprites/basic_button.png" , 0, 0, 3, 3},
  {SPRITE_ID::Ice_0,  "assets/sprites/ice_0.png",10,20 },
  {SPRITE_ID::Ice_1,  "assets/sprites/ice_1.png",10,20 },
  {SPRITE_ID::Ice_2,  "assets/sprites/ice_2.png",10,20 },
  {SPRITE_ID::Puddle, "assets/sprites/puddle.png",8,8 },
  {SPRITE_ID::Lava,   "assets/sprites/lava.png",8,8,6,1, 6 },// last var is framerate
  {SPRITE_ID::WALL_RULE_TILESET, "assets/sprites/Walls_prototype_ruletile_art.png", 0, 3, 4, 4}, // Test offsetting wall y: by 6 - "_art" is the art witout the debug
  {SPRITE_ID::GROUND_TILESET, "assets/sprites/ground_prototype.png",0,0 ,3 ,3},
  {SPRITE_ID::PLAYER, "assets/sprites/player.png",12, 24, 5, 5 }, // OBS CHECK ACUAL RESOLUTION! now 2 frames to the right unused
  {SPRITE_ID::GROUND_EDITOR, "assets/sprites/ground.png", 8, 8},
  {SPRITE_ID::WALL_EDITOR, "assets/sprites/wall.png", 8, 8},
  {SPRITE_ID::WALL_BG, "assets/sprites/wall_tile_fill.png", 0, 0},
  {SPRITE_ID::Lava2,   "assets/sprites/lava_super.png",8,8,6,1, 6 },// last var is framerate
  {SPRITE_ID::Box_Wood,   "assets/sprites/box_wood.png",10,20 },
  {SPRITE_ID::HEADBANG, "assets/sprites/player_headbang.png",12,24, 7, 1, 8},
  {SPRITE_ID::FIREBALL, "assets/sprites/fireballs.png", 12,24, 5, 1, 12}
};


// FUNCTIONS
// this works as the spriteBuffer follows the same order as the enum SPRITE_ID as its used for loading the sprites
Sprite* GetSprite(SPRITE_ID sprite_id, Sprite* spriteBuffer){
  return &spriteBuffer[(int)sprite_id];
}

// The ID here must mach the level data json spritesheet int number in  order to work
Sprite* GetSpriteFromID(ENTITY_ID id, Sprite* spriteBuffer)
{
  Sprite* sprite_to_return = nullptr;
  
  switch (id)
  {
    case ENTITY_ID::DEMON:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::DEMON];
      break;
    case ENTITY_ID::ROCK:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::ROCK];
      break;
    case ENTITY_ID::MEDUSA:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::MEDUSA]; // Comment out line when working with animations
      break;
    case ENTITY_ID::GHOST:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::GHOST];
      break;
    case ENTITY_ID::GOLEM:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::GOLEM];
      break;
    case ENTITY_ID::SIREN:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::SIREN];
      break;

    case ENTITY_ID::ICE:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Ice_0];
      break;      
    case ENTITY_ID::ICE2:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Ice_1];
      break;      
    case ENTITY_ID::ICE3:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Ice_2];
      break;      
    case ENTITY_ID::LAVA:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Lava];
      break;
    case ENTITY_ID::LAVA2:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Lava2];
      break;
    case ENTITY_ID::PUDDLE:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Puddle];
      break;
    case ENTITY_ID::PLAYER:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::PLAYER];
      break;
    case ENTITY_ID::BOXWOOD:
      sprite_to_return = &spriteBuffer[(int)SPRITE_ID::Box_Wood];
      break;
    // default:
    //   sprite_to_return = &spriteBuffer[(int)SPRITE_ID::FALLBACK];
    //   break;
  }

  // check if any problems with the return happend, and if there is a texture for the sprite we try to load
  // order of checks matter we first check the pointer and then check the texture
  if(sprite_to_return == nullptr || sprite_to_return->texture == nullptr){
    sprite_to_return = &spriteBuffer[(int)SPRITE_ID::FALLBACK];
  }

  return sprite_to_return;
}


SpriteRenderInfo GetSprite_FromEntityState(Entity* entity, Sprite* spriteBuffer,const uint64_t* ticks_total)
{
  if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
    return GetSprite(SPRITE_ID::ROCK, spriteBuffer);
  }

  // if(entity->id == ENTITY_ID::PLAYER)
  // {
    
  // }

  // use turn around sprite sheet to turn with animation
  if(entity->id == ENTITY_ID::MEDUSA && entity->action == Actions::ROTATING)
  {
    Sprite* spritesheet = GetSprite(SPRITE_ID::MEDUSA_ROTATE, spriteBuffer);
    
    int start = 0;
    int end = 0;
    switch (entity->facing_previous)
    {
      case Direction::RIGHT:
        start = 6;
        break;
      case Direction::LEFT:
        start = 2;
        break;
      case Direction::UP:
        start = 4;
        break;
      case Direction::DOWN:
        start = 0;
        break;
    }
    switch(entity->facing_current)
    {
      case Direction::RIGHT:
        end = 6;
        break;
      case Direction::LEFT:
        end = 2;
        break;
      case Direction::UP:
        end = 4;
        break;
      case Direction::DOWN:
        end = 0;
        break;      
    }

    // The math here is a bit fancy but it is for working out the distance to our new direction in frames
    // we do lots of modulo operations to allow the end to be past our frame sheet , and then modulo again to be within
    // double check by talking to a LLM or something
    int sprite_count = GetSpriteCount(spritesheet);
    int forward = ((end-start) % sprite_count + sprite_count) % sprite_count;
    int backward = sprite_count - forward;

    start += sprite_count; // fixed
    end = (forward <= backward) ? (start + forward) : (start - backward);
    // here we modulo to be sure we are in the allowed frames as the end can be past
    int current_frame = ((int)std::lerp(start,end,entity->progress_01) % sprite_count);
    return {current_frame, spritesheet};
  }
  

  Sprite* sprite = nullptr; 
  int frame = 0;
  
  switch (entity->id)
  {
    case ENTITY_ID::MEDUSA:
    {
      // we use the formula (total_ticks * framerate) / game_update_speed_in_frames % frames_in_animation
      switch (entity->facing_current)
      {
        case Direction::RIGHT:
          sprite = GetSprite(SPRITE_ID::MEDUSA_IDLE_LEFT, spriteBuffer);
          frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
          return {frame,sprite, true}; // flip sprite in x 
        case Direction::LEFT:
          sprite = GetSprite(SPRITE_ID::MEDUSA_IDLE_LEFT, spriteBuffer);
          frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
          return {frame,sprite};
        case Direction::DOWN:
          sprite = GetSprite(SPRITE_ID::MEDUSA_IDLE_BACK, spriteBuffer);
          frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
          return {frame,sprite};
        case Direction::UP:
          sprite = GetSprite(SPRITE_ID::MEDUSA_IDLE_FRONT, spriteBuffer);
          frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
          return {frame,sprite};
      }
    }
    case ENTITY_ID::PLAYER:
      {
          int maxHealth = entity->maxHealth;

          if(entity->health <= 0)
          {         
            sprite = GetSprite(SPRITE_ID::PLAYER, spriteBuffer);
            // animation logic for fire sprites in player sprite
            frame = (int)((*ticks_total * 12) / FPS % sprite->sprite_count_x) + (maxHealth  * sprite->sprite_count_x);
            // frame = 0 + (maxHealth  * sprite->sprite_count_x);
            return {frame, sprite};
          }
        // we use the formula (total_ticks * framerate) / game_update_speed_in_frames % frames_in_animation
        switch (entity->facing_current)
        {
          case Direction::RIGHT:
            sprite = GetSprite(SPRITE_ID::PLAYER, spriteBuffer);
            // frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
            frame = 1 + ((maxHealth-entity->health) * sprite->sprite_count_x);
            return {frame,sprite, true}; // flip sprite in x 
          case Direction::LEFT:
            sprite = GetSprite(SPRITE_ID::PLAYER, spriteBuffer);
            // frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
            frame = 1 + ((maxHealth-entity->health) * sprite->sprite_count_x);
            return {frame,sprite};
          case Direction::DOWN:
            sprite = GetSprite(SPRITE_ID::PLAYER, spriteBuffer);
            // frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
            frame = 2 + ((maxHealth-entity->health) * sprite->sprite_count_x);
            return {frame,sprite};
          case Direction::UP:
            sprite = GetSprite(SPRITE_ID::PLAYER, spriteBuffer);
            // frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
            frame = 0 + ((maxHealth-entity->health) * sprite->sprite_count_x);
            return {frame,sprite};
        }
        
      }
    case ENTITY_ID::DEMON:
      return GetSprite(SPRITE_ID::DEMON, spriteBuffer);
    case ENTITY_ID::GOLEM:
      return GetSprite(SPRITE_ID::GOLEM, spriteBuffer);
    case ENTITY_ID::SIREN:
      return GetSprite(SPRITE_ID::SIREN,spriteBuffer);
    case ENTITY_ID::ROCK:
      return GetSprite(SPRITE_ID::ROCK, spriteBuffer);
    case ENTITY_ID::GHOST:
      return GetSprite(SPRITE_ID::GHOST, spriteBuffer);
    case ENTITY_ID::ICE:
      switch (entity->health)
      {
        case 0:
          return GetSprite(SPRITE_ID::Puddle, spriteBuffer);
      
        case 3:
          return GetSprite(SPRITE_ID::Ice_0, spriteBuffer);
        case 2:
          return GetSprite(SPRITE_ID::Ice_1, spriteBuffer);
        case 1:
          return GetSprite(SPRITE_ID::Ice_2, spriteBuffer);
      }
    case ENTITY_ID::LAVA:
      switch (entity->health)
      {
        case 1:
          sprite = GetSprite(SPRITE_ID::Lava, spriteBuffer);
          frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
          return {frame,sprite};
        case 2:
          sprite = GetSprite(SPRITE_ID::Lava2, spriteBuffer);
          frame = (int)((*ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
          return {frame,sprite};
      }
    case ENTITY_ID::PUDDLE:
      return GetSprite(SPRITE_ID::Puddle, spriteBuffer);
    case ENTITY_ID::BOXWOOD:
      return GetSprite(SPRITE_ID::Box_Wood, spriteBuffer);

// Add new player states for burning and shit
    default:
      return GetSprite(SPRITE_ID::FALLBACK, spriteBuffer);
      break;
  }

  return nullptr;
}

namespace AssetManagement
{
  
  // Call the load sprite for all_sprite_data array defined in this file
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer)
  {
    // this is a shorthand for looping over a collection?
    for (SpriteDataEntry entry : all_sprite_data) {
      LoadSprite(spriteBuffer, entry, renderer);
    }
  }

  
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer)
  {
    SDL_Surface* surface = IMG_Load(entry.path);
    if(surface == nullptr){
      surface = IMG_Load(FALLBACK_PATH);
    }

    assert(surface != nullptr); // Make sure we could load something - or crash

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    Sprite* sprite = &spriteBuffer[(int)entry.id]; // this line keeps the order of the enum the same as in the array
    sprite->texture = texture;
    sprite->height = texture->h;
    sprite->width = texture->w;

    // If pivot is NOT_SET (sentinel value -1) center pivot 
    if(entry.pivot_x == NOT_SET || entry.pivot_y == NOT_SET){
      sprite->pivot_x = sprite->width  / 2;
      sprite->pivot_y = sprite->height / 2;
    }else{
      sprite->pivot_x = entry.pivot_x;
      sprite->pivot_y = entry.pivot_y;
    }

    sprite->sprite_count_x = entry.tileset_cell_count_x;
    sprite->sprite_count_y = entry.tileset_cell_count_y;

    sprite->framerate = entry.framerate;

    SDL_DestroySurface(surface);
  }
  
}

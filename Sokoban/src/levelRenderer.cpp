// LevelRender.cpp

#include <algorithm> // used for sorting
#include <cstdint>
#include <cmath> // for lerp functions
#include "levelRenderer.h"
#include "Parsers/json.hpp"
#include "SDL3/SDL_render.h"
#include "arena.h"
#include "common.h"
#include "entity.h"
#include "gameState.h"
#include "levels.h"
#include "rendering.h"
#include "camera.h"
#include "spriteLibrary.h"
#include "tilesetLibrary.h"

// Sorting function
bool IsEntityBelowOtherEntity(Entity* a, Entity* b)
{
  // returns true if a has lower y value than b
  // TEST simple ordering thing
  bool isAtSamePlace = a->y == b->y;
  if(isAtSamePlace){
    if(HasBehaviour(b, IS_PLAYER)){
      return true;
    }
  }
  return a->y < b->y;
}

Sprite* GetSpriteFromTileset(TILESETS type, GameData* gameData)
{
  
  Sprite* sprite;
  switch (type)
  {
    case TILESETS::Dungeon:
      sprite = GetSprite(SPRITE_ID::dungeon_tileset, gameData->spriteBuffer);
      break;
    case TILESETS::GROUND:
      sprite = GetSprite(SPRITE_ID::GROUND_TILESET, gameData->spriteBuffer);
      break;
    case TILESETS::WALLS:
      sprite = GetSprite(SPRITE_ID::WALL_RULE_TILESET, gameData->spriteBuffer);
      break;
    case TILESETS::NONE:
    case TILESETS::COUNT:
      assert(false);
      break;
  }
  
  return sprite;
}

void RenderLevelLayerWalls(GameData* gameData, SDL_Renderer* renderer,LevelLayer layerType)
{
  Gameplay* gameplay = &gameData->scenes.gameplay;
  LevelData* level = &gameplay->levels[gameplay->currentLevelIndex];

  Sprite* sprite = GetSpriteFromTileset(TILESETS::WALLS, gameData);

  Sprite* spriteBgWall = GetSprite(SPRITE_ID::WALL_BG, gameData->spriteBuffer); // make sure pivot is set to 0,0 for sprite to rener here

  // loop through all the tiles
  for (int x = 0; x < level->w; x++)
  {
    for (int y = 0; y < level->h; y++)
    {
      uint16_t id = GetCellID(level, x, y, layerType) - 1; // helper function in LevelData gets us the id - 1 for walls
      if( id >= 0 && id < 60000 ) // for walls we can get -1 (uint16_t) -1 is around  63 000
      {
        // RenderSprite_World({0,spriteBgWall},renderer,&gameData->camera,x,y,1,1,0);
        // RenderTile(sprite, id, level, renderer, &gameData->camera, x, y, 1, 1); 
        RenderTile(spriteBgWall, 0, level, renderer, &gameData->camera, x, y, 1, 1);
        float xtemp = x;
        float ytemp = y;
        camera::GridToWorld(&xtemp, &ytemp, level);
        ytemp -= 6;
        RenderSprite_World({id,sprite}, renderer, &gameData->camera, xtemp, ytemp, 1, 1, 0);
      }
    }
  }
}

void RenderSingleWallTile(GameData* gameData, SDL_Renderer* renderer, int x, int y)
{
  
}


// Function for rendering the level tiles (Background)
void RenderLevel(GameData* gameData, SDL_Renderer* renderer)
{
  Gameplay* gameplay = &gameData->scenes.gameplay;
  LevelData* level = &gameplay->levels[gameplay->currentLevelIndex];

  Sprite* sprite = GetSpriteFromTileset(TILESETS::GROUND, gameData);

  // loop through all the tiles
  for (int x = 0; x < level->w; x++)
  {
    for (int y = 0; y < level->h; y++)
    {
      uint16_t id = GetCellID(level, x, y, LevelLayer::GROUND); // helper function in LevelData gets us the id
      RenderTile(sprite, id, level, renderer, &gameData->camera, x, y, 1, 1);
    }
  }

  // Render the inbetween layer where Goals are placed
  for (int i = 0; i < level->goalCount; i++)
  {
    Goal goal = level->goals[i];
    Sprite* sprite = GetSprite(SPRITE_ID::Goal, gameData->spriteBuffer);
    // changes sprite every 0.2 seconds - blink timer gets updated in UpdateGame()
    int frame = (int)(goal.blink_timer / 0.2f) % (sprite->sprite_count_x * sprite->sprite_count_y);
    RenderSprite_OnTile({frame, sprite}, level, renderer, &gameData->camera, goal.x, goal.y);
  }

  // Render lava
  RenderLavaEntities(gameData,renderer); // as it should be on the ground layer
  
  // Render Walls
  RenderLevelLayerWalls(gameData, renderer, LevelLayer::WALL);
}

// Render LAVA Entities
void RenderLavaEntities(GameData* data, SDL_Renderer* renderer)
{
  LevelData* level = &data->scenes.gameplay.levels[data->scenes.gameplay.currentLevelIndex];

  // Allocate an array of entity pointers - and make it match the current entity pointes in level
  // To hold an array of pointers it becomes a pointer that points to a pointer **
  Entity** SortedEntities = ALLOC_ARRAY(data->arena_scratch, Entity*, level->entityCount);
  for(int i = 0; i < level->entityCount; i++){
    SortedEntities[i] = &level->entityBuffer[i];
  }
  std::sort(SortedEntities, SortedEntities + level->entityCount, IsEntityBelowOtherEntity);

  Gameplay* gameplay = &data->scenes.gameplay;
  Entity* activeEntity = gameplay->activePlayerBuffer[gameplay->activePlayerIndex];

  for (int i = 0; i < level->entityCount; i++)
  {
    Entity* entity = SortedEntities[i]; // we grab this entity from the sorted entities so we render them i the right order

    if (entity->active == false){ // no data in cell in this buffer place ignore
      continue;
    }

    if (entity->id == ENTITY_ID::LAVA)
    {
    
      SpriteRenderInfo spriteRenderInfo = GetSprite_FromEntityState(entity,data->spriteBuffer, data->ticks_total);

      // Render the entity
      RenderSprite_OnTile(spriteRenderInfo, level, renderer, &data->camera, entity->x, entity->y, 1, 1, false); // old facing -> entity->facing_current == Direction::RIGHT);
    }
    
  }
}

// Render the Entities
void RenderEntities(GameData* data, SDL_Renderer* renderer)
{
  LevelData* level = &data->scenes.gameplay.levels[data->scenes.gameplay.currentLevelIndex];

  // Allocate an array of entity pointers - and make it match the current entity pointes in level
  // To hold an array of pointers it becomes a pointer that points to a pointer **
  Entity** SortedEntities = ALLOC_ARRAY(data->arena_scratch, Entity*, level->entityCount);
  for(int i = 0; i < level->entityCount; i++){
    SortedEntities[i] = &level->entityBuffer[i];
  }
  // Sort the entities on the lowest y value
  // SortedEntitis[0] will then point to the entity that has the lowest y value
  // sort takes 3 parameters - The first entry in the array, the last entry in the array, the way we want to sort it
  // SortedEntities + level->entityCount = gives us the last entity in the array (moves us entityCount steps of Entity*)
  // IsEntityBelowOtherEntity - is a function we pass along to sort - we dont call the function with () just provide the function
  // We just provide the function to sort to use! the compiler knows how to use Entity* as that is the array we pased into sort (probably a generic function)
  std::sort(SortedEntities, SortedEntities + level->entityCount, IsEntityBelowOtherEntity);

  Gameplay* gameplay = &data->scenes.gameplay;
  Entity* activeEntity = gameplay->activePlayerBuffer[gameplay->activePlayerIndex];

  for (int i = 0; i < level->entityCount; i++)
  {
    Entity* entity = SortedEntities[i]; // we grab this entity from the sorted entities so we render them i the right order

    if (entity->active == false){ // no data in cell in this buffer place ignore
      continue;
    }

    if (entity->id == ENTITY_ID::LAVA){
      continue;
    }
    
    SpriteRenderInfo spriteRenderInfo = GetSprite_FromEntityState(entity,data->spriteBuffer, data->ticks_total);

    if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
      spriteRenderInfo = GetSprite(SPRITE_ID::ROCK, data->spriteBuffer); // get a normal sprite using SPRITE_ID
    }
    
    // Animation Lerp
    float x_animated = std::lerp(entity->x_prev, entity->x, entity->progress_01);
    float y_animated = std::lerp(entity->y_prev, entity->y, entity->progress_01);
    float ground_y = y_animated; // set the shadow to ground y height

    if (entity->action == Actions::MOVING && HasBehaviour(entity, Behaviour::JUMPS)
        && !HasBehaviour(entity, Behaviour::IS_PUSHING))
    {
      // Jump Math makes a parabola 0,1 is grounded and 0.5 (progress_01) is max air height
      y_animated -= 0.5f * sinf(entity->progress_01 * 3.14f);  
    }

    Sprite* dropshadow = &data->spriteBuffer[(int)SPRITE_ID::DROPSHADOW];

    // Render - Dropshadow 
    RenderSprite_OnTile(dropshadow, level, renderer, &data->camera, x_animated, ground_y, 1, 0.4f, false);
    // Selection Marker
    if(entity == activeEntity){
      SpriteRenderInfo selection_marker = GetSprite(SPRITE_ID::selection_marker, data->spriteBuffer);
      RenderSprite_OnTile(selection_marker, level, renderer, &data->camera, x_animated, ground_y);
    }
    // Render the entity
    RenderSprite_OnTile(spriteRenderInfo, level, renderer, &data->camera, x_animated, y_animated, 1, 1, false); // old facing -> entity->facing_current == Direction::RIGHT);

    // Render wall tile that might cover this entity?
    
  }
}

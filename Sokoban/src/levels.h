#pragma once
#include <cstdint>
#include <vector>
#include "arena.h"
#include "entity.h"
#include "tilesetLibrary.h"
using namespace Memory;

enum class LevelLayer{
  GROUND,
  WALL
};

struct Goal{
  int x;
  int y;
  float blink_timer;
};

// Level Data Struct
// uint8_t holds a 1 byte number 0-255 .
// At the moment no safe guard , level will crash if .tmj file dont exsists
struct LevelData{
  int w;
  int h;
  uint16_t* cells; // ids of render sprite
  uint16_t* cells_walls;
  Goal* goals;
  int goalCount;
  const char* level_path;
  Entity* entityBuffer;
  int entityCount;
  const Tileset* tileset_ground;
  const Tileset* tileset_walls;
};


// Function declarations


uint16_t GetCellID(LevelData* level, int x, int y, LevelLayer layer);
Entity* GetEntity(LevelData* level, int x, int y);

void CreateLevel(Arena *arena, LevelData* level_data, Tileset* tileset_ground, Tileset* tileset_walls ,const char* level_path);
void CreateEntities(LevelData * level_data, Arena *arena);

// void CreateGroundLayer(LevelData* level_data, Tileset* tileset);
// void CreateWallsLayer(LevelData* level_data, Tileset* tileset);

Entity* GetNextAvailableEntity(LevelData* level);
void AddEntitiy(ENTITY_ID entity_id, int x, int y, LevelData* level);
void RemoveEntity(int x, int y, LevelData* level);
void RestoreEntity(ENTITY_ID entity_id, int x, int y, LevelData* level, Entity* entity);

Entity* RaycastFirstEntity(int x_origin, int y_origin, Direction direction, LevelData* level, bool ignore_walls = false);

bool IsWalkable(int x, int y, LevelData* level);


namespace AssetManagement{
  std::vector<uint16_t> GetCellDataFromJsonLayer(nlohmann::json& parsedJson, const char* layerName, bool* wasFound);
  int GetFirstNonZeroCell(std::vector<uint16_t>* listOfCells);
}

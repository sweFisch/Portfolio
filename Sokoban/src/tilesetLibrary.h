// tilesetLibrary.h

#pragma once

#include "Parsers/json.hpp"
#include <cstdint>

using namespace nlohmann; // for our json parser

// forward declaration of arena
namespace Memory{
  struct Arena;
}

enum class TILESETS {
  NONE = 0,
  Dungeon = 1,// the acual tileset we use , none and count are just helper enums
  GROUND = 2,
  WALLS = 3,
  COUNT = 4
};

// struct containing an array of our walkable bools
struct Tileset{
  TILESETS type;
  bool* walkableBuffer;
};

// Data format for loading our sprites from the library
struct TilesetDataEntry {
  TILESETS type;
  const char* path;
};

// 
uint16_t GetLocalTileID(uint16_t id_global, const json& tmj_result);
// Get the id within the tilemap file
uint16_t Get_Tileset_ID_Offset_From_Tilemap(int id_limit, const json& tmj_result);

namespace AssetManagement{
  void LoadAllTilesets(Tileset* tilesetBuffer, Memory::Arena* arena_images);
  void LoadTileset(TilesetDataEntry* entry, Tileset* tilesetBuffer, Memory::Arena* arena_images);
}

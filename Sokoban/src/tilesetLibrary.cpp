// tilesetLibrary.cpp
//

#include "Parsers/json.hpp"
#include "tilesetLibrary.h"
#include "arena.h"
#include <cassert>
#include <fstream>
#include "fstream"

using namespace nlohmann;
using namespace std;

// Setup our Data paths for each tileset
static const TilesetDataEntry all_tilesets_data[]{
  // {TILESETS::Dungeon, "assets/tilesets/dungeon_tileset.tsj"},
  {TILESETS::GROUND, "assets/tilesets/ground_prototype.tsj"},
  {TILESETS::WALLS, "assets/tilesets/Walls_prototype_ruletile.tsj"}
};

// Load data from .tmj - the Level to get the offset of tilemap numbering (global) to local (tilemap_name.tsj)
// returns the local tile id converted from global_tile_id
// global id beeing what is written as a number for that cell in the map
// the global id takes all tilemaps used and increments them tilemap by tilemap.
// Tilemap 01 - 6 tiles (0-5), whould meen that tilemap_02 starting "firstgid" is 6
// But getting a tile from tilemap_02 requires the local id that the tilemap specifies, thus global_id - 6 in this case
uint16_t GetLocalTileID(uint16_t id_global, const json& tmj_result)
{
  return id_global - Get_Tileset_ID_Offset_From_Tilemap(id_global, tmj_result);
}


// Get the id within the level file of the tilemaps used - order ect of tilemaps used may matter..
// Take the global id and return the firstgid that is just lower than that
uint16_t Get_Tileset_ID_Offset_From_Tilemap(int id_global, const json& tmj_result)
{
  int highest_tilemap_start_id = 0;
  for (const json& tileset : tmj_result["tilesets"])
  {
    // OBS "first-gid" - is in the level.tmj file where the tilemaps are specified
    // the number specifies where the tile sets tile id starts its mapping in this map. map01 - might be 6, map02 -42 .. ect
    // tries to get the highest firstgid that is still lower than the global_id beeing passed into the function.
    // That gives us the starting id for our current tilemap we should look into to find the sprite (using the local id number in the sprite sheet)
    int first_id = tileset["firstgid"].get<int>();
    if(first_id <= id_global && first_id > highest_tilemap_start_id){
      highest_tilemap_start_id = first_id;
    }
  }
  return highest_tilemap_start_id;
}


// Here we load from .tsj tileset files 
namespace AssetManagement
{  
  // Loop through all tilesets defined in all_tilesets_data (TilesetDataEntry) 
  void LoadAllTilesets(Tileset* tilesetBuffer, Memory::Arena* arena_images)
  {
    for (TilesetDataEntry entry : all_tilesets_data){
      LoadTileset(&entry, tilesetBuffer, arena_images);
    }
  }


  // Load the tileset from disk (tileset-name.tsj) -> move data to buffer
  // Load custom properties for the tiles  and allocate buffers 
  void LoadTileset(TilesetDataEntry* entry, Tileset* tilesetBuffer, Memory::Arena* arena_images)
  {
    assert(entry->type != TILESETS::NONE);
    assert(entry->type != TILESETS::COUNT);

    // Crete a pointer to the current working tileset from our (pointer)tilesetBuffer[id] we provided
    Tileset* tileset = &tilesetBuffer[(int)entry->type];
    tileset->type = entry->type; // set the tileset id from the entry->type
    // open the file and read the JSON
    fstream stream(entry->path);
    auto jsonResult = json::parse(stream);
    int tile_count = jsonResult["tilecount"].get<int>(); // extract the tilecount from the json blob (stream)
    // ALLOCATE the memory for the bool array
    tileset->walkableBuffer = ALLOC_ARRAY(arena_images, bool, tile_count); // allocate space for holding the bool array

    auto& tiles = jsonResult["tiles"]; // extract all the "tiles" data from the jsonResults

    // Loop through all tiles and - Read the custom properties stored in the tilemaps tiles
    for (const auto& tile : tiles)
    {
      int tiles_id = tile["id"].get<int>();
      // read the .tsj file with a text editor and investigate, if you want to add more properties
      for (const auto& tile_property : tile["properties"])
      {
        if(tile_property["name"] == "walkable")
        {
          tileset->walkableBuffer[tiles_id] = tile_property["value"].get<bool>();
        }
      }
      
    }
    stream.close();
  }
  
  
}

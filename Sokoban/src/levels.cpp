// Levels cpp
#include <cstddef>
#include <cstdint> //gives access to uint8_t
#include <filesystem>
#include <fstream> // reading from disk
#include <vector> // this vill be the format from the json parser
#include "levels.h" // LevelData
#include "arena.h" // so we can pass a arena pointer
#include "Parsers/json.hpp"
#include "common.h"
#include "entity.h"
#include "tilesetLibrary.h"

#include "SDL3/SDL_log.h"
#include "ruleTile.h"


using namespace std;

// The const specify the level index in our .tmj file
// const int LEVEL_INDEX = 0;
// const int ENTITIES_INDEX = 1;

// Helper function to convert x, y to cell index
uint16_t GetCellID(LevelData* level, int x, int y, LevelLayer layer)
{
  switch (layer)
  {
    case LevelLayer::GROUND:
      return level->cells[ (y * level->w) + x]; // convert x,y to 1D index
      break;
    case LevelLayer::WALL:
      return level->cells_walls[ (y * level->w) + x]; // convert x,y to 1D index
      break;
  }
  return level->cells[ (y * level->w) + x]; // Return Ground layer id
}


// Return a pointer for a entity on position x,y , nullptr if not found!
Entity* GetEntity(LevelData* level, int x, int y){
  for (int i = 0; i < level->entityCount; i++){
    if(level->entityBuffer[i].x == x && level->entityBuffer[i].y == y){
      return &level->entityBuffer[i];
    }
  }

  return nullptr;
}


Entity* GetNextAvailableEntity(LevelData* level)
{
  /* Allowing gaps
  // checking if we have a gap in our array of entities we can use
  // be sure not to use <= then the agortim breaks (it wont increment)
  for (int i = 0; i < level->entityCount; i++){
    if(level->entityBuffer[i].active == false){
    // if(level->entityBuffer[i].id == ENTITY_ID::NONE){
      return &level->entityBuffer[i];
    }
  }
  */
  
  // if no gaps return the empty one at the entityCount and increment entityCount after returning it
  return &level->entityBuffer[level->entityCount++];
}


void AddEntitiy(ENTITY_ID entity_id, int x, int y, LevelData* level)
{
  Entity* entity = GetEntity(level, x, y); // get a entity at the x,y pos if there is one

  // if there is no entity at the position
  // we get the next empty slot in our entity array and use that as our entity
  if (entity == nullptr){
    entity = GetNextAvailableEntity(level);
  }
  // fill the entity with data - can be an old entity or a new one
  entity->active = true;
  entity->x = x;
  entity->y = y;
  entity->x_prev =  x;
  entity->y_prev = y;
  entity->id = entity_id; // id used for rendering and behaviour
  entity->action = Actions::NONE;
  
  InitializeBaseBehaviour(entity);// use or id to set the behaviour
}

void RestoreEntity(ENTITY_ID entity_id, int x, int y, LevelData* level, Entity* entity)
{
  // Entity* entity = GetEntity(level, x, y); // get a entity at the x,y pos if there is one

  // if there is no entity at the position
  // we get the next empty slot in our entity array and use that as our entity
  // if (entity == nullptr){
    // entity = GetNextAvailableEntity(level);
  // }
  // fill the entity with data - can be an old entity or a new one
  entity->active = true;
  entity->x = x;
  entity->y = y;
  entity->x_prev =  x;
  entity->y_prev = y;
  entity->id = entity_id; // id used for rendering and behaviour
  entity->action = Actions::NONE;
  
  InitializeBaseBehaviour(entity);// use or id to set the behaviour
}

void RemoveEntity(int x, int y, LevelData* level)
{
  Entity* entity = GetEntity(level, x, y); // get the entity at x,y position
  if(entity == nullptr){
    return;
  }

  *entity = {}; // This will zero out all variables in our struct - its struct default
}


// level_name is the path and name of our level data .tmj file - assets/maps/testLevel.tmj tex
// we parse with the json.hpp file, going through its namespaces untill we get our function
// passing in the stream (fstream) of data
// into a jsonResults to nlohmann::basic_json<> format using auto in this case
//  we can now take our jsonResult and handle it as a nested set of arrays, accessable by name
// LEVEL_INDEX and ENTITY_INDEX we stored before as constants are used to traverse the file data
// we take our "data" inside the layer LEVEL_INDEX and process the numbers that are stored as
// vector of uint8_t values (0-255 numbers)
// a vector is lika an array but its size can change during runtime
// .get<type>() will take the contents of our json and puts it into the right type.
// Before this the type is not known
// we take our passed in level_data pointer and write the w,h,level_path values
// specifying what kind of data they are stored as int (w,h), const char*
// Calculate the memory needed for storing the cells size_of_data(one cell) * w * h (this case 1 byte * w * h)
// Allocate our needed memory into the arena and get a pointer to the first cell of this memory
// loop through the cells and take the data from the vector dataField (we got from our json) into each cell
// making cells contain all our IDs for our tiles
void CreateLevel(Arena *arena, LevelData* level_data,Tileset* tileset_ground, Tileset* tileset_walls ,const char* level_path){
  if(std::filesystem::exists(level_path) == true){
    std::printf("\n -Path to maps (level)! - %s \n", level_path);
  }else{
    std::printf("\n -Path to maps not found! - %s\n", level_path);
  }
  
  fstream stream(level_path);
  auto jsonResult = nlohmann::json::parse(stream);

  // Read the Ground Layer from the .tmj file ----
  //    Read the layer data from layers in .tmj file, store as a vector uint16_t number (array like structure)
  //    vector dataField = jsonResult["layers"][LEVEL_INDEX]["data"].get<vector<uint8_t>>(); <- naive way
  bool found = false;
  vector<uint16_t> layerData = AssetManagement::GetCellDataFromJsonLayer(jsonResult, "Ground", &found); // using our helper function
  assert(found); // make sure last step got some data
  
  // Get one id from the map - so we can get the offset with Get_Tileset_ID_Offset_From_Tilemap
  // offset is the difference between global values that the level uses and the values for the tilemap
  int first_non_zero_id = AssetManagement::GetFirstNonZeroCell(&layerData);

  // use our helper function to get the offset for the tilmap we are using
  int id_offset = Get_Tileset_ID_Offset_From_Tilemap(first_non_zero_id, jsonResult);
  
  
  level_data->w = jsonResult["width"].get<int>();
  level_data->h = jsonResult["height"].get<int>();
  level_data->level_path = level_path;
  level_data->tileset_ground = tileset_ground;
  // ALLOCATE memory for the cells of the level
  level_data->cells  = ALLOC_ARRAY(arena, uint16_t, level_data->w * level_data->h); // ground layer

  for (int i = 0; i < (level_data->w * level_data->h); i++)
  {
    int local_id = layerData[i] - id_offset; // Calculate the tilemap local id
    if(local_id < 0){
      local_id = 0; // zero values have become non zero by the offeset make sure they are 0 again!
    }
    level_data->cells[i] = local_id; // store the id in the array
  }

  // --- Read walls layer 
  vector<uint16_t> layerDataWalls = AssetManagement::GetCellDataFromJsonLayer(jsonResult, "Walls", &found); // using our helper function
  assert(found); // make sure last step got some data
  
  // Get one id from the map - so we can get the offset with Get_Tileset_ID_Offset_From_Tilemap
  // offset is the difference between global values that the level uses and the values for the tilemap
  first_non_zero_id = AssetManagement::GetFirstNonZeroCell(&layerDataWalls);

  // use our helper function to get the offset for the tilmap we are using
  id_offset = Get_Tileset_ID_Offset_From_Tilemap(first_non_zero_id, jsonResult);
 
  level_data->tileset_walls = tileset_walls;
  // ALLOCATE memory for the cells of the level - Walls
  level_data->cells_walls  = ALLOC_ARRAY(arena, uint16_t, level_data->w * level_data->h); // wall layer

  for (int i = 0; i < (level_data->w * level_data->h); i++)
  {
    int local_id = layerDataWalls[i] - id_offset + 1; // Calculate the tilemap local id - offset wall numbers by 1
    if(local_id < 0){
      local_id = 0; // zero values have become non zero by the offeset make sure they are 0 again!
    }
    level_data->cells_walls[i] = local_id; // store the id in the array
  }

  // Run rule tile algorithm on the wall cells
  ProcessWallData(level_data->cells_walls, level_data);
  

  // --- Create goals layer - on_level ---
  // At the moment it only cares about x,y position and will populate the same grapics everywere, goals only contain x,y position
  // Read data layer of "on_Level" layer in .tmj - all non zero numbers will be converted to goals
  vector<uint16_t> onLevel = AssetManagement::GetCellDataFromJsonLayer(jsonResult, "on_level", &found);
  if(found)
  {
    int firstNonZero = AssetManagement::GetFirstNonZeroCell(&onLevel);
    id_offset = Get_Tileset_ID_Offset_From_Tilemap(firstNonZero, jsonResult);

    // Loop through and find how many tiles have data above - so we can allocate the correct amount of memory
    level_data->goalCount = 0;
    for (int i = 0; i < level_data->w * level_data->h; i++){
      int local_id = onLevel[i] - id_offset + 1; // this is a bit messy as offset is 90 witch is our data we want so +1 so its not 0
      if(local_id > 0){
        level_data->goalCount++;
      }
    }
    // Allocate space for that many goals 
    level_data->goals = ALLOC_ARRAY(arena, Goal, level_data->goalCount);
    // Populate the array with - goals - (can be expanded later to contain more data)
    int index = 0;
    for (int i = 0; i < level_data->w * level_data->h; i++)
    {
      int local_id = onLevel[i] - id_offset + 1;
      if(local_id < 0)
      {
        local_id = 0;
      }
      if(local_id != 0)
      {
        int x;
        int y;
        Expand1Dto2D(i, level_data->w, &x, &y);
        level_data->goals[index].x = x;
        level_data->goals[index].y = y;
        index++; // Index only increments after a successfull goal find.
      }
    }
  } // end block that handels goals level - "onLevel" layer
  else
  {
    SDL_Log("Found no goals in level");
  }
  
  stream.close();
  std::printf("- Created Level -");
}




void CreateEntities(LevelData * level_data, Arena *arena){
  // Reset the data in the entity memeory arena
  Memory::Reset(arena);
  level_data->entityCount = 0; // Reset the entity counter in level data
  
  
  if(std::filesystem::exists(level_data->level_path) == true){
    std::printf("\n -Path to maps (Entities)! - %s \n", level_data->level_path);
  }else{
    std::printf("\n -Path to maps not found (Entities)! - %s\n", level_data->level_path);
  } // just a file exists check 
  
  fstream stream(level_data->level_path); // get the path saved in level data as open file as a stream
  auto jsonResult = nlohmann::json::parse(stream); // parse the json found in the .tmj
  
  // get the data from the entities layer - as a vector (data type expected uint16_t)  
  bool found = false;
  vector<uint16_t> entities = AssetManagement::GetCellDataFromJsonLayer(jsonResult, "Entities", &found);

  if(found == false){
    std::printf("No entities in level file...");
    return;
  }
  
  // ALLOCATE entityBuffer memory
  int maxEntities = level_data->w * level_data->h;
  level_data->entityBuffer = (Entity*)Memory::Allocate(arena, sizeof(Entity) * maxEntities);

  // Loop trough our level and add entities
  for (int i = 0; i < level_data->w * level_data->h; i++)
  {
    if(entities[i] == 0){
      continue;
    }
    uint16_t entity_id = GetLocalTileID(entities[i], jsonResult);
    
    // get a 2D point from a 1D array
    // int x = i % level_data->w; // finds the x coord - using modulo on the width gives ut the remainder
    // int y = i / level_data->w; // finds the y coord - dividing by width gets how many rows down we are - int rounding down helps us here
    int x;
    int y;
    Expand1Dto2D(i, level_data->w, &x, &y);
    AddEntitiy((ENTITY_ID)entity_id, x, y, level_data);
  }

  stream.close();
  std::printf("Created Entities");
}


bool IsWalkable(int x, int y, LevelData* level)
{
  uint16_t id = GetCellID(level, x, y, LevelLayer::GROUND);
  bool groundLayer =  level->tileset_ground->walkableBuffer[id];
  bool wallLayer = GetCellID(level, x, y, LevelLayer::WALL) <= 0;
  // SDL_Log("levelLayer wall :  %i", GetCellID(level, x, y, LevelLayer::WALL));
  return (groundLayer == true && wallLayer == true); // both must be true
}


// Searching through the grid in the facing direction and if hitting an entity returning it
Entity* RaycastFirstEntity(int x_origin, int y_origin, Direction direction, LevelData* level, bool ignore_walls)
{
  Position facingVector;
  switch(direction)
  {
    case Direction::RIGHT:
      facingVector = {1,0};
      break;
    case Direction::LEFT:
      facingVector = {-1,0};
      break;
    case Direction::UP:
      facingVector = {0,1};
      break;
    case Direction::DOWN:
      facingVector = {0,-1};
      break;
  }

  // Adding a step in the facing direction before checking for entities 
  int x_search = x_origin + facingVector.x;
  int y_search = y_origin + facingVector.y;

  while(x_search  > 0 && x_search < level->w && y_search > 0 && y_search < level->h)
  {
    // ENTITY_ID cellID = (ENTITY_ID)GetCellID(level, x_search, y_search);// - OLD - REMOVE
    // if(cellID == ENTITY_ID::WALL && ignore_walls == false){
    //   break;
    // }
    if (IsWalkable(x_search, y_search, level) == false && ignore_walls == false)
    {
      break;
    }

    
    Entity* entity_search = GetEntity(level, x_search, y_search);
    if(entity_search != nullptr){
      return entity_search;
    }

    x_search += facingVector.x;
    y_search += facingVector.y;
  }

  return nullptr;
}


namespace AssetManagement
{
  std::vector<uint16_t> GetCellDataFromJsonLayer(nlohmann::json& parsedJson, const char* layerName, bool* wasFound)
  {    
    // Read the layer data from layers in .tmj file, store as a vector uint16_t number (array like structure)
    // vector dataField = jsonResult["layers"][LEVEL_INDEX]["data"].get<vector<uint8_t>>();
    *wasFound = false;
    vector<uint16_t> results;
    for (const auto& layer : parsedJson["layers"])
    {
      if (layer["name"] == layerName) // We are looking for the input layerName in the  .tmj file
      {
        results = layer["data"].get<vector<uint16_t>>(); // store the data at the temp vector levelData
        *wasFound = true;
        break;
      }
    }
    return results; // return the cell array of data from the json file
  }

  
  int GetFirstNonZeroCell(std::vector<uint16_t>* listOfCells)
  {    
    // Get one id from the map - so we can get the offset with Get_Tileset_ID_Offset_From_Tilemap
    // offset is the difference between global values that the level uses and the values for the tilemap
    for (int id : *listOfCells)
    {
      if(id != 0){
        return id;
      }
    }
    assert(false);
    return -1;
  }
  
}// - AssetManagement namespace end

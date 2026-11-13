#include "Parsers/json.hpp"
#include "SDL3/SDL_log.h"
#include "gameState.h"
#include <fstream>
#include "saveGame.h"


void SaveGame(GameData *data){
  nlohmann::json save;

  save["levelIndex"] = data->scenes.gameplay.currentLevelIndex;

  std::ofstream file("save.json");
  file << save.dump(2);
  file.close();

  SDL_Log("Game was saved.");
  }

bool LoadGame(GameData* data){
  std::ifstream file("save.json");
  if(!file.is_open())
  {
    SDL_Log("No save found");
    return false;
  }

  nlohmann::json save = nlohmann::json::parse(file, nullptr, false);
  if(save.is_discarded())
  {
    SDL_Log("Save file broken");
    return false;
  }
  
 Gameplay* gameplay = &data->scenes.gameplay;
  int levelIndex = save.value("levelIndex", 0); // gives 0 if "levelIndex" is missing

  // the level might not exist anymore
  if (levelIndex < 0 || levelIndex >= gameplay->levelCount) {
    SDL_Log("Saved level %d does not exist - starting fresh", levelIndex);
    return false;
  }

  gameplay->currentLevelIndex = levelIndex;
  SDL_Log("Game loaded! Level %d", levelIndex);
  return true;

}

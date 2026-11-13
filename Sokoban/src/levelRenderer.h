#pragma once

#include "SDL3/SDL_render.h"
#include "gameState.h"


void RenderLevel(GameData* gameData, SDL_Renderer* renderer);
void RenderEntities(GameData* gameData, SDL_Renderer* renderer);

void RenderLavaEntities(GameData* data, SDL_Renderer* renderer);

#pragma once
#include "SDL3/SDL_render.h"
#include "spriteLibrary.h"
#include "camera.h"
#include "levels.h"

// ForwardDeclaration
struct Button;
struct FontAtlas;

enum class Aligment
{
  Right,
  Centered
};

// void RenderSprite(Image* sprite, SDL_Renderer* renderer, int xPos, int yPos, float scale = 1.0f);
// void RenderSprite_Grid(Sprite* sprite, LevelData* lvl, SDL_Renderer* renderer,const Camera* camera, float x, float y, float scale = 1.0f, float alpha = 1.0f, bool flipped = 1);

void RenderTile(Sprite* tileset, uint16_t cell_id, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1.0f, float alpha = 1.0f);

void RenderSprite_World(SpriteRenderInfo spriteRenderInfo, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1.0f, float alpha = 1.0f, bool flipped = 1);

void RenderSprite_OnTile(SpriteRenderInfo spriteRenderInfo, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale = 1.0f, float alpha = 1.0f, bool flipped = 1);

void RenderButton(Button* button, bool is_selected, SDL_Renderer* renderer);

void RenderText(FontAtlas* atlas, const char* text, SDL_Renderer* renderer, Camera* camera, const float x, const float y, Aligment aligment);

void RenderButton_Dynamic(Button* button, bool is_selected, SDL_Renderer* renderer);

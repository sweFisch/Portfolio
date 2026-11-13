// levelEditor.h
//

#pragma once

#include "camera.h"
#include "input.h"
#include "levels.h"
#include "spriteLibrary.h"

enum class EditorMode
{
  NONE = 0,
  DRAW_GROUND = 1,
  DRAW_WALLS = 2,
  DRAW_ENTITIES = 3
};

struct EditorData{
  ENTITY_ID object_to_place_id;
  bool edit_level;
  EditorMode mode;
  
  float* fps_buffer;
  int fps_buffer_index;
  int fps_buffer_count;
};

namespace EDITOR{
  //  Create a new window - that displays all entities , tiles we can place on our level
  void DrawObjectPanel(EditorData* editor, Sprite* spriteBuffer);

  // put an entity on the tile
  void PlaceObject(const int x, const int y, EditorData* editor, LevelData* level, CommandBuffer* commandBuffer);

  // check our mouse inputs what to do
  void Update(EditorData* editor, Input* input, LevelData* level, CommandBuffer* commandBuffer);

  // Draw a transparent version of our tile as a preview
  void DrawPreview(EditorData* editor, Input* input, SDL_Renderer* renderer, LevelData* level, Camera* camera, Sprite* spriteBuffer);
}


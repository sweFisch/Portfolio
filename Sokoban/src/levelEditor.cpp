// leveleditor.cpp
//

#include "levelEditor.h"
#include "camera.h"
#include "command.h"
#include "entity.h"
#include "imgui/imgui.h"
#include "input.h"
#include "levels.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include "ruleTile.h"
#include "SDL3/SDL_log.h"


namespace EDITOR{
  //  Create a new window - that displays all entities , tiles we can place on our level
  void DrawObjectPanel(EditorData* editor, Sprite* spriteBuffer)
  {
    // ImGui:: Begin and End makes this its own window
    ImGui::Begin("objects");
    ImVec2 size = {32, 32};

    // TODO - Get ground and wall working again
    // imgui_impl_sdlrenderer3.h - makes the SDL texture work with ->  imgui
    // Handle background drawing
    // if(ImGui::ImageButton("Ground", (ImTextureID)GetSprite(SPRITE_ID::GROUND_EDITOR, spriteBuffer)->texture, size))
    // {
    //   editor->mode = EditorMode::DRAW_GROUND;
    //   // editor->object_to_place_id = ENTITY_ID::GROUND; // update our object_to_place_id when the button is pressed
    // }
    // ImGui::SameLine();
    
    if(ImGui::ImageButton("Wall", (ImTextureID)GetSprite(SPRITE_ID::WALL_EDITOR, spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_WALLS;
      // editor->object_to_place_id = ENTITY_ID::WALL;
    }

    // Place Entities on map
    ImGui::SameLine();
    if(ImGui::ImageButton("Player", (ImTextureID)GetSpriteFromID(ENTITY_ID::DEMON,spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_ENTITIES;
      editor->object_to_place_id = ENTITY_ID::PLAYER;
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("lava", (ImTextureID)GetSpriteFromID(ENTITY_ID::LAVA, spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_ENTITIES;
      editor->object_to_place_id = ENTITY_ID::LAVA;
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("lava2", (ImTextureID)GetSpriteFromID(ENTITY_ID::LAVA2, spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_ENTITIES;
      editor->object_to_place_id = ENTITY_ID::LAVA2;
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("ice", (ImTextureID)GetSpriteFromID(ENTITY_ID::ICE, spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_ENTITIES;
      editor->object_to_place_id = ENTITY_ID::ICE;
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("ice2", (ImTextureID)GetSpriteFromID(ENTITY_ID::ICE2, spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_ENTITIES;
      editor->object_to_place_id = ENTITY_ID::ICE2;
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("ice3", (ImTextureID)GetSpriteFromID(ENTITY_ID::ICE3, spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_ENTITIES;
      editor->object_to_place_id = ENTITY_ID::ICE3;
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("puddle", (ImTextureID)GetSpriteFromID(ENTITY_ID::PUDDLE, spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_ENTITIES;
      editor->object_to_place_id = ENTITY_ID::PUDDLE;
    }
    ImGui::SameLine();
    if (ImGui::ImageButton("box_wood", (ImTextureID)GetSpriteFromID(ENTITY_ID::BOXWOOD, spriteBuffer)->texture, size))
    {
      editor->mode = EditorMode::DRAW_ENTITIES;
      editor->object_to_place_id = ENTITY_ID::BOXWOOD;
    }
    
    // Add more ids here...
    // 
    ImGui::End();
  }

 
  // put an entity on the tile or draw walls or ground
  void PlaceObject(const int x, const int y, EditorData* editor, LevelData* level, CommandBuffer* commandBuffer)
  {
    if (editor->mode == EditorMode::DRAW_ENTITIES)
    {
      // Add as an entities - As an command that gets executed
      // AddEntitiy(editor->object_to_place_id, x, y, level);
      AddCommand add(x,y,editor->object_to_place_id);
      Push(commandBuffer, add, level);
    }
    else if (editor->mode == EditorMode::DRAW_GROUND)
    {
    }
    else if (editor->mode == EditorMode::DRAW_WALLS)
    {
      int index = x + y * level->w;
      level->cells_walls[index] = 10;
      ProcessWallData(level->cells_walls, level); // update the ruletile
      
    }
  }

 
  // check our mouse inputs what to do
  void Update(EditorData* editor, Input* input, LevelData* level, CommandBuffer* commandBuffer)
  {
    // --- Add Entity ---
    if(MousePressed(input, MouseButtons::LEFT))
    {
      if(camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level))
      {
        int x;
        int y;
        camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
        PlaceObject(x, y, editor, level, commandBuffer);
      }
    }
    
    // --- Remove Entity ---
    else if(MousePressed(input, MouseButtons::RIGHT))
    {
      if(camera::GetIsPointInsideGrid(input->mouse_x, input->mouse_y, level))
      {
        int x;
        int y;
        camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);

        // Handle Wall removal
        if(editor->mode == EditorMode::DRAW_WALLS)
        {
          SDL_Log("Trying to remove wall");
          int index = x + y * level->w;
          level->cells_walls[index] = 0;
          ProcessWallData(level->cells_walls, level); // update the ruletile
        }

        // Handle Entitiy Removal
        Entity* entity = GetEntity(level, x, y);
        if(entity == nullptr){
          return;
        }
        
        if(editor->mode == EditorMode::DRAW_ENTITIES)
        {
          // RemoveEntity(x, y, level); // old Remove - Now we push remove as an command
          RemoveCommand remove(entity); // Create the remove command with the entity
          Push(commandBuffer, remove, level);
        }
      }
    } // end mouse Right
    
  }// end update

 
  // Draw a transparent version of our tile as a preview
  void DrawPreview(EditorData* editor, Input* input, SDL_Renderer* renderer, LevelData* level, Camera* camera, Sprite* spriteBuffer)
  {
    int x;
    int y;
    // convert our mouse x,y to grid x,y. passing our int's memory adresses
    camera::WorldToGrid(input->mouse_x, input->mouse_y, &x, &y, level);
    Sprite* preview = nullptr;

    switch (editor->mode)
    {
      case EditorMode::DRAW_ENTITIES:
        preview = GetSpriteFromID(editor->object_to_place_id, spriteBuffer);
        break;
      case EditorMode::DRAW_GROUND:
        preview = GetSprite(SPRITE_ID::GROUND_EDITOR, spriteBuffer);
        break;
      case EditorMode::DRAW_WALLS:
        preview = GetSprite(SPRITE_ID::WALL_EDITOR, spriteBuffer);
        break;
      case EditorMode::NONE:
        break;
    }

    
    if(preview != nullptr)
    {
      // TODO - Get ground and walls working again
      // if(editor->object_to_place_id == ENTITY_ID::GROUND || editor->object_to_place_id == ENTITY_ID::WALL){
      //   RenderSprite_Grid(preview, level, renderer, camera, x, y, 1.0f, 0.5f);// passing in our optional scale, and alpha at the end
      // }
      // else{
        RenderSprite_OnTile(preview, level, renderer, camera, x, y, 1.0f, 0.5f);
      // }
      
    }
    // TODO - look into fixing preview for entities? - using
  }

 }


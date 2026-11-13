// dev_gui.cpp
//
//
#ifdef DEBUG

#include "dev_gui.h"
#include "SDL3/SDL_video.h"
#include "arena.h"
#include "gameState.h"
#include "command.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "SDL3/SDL_render.h"
#include "imgui/imgui_internal.h"
#include "levelEditor.h"
#include "levels.h"
#include <string>
#include "common.h"

using namespace std;


// the initialize creates the context 
void DEV::Initialize(SDL_Window *window, SDL_Renderer *renderer){
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  ImGuiIO& io = ImGui::GetIO();
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  io.DisplaySize = ImVec2((float)w, (float)h);
}


void DEV::ProcessEvents(SDL_Event *event){
  ImGui_ImplSDL3_ProcessEvent(event);
}

void DEV::PreDraw(ImGuiContext* saved_context){
  // If we hot reload, take the saved context (in gameData on the exe) and assign it as current
  if(ImGui::GetCurrentContext() == nullptr){
    ImGui::SetCurrentContext(saved_context);
  }
  
  ImGui::NewFrame();
}

// Draw a bar showing arena usage
void Draw_Imgui_Arena_Usage(Arena* arena, std::string name_of_arena){
  float fraction = (float)arena->used / (float)arena->size;
  string barText = name_of_arena;
  barText += " " + to_string(arena->used);
  barText += " /" + to_string(arena->size);
  // ImVec2(-1,0) makes the bar strech the whole window
  // .c_str() converts string to a const char* , which is the data type the function expects
  ImGui::ProgressBar(fraction, ImVec2(-1,0), barText.c_str());
}


// A scrubbing slider for Undo Redo.
void Draw_History(CommandBuffer* buffer, LevelData* level){
  int sliderPos = buffer->index;

  if(ImGui::SliderInt("history", &sliderPos, 0, buffer->head)){
    while(buffer->index > sliderPos){
      Undo(buffer, level);
    }
    while(buffer->index < sliderPos){
      Redo(buffer, level);
    }
  }
}

// Calculate the FPS of our game
void DrawFPS(GameData* data)
{
  EditorData* editor = &data->editor_data;
  // Here we multiply by "dt_scaler" as well to correct the number
  // %0.f gives us no decimals
  editor->fps_buffer[editor->fps_buffer_index++] = 1.0f / *data->dt * *data->dt_scaler; // calculate the current fps
  editor->fps_buffer_index %= editor->fps_buffer_count; // write in an ring into the array
  ImGui::PlotHistogram("fps", editor->fps_buffer, editor->fps_buffer_count, 0, nullptr, 0, FPS, ImVec2( -1, 35) );
  ImGui::Text("FPS %0.f", 1 / *data->dt * *data->dt_scaler);
}


void DEV::Draw(GameData *data, SDL_Renderer *renderer){
  ImGui::Begin("Dev Tools"); // start a window
  ImGui::Text("Memory arena usage");
  
  // our specific IMGUI code will go here - code for buttons and sliders ect
  Draw_Imgui_Arena_Usage(data->arena_main, "all memory");
  Draw_Imgui_Arena_Usage(data->arena_images, "images");
  Draw_Imgui_Arena_Usage(data->arena_levels, "levels");
  Draw_Imgui_Arena_Usage(data->arena_commands, "commands");
  Draw_Imgui_Arena_Usage(data->arena_entities, "entities");
  Draw_Imgui_Arena_Usage(data->arena_input, "input");
  Draw_Imgui_Arena_Usage(data->arena_scratch, "scratch");

  Draw_History(data->scenes.gameplay.commandBuffer, GetCurrentLevel(&data->scenes.gameplay));

  DrawFPS(data);
  ImGui::SliderFloat("deltaTimeScaler", data->dt_scaler, 0.01f, 5.0f); // slider for game speed
  
  ImGui::End(); // end a dev window

  // if --- Edit mode ---
  if(data->editor_data.edit_level){
    // EDITOR::Update gets called from game.cpp
    EDITOR::DrawObjectPanel(&data->editor_data, data->spriteBuffer);
    EDITOR::DrawPreview(&data->editor_data, &data->input, renderer, GetCurrentLevel(&data->scenes.gameplay), &data->camera, data->spriteBuffer);
  }
  
  ImGui::Render(); // make a generic render
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer); // render the generic render using sdl3
}

#endif

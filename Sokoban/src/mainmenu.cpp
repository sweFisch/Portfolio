// mainmenu.cpp


#include "mainmenu.h"
#include "SDL3/SDL_scancode.h"
#include "arena.h"
#include "button.h"
#include "common.h"
#include "gameState.h"
#include "input.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <cassert>
#include <cstddef>


void InitializeMenu(MainMenu* mainmenu, Sprite* spriteBuffer, FontAtlas* font, Memory::Arena* arena_main)
{
  assert(mainmenu->initialized == false);

  // Buttons
  mainmenu->button_count = 3;
  mainmenu->buttons = ALLOC_ARRAY(arena_main, Button, mainmenu->button_count);

  
  SetupButton(&mainmenu->buttons[0],
              ButtonType::CONTINUE,
              spriteBuffer,
              {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 300, 100},
              Aligment::Centered,
              font,
              "Continue");
  
  mainmenu->buttons[0].is_dynamic = true;

  SetupButton(&mainmenu->buttons[1],
              ButtonType::START_GAME,
              spriteBuffer,
              {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 150, 300, 100},
              Aligment::Centered,
              font,
              "New Game");
  
  mainmenu->buttons[1].is_dynamic = true;
  
  SetupButton(&mainmenu->buttons[2],
              ButtonType::QUIT,
              spriteBuffer,
              {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f + 300, 200, 100},
              Aligment::Centered,
              font,
              "Quit");

  mainmenu->buttons[2].is_dynamic = true;
  
  // Parralax graphics
  mainmenu->background_horizon     = GetSprite(SPRITE_ID::Menu_Horizon, spriteBuffer);
  mainmenu->background_cloud_back  = GetSprite(SPRITE_ID::Menu_Cloud_Back, spriteBuffer);
  mainmenu->background_cloud_front = GetSprite(SPRITE_ID::Menu_Cloud_Front, spriteBuffer);
  mainmenu->background_middle      = GetSprite(SPRITE_ID::Menu_Middle, spriteBuffer);
  mainmenu->background_front       = GetSprite(SPRITE_ID::Menu_Front, spriteBuffer);

  mainmenu->activeButtonIndex = 0;
  mainmenu->initialized = true;
}


void UpdateMenu(GameData* data)
{
  MainMenu* mainmenu = &data->scenes.mainMenu;
  Input* input = &data->input;
  // Check how many active buttons we have - call helper function to count all active ones
  mainmenu->activeButtonCount = GetActiveButtonCount(mainmenu->buttons, mainmenu->button_count);
  if(mainmenu->activeButtonCount == 0){
    return;
  }
  // Allocate activeButtons buffer in arena_scratch
  mainmenu->activeButtons = ALLOC_ARRAY(data->arena_scratch, Button*, mainmenu->activeButtonCount);

  // Walk trough all buttons and store pointers to the active ones- in activeButtons array
  int index = 0;
  for (int i = 0; i < mainmenu->button_count; i++)
  {
    Button* button = &mainmenu->buttons[i];
    if(button->is_active){
      mainmenu->activeButtons[index] = button;
      index += 1;
    }
  }

  // Store a pointer to the activeButtonIndex
  int* buttonIndex = &mainmenu->activeButtonIndex;

  // Check if we are hovering over any buttons , and if we move the mouse
  bool anyHoverdOver = false;
  bool mouseMoving = input->mouse_magnitude > 0.1;
  if(mouseMoving)
  {
    // walk all active buttons and set the index to the one hovered over
    for(int i = 0; i < mainmenu->activeButtonCount; i++)
    {
      Button* button = mainmenu->activeButtons[i];
      if (IsHoveringOver(button, input->mouse_x, input->mouse_y))
      {
        anyHoverdOver = true;
        *buttonIndex = i; // sets the hovered over button as the one we would press if we click
        break;
      }
    }
  }

  // Check if arrow where used to change selection
  bool upKeyPressed = KeyPressed(input, SDL_SCANCODE_UP);
  bool downKeyPressed = KeyPressed(input, SDL_SCANCODE_DOWN);
  if(upKeyPressed||downKeyPressed)
  {
    int direction = upKeyPressed ? -1: 1;
    *buttonIndex += direction + mainmenu->activeButtonCount;
    *buttonIndex = *buttonIndex % mainmenu->activeButtonCount;
  }

  // Make a button pointer to the selected buttonIndex
  Button* selected = mainmenu->activeButtons[*buttonIndex];

  // If a selection exists - Handle confirm input, Enter,key X, Left Mouse button
  if(selected != nullptr)
  {
    if(KeyPressed(input, SDL_SCANCODE_RETURN) || KeyPressed(input, SDL_SCANCODE_X))
    {
      PressButton(mainmenu->activeButtons[*buttonIndex], data);
      return;
    }

    if(IsHoveringOver(selected, input->mouse_x, input->mouse_y))
    {
      if(MousePressed(input, MouseButtons::LEFT)){
        PressButton(selected, data);
        return;
      }
    }
  }// end nullptr check

  // TEMP test!
  // if (data->time_in_scene < 1.0f){
  //   mainmenu->buttons[0].text = "Start";
  // }
  // if(data->time_in_scene > 3.0f && data->time_in_scene< 5.0f){
  //   mainmenu->buttons[0].text = "Lol";
  // }
  // if (data->time_in_scene > 5.5f){
  //   mainmenu->buttons[0].text = "Play?";
  // }
}


void DrawMenu(MainMenu* mainmenu, SDL_Renderer* renderer, Sprite* spriteBuffer, Input* input)
{
  // Draw main menu with parralax 
  float scale = (SCREEN_HEIGHT / ((float)mainmenu->background_horizon->height * UPSCALE_FACTOR));
  scale *= 1.20f;
  float mouse_x = input->mouse_x;
  float mouse_y = input->mouse_y;
  float center_x = SCREEN_WIDTH / 2.0f;
  float center_y = SCREEN_HEIGHT / 2.0f;
  float offset_x = center_x - mouse_x;
  float offset_y = center_y - mouse_y;

  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Horizon, spriteBuffer), renderer, NULL, center_x, center_y, scale,1,0);
  
  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Cloud_Back, spriteBuffer), renderer, NULL,
                     center_x + (offset_x / 11.0f), center_y + (offset_y / 11.0f), scale, 1,0);

  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Cloud_Front, spriteBuffer), renderer, NULL,
                     center_x + (offset_x / 9.0f), center_y + (offset_y / 9.0f), scale, 1,0);
  
  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Middle, spriteBuffer), renderer, NULL,
                     center_x + (offset_x / 7.0f), center_y + (offset_y / 7.0f), scale, 1 ,0);

  RenderSprite_World(GetSprite(SPRITE_ID::Menu_Front, spriteBuffer), renderer, NULL,
                     center_x + (offset_x / 5.0f), center_y + (offset_y / 5.0f), scale, 1 , 0);

  // Simple backgound picture
  // Sprite* background = GetSprite(SPRITE_ID::titlescreen_background, spriteBuffer);
  // RenderSprite_World(GetSprite(SPRITE_ID::titlescreen_background, spriteBuffer),
  //                     renderer, NULL, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f, scale);

  // Render buttons - normal and dynamic
  for (int i = 0; i< mainmenu->activeButtonCount; i++)
  {
    Button* button = mainmenu->activeButtons[i];
    if(button->is_dynamic)
    {
      RenderButton_Dynamic(button, i == mainmenu->activeButtonIndex, renderer);
    }
    else
    {
      RenderButton(button, i == mainmenu->activeButtonIndex, renderer);
    }
  }
  mainmenu->activeButtonCount = 0; // temp BUG fix .

  
}



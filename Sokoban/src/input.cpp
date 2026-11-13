// input.cpp
#include "input.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include <cstring>




// FUNCTIONS




// fist nullptr check is for the first frame of our game and if we messed something up
bool KeyPressed(const Input* input, SDL_Scancode key)
{
  if(input->keys_previous == nullptr){
    return input->keys_current[key];
  }
  // current_key is pressed now and was not pressed last frame
  return input->keys_current[key] && (input->keys_previous[key] == false);
}

bool KeyHeld(const Input* input, SDL_Scancode key)
{
  if(input->keys_previous == nullptr){
    return false;
  }
  return input->keys_current[key] && input->keys_previous[key]; // pressed now and last frame
}

bool KeyReleased( const Input* input, SDL_Scancode key)
{
  if(input->keys_previous == nullptr){
    return false;
  }
  return (input->keys_current[key] == false) && input->keys_previous[key]; // not pressed now and was pressed last frame
}

bool KeyHeld_ForTime(const Input* input, SDL_Scancode key, float min_lenght)
{
  return input->keys_held_time[key] >= min_lenght;
}


bool AnyKeyPressed(const Input* input)
{
  for (int i = 0; i < SDL_SCANCODE_COUNT; i++){
    if(KeyPressed(input, (SDL_Scancode)i)){
      return true;
    }
  }
  return false;
}


void UpdateKeys(Input* input, float dt)
{
  for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
  {
    if(input->keys_current[i]){
      input->keys_held_time[i] += dt;
    }
    else
    {
      input->keys_held_time[i] = 0.0f;
    }
  }
  // copy the memory from current keys over the previous keys 
  memcpy((void*)input->keys_previous, input->keys_current, SDL_SCANCODE_COUNT * sizeof(bool));
}

void ResetKeyHeldTime(Input* input, SDL_Scancode key)
{
  input->keys_held_time[key] = 0.0f;
}

// Reset the memory holing the input data
void ResetAll (Input* input)
{
  memset((void*)input->keys_current, 0, sizeof(bool) * SDL_SCANCODE_COUNT);
  memset((void*)input->keys_previous, 0, sizeof(bool) * SDL_SCANCODE_COUNT);
  memset((void*)input->keys_held_time, 0, sizeof(float) * SDL_SCANCODE_COUNT);
}


// Mouse Functions
// Helper function to write the SDL_Button_xMASK more ledgeble
SDL_MouseButtonFlags ButtonToFlag(MouseButtons button){
  switch (button) {
    case MouseButtons::LEFT:
      return SDL_BUTTON_LMASK;
    case MouseButtons::MIDDLE:
      return SDL_BUTTON_MMASK;
    case MouseButtons::RIGHT:
      return SDL_BUTTON_RMASK;
      break;
  }
}


bool MousePressed(const Input* input, MouseButtons button)
{
  SDL_MouseButtonFlags flag = ButtonToFlag(button);
  // compare the current bit flags from the mouse to
  //  the flag for the mouse button we want to check
  // - 0101 example current mouse
  // - 0001 mouse button we try to check (flag)
  // bitwise & , gives 0001 (as the first flags dont match) last bit match
  // 0000 is returned if the button flag is not active
  // (mouse & flag) == 0 is treated as false
  // (mouse & flag) != 0 is treated as true
  return (input->mouse_current & flag) != 0 && (input->mouse_previous & flag) == 0;
}


bool MouseReleased(const Input* input, MouseButtons button)
{
  SDL_MouseButtonFlags flag = ButtonToFlag(button);
  return (input->mouse_current & flag) == 0 && (input->mouse_previous & flag) != 0;
}


bool MouseHeld(const Input* input, MouseButtons button)
{
  SDL_MouseButtonFlags flag = ButtonToFlag(button);
  return (input->mouse_current & flag) != 0 && (input->mouse_previous & flag) != 0;
}


bool MouseHeld_ForTime(const Input* input, MouseButtons button, float min_lenght)
{
  SDL_MouseButtonFlags flag = ButtonToFlag(button);
  return input->mouse_held_time[flag] >= min_lenght;
}


// Increment deltatime for held mouse buttons and save currently held buttons to mouse_previous
void UpdateMouse(Input* input, float dt)
{
  if(MouseHeld(input, MouseButtons::LEFT)) {
    input->mouse_held_time[(int)MouseButtons::LEFT] += dt;
  }
  else {  
    input->mouse_held_time[(int)MouseButtons::LEFT] = 0;
  }
  
  if(MouseHeld(input, MouseButtons::MIDDLE)) {
    input->mouse_held_time[(int)MouseButtons::MIDDLE] += dt;
  }
  else {  
    input->mouse_held_time[(int)MouseButtons::MIDDLE] = 0;
  }
  
  if(MouseHeld(input, MouseButtons::RIGHT))
  {
    input->mouse_held_time[(int)MouseButtons::RIGHT] += dt;
  }
  else {  
    input->mouse_held_time[(int)MouseButtons::RIGHT] = 0;
  }
  // save the currently held buttons to prev
  input->mouse_previous = input->mouse_current;
}

// input.h
#pragma once

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"

enum class MouseButtons{
  LEFT = 0,
  MIDDLE = 1,
  RIGHT = 2,
};


// STRUCTS
// Input struct for Input - mouse and  keyboard presses
struct Input{
  // Keyboard
  const bool* keys_current;
  const bool* keys_previous;
  float* keys_held_time; // an array keeping track of how long a each key has been held down
  // Mouse input
  SDL_MouseButtonFlags mouse_current;
  SDL_MouseButtonFlags mouse_previous;
  float* mouse_held_time;
  float mouse_x;
  float mouse_y;
  float mouse_x_delta;
  float mouse_y_delta;
  float mouse_magnitude;
};


// FUNCTIONS
// const is so we dont get to modify the array we put in as a parameter
bool KeyPressed(const Input* input, SDL_Scancode key); // this frame
bool KeyHeld(const Input* input, SDL_Scancode key);
bool KeyReleased( const Input* input, SDL_Scancode key); // this frame
bool KeyHeld_ForTime(const Input* input, SDL_Scancode key, float min_lenght);
void UpdateKeys(Input* input, float dt);
void ResetKeyHeldTime(Input* input, SDL_Scancode key);
void ResetAll (Input* input);

bool MousePressed(const Input* input, MouseButtons button);
bool MouseReleased(const Input* input, MouseButtons button);
bool MouseHeld(const Input* input, MouseButtons button);
bool MouseHeld_ForTime(const Input* input, MouseButtons button, float min_lenght);
void UpdateMouse(Input* input, float dt);

bool AnyKeyPressed(const Input* input);

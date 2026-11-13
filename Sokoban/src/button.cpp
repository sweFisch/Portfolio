// button.cpp

#include "button.h"
#include "collision.h"
#include "common.h"
#include "game.h"
#include "gameState.h"
#include "spriteLibrary.h"
#include <cassert>
#include "rendering.h"

void PressButton(Button* button, GameData* data)
{
    if (button == nullptr){
      return;
    }
    assert(button->is_active);
    switch (button->type)
    {
      case ButtonType::NONE:
        assert(false);
      case ButtonType::START_GAME:
        data->scenes.gameplay.currentLevelIndex = 0;
        ChangeScene(data, SCENE_TYPES::GAME);
        break;
      case ButtonType::CONTINUE:
        ChangeScene(data, SCENE_TYPES::GAME);
        break;
      case ButtonType::QUIT:
        data->running = false;
        break;
    
    }
}


int GetActiveButtonCount(Button* buttons, int count)
{
  if (count == 0) { return 0;}
  int amount = 0;
  for (int i = 0; i< count; i++)
  {
    if(buttons[i].is_active){
      amount++;
    }
  }
  return amount;
}


bool IsHoveringOver(Button* button, float x, float y)
{
  if(button == nullptr) return false;
  if(button->is_active == false ) return false;

  assert(button->type != ButtonType::NONE);

  return CheckCollisionInsideBounds(button->rect, x, y);
}


void SetupButton(Button* button, ButtonType type, Sprite* spriteBuffer, SDL_FRect rect,
                 Aligment mode, FontAtlas* font, const char* text)
{
  assert(type != ButtonType::NONE);
  button->type = type;
  button->rect = rect;
  
  if(mode == Aligment::Centered)
  {
    button->rect.x -= button->rect.w / 2;
    button->rect.y -= button->rect.h / 2;
  }
  
  button->is_active = true;

  switch(button->type)
  {
    case ButtonType::START_GAME:
      button->sprite = GetSprite(SPRITE_ID::Button_Basic, spriteBuffer);
      break;
    case ButtonType::CONTINUE:
      button->sprite = GetSprite(SPRITE_ID::Button_Basic, spriteBuffer);
      break;
    case ButtonType::QUIT:
      button->sprite = GetSprite(SPRITE_ID::Button_Basic, spriteBuffer);
      break;
    default:
      button->sprite = GetSprite(SPRITE_ID::FALLBACK, spriteBuffer);
      break;
  }

  bool hasText = !IsStringEmpty(text);
  if(font == nullptr){ // assert that if font is missing , no text is given
    assert(!hasText);
  }
  if(hasText){ // if we have text we should have a font as well
    assert(font != nullptr);
  }

  button->font = font;
  button->text = text;
}



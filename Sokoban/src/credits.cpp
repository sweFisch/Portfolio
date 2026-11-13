#include "credits.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "arena.h"
#include "common.h"
#include "gameState.h"
#include "rendering.h"
#include "spriteLibrary.h"
#include <cassert>
#include "SDL3/SDL_log.h"
#include <string>

// Data array of all credit rows
// static const
// For changing data you cannot use for(auto : all_credits_text) this will only allow for calculations but not changing memory in the struct!
CreditsTextRow all_credit_text[]{
  {"Thanks for playing!",0},
  {"Progamming by:", 0},
  {"Niklas Fischer", 0},
  {"Josef Ubaka", 0},
  {"Sound by:", 0},
  {"Josef Ubaka", 0},
  {"Art by:",0},
  {"Niklas Fischer", 0},
  {"--------------", 0},
  {"Tilepack base - by Beast", 0},
  {"www.opengameart.org/content/cave-tileset-0", 0},
  {"Title Music - by 8 Bit Universe : ", 0},
  {"8 Bit Cover Tribute to Dragonforce",0 },
  {"....", 0},
  {"Press Any Key to continue", 0}
};

int all_credit_row_count = 15; // manual track keepng of count


void InitializeCredits(Credits* credits, Sprite* spriteBuffer, FontAtlas* font, Memory::Arena* arena_main, const uint64_t* ticks_total, float* time_in_scene)
{
  if(credits->initialized == true){SDL_Log("Credits Already initialized - early return -"); return; }
  
  credits->initialized = true; // only important if we are to allocate memory here
  // credits->text = "Thanks for playing \n Made by Niklas Fischer";
  credits->scrollSpeed = 50.0f;
  credits->font = font;
  credits->yPosOffset = 0;
  credits->ticks_total = ticks_total;
  credits->time_in_scene = time_in_scene;

  // Pre Count the lenght of each CreditTextRow.text in chars
  // for (CreditsTextRow text_row : all_credit_text) - this does not work ! changes are not allowed !
  for (int j = 0; j < all_credit_row_count; j++)
  {
    for (int i = 0; i < 100; i++)
    {
      if (all_credit_text[j].text[i] == STOP_CHAR){
        all_credit_text[j].char_count = i;
        // SDL_Log("%s - %i - row count %i \n", all_credit_text[j].text, i, all_credit_text[j].char_count);
        break;
      }
    }
    credits->total_nr_charcters += all_credit_text[j].char_count; // update the total chars variable
    // SDL_Log("%i - running total char", credits->total_nr_charcters);
  }
  credits->show_chars_count = 0;
  
}


void UpdateCredits(GameData* data)
{
  Credits* credits = &data->scenes.credits;

  // initial wait
  // data->time_in_scene
  if(*credits->time_in_scene < 1.0f){ // same time_in_scene that we find in data
   credits->yPosOffset = 0.0f;
   credits->show_chars_count = 0;
   // SDL_Log("waiting before scroll");
   return; 
  }

  // Soft Scrolling
  // if(credits->yPosOffset < 200 && credits->yPosOffset > -200)
  // {
  //   credits->yPosOffset -= *data->dt * credits->scrollSpeed;
  //   // SDL_Log("  %.f \n", credits->yPosOffset);
  // }

  // scroll by finished line
  int char_count = 0;
  int row_count = 0;
  int row_offset = 40;
  for (int j = 0; j < all_credit_row_count -1; j++ ) // -1 to skip adding a line at the end
  {
    if( (int)credits->show_chars_count < char_count)
    { //char count bigger than show char running in update
      break;
    }
    
    if( (char_count + all_credit_text[j].char_count) <  credits->show_chars_count) // Less than what we should show
    { // new row compensation
      row_count++;      
    }
    char_count += all_credit_text[j].char_count;
  }
  credits->yPosOffset = row_count * row_offset * -1 + (row_count * row_offset / 2.0f);


  // (int)(*credits->time_in_scene * 100) % 10;
  // credits->show_chars_count = credits->total_nr_charcters / (int)(*credits->time_in_scene * 10);
  int charSpeed = 15;
  credits->show_chars_count += *data->dt * charSpeed;

  // Extra speed for long text blocks
  float extraSpeedThreshold = 20.0f;
  if(row_count < all_credit_row_count -2)
  {
    if(all_credit_text[row_count].char_count > extraSpeedThreshold){
      float additionalSpeed = (all_credit_text[row_count].char_count / extraSpeedThreshold);
      credits->show_chars_count += *data->dt * (charSpeed * additionalSpeed - charSpeed);
    }
  }
  // SDL_Log("%.4f", credits->show_chars_count);
}


void DrawCredits(Credits* credits, SDL_Renderer* renderer, Sprite* spriteBuffer, Input* input)
{

  // Render background - BLACK
  Sprite* background = GetSprite(SPRITE_ID::black_1x1, spriteBuffer);
  float scale = SCREEN_WIDTH;
  RenderSprite_World(background, renderer, nullptr, 0.0f, 0.0f, scale);

  int columCount = 0;
  Sprite* sprite;
  int frame;
  SpriteRenderInfo spriteRenderInfo;

  
  // Render flaming rain small balls
  columCount = 16;
  sprite = GetSprite(SPRITE_ID::FIREBALL, spriteBuffer);
  spriteRenderInfo = {frame, sprite};
  for (int k = 0; k < columCount; k++)
  {
    frame = (int)((*credits->ticks_total * 12 + k) / FPS % sprite->sprite_count_x);// + (4  * sprite->sprite_count_x);
    // int x = ((5712 * k) + 550) % SCREEN_WIDTH; // this gives a gap in the middle for the text..
    int x = ((1737) * k) % SCREEN_WIDTH;
    int y = (int)((*credits->ticks_total * (((123 * k + 1) % 560) + 300) / FPS) + k * k * 2482) % (SCREEN_HEIGHT + 100);

    float scaleFire = (float)((k * 1974) % 50)/(float)100 + 0.2f;

    SDL_FRect tilesetRect;
    int width = sprite->width / sprite->sprite_count_x;
    int height = sprite->height / sprite->sprite_count_y;
    tilesetRect.w = width;
    tilesetRect.h = height;
    Expand1Dto2D(frame,sprite->sprite_count_x, &tilesetRect.x,&tilesetRect.y);
    tilesetRect.x *= width;
    tilesetRect.y *= height;
  
    // Offset by scale ,pivot, camera
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    float final_scale = UPSCALE_FACTOR * scaleFire;
    rect.h = tilesetRect.h * final_scale;
    rect.w = tilesetRect.w * final_scale;
    // Adjusting for pivot point
    rect.x -= sprite->pivot_x * final_scale;
    rect.y -= sprite->pivot_y * final_scale;
  
    // Scaling and alpha
    SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART);
    SDL_SetTextureAlphaModFloat(sprite->texture, 1);
    // Rendering with rotation and possible fliping of sprite
    SDL_FlipMode flip = (false || spriteRenderInfo.flipped_x) ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE;
    SDL_RenderTextureRotated(renderer, sprite->texture, &tilesetRect, &rect, 0, 0, flip); // turning op for chosing how we flip the sprite
  }
  
  // Render flaming rain
  columCount = 24;
  sprite = GetSprite(SPRITE_ID::FIREBALL, spriteBuffer);
  spriteRenderInfo = {frame, sprite};
  for (int k = 0; k < columCount; k++)
  {
    frame = (int)((*credits->ticks_total * 12 + k) / FPS % sprite->sprite_count_x);// + (4  * sprite->sprite_count_x);
    int x = ((5712 * k) + 550) % SCREEN_WIDTH; // this gives a gap in the middle for the text..
    // int x = ((2937) * k) % SCREEN_WIDTH;
    int y = (int)((*credits->ticks_total * (((194 * k + 1) % 800) + 520) / FPS) + k * k * 2482) % (SCREEN_HEIGHT + 100);

    float scaleFire = (float)((k * 1974) % 75)/(float)100 + 0.5f;

    SDL_FRect tilesetRect;
    int width = sprite->width / sprite->sprite_count_x;
    int height = sprite->height / sprite->sprite_count_y;
    tilesetRect.w = width;
    tilesetRect.h = height;
    Expand1Dto2D(frame,sprite->sprite_count_x, &tilesetRect.x,&tilesetRect.y);
    tilesetRect.x *= width;
    tilesetRect.y *= height;
  
    // Offset by scale ,pivot, camera
    SDL_FRect rect;
    rect.x = x;
    rect.y = y;
    float final_scale = UPSCALE_FACTOR * scaleFire;
    rect.h = tilesetRect.h * final_scale;
    rect.w = tilesetRect.w * final_scale;
    // Adjusting for pivot point
    rect.x -= sprite->pivot_x * final_scale;
    rect.y -= sprite->pivot_y * final_scale;
  
    // Scaling and alpha
    SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART);
    SDL_SetTextureAlphaModFloat(sprite->texture, 1);
    // Rendering with rotation and possible fliping of sprite
    SDL_FlipMode flip = (false || spriteRenderInfo.flipped_x) ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE;
    SDL_RenderTextureRotated(renderer, sprite->texture, &tilesetRect, &rect, 0, 0, flip); // turning op for chosing how we flip the sprite
  }

  
  // Render burning bottom
  sprite = GetSprite(SPRITE_ID::PLAYER, spriteBuffer);
  // animation logic for fire sprites in player sprite
  frame = (int)((*credits->ticks_total * 12) / FPS % sprite->sprite_count_x) + (4  * sprite->sprite_count_x);

  columCount = (SCREEN_WIDTH)/(24 * UPSCALE_FACTOR) + 1 + 10;
  for (int k = 0; k < columCount;k++)
  {
    // animation logic for fire sprites in player sprite
    frame = (int)((*credits->ticks_total * 12 + k * 2) / FPS % sprite->sprite_count_x) + (4  * sprite->sprite_count_x);
    RenderSprite_World({frame,sprite}, renderer, nullptr, k * 16 * UPSCALE_FACTOR, SCREEN_HEIGHT);
  }

  // Animate a  Headbanging deamon -- |UU|
  sprite = GetSprite(SPRITE_ID::HEADBANG, spriteBuffer);
  frame = (int)((*credits->ticks_total * sprite->framerate) / FPS % GetSpriteCount(sprite));
  RenderSprite_World({frame, sprite}, renderer, nullptr, 480, 540);



  // --- Render TEXT ! ---
  float glyph_height = credits->font->glyphs['H'].atlasPosition.h / 2.0f;

  int char_count = 0;
  int row_count = 0;
  int row_offset = 40;
  for (int j = 0; j < all_credit_row_count; j++ )
  {
    if( (int)credits->show_chars_count < char_count)
    { //char count bigger than show char running in update
      continue;
    }
    
    if( (char_count + all_credit_text[j].char_count) <  credits->show_chars_count) // Less than what we should show
    { // render whole row as is
      if(IsStringEmpty(all_credit_text[j].text) == false)
      {
        RenderText( credits->font,
                    all_credit_text[j].text,
                    renderer,
                    nullptr,
                   (SCREEN_WIDTH / 2.0f),
                   (SCREEN_HEIGHT / 2.0f) - glyph_height + credits->yPosOffset + (row_count * row_offset),
                   Aligment::Centered
                 );
      }
      
    }
    else
    { // we should slice the text
      int slicePos = (int)credits->show_chars_count - char_count;
      std::string slice_text = all_credit_text[j].text;
      slice_text[slicePos] = STOP_CHAR;
      
      if(IsStringEmpty(slice_text.c_str()) == false)
      {
        RenderText( credits->font,
                    slice_text.c_str(),
                    renderer,
                    nullptr,
                   (SCREEN_WIDTH / 2.0f),
                   (SCREEN_HEIGHT / 2.0f) - glyph_height + credits->yPosOffset + (row_count * row_offset),
                   Aligment::Centered
                 );
      
      }
    }

    char_count += all_credit_text[j].char_count; // used for slicing text
    row_count++; // used for offset
  }


  


  // Render all text in all_credit_text row by row with delay
  // int row_count = 0;
  // int row_offset = 40;
  // for (CreditsTextRow text_row : all_credit_text)
  // {
  //   if(*credits->time_in_scene < row_count){
  //     continue;
  //   }
    
  //   if(IsStringEmpty(text_row.text) == false)
  //   {
  //     RenderText( credits->font,
  //                 text_row.text,
  //                 renderer,
  //                 nullptr,
  //                (SCREEN_WIDTH / 2.0f),
  //                (SCREEN_HEIGHT / 2.0f) - glyph_height + credits->yPosOffset + (row_count * row_offset),
  //                Aligment::Centered
  //              );
  //     row_count++;
  //   }
  // }

  
  // simple text rendering test
  // RenderText(credits->font, "Thanks for playing",
  //             renderer, nullptr, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f, Aligment::Centered);
  
  // if(IsStringEmpty(credits->text) == false)
  // {  
  //   RenderText( credits->font,
  //               credits->text,
  //               renderer,
  //               nullptr,
  //              (SCREEN_WIDTH / 2.0f),
  //              (SCREEN_HEIGHT / 2.0f) - glyph_height + credits->yPosOffset,
  //              Aligment::Centered
  //            );
    
  // if(*credits->time_in_scene > 4.0f) // time based pop up
  // {
  //   RenderText(credits->font, "Press any key to ge back to main menu",
  //               renderer, nullptr, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f, Aligment::Centered);
  // }

}



#include "rendering.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "camera.h"
#include "common.h"
#include "fontLibrary.h"
#include "spriteLibrary.h"
#include <cassert>
#include <cstddef>
#include "button.h"



void RenderSprite_World(SpriteRenderInfo spriteRenderInfo, SDL_Renderer* renderer, const Camera *camera, float x, float y, float scale, float alpha, bool flipped)
{
  // temp variables
  int frame = spriteRenderInfo.frame;
  Sprite* sprite = spriteRenderInfo.sprite;

  // Check if we are working with a tileset / spritesheet by calling 'GetSpriteCount()' a helper function
  SDL_FRect tilesetRect;
  if(GetSpriteCount(sprite) > 1)
  {
    int width = sprite->width / sprite->sprite_count_x;
    int height = sprite->height / sprite->sprite_count_y;
    tilesetRect.w = width;
    tilesetRect.h = height;
    // 1D to 2D conversion of the frame to grid space
    // tilesetRect.x = (frame % sprite->sprite_count_x) * width;
    // tilesetRect.y = (frame / sprite->sprite_count_x) * height;
    Expand1Dto2D(frame, sprite->sprite_count_x,&tilesetRect.x, &tilesetRect.y);
    tilesetRect.x *= width;
    tilesetRect.y *= height;
  }
  else
  {
    tilesetRect.w = sprite->width;
    tilesetRect.h = sprite->height;
    tilesetRect.x = 0;
    tilesetRect.y = 0;
  }
  
  
  // Offset by scale ,pivot, camera
  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  float final_scale = UPSCALE_FACTOR * scale;
  rect.h = tilesetRect.h * final_scale;
  rect.w = tilesetRect.w * final_scale;
  // Adjusting for pivot point
  rect.x -= sprite->pivot_x * final_scale;
  rect.y -= sprite->pivot_y * final_scale;
  // Adjusting for camera
  if(camera != NULL)
  {
    rect.x -= camera->camera_x;
    rect.y -= camera->camera_y;
  }
  
  // Scaling and alpha
  SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART);
  SDL_SetTextureAlphaModFloat(sprite->texture, alpha);
  // Rendering with rotation and possible fliping of sprite
  SDL_FlipMode flip = (flipped || spriteRenderInfo.flipped_x) ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE;
  SDL_RenderTextureRotated(renderer, sprite->texture, &tilesetRect, &rect, 0, 0, flip); // turning op for chosing how we flip the sprite
  
}


void RenderTile(Sprite* tileset, uint16_t cell_id, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale, float alpha)
{
  camera::GridToWorld(&x, &y, level);
  RenderSprite_World({cell_id,tileset}, renderer, camera, x, y, scale, alpha, false); 
}


void RenderSprite_OnTile(SpriteRenderInfo spriteRenderInfo, LevelData* level, SDL_Renderer* renderer, const Camera* camera, float x, float y, float scale, float alpha, bool flipped)
{
  // Helper function making entities be at the center of the tile before pivot point adjutments,
  // pivot point adjustment in RenderSprite_World will place the sprite at the location we want
  camera::GridToWorld(&x, &y, level);
  x += TILE_SIZE_PX_SCALED / 2.0f;
  y += TILE_SIZE_PX_SCALED / 2.0f;
  RenderSprite_World(spriteRenderInfo, renderer, camera, x, y, scale, alpha, flipped);
}


void RenderButton(Button* button, bool is_selected, SDL_Renderer* renderer)
{
  SDL_Texture* texture = button->sprite->texture;
  SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_SCALEMODE_PIXELART);
  uint8_t colorOverlay = is_selected ? 255: 230;
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(texture, colorOverlay, colorOverlay, colorOverlay); // overlay color blend
  SDL_RenderTexture(renderer, button->sprite->texture, NULL ,&button->rect); // book said button->texture


  // Render text in button - if we have text
  SDL_FRect rect = button->rect;
    
  if(IsStringEmpty(button->text) == false)
  {
    float glyph_height = button->font->glyphs['H'].atlasPosition.h / 2.0f;
    RenderText( button->font, button->text, renderer, nullptr,
               rect.x + (rect.w / 2.0f),
               rect.y + (rect.h / 2.0f) - glyph_height,
               Aligment::Centered);
  }
}

// Break out 9 slice to a simple render panel - ignoring frame settings as they dont do anything atm
// render text maybee to?
void RenderButton_Dynamic(Button *button, bool is_selected, SDL_Renderer *renderer)
{
  // Using 9-slicing to render streched textures with good corners 
  // a sliced texture should be 3x3 9 frames in total
  assert(button->sprite->sprite_count_x == 3);
  assert(button->sprite->sprite_count_y == 3);

  uint8_t colorOverlay = is_selected ? 255: 230;
  SDL_Texture* texture = button->sprite->texture;
  SDL_FRect rect = button->rect;

  float part_w = texture->w / 3.0f;
  float part_h = texture->h / 3.0f;
  float vertical_center_height = rect.h - (part_h * 2);
  float horizontal_center_width = rect.w - (part_w * 2);

  // floats for calculating the different parts of the button
  float right_x = rect.x + rect.w - part_w;
  float bottom_y = rect.y + rect.h - part_h;
  float center_y = rect.y + part_h;
  float center_x = rect.x + part_w;

  // setting up the destination rect for our button - rectangles where the the source (src) will be rendered
  SDL_FRect topLeftdst      = {rect.x,   rect.y,   part_w,                   part_h};
  SDL_FRect topRightdst     = {right_x,  rect.y,   part_w,                   part_h};
  SDL_FRect topCenterdst    = {center_x, rect.y,   horizontal_center_width,  part_h};
  
  SDL_FRect bottomLeftdst   = {rect.x,   bottom_y, part_w,                   part_h};
  SDL_FRect bottomRightdst  = {right_x,  bottom_y, part_w,                   part_h};
  SDL_FRect bottomCenterdst = {center_x, bottom_y, horizontal_center_width,  part_h};
  
  SDL_FRect centerLeftdst   = {rect.x,   center_y,  part_w,                  vertical_center_height};
  SDL_FRect centerRightdst  = {right_x,  center_y,  part_w,                  vertical_center_height};
  SDL_FRect centerdst       = {center_x, center_y,  horizontal_center_width, vertical_center_height};

  // Setting up the source pixels to fill those rectangles from the sprite


  SDL_FRect topLeftsrc      = {0,          0,          part_w, part_h };
  SDL_FRect topRightsrc     = {part_w * 2, 0,          part_w, part_h };
  SDL_FRect topCentersrc    = {part_w * 1, 0,          part_w, part_h };

  SDL_FRect bottomLeftsrc   = {0,          part_h * 2, part_w, part_h };
  SDL_FRect bottomRightsrc  = {part_w * 2, part_h * 2, part_w, part_h };
  SDL_FRect bottomCentersrc = {part_w * 1, part_h * 2, part_w, part_h };

  SDL_FRect centerLeftsrc   = {0,          part_h * 1, part_w, part_h };
  SDL_FRect centerRightsrc  = {part_w * 2, part_h * 1, part_w, part_h };
  SDL_FRect centersrc       = {part_w * 1, part_h * 1, part_w, part_h };

  // Final rendering - copy from button ATMi
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureColorMod(texture, colorOverlay, colorOverlay, colorOverlay); // overlay color blend

  // Render the textures specified into the right screen space rectangles specified
  SDL_RenderTexture(renderer, texture, &topLeftsrc      ,&topLeftdst );
  SDL_RenderTexture(renderer, texture, &topCentersrc    ,&topCenterdst );
  SDL_RenderTexture(renderer, texture, &topRightsrc     ,&topRightdst );
  SDL_RenderTexture(renderer, texture, &centerLeftsrc   ,&centerLeftdst );
  SDL_RenderTexture(renderer, texture, &centersrc       ,&centerdst );
  SDL_RenderTexture(renderer, texture, &centerRightsrc  ,&centerRightdst );
  SDL_RenderTexture(renderer, texture, &bottomLeftsrc   ,&bottomLeftdst );
  SDL_RenderTexture(renderer, texture, &bottomCentersrc ,&bottomCenterdst );
  SDL_RenderTexture(renderer, texture, &bottomRightsrc  ,&bottomRightdst );

  // Render text in button  - if we have text
  if(IsStringEmpty(button->text) == false)
  {
    float glyph_height = button->font->glyphs['H'].atlasPosition.h / 2.0f;
    RenderText( button->font, button->text, renderer, nullptr,
               rect.x + (rect.w / 2.0f),
               rect.y + (rect.h / 2.0f) - glyph_height,
               Aligment::Centered);
  }
}


void RenderText(FontAtlas *atlas, const char *text, SDL_Renderer *renderer, Camera *camera, const float x, const float y, Aligment mode)
{
  assert(atlas->atlasTexture != nullptr);
  float draw_position_x = x;
  float draw_position_y = y;
  // offset for camera pos if not nullptr
  if(camera != nullptr){
    draw_position_x -= camera->camera_x;
    draw_position_y -= camera->camera_y;
  }

  if(mode == Aligment::Centered)
  {
    float totalWidth = 0;
    for (int i = 0; text[i] != STOP_CHAR; i++) // Loop as long as we dont hit our stop character 
    {
      // text[i] is a char value and we used ints based on char when setting up our atlas so it maps perfectly
      totalWidth += atlas->glyphs[text[i]].atlasPosition.w;
    }
    draw_position_x -= totalWidth / 2.0f; // offset the draw position with the calculated width of the text
  }

  // Render text using the font atlas
  for (int i = 0; text[i] != STOP_CHAR; i++) // read text until we hit a stop char (normal end string char)
  {
    Glyph glyph = atlas->glyphs[text[i]]; // use the text value to index into our font atlas
    // setup a renderRectangle using the font atlas.glyphs[i].glyph width and height and the draw position x y in screen space
    SDL_FRect renderRectangle = {draw_position_x, draw_position_y,glyph.atlasPosition.w, glyph.atlasPosition.h};
    // use sdl_renderer , for rendering a slice of the atlasTexture, specified by the glyph.atlasPosition
    // in our defined renderRectangle (screen space surface we have defined)
    SDL_RenderTexture(renderer, atlas->atlasTexture, &glyph.atlasPosition, &renderRectangle);

    draw_position_x += glyph.atlasPosition.w; // increment the x drap position with the width of the char
  }
}

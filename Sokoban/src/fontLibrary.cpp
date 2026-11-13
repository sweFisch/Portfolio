// fontLibrary.cpp

#include "fontLibrary.h"
#include "SDL3/SDL_pixels.h"
// #include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <cassert>


namespace AssetManagement
{
  // ptsize will determine how large the font will be drawn in our font atlas texture
  void LoadFont(SDL_Renderer* renderer, const char* font_path, FontAtlas* fontAtlas, float ptsize)
  {
    TTF_Font* font = TTF_OpenFont(font_path, ptsize); // open the fount giving us access to the data
    assert(font != nullptr);
    SDL_Color white = {255,255,255,255}; // define a SDL_color variable

    // Proto texture we will fill with our glyphs
    int atlas_size = 1024;
    SDL_Surface* atlas_surface;
    atlas_surface = SDL_CreateSurface(atlas_size, atlas_size, SDL_PIXELFORMAT_RGBA32); // SDL_Surface lives on the CPU , SDL_Texture on the GPU
    // We have to start with a CPU represenation before we move on to a GPU texture

    // where we draw the current glyph
    int draw_point_x = 0;
    int draw_point_y = 0;
    int tallest_glyph_in_row = 0; // the tallest glyph we found in the row , used for moving down in y

    // first 0-31 chars in ASCII table are what is called control codes , these are not ment to be rendererd
    int FIRST_RELEVANT_GLYPH = 32;

    for (int i = FIRST_RELEVANT_GLYPH; i < FontAtlas::GLYPH_COUNT; i++)
    {
      // creating our glyph using a TTF_RenderGlyph_Blended
      SDL_Surface* glyph_surface = TTF_RenderGlyph_Blended(font, i, white);
      // if the character is missing we ignore and continue to the next
      if(glyph_surface == nullptr) {continue;}

      // if we would get outside of the atlas width with this character
      // Reset x, and add tallest_glyph in row to y
      if(draw_point_x + glyph_surface->w > atlas_size)
      {
        draw_point_x = 0;
        draw_point_y += tallest_glyph_in_row;
        tallest_glyph_in_row = 0;
      }
      // if the current character is taller than any other in this row update tallest_glyf_in_row
      if(tallest_glyph_in_row < glyph_surface->h)
      {
        tallest_glyph_in_row = glyph_surface->h;
      }

      // create a SDL_rect that we use for stamping the glyph to the texture atlas
      // using the position we want to draw on the atlas and the size of the glyph we have read from the font file
      SDL_Rect glyph_position = {draw_point_x, draw_point_y, glyph_surface->w, glyph_surface->h};
      // SDL_BlitSurface takes pixels from one SDL_Surface and adds it on another SDL_Surface on a specified position.
      // The null Value makes the program take the entire glyph_surface instead of a portion of it
      SDL_BlitSurface(glyph_surface, NULL, atlas_surface, &glyph_position);

      // float is becasue the Glyph struct inside FontAtlas is a FRect!
      // we use FRect bacause our SDL_Rendering we will do demands a FRect
      // Create a struct of Glyph inside our fontatlas.glyphs (Array)
      fontAtlas->glyphs[i].atlasPosition = {(float)glyph_position.x,
                                            (float)glyph_position.y,
                                            (float)glyph_position.w,
                                            (float)glyph_position.h
                                          };
      
      draw_point_x += glyph_surface->w; // shift our draw point x with the size of the processed glyph

      SDL_DestroySurface(glyph_surface); // free the memory used
    }
    // Make the final conversion to a Texture living on the GPU
    fontAtlas->atlasTexture = SDL_CreateTextureFromSurface(renderer, atlas_surface);
    SDL_DestroySurface(atlas_surface); // free the memory used
    TTF_CloseFont(font); // close the read stream of the font file we used
  }
}




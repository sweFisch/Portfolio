#include "ruleTile.h"

#include "levels.h"
#include "rendering.h"
#include <cstdint>
#include <cstdlib>
#include "SDL3/SDL_log.h"
#include "common.h"
#include "spriteLibrary.h"
#include <string>

Walls walls; // temp var


// temp data init
bool wallData[] = {
  1,0,0,0,0,0,1,0,0,1,
  0,0,0,1,0,0,0,0,1,0,
  1,1,1,0,1,1,1,0,1,0,
  1,1,1,0,0,0,0,0,1,0,
  0,0,0,0,0,0,1,1,0,0,
  0,0,1,1,0,0,1,1,0,1,
  0,0,1,0,0,0,1,1,1,1,
  0,1,1,1,0,0,1,1,1,1,
  0,0,1,0,1,0,0,0,0,0,
  1,0,0,0,1,0,0,0,0,1,
};

bool InsideBounds(int x, int y, int maxX, int maxY){
  if (x < 0) return false;
  if (y < 0) return false;
  if (x >= maxX) return false;
  if (y >= maxY) return false;
  return true;
}


enum Pattern : uint32_t{
  TOP_LEFT = 1<<0,
  TOP_MIDDLE = 1<<1,
  TOP_RIGHT = 1<<2,
  MIDDLE_LEFT = 1<<3,
  MIDDLE_MIDDLE = 1<<4,
  MIDDLE_RIGHT = 1<<5,
  BOTTOM_LEFT = 1<<6,
  BOTTOM_MIDDLE = 1<<7,
  BOTTOM_RIGHT = 1<<8
};

bool HasPattern(Pattern pattern, Pattern flags){
  return (pattern & flags) == flags;
}

// int PatternMatching(Pattern pattern)
// {
//   return 0;
// }

// Old test code
void InitializeWalls()
{
  return; // dont do anything right now
  
  // Read data from file code go here
  void * blob= malloc(sizeof(bool) * 100); // REPLACE - arena call malloc later
  walls.data = (bool*)blob;

  void * blob2 = malloc(sizeof(int) * 101);
  walls.renderData = (int*)blob2;
  
  for (int i = 0; i < walls.xMax*walls.yMax ; i++)
  {
    walls.data[i] = wallData[i];
  }
  SDL_Log("bool 0 :  %i", (int)walls.data[0]);

  // calculate the sprite number to use
  
  for (int i = 0; i <walls.xMax*walls.yMax ; i++)
  {
    walls.renderData[i] = -1; // no valid sprite
    
    if(walls.data[i] == false){continue;} // no wall continue loop

    // Found wall check surroundings
    bool patternBool[9]{false,false,false,false,false,false,false,false,false};
    Pattern pattern = (Pattern)0;
    
    // get current x, y position
    int x = 0;
    int y = 0;
    Expand1Dto2D(i, walls.xMax, &x, &y);

    int checkX;
    int checkY;

    int loopCount = 0;
    for (int offsetY = -1; offsetY <= 1 ; offsetY++)
    {
      for (int offsetX = -1; offsetX <= 1; offsetX++)
      {
        checkX = x + offsetX;
        checkY = y + offsetY;
        
        if(InsideBounds(checkX, checkY, walls.xMax, walls.yMax))
        {
          int index = checkX + checkY * walls.xMax; // current x + curr Y * maxX (width in 2D)
          patternBool[loopCount] = walls.data[index];
        }
        else
        {
          patternBool[loopCount] = true;
        }
        loopCount++;
      }
    }
    if(patternBool[0]){pattern = (Pattern)(pattern | TOP_LEFT);}
    if(patternBool[1]){pattern = (Pattern)(pattern | TOP_MIDDLE);}
    if(patternBool[2]){pattern = (Pattern)(pattern | TOP_RIGHT);}

    if(patternBool[3]){pattern = (Pattern)(pattern | MIDDLE_LEFT);}
    if(patternBool[4]){pattern = (Pattern)(pattern | MIDDLE_MIDDLE);}
    if(patternBool[5]){pattern = (Pattern)(pattern | MIDDLE_RIGHT);}
    
    if(patternBool[6]){pattern = (Pattern)(pattern | BOTTOM_LEFT);}
    if(patternBool[7]){pattern = (Pattern)(pattern | BOTTOM_MIDDLE);}
    if(patternBool[8]){pattern = (Pattern)(pattern | BOTTOM_RIGHT);}

    // check Log bitwise sanity check
    SDL_Log("--- index :  %i ----", i);
    SDL_Log("%i",pattern);

    std::string output = "";
    for (int k = 0; k < 9; k++)
    {
      // SDL_Log("%i", (int)patternBool[k]);
      if(k % 3 == 0)
      {
        SDL_Log("%s",output.c_str());
        output = "";
      }
        int value = (int)patternBool[k];
        std::string s = std::to_string(value);
        output += s + " ,";
    }
    SDL_Log("%s", output.c_str());


    walls.renderData[i] = 0; // singel tile - base case as we have found a tile above
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | TOP_MIDDLE))){
      walls.renderData[i] = 1;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE))) // Has behaviour pattern using & pattern
    {
      walls.renderData[i] = 2;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | MIDDLE_LEFT))){
      walls.renderData[i] = 3;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | MIDDLE_RIGHT))){
      walls.renderData[i] = 4;
    }

    if (  HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | TOP_MIDDLE | MIDDLE_LEFT)) // HAS walls
       && !(HasPattern(pattern, (Pattern)( BOTTOM_MIDDLE)))
       && !(HasPattern(pattern, (Pattern)( MIDDLE_RIGHT )))

  ){        // Has NOT wall
      walls.renderData[i] = 5;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | TOP_MIDDLE | MIDDLE_RIGHT))
       && !(HasPattern(pattern, (Pattern)( BOTTOM_MIDDLE)))
       && !(HasPattern(pattern, (Pattern)( MIDDLE_LEFT )))

  ){        // Has NOT wall
      walls.renderData[i] = 6;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_RIGHT))
       && !(HasPattern(pattern, (Pattern)( MIDDLE_LEFT)))
       && !(HasPattern(pattern, (Pattern)( TOP_MIDDLE )))

        ){        // Has NOT wall
      walls.renderData[i] = 7;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_LEFT))
       && !(HasPattern(pattern, (Pattern)( MIDDLE_RIGHT)))
       && !(HasPattern(pattern, (Pattern)( TOP_MIDDLE )))
    
  ){        // Has NOT wall
      walls.renderData[i] = 8;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_RIGHT | TOP_MIDDLE))
       && !(HasPattern(pattern, (Pattern)(MIDDLE_LEFT))) ){        // Has NOT wall
      walls.renderData[i] = 9;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | MIDDLE_RIGHT | MIDDLE_LEFT))
       && !(HasPattern(pattern, (Pattern) ( BOTTOM_MIDDLE )))
       && !(HasPattern(pattern, (Pattern) ( TOP_MIDDLE )))

  ){        // Has NOT wall
      walls.renderData[i] = 10;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_RIGHT | MIDDLE_LEFT))
       && !(HasPattern(pattern, (Pattern)(TOP_MIDDLE)) )){        // Has NOT wall
      walls.renderData[i] = 11;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | MIDDLE_LEFT | MIDDLE_RIGHT | TOP_MIDDLE))
       && !(HasPattern(pattern, (Pattern)(BOTTOM_MIDDLE)) )){        // Has NOT wall
      walls.renderData[i] = 12;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_LEFT | TOP_MIDDLE))
       && !(HasPattern(pattern, (Pattern)(MIDDLE_RIGHT)) )){        // Has NOT wall
      walls.renderData[i] = 13;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | TOP_MIDDLE))
       && !(HasPattern(pattern, (Pattern)(MIDDLE_LEFT )))
       && !(HasPattern(pattern, (Pattern)(MIDDLE_RIGHT)))

  ){        // Has NOT wall
      walls.renderData[i] = 14;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | TOP_MIDDLE | MIDDLE_LEFT | MIDDLE_RIGHT | BOTTOM_MIDDLE))){
      walls.renderData[i] = 15;
    }

    SDL_Log("RenderPattern[i] :  %i", walls.renderData[i]);    
    
  }

  std::string output = "";
  for (int i = 0; i < walls.xMax*walls.yMax ; i++)
  {
    if(i % walls.xMax == 0)
    {
      SDL_Log("%s",output.c_str());
      output = "";
    }
    // if (walls.renderData[i] < 0){
    //   output += "g ,";
    // }
    // else
    // {
      int value = walls.renderData[i];
      std::string s = std::to_string(value);
      // SDL_Log("%s",s.c_str());
      output += s + " ,";
    // }
    
    
  }
  SDL_Log("--");
  SDL_Log("%s",output.c_str());
}

void ProcessWallData(uint16_t* cells, LevelData* level)
{
  bool debug_rules = false;
  
  int xMax = level->w;
  int yMax = level->h;
 
  // Read data from file code go here
  void * blob= malloc(sizeof(bool) * xMax * yMax); // REPLACE - arena call malloc later
  bool* data = (bool*)blob; // walls.data

  // Make a bool array to work on - bool* data
  for (int i = 0; i < xMax * yMax ; i++)
  {
    data[i] = 0 < cells[i] ; // true if over 0 in cell
  }

  // Calculate the sprite number to use - RULE TILE 
  for (int i = 0; i <xMax * yMax ; i++)
  {
    cells[i] = -1; // no valid sprite
    
    if(data[i] == false)
    {
      continue;
    } // no wall continue loop

    // Found wall check surroundings
    bool patternBool[9]{false,false,false,false,false,false,false,false,false};
    Pattern pattern = (Pattern)0;
    
    // get current x, y position
    int x = 0;
    int y = 0;
    Expand1Dto2D(i, xMax, &x, &y);

    int checkX;
    int checkY;

    int loopCount = 0;
    for (int offsetY = -1; offsetY <= 1 ; offsetY++)
    {
      for (int offsetX = -1; offsetX <= 1; offsetX++)
      {
        checkX = x + offsetX;
        checkY = y + offsetY;
        
        if(InsideBounds(checkX, checkY, xMax, yMax))
        {
          int index = checkX + checkY * xMax; // current x + curr Y * maxX (width in 2D)
          patternBool[loopCount] = data[index];
        }
        else
        {
          patternBool[loopCount] = true;
        }
        loopCount++;
      }
    }
    if(patternBool[0]){pattern = (Pattern)(pattern | TOP_LEFT);}
    if(patternBool[1]){pattern = (Pattern)(pattern | TOP_MIDDLE);}
    if(patternBool[2]){pattern = (Pattern)(pattern | TOP_RIGHT);}

    if(patternBool[3]){pattern = (Pattern)(pattern | MIDDLE_LEFT);}
    if(patternBool[4]){pattern = (Pattern)(pattern | MIDDLE_MIDDLE);}
    if(patternBool[5]){pattern = (Pattern)(pattern | MIDDLE_RIGHT);}
    
    if(patternBool[6]){pattern = (Pattern)(pattern | BOTTOM_LEFT);}
    if(patternBool[7]){pattern = (Pattern)(pattern | BOTTOM_MIDDLE);}
    if(patternBool[8]){pattern = (Pattern)(pattern | BOTTOM_RIGHT);}

    if(debug_rules == true)
    {
      // check Log bitwise sanity check
      SDL_Log("--- index :  %i ----", i);
      SDL_Log("%i",pattern);

      std::string output = "";
      for (int k = 0; k < 9; k++)
      {
        // SDL_Log("%i", (int)patternBool[k]);
        if(k % 3 == 0)
        {
          SDL_Log("%s",output.c_str());
          output = "";
        }
          int value = (int)patternBool[k];
          std::string s = std::to_string(value);
          output += s + " ,";
      }
      SDL_Log("%s", output.c_str());
    }

    cells[i] = 0; // singel tile - base case as we have found a tile above
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | TOP_MIDDLE))){
      cells[i] = 1;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE))) // Has behaviour pattern using & pattern
    {
      cells[i] = 2;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | MIDDLE_LEFT))){
      cells[i] = 3;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | MIDDLE_RIGHT))){
      cells[i] = 4;
    }

    if (  HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | TOP_MIDDLE | MIDDLE_LEFT)) // HAS walls
       && !(HasPattern(pattern, (Pattern)( BOTTOM_MIDDLE)))
       && !(HasPattern(pattern, (Pattern)( MIDDLE_RIGHT )))

  ){        // Has NOT wall
      cells[i] = 5;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | TOP_MIDDLE | MIDDLE_RIGHT))
       && !(HasPattern(pattern, (Pattern)( BOTTOM_MIDDLE)))
       && !(HasPattern(pattern, (Pattern)( MIDDLE_LEFT )))

  ){        // Has NOT wall
      cells[i] = 6;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_RIGHT))
       && !(HasPattern(pattern, (Pattern)( MIDDLE_LEFT)))
       && !(HasPattern(pattern, (Pattern)( TOP_MIDDLE )))

        ){        // Has NOT wall
      cells[i] = 7;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_LEFT))
       && !(HasPattern(pattern, (Pattern)( MIDDLE_RIGHT)))
       && !(HasPattern(pattern, (Pattern)( TOP_MIDDLE )))
    
  ){        // Has NOT wall
      cells[i] = 8;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_RIGHT | TOP_MIDDLE))
       && !(HasPattern(pattern, (Pattern)(MIDDLE_LEFT))) ){        // Has NOT wall
      cells[i] = 9;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | MIDDLE_RIGHT | MIDDLE_LEFT))
       && !(HasPattern(pattern, (Pattern) ( BOTTOM_MIDDLE )))
       && !(HasPattern(pattern, (Pattern) ( TOP_MIDDLE )))

  ){        // Has NOT wall
      cells[i] = 10;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_RIGHT | MIDDLE_LEFT))
       && !(HasPattern(pattern, (Pattern)(TOP_MIDDLE)) )){        // Has NOT wall
      cells[i] = 11;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | MIDDLE_LEFT | MIDDLE_RIGHT | TOP_MIDDLE))
       && !(HasPattern(pattern, (Pattern)(BOTTOM_MIDDLE)) )){        // Has NOT wall
      cells[i] = 12;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | MIDDLE_LEFT | TOP_MIDDLE))
       && !(HasPattern(pattern, (Pattern)(MIDDLE_RIGHT)) )){        // Has NOT wall
      cells[i] = 13;
    }
    
    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | BOTTOM_MIDDLE | TOP_MIDDLE))
       && !(HasPattern(pattern, (Pattern)(MIDDLE_LEFT )))
       && !(HasPattern(pattern, (Pattern)(MIDDLE_RIGHT)))

  ){        // Has NOT wall
      cells[i] = 14;
    }

    if(HasPattern(pattern, (Pattern)(MIDDLE_MIDDLE | TOP_MIDDLE | MIDDLE_LEFT | MIDDLE_RIGHT | BOTTOM_MIDDLE))){
      cells[i] = 15;
    }

    if(debug_rules == true)
    {
      SDL_Log("RenderPattern[i] :  %i", cells[i]);   
    }

    // offset by 1 for walls
    cells[i] += 1;
  }

  // Get the 0 back into the cells
  for (int i = 0; i < xMax * yMax;i++)
  {
    if(cells[i] > 1000){
      cells[i] = 0;
    }
  }



  if(debug_rules == true)
  {
    std::string output = "";
    for (int i = 0; i < xMax * yMax ; i++)
    {
      if(i % xMax == 0)
      {
        SDL_Log("%s",output.c_str());
        output = "";
      }
        int value = cells[i];
        std::string s = std::to_string(value);
        output += s + " ,";
    }
    SDL_Log("--");
    SDL_Log("%s",output.c_str());
  }

}


void UpdateRuleTile()
{
  
}


void DrawRuleTile(SDL_Renderer* renderer, Sprite* spriteBuffer, Walls* wallsStruct)
{
  for (int i = 0; i <walls.xMax*walls.yMax ; i++)
  {
    if(walls.data[i] == false){continue;} // no wall continue loop
    
    // Found wall render sprite
    // get current x, y position
    int x = 0;
    int y = 0;
    Expand1Dto2D(i, walls.xMax, &x, &y);

    
    x *= TILE_SIZE_PX_SCALED;
    y *= TILE_SIZE_PX_SCALED;
    
    Sprite* sprite = GetSprite(SPRITE_ID::WALL_RULE_TILESET, spriteBuffer);
    int frame = walls.renderData[i];
    // SpriteRenderInfo spriteRI = {frame}
    RenderSprite_World({frame,sprite,false}, renderer, nullptr, x, y, 1,1, false);
  }
}



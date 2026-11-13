#pragma once
// #include "levels.h"
#include <cassert> // so we can use asserts
#include <cstdint> // so we have access to uint8_t

// forward-declare - to fix circular dependecis - we are not directly using LevelData here
// we just declare that there will be a struct called LevelData
struct LevelData; // we get a circular dependicy if we have levels.h as it also references entity.h
struct CommandBuffer;

// entety.cpp will then include levels.h and fill in the struct from that file

// using a bitwise operator  left-shift operation -  to create flags,
// << will shift one bit to the left the steps specified
// using a 32 bit number (uint32_t) having 32 (bits zeros and one) to work with
enum Behaviour : uint32_t {
  NONE = 0, // no behaviour                                          -value 0 of all bits are 0
  CAN_MOVE = 1 << 0, // 1 in the first bit - we can move             -value 1 
  IS_PLAYER = 1 << 1, // 1 in the second bit - is player             -value 2
  RESPOND_TO_INPUT = 1 << 2, // 1 in the third bit respond to input   -value 4
  IS_PETRIFIED = 1 << 3,
  CAN_ROTATE = 1 << 4,
  UNPUSHABLE = 1 << 5,
  JUMPS = 1 << 6,
  IS_PUSHING = 1 << 7,
  BURN = 1 << 8,
  MELT = 1 << 9,
  WATER = 1 << 10,
  WOOD = 1 << 11
};

// excample with 8 bits to work with - can be used as holding multiple bools 
// 00000000 = 0   - zero in first
// 00000001 = 1   - 1 in first
// 00000010 = 2   - 1 in second
// 00000011 = 3   - 1 in first and second
// 00000101 = 5   - 1 in third and first
// 01000000 = 32



// ID - should be synct with the level tilemap and map .tmj .tsx - TODO a more robust system later
// using the enum class you are forced to write the class first and then the enum
// ID::GROUND - for exemple - similar to a namespace
enum class ENTITY_ID : uint8_t {
  PLAYER = 0,
  LAVA = 1,
  ICE = 2,
  ICE2 = 3,
  ICE3 = 4,
  PUDDLE = 5,
  ROCK = 6,// not used atm but 6 needs to be empty because resons
  LAVA2 = 7,
  BOXWOOD = 8,
  

  // Trash
  DEMON = 117,
  SIREN = 118,
  GOLEM = 119,
  GHOST = 110,
  MEDUSA = 111,
};

// class makes so you haove to write Direction:: before enum
enum class Direction {
  RIGHT,
  LEFT,
  UP,
  DOWN
};

enum class Actions {
  NONE = 0,
  MOVING = 1,
  ROTATING = 2
};

inline Direction DirectionFromXY(int xDir, int yDir)
{
  assert(xDir * yDir == 0); //if multiplication of x and y is not 0 something has gone wrong
  if(xDir == 1)  { return Direction::RIGHT; }
  if(xDir == -1) { return Direction::LEFT;  }
  if(yDir == 1)  { return Direction::UP;    }
  else           { return Direction::DOWN;  }
}


// simple position struct, used in ring buffer and other
struct Position{
  int x;
  int y;
};

struct Entity
{
  Actions action;
  ENTITY_ID id;
  bool active;  
  Direction facing_current;
  Direction facing_previous;
  int strength; // how many blocks we can push
  int health;
  int maxHealth;
  
  int x; // position variables
  int y;
  int x_prev; // lerp variables
  int y_prev;
  float progress_01;


  Behaviour behaviour;
};


// Function Declarations

bool IsMoving(Entity* e);
bool IsActing(Entity* e);

void SetBehaviour(Entity* entity, Behaviour flags);
void AddBehaviour(Entity* entity, Behaviour flags);
void RemoveBehaviour(Entity* entity, Behaviour flags);
bool HasBehaviour(Entity* entity, Behaviour flags);
void InitializeBaseBehaviour(Entity* entity);

void PostMove(Entity* entity, LevelData* level, CommandBuffer* commandBuffer);
void PostRotation(Entity* entity, LevelData* level, CommandBuffer* commandBuffer, Direction from, Direction to);
void PreRotation(Entity* entity, LevelData* level, CommandBuffer* commandBuffer, Direction from, Direction to);

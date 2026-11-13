// command.h

#pragma once
#include "entity.h"
#include "levels.h"
#include <cstdint> // uint8_t



// we make it a class to require us to type 'CMD_TYPE' when accessing it
enum class CMD_TYPE : uint8_t {
  NONE = 0,
  MOVE = 1,
  ROTATE = 2,
  MODIFY_BEHAVIOUR = 3,
  ADD = 4,
  REMOVE = 5,
  SWAP_ACTIVE = 6,
  MODIFY_HEALTH = 7
};

// base struct - gets inherithed by other command types
struct Command {
  CMD_TYPE type = CMD_TYPE::NONE; // all command should set their type but if that never happends we can now catch it 
  uint32_t timestamp;
};

// Add a entity to the bord/ level
struct AddCommand : Command {
  int x; // entity position
  int y;
  ENTITY_ID id; // the ID of the entity we want to spawn

  AddCommand(int x, int y, ENTITY_ID id){
    this->x = x;
    this->y = y;
    this->id = id;
    type = CMD_TYPE::ADD;
  }
};

// Remove a entity from the board / level
struct RemoveCommand : Command {
  int x;
  int y;
  Behaviour storedBehaviour;
  ENTITY_ID storedID;

  Entity* entity;

  RemoveCommand(Entity* entity){
    this->entity = entity;
    x = entity->x;
    y = entity->y;
    storedBehaviour = entity->behaviour;
    storedID = entity->id;
    type = CMD_TYPE::REMOVE;
  }
};

// A move command that inheriths from Command - inheriting the CMD_TYPE variable
struct MoveCommand : Command {
  Entity* entity;
  int xDir;
  int yDir;

  // Constructor
  MoveCommand(Entity* entity, int xDir, int yDir){
    this->entity = entity;
    this->xDir = xDir;
    this->yDir = yDir;
    type = CMD_TYPE::MOVE;
  }
};

struct RotateCommand : Command
{
  Entity* entity;
  Direction from;
  Direction to;

  // Constructor
  RotateCommand(Entity* entity, Direction from, Direction to){
    this->entity = entity;
    this->from = from;
    this->to = to;
    type = CMD_TYPE::ROTATE;
  }
};


struct ModifyBehaviourCommand : Command {
  enum Mode{
    ADD,
    REMOVE
  };

  Entity* entity;
  Behaviour flag;
  Mode mode;

  // Constructor  
  ModifyBehaviourCommand(Entity* entity, Behaviour flag, Mode mode){
    this->entity = entity;
    this->flag = flag;
    this->mode = mode;
    type = CMD_TYPE::MODIFY_BEHAVIOUR;
  }
};

struct ModifyHealthCommand : Command
{
  // enum Mode{
  //   ADD,
  //   REMOVE
  // };

  Entity* entity;
  int oldHealth;
  int newHealth;
  
  ModifyHealthCommand(Entity* entity, int oldHealth, int newHealth){
    this->oldHealth = oldHealth;
    this->newHealth = newHealth;
    this->entity = entity;
    type = CMD_TYPE::MODIFY_HEALTH;
  }
};

struct SwapActiveEntityCommand : Command
{
  int index_current;
  int index_previous;
  int* value_to_change;

  SwapActiveEntityCommand(int* activeEntityIndex, int limit)
  {
    index_previous = *activeEntityIndex; // dereference the activeEntityIndex int pointer - storing a prev makes undo easier
    index_current = *activeEntityIndex +1;
    value_to_change = activeEntityIndex; // store the int pointer so we can change it in Execute() and Undo()
    index_current %= limit; // make the index loop around on itself (wrap back to the start)
    type = CMD_TYPE::SWAP_ACTIVE;
  }
  
};


// the union keyword makes the new AnyCommand have the same size as the largest struct it could represent
// that means when we allocate AnyCommand it will take the largest of the structs for its size,
// allowing all forms of commands to fit in the memory space allocated for one command
union AnyCommand {
  Command command;
  MoveCommand move;
  RotateCommand rotate;
  ModifyBehaviourCommand modify;
  AddCommand add;
  RemoveCommand remove;
  SwapActiveEntityCommand swapActiveEntity;
  ModifyHealthCommand mod_health;
  
  // This is a constructor, making it possible to cast MoveCommand into AnyCommand
  AnyCommand(MoveCommand mv){
    move = mv;
  };

  AnyCommand(RotateCommand rc){
    rotate = rc;
  };

  AnyCommand(ModifyBehaviourCommand mod){
    modify = mod;
  }
  AnyCommand(AddCommand add){
    this->add = add;
  }
  AnyCommand(RemoveCommand remove){
    this->remove = remove;
  }
  AnyCommand(SwapActiveEntityCommand swapActiveEntity){
    this->swapActiveEntity = swapActiveEntity;
  }
  AnyCommand(ModifyHealthCommand mod_health){
    this->mod_health = mod_health;
  }
  
}; //  ---- Any Command Union end  ----- Allowing for easy allocation and casting to the right command


struct CommandBuffer{
  AnyCommand* allCommands;
  int capacity; // bounds of the array
  int index; // between 0 -> capacity , wich command we are on
  int head; // index and head are the same when playing, head stays while index moves during undo operations keeping track on max index
  uint32_t timestamp;
};

// ------- FUNCTIONS for handling commands - Declarations -------

void Push(CommandBuffer* commandBuffer, AnyCommand cmd,LevelData* level);

void Undo(CommandBuffer* commandBuffer, LevelData* level);
void Redo(CommandBuffer* commandBuffer, LevelData* level);

void ResetCommandBuffer(CommandBuffer* buffer);

// Command.cpp -
// Command pattern , using something close to a queue
// 
#include "command.h"
#include "entity.h"
#include "levels.h"
#include <cassert>
#include <cstdint>
#include "SDL3/SDL_log.h"

enum class FromRedo {No, Yes};

// execute a command- taking in a AnyCommand and converting to the right type of action
// New commands should be added to the switch case in Execute and Undo!
void Execute(AnyCommand cmd, LevelData* level, CommandBuffer* commandBuffer, FromRedo fromRedo = FromRedo::No) // from_redo is an optional bool parameter
{
  switch (cmd.command.type)
  {
    case CMD_TYPE::NONE:
      break;
    case CMD_TYPE::MOVE:
    { // case curly braces are needed if we add variables in the case! {}
      MoveCommand* mv = &cmd.move;
      mv->entity->x_prev = mv->entity->x; // save current position to _prev
      mv->entity->y_prev = mv->entity->y;
      mv->entity->x += mv->xDir; // save new position to x
      mv->entity->y += mv->yDir;
      // if from redo, set the lerp animation to be done instantly!
      if(fromRedo == FromRedo::Yes){
        mv->entity->progress_01 = 1;
      }
      mv->entity->action = Actions::MOVING;
      if(fromRedo == FromRedo::No){
        PostMove(mv->entity, level, commandBuffer);
      }
      break;
    }
    case CMD_TYPE::ROTATE:
    {   
      RotateCommand* rotate = &cmd.rotate;
      if(!HasBehaviour(rotate->entity, CAN_ROTATE))
      { // break if missing rotate behaviour 
        break;
      }
      if(fromRedo == FromRedo::Yes){
        rotate->entity->progress_01 = 1;
      }
      rotate->entity->action = Actions::ROTATING;
      if(fromRedo == FromRedo::No){
        PreRotation(rotate->entity, level, commandBuffer, rotate->from, rotate->to);
      }
      rotate->entity->facing_previous = rotate->from;      
      rotate->entity->facing_current  = rotate->to;
      // SDL_Log("facing_current: %i",(int)rotate->to); // Logging directions
      // SDL_Log("facing_previous: %i",(int)rotate->from);
      
      if(fromRedo == FromRedo::No){
        PostRotation(rotate->entity, level, commandBuffer, rotate->from, rotate->to);
      }
      
      break;
    }
    case CMD_TYPE::MODIFY_BEHAVIOUR:
    {
      ModifyBehaviourCommand* modify = &cmd.modify;
      if(modify->mode == ModifyBehaviourCommand::ADD){
        AddBehaviour(modify->entity, modify->flag);
      }
      else{
        RemoveBehaviour(modify->entity, modify->flag);
      }
      
      break;
    }
    case CMD_TYPE::ADD:
    {
      AddCommand* add = &cmd.add;
      AddEntitiy(add->id, add->x, add->y, level);
      break;
    }
    case CMD_TYPE::REMOVE:
    {
      RemoveCommand* remove = &cmd.remove;
      RemoveEntity(remove->x, remove->y, level);
      break;
    }
    case CMD_TYPE::SWAP_ACTIVE:
    {
      SwapActiveEntityCommand* swap = &cmd.swapActiveEntity;
      *swap->value_to_change = swap->index_current;
      break;
    }
    case CMD_TYPE::MODIFY_HEALTH:
    {
      ModifyHealthCommand* mod_health = &cmd.mod_health;
      mod_health->entity->health = cmd.mod_health.newHealth;
      break;
    }
  }
}


void Push(CommandBuffer* commandBuffer, AnyCommand cmd, LevelData* level)
{
  assert(cmd.command.type != CMD_TYPE::NONE); // Assert that the command is not NONE, it should never happen!
  
  commandBuffer->allCommands[commandBuffer->index] = cmd; // write our AnyCommand to the Array Buffer at current index
  commandBuffer->allCommands[commandBuffer->index].command.timestamp = commandBuffer->timestamp; // Add timestamp to command in buffer
  commandBuffer->index++; // increment the index to the next place
  commandBuffer->head = commandBuffer->index; // move the HEAD to be at the same place as our index - only happens on push
  Execute( cmd, level, commandBuffer); // make the command execute , no need to put in ,false as it is an optional parameter
}


// move the idex back and depending on the type of command do the opposite
void Undo(CommandBuffer* commandBuffer, LevelData* level)
{
  // No more undos left
  if(commandBuffer->index == 0){
    return;
  }
  commandBuffer->index--; // move the index one step back

  AnyCommand cmd = commandBuffer->allCommands[commandBuffer->index];
  uint32_t timestamp = cmd.command.timestamp; // get the timestamp
  
  switch (cmd.command.type)
  {
    case CMD_TYPE::NONE:
      break;
    case CMD_TYPE::MOVE:
    {
      MoveCommand* mv = &cmd.move;
      mv->entity->x -= mv->xDir; // negative instead of the positive found in Execute
      mv->entity->y -= mv->yDir;
      mv->entity->progress_01 = 1; // set lerp to be done instantly!
      break;
    }
    case CMD_TYPE::ROTATE:
    {
      RotateCommand* rotate = &cmd.rotate;
      if(!HasBehaviour(rotate->entity, CAN_ROTATE)){
        break;
      }
      rotate->entity->facing_current = rotate->from;
      break;
    }
    case CMD_TYPE::MODIFY_BEHAVIOUR:
    {
      ModifyBehaviourCommand* modify = &cmd.modify;
      if(modify->mode == ModifyBehaviourCommand::ADD)
      {
        RemoveBehaviour(modify->entity, modify->flag);
      }
      else{
        AddBehaviour(modify->entity, modify->flag);
      }
      break;
    }
    case CMD_TYPE::ADD:
    {
      AddCommand* add = &cmd.add;
      RemoveEntity(add->x, add->y, level);
      break;
    }
    case CMD_TYPE::REMOVE:
    {
      RemoveCommand* remove = &cmd.remove;
      RestoreEntity(remove->storedID, remove->x, remove->y, level, remove->entity);
      Entity* entity = GetEntity(level, remove->x, remove->y);
      SetBehaviour(entity, remove->storedBehaviour); // restore the behaviour the previously removed enetity had
      break;     
    }
    case CMD_TYPE::SWAP_ACTIVE:
    {
      SwapActiveEntityCommand* swap = &cmd.swapActiveEntity;
      *swap->value_to_change = swap->index_previous;
      break;
    }    
    case CMD_TYPE::MODIFY_HEALTH:
    {
      ModifyHealthCommand* mod_health = &cmd.mod_health;
      mod_health->entity->health = cmd.mod_health.oldHealth;
      break;
    }
  }
 
  // Undo all commands executed during the same timestamp - check if larger than 0 before doing it
  // Recursive call
  if(commandBuffer->index > 0){
    if(commandBuffer->allCommands[commandBuffer->index -1].command.timestamp == timestamp){
      Undo(commandBuffer, level);
    }
  }
} // -- undo command end --


void Redo(CommandBuffer* commandBuffer, LevelData* level)
{
  AnyCommand cmd = commandBuffer->allCommands[commandBuffer->index];

  // no more redos left
  if (commandBuffer->index == commandBuffer->head) {
    return;
  } // index should never be bigger than head
  
  if (cmd.command.type == CMD_TYPE::NONE){
    return;
  }
  
  // note switched places on p 157
  Execute(cmd, level, commandBuffer, FromRedo::Yes);    // Execute command - using the optional bool to skip animation
  commandBuffer->index++; // increment index

  uint32_t timestamp = cmd.command.timestamp;

  // Recursive redo for all timestamps that are equal
  if(commandBuffer->index != commandBuffer->head)
  {
    AnyCommand nextCommand = commandBuffer->allCommands[commandBuffer->index];
    if(nextCommand.command.timestamp == timestamp)
    {
      Redo(commandBuffer,level);
    }
  }
}


void ResetCommandBuffer(CommandBuffer* buffer)
{
  buffer->index = 0;
  buffer->head = 0;
  buffer->timestamp = 0;
}

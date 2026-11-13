#include "entity.h"
#include "command.h"
#include "levels.h" // include levels here and not in .h file - solving a circular dependecy


// - compares x,y values vs the _prev x,y values - in Entity*
bool IsMoving(Entity* e)
{
  // return true if either x or y does not have the same value as their _prev conterparts - AKA we are not done moving!
  return e->x != e->x_prev || e->y != e->y_prev;
}

// Check if entity is Acting - aka performing an Action like Rotating or Moving
bool IsActing(Entity* e){
  if(e->active == false) return false;
  return e->action != Actions::NONE;
}

// overwrites the current flags with new flags
void SetBehaviour(Entity* entity, Behaviour flags)
{
  entity->behaviour = flags;
}

// combines the current flags with the flags that are input into the function (can be multiple)
// using a bitwise or | - returns 1 if either a or b is true.
void AddBehaviour(Entity* entity, Behaviour flags)
{
  entity->behaviour = (Behaviour)(entity->behaviour|flags);
}

// remove the specified flags from the behaviour
// bitwise and & - sets the bit to 1 if present in both
// bitwise not ~ - inverts all 1 to 0 (after the bitwise and & ) removing the ones spcified
void RemoveBehaviour(Entity* entity, Behaviour flags)
{
  entity->behaviour = (Behaviour)(entity->behaviour & ~flags);
}

// HasBehaviour() will check if all the flags are present in this entitys behaviour
// Checks multiple flags at once
bool HasBehaviour(Entity* entity, Behaviour flags)
{
  // behaviour = 0100 1001
  // flags     = 0000 1001
  // (behaviour & flags) will result in
  //             0000 1001  as if checks if the bit is switched on both behavior and flags
  // (behaviour & flags) == flags -> returns false in the example
  return (entity->behaviour & flags) == flags;
}

void InitializeBaseBehaviour(Entity* entity)
{
  assert(entity->active);
  switch (entity->id){
    default:
      SetBehaviour(entity, (Behaviour)NONE);
      break;
    case ENTITY_ID::PLAYER:
      {
        SetBehaviour(entity, (Behaviour)( CAN_MOVE| CAN_ROTATE | IS_PLAYER | RESPOND_TO_INPUT ));
        AddBehaviour(entity, JUMPS); // Jumps
        entity->strength = 1;
        entity->health = 4; // important else cannot move
        entity->maxHealth = 4;
        break;
      }
    case ENTITY_ID::LAVA:
      SetBehaviour(entity, (Behaviour)BURN);
      entity->health = 1;
      entity->maxHealth = 1;
      break;
    case ENTITY_ID::LAVA2:
      entity->id = ENTITY_ID::LAVA;
      SetBehaviour(entity, (Behaviour)BURN);
      entity->health = 2;
      entity->maxHealth = 2;
      break;
    case ENTITY_ID::ICE:
      SetBehaviour(entity, (Behaviour)(MELT | CAN_MOVE));
      entity->health = 3;
      entity->maxHealth = 3;
      break;
    case ENTITY_ID::ICE2:
      entity->id = ENTITY_ID::ICE;
      SetBehaviour(entity, (Behaviour)(MELT | CAN_MOVE));
      entity->health = 2;
      entity->maxHealth = 3;
      break;
    case ENTITY_ID::ICE3:
      entity->id = ENTITY_ID::ICE;
      SetBehaviour(entity, (Behaviour)(MELT | CAN_MOVE));
      entity->health = 1;
      entity->maxHealth = 3;
      break;
    case ENTITY_ID::PUDDLE:
      SetBehaviour(entity, (Behaviour)WATER);
      break;
    case ENTITY_ID::BOXWOOD:
      SetBehaviour(entity, (Behaviour)(WOOD | CAN_MOVE));
      entity->health = 1;
      entity->maxHealth = 1;
      break;
    case ENTITY_ID::ROCK:
      SetBehaviour(entity, (Behaviour)CAN_MOVE);
      break;

    // TRASH -->
    case ENTITY_ID::DEMON: // NERF to nothing
      // SetBehaviour(entity, (Behaviour)( CAN_MOVE| CAN_ROTATE | IS_PLAYER | RESPOND_TO_INPUT ));
      // AddBehaviour(entity, JUMPS); // Jumps
      // entity->strength = 1;
      break;
    case ENTITY_ID::GOLEM:
      SetBehaviour(entity, (Behaviour)( CAN_MOVE| CAN_ROTATE | IS_PLAYER | RESPOND_TO_INPUT ));
      AddBehaviour(entity, Behaviour::UNPUSHABLE); // Golems cant be pushed
      entity->strength = 999;           
      break;
    case ENTITY_ID::MEDUSA:
      SetBehaviour(entity, (Behaviour)( CAN_MOVE| CAN_ROTATE | IS_PLAYER | RESPOND_TO_INPUT ));
      AddBehaviour(entity, JUMPS); // Jumps
      entity->strength = 1; // Medusa will petrify as well in lanes it looks at
      entity->health = 3;
      break;
    case ENTITY_ID::SIREN:
      SetBehaviour(entity, (Behaviour)( CAN_MOVE| CAN_ROTATE | IS_PLAYER | RESPOND_TO_INPUT ));
      AddBehaviour(entity, JUMPS); // Jumps
      entity->strength = 0;
      break;
    case ENTITY_ID::GHOST:
      // SetBehaviour(entity, (Behaviour)(CAN_MOVE|IS_PLAYER|RESPOND_TO_INPUT));
      // SetBehaviour(entity, (Behaviour)(UNPUSHABLE));
      entity->strength = 0;
      break;
  }
}

// POST - MOVE
void PostMove(Entity* entity, LevelData* level, CommandBuffer* commandBuffer)
{
  if (entity->id == ENTITY_ID::MEDUSA){
    Entity* entity_looked_at = RaycastFirstEntity(entity->x,entity->y,entity->facing_current,level);
    if(entity_looked_at != nullptr){
      if(!HasBehaviour(entity_looked_at, Behaviour::IS_PETRIFIED)){
        // ADD - modify behavior to commands 
        ModifyBehaviourCommand modify(entity_looked_at, Behaviour::IS_PETRIFIED,ModifyBehaviourCommand::ADD);
        Push(commandBuffer, modify, level);
        // AddBehaviour(entity_looked_at, Behaviour::IS_PETRIFIED);
      }
    }
  }
}


// POST - ROTATION
void PostRotation(Entity* entity, LevelData* level, CommandBuffer* commandBuffer, Direction from, Direction to)
{
  if(from == to){
    return; // no new direction return
  }

  // ADD Petrification
  if(entity->id == ENTITY_ID::MEDUSA){
    // raycasting in the NEW (TO) direction
    Entity* entity_looked_at = RaycastFirstEntity(entity->x,entity->y, to, level);
    if(entity_looked_at != nullptr){
      if( !HasBehaviour(entity_looked_at, Behaviour::IS_PETRIFIED)){        
        // ADD - modify behavior to commands - if we are not petrified!
        ModifyBehaviourCommand modify(entity_looked_at, Behaviour::IS_PETRIFIED,ModifyBehaviourCommand::ADD);
        Push(commandBuffer, modify, level);
        // AddBehaviour(entity_looked_at, Behaviour::IS_PETRIFIED);
      }
    }
  }
}


// PRE - ROTATION
void PreRotation(Entity* entity, LevelData* level, CommandBuffer* commandBuffer, Direction from, Direction to)
{
  if(from==to){
    return; // no new direction return
  }

  // REMOVE Petrification
  if(entity->id == ENTITY_ID::MEDUSA){
    // raycasting from the PREVIOUS (FROM) direction
    Entity* entity_previously_looked_at = RaycastFirstEntity(entity->x,entity->y,from,level);

    if(entity_previously_looked_at != nullptr){
      if(HasBehaviour(entity_previously_looked_at, Behaviour::IS_PETRIFIED)){
        // REMOVE - modify behavior to commands 
        ModifyBehaviourCommand modify(entity_previously_looked_at, Behaviour::IS_PETRIFIED,ModifyBehaviourCommand::REMOVE);
        Push(commandBuffer, modify, level);
        // RemoveBehaviour(entity_previously_looked_at, Behaviour::IS_PETRIFIED);
      }
    }
  }
}





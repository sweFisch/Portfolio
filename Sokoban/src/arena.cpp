// Arena cpp
// cstring - includes memset function

#include <cstring>
#include "arena.h"

// setup the block of memory
void Memory::Initialize(Arena *arena, void* mem_start, size_t size) {
  arena->base = (unsigned char*)mem_start;
  arena->size = size;
  arena->used = 0;
}

// in the block of memory, reserve memory of the size we want and
// reurn a void* to the start of that reserved memory
void* Memory::Allocate(Arena *arena, size_t size) {
  void* front = arena->base + arena->used;
  arena->used += size;
  memset(front, 0, size); // memset will zero out the memory we allocate (removing garbage)

  return front;
}

// place the used at 0, letting new allocations override the memory
void Memory::Reset(Arena *arena) {
  arena->used = 0;
}


// creates a sub memory arena inside a already existing memory arena
Memory::Arena* Memory::CreateSubArena(Arena* parent_arena, size_t size){
  Memory::Arena* sub_arena = (Memory::Arena*) Allocate(parent_arena, sizeof(Memory::Arena));
  void* memory_start = Allocate(parent_arena,size);
  Memory::Initialize(sub_arena, memory_start, size);
  return sub_arena;
}

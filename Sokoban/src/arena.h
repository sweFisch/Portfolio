#pragma once
// Arena .h file
//

// MACRO DEFINITIONS
// Allocating memory inside an arena
#define ALLOC(arena, type) (type*)Memory::Allocate((arena), sizeof(type));
// Allocating an array inside an arena
#define ALLOC_ARRAY(arena, type, count) (type*)Memory::Allocate((arena), sizeof(type) * count);


namespace Memory{
  
  struct Arena {
    unsigned char* base;
    size_t size;
    size_t used;
  };

  void Initialize(Arena* arena, void* mem_start, size_t size);
  void* Allocate(Arena* arena, size_t size);
  void Reset(Arena* arena);
  Arena* CreateSubArena(Arena* parent_arena, size_t size);
}

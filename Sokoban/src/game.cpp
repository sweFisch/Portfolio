
#include <cassert>
#include <cstdint>
#include <cmath>
#include <exception>

#include "game.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_log.h"
#include "arena.h"
#include "audioSystem.h"
#include "camera.h" // maybe not - TODO remove?
#include "credits.h"
#include "entity.h"
#include "fontLibrary.h"
#include "gameState.h"

#ifdef DEBUG
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#endif

#include "levelEditor.h"
#include "levelRenderer.h"
#include "rendering.h"
#include "levels.h"
#include "command.h"
#include "dev_gui.h"
#include "common.h"
#include "input.h"
#include "spriteLibrary.h"
#include "tilesetLibrary.h"
#include "mainmenu.h"

#include "ruleTile.h" // TEST DEBUG

#include "audioSystem.h"
#include "saveGame.h"

// using the exter "C" scope as it calls the functions in the dll - avoids name mangling
// here the code runs so it wont need the tag before __declspec(dllexport)
extern "C"
{
  // Function InizializeGame is not ment to be called outside of game.cpp (not defined in .h file only exists in .cpp)
  void InitializeGame(Gameplay* gameplay, Arena* arena_levels, Tileset* tilesetBuffer){
    assert(gameplay->initialized == false); // should never be called twice
    gameplay->currentLevelIndex = 0;

    // level count and allocation is happening in main.cpp!
    gameplay->levelCount = 6; // this is just TEMP overriding the count 
    
    // Levels to Load
    CreateLevel(arena_levels, &gameplay->levels[0], &tilesetBuffer[(int)TILESETS::GROUND],&tilesetBuffer[(int)TILESETS::WALLS], "assets/_TiledProject/Levels/Level_1.tmj");
    CreateLevel(arena_levels, &gameplay->levels[1], &tilesetBuffer[(int)TILESETS::GROUND],&tilesetBuffer[(int)TILESETS::WALLS], "assets/_TiledProject/Levels/Level_2.tmj");
    CreateLevel(arena_levels, &gameplay->levels[2], &tilesetBuffer[(int)TILESETS::GROUND],&tilesetBuffer[(int)TILESETS::WALLS], "assets/_TiledProject/Levels/Level_3.tmj");
    CreateLevel(arena_levels, &gameplay->levels[3], &tilesetBuffer[(int)TILESETS::GROUND],&tilesetBuffer[(int)TILESETS::WALLS], "assets/_TiledProject/Levels/Level_4.tmj");
    CreateLevel(arena_levels, &gameplay->levels[4], &tilesetBuffer[(int)TILESETS::GROUND],&tilesetBuffer[(int)TILESETS::WALLS], "assets/_TiledProject/Levels/Level_5.tmj");
    CreateLevel(arena_levels, &gameplay->levels[5], &tilesetBuffer[(int)TILESETS::GROUND],&tilesetBuffer[(int)TILESETS::WALLS], "assets/_TiledProject/Levels/Level_6.tmj");
    // CreateLevel(arena_levels, &gameplay->levels[0], &tilesetBuffer[(int)TILESETS::Dungeon], "assets/levels/level_01.tmj");
    // CreateLevel(arena_levels, &gameplay->levels[1], &tilesetBuffer[(int)TILESETS::Dungeon], "assets/levels/level_02.tmj");
    // -BUG- currently crashes if we go past level 2
    
    gameplay->initialized = true;
  }

  
  void Initialize(GameData* data, SDL_Renderer* renderer, SDL_Window* window)
  {
    *data->ticks_total = 0; // set the value of the pointer to 0 , ticks total is a framecounter
    
    // Audio setup - order matters with FMOD Studio:
    // 1. start the Studio system, 2. load the banks, 3. look up the events inside them
    InitializeAudioSystem(&data->audio, data->arena_main);
    AssetManagement::LoadAllBanks(&data->audio);
    AssetManagement::LoadAllEvents(&data->audio);

    // Setup images / Sprites / Font Atlas
    AssetManagement::LoadAllSprites(data->spriteBuffer, renderer);
    AssetManagement::LoadAllTilesets(data->tilesetBuffer, data->arena_images);
    AssetManagement::LoadFont(renderer, "assets/fonts/ByteBounce.ttf", &data->font, 48);
    
    // DEV tools
    #ifdef DEBUG
    DEV::Initialize( window, renderer);
    data->imGui_context = ImGui::GetCurrentContext(); // store the context of ImGui
    #endif
    // Setup fade to black texture
    SDL_Texture* blackfade = GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer)->texture;
    SDL_SetTextureBlendMode(blackfade, SDL_BLENDMODE_BLEND);  

    InitializeGame(&data->scenes.gameplay, data->arena_levels, data->tilesetBuffer);

    LoadGame(data); // needs to be before Initilize game since it sets the level to 0
    InitializeMenu(&data->scenes.mainMenu, data->spriteBuffer, &data->font, data->arena_main);
    InitializeCredits(&data->scenes.credits, data->spriteBuffer, &data->font, data->arena_main, data->ticks_total, &data->time_in_scene); // not allocating time_in_seconds might mess with hotreloading?
    PlaySong(SONG_ID::THEME); // play music

    InitializeWalls(); // TEST
    
    ChangeScene(data, SCENE_TYPES::MAINMENU);
  }

  // Start Level is exclusive to game.cpp
  void StartLevel(Gameplay* gameplay, Arena* arena_command, Arena* arena_entities){
    ResetCommandBuffer(gameplay->commandBuffer);
    Reset(arena_command);
    CreateEntities(&gameplay->levels[gameplay->currentLevelIndex], arena_entities);
    gameplay->activePlayerIndex = 0; // reset selected player
  }

  
  void ChangeScene(GameData* data, SCENE_TYPES new_scene)
  {
    if(new_scene == data->scene_current)
    {
      SDL_Log("Trying to load the same scene - ");
      // assert(new_scene != data->scene_current); // assert that we dont try to load the scene we are currently on
    }
    data->time_in_scene = 0.0f; // Reset scene timer
    
    // Set the current and prev scenes
    data->scene_previous = data->scene_current;
    data->scene_current = new_scene;
    // set the transition type - this will help with deciding if we are on the first ever scene in our program
    data->transition.state = data->scene_previous == SCENE_TYPES::NONE ? Transition::FadeFrom : Transition::FadeTo;
    data->transition.fade_time_elapsed = 0.0f;

    switch (data->scene_current)
    {
      case SCENE_TYPES::TITLESCREEN:
        data->transition.fade_time_duration = 0.5f;
        StopAllAmbience();
        break;
      case SCENE_TYPES::MAINMENU:
        data->transition.fade_time_duration = 0.5f;// new
        PlaySong(SONG_ID::THEME);
        StopAllAmbience();
        break;
      case SCENE_TYPES::GAME:
        {
          StopSong();
          PlayAmbience(AMBIENCE_ID::FIRESPARKS);
          data->transition.fade_time_duration = 0.5f;
          Gameplay* gameplay = &data->scenes.gameplay;
          assert(gameplay->initialized);
          StartLevel(gameplay, data->arena_commands, data->arena_entities);
          break;
        }
      case SCENE_TYPES::CREDITS:
        PlaySong(SONG_ID::THEME);
        data->transition.fade_time_duration = 0.5f;
        break;
      case SCENE_TYPES::TEST_RULETILE:
        data->transition.fade_time_duration = 0.5f;
        break;
      case SCENE_TYPES::NONE:
        assert(false); // will crash our program if we get here
        break;
    }
  }

  
  bool HandleEvents(GameData *data, SDL_Event event) {
    #ifdef DEBUG

    DEV::ProcessEvents(&event); // DEV TOOL <-

    #endif

    if(event.type != SDL_EVENT_KEY_DOWN) {
      return true;
    }

    if (event.key.key == SDLK_ESCAPE){
      return false;
    }
    return true;
  }


  void UpdateTitleScreen(TitleScreen* titlescreen, const float dt)
  {

  }

  void UpdateCamera(GameData* data, float dt)
  {
    // --- Read inputs for camera -------
    float cam_speed_factor = 1.0f;

    if (KeyPressed(&data->input, SDL_SCANCODE_L)){
      data->cam_lock_to_player = !data->cam_lock_to_player;
    }
    
    if (data->cam_lock_to_player == false){
      cam_speed_factor = TILE_SIZE_PX_SCALED;
    }
    
    if(KeyHeld(&data->input,SDL_SCANCODE_A)){
      data->cam_target_x -= dt * 10.0f * cam_speed_factor;
    }
    else if(KeyHeld(&data->input,SDL_SCANCODE_D)){
      data->cam_target_x += dt * 10.0f * cam_speed_factor;
    }
    else if(KeyHeld(&data->input,SDL_SCANCODE_W)){
      data->cam_target_y -= dt * 10.0f * cam_speed_factor;
    }
    else if(KeyHeld(&data->input,SDL_SCANCODE_S)){
      data->cam_target_y += dt * 10.0f * cam_speed_factor;
    }
    // ----- Read inputs for camera - END

    Gameplay* gameplay = &data->scenes.gameplay;
    // Get active entity to lock on to
    Entity* entity = gameplay->activePlayerBuffer[gameplay->activePlayerIndex]; // GetActiveEntity() ?
    if (data->cam_lock_to_player && entity != nullptr)
    {
      data->cam_target_x = std::lerp(entity->x_prev, entity->x, entity->progress_01);
      data->cam_target_y = std::lerp(entity->y_prev, entity->y, entity->progress_01);

      data->camera.camera_x = (data->cam_target_x - GetCurrentLevel(gameplay)->w / 2.0f) * TILE_SIZE_PX_SCALED - (TILE_SIZE_PX_SCALED/2.0f);
      data->camera.camera_y = (data->cam_target_y - GetCurrentLevel(gameplay)->h / 2.0f) * TILE_SIZE_PX_SCALED - (TILE_SIZE_PX_SCALED/2.0f);
    }else{
      // Normal camera with controls
      data->camera.camera_x = data->cam_target_x;
      data->camera.camera_y = data->cam_target_y; 
    }
    
    // End Move Camera
  }

  void UpdateGame(Gameplay* gameplay, Input* input, Arena* arena_scratch , Arena* arena_commands, Arena* arena_entities, const float dt, GameData* data)
  {
    LevelData* level = GetCurrentLevel(gameplay);
    // LevelData* level = &gameplay->levels[gameplay->currentLevelIndex];

    // Creating a current keyboard bool array of all the keys  
    //Adding const to it make it so that the value of the bool stored in memory at
    // the address the pointer points to can’t be changed - SDL does this because
    // we are not supposed to change the status of the keyboard in code, we just
    // read what it is.
    const bool* keys = SDL_GetKeyboardState(nullptr);
    // const bool* keys - accualy points to an array written in C.
    // as such it points to the first memory adress
    // SDL_SCANCODE - is a enum SDL provides for writing out the numbers more easily

    // Restart level on Key - R
    if(KeyPressed(input, SDL_SCANCODE_R)){
      StartLevel(gameplay, arena_commands, arena_entities);
      return;
    }
    // Level skip forward -> N
    if(KeyPressed(input, SDL_SCANCODE_N))
    {
      gameplay->currentLevelIndex++;
      if(gameplay->currentLevelIndex >= gameplay->levelCount)
      {
        gameplay->currentLevelIndex = 0;
      }
      StartLevel(gameplay, arena_commands, arena_entities);
      return;
    }
    // Level skip backward -> B
    if(KeyPressed(input, SDL_SCANCODE_B))
    {
      gameplay->currentLevelIndex--;
      if(gameplay->currentLevelIndex < 0)
      {
        gameplay->currentLevelIndex = gameplay->levelCount - 1;
      }
      StartLevel(gameplay, arena_commands, arena_entities);
      return;
    }
    
    // READ KEY Z , SHIFT Z --- UNDO REDO
    // undo speedup ? 
    if (KeyPressed(input, SDL_SCANCODE_Z) || KeyHeld_ForTime(input, SDL_SCANCODE_Z, UNDO_REPEAT_TIME))
    {
      ResetKeyHeldTime(input, SDL_SCANCODE_Z);
      if(KeyHeld(input, SDL_SCANCODE_LSHIFT)){
        Redo(gameplay->commandBuffer, GetCurrentLevel(gameplay));
      }
      else{
        Undo(gameplay->commandBuffer,GetCurrentLevel(gameplay));
      }
    }

    // READ KEYS AND PUT INTO RING BUFFER
    if(KeyPressed(input, SDL_SCANCODE_RIGHT) || KeyHeld_ForTime(input, SDL_SCANCODE_RIGHT, KEY_REPEAT_TIME))
    {
      ResetKeyHeldTime(input, SDL_SCANCODE_RIGHT);
      // access the ring buffer , at the total write count % capacity => giving us the remainder that is inside the ring buffer
      // and after that! incrementing the input_buffer_write_count++ ! -- OBS 
      // write to the Position struct by following the struct data layout {int, int} - same as writing Position(1,0)
      gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = {1,0};
    }
    else if(KeyPressed(input, SDL_SCANCODE_LEFT) || KeyHeld_ForTime(input, SDL_SCANCODE_LEFT, KEY_REPEAT_TIME))
    {
      ResetKeyHeldTime(input, SDL_SCANCODE_LEFT);
      //   data->input_buffer[data->input_buffer_write_count++ % data->input_buffer_capacity] = {-1,0};
      // Same as below! using less syntactic sugar !
      gameplay->input_buffer[gameplay->input_buffer_write_count % gameplay->input_buffer_capacity] = Position(-1,0);
      gameplay->input_buffer_write_count++;
    }
    else if(KeyPressed(input, SDL_SCANCODE_UP) || KeyHeld_ForTime(input, SDL_SCANCODE_UP, KEY_REPEAT_TIME))
    {
      ResetKeyHeldTime(input, SDL_SCANCODE_UP);
      gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = {0,-1}; // TODO - atm Up is down
    }
    else if(KeyPressed(input, SDL_SCANCODE_DOWN) || KeyHeld_ForTime(input, SDL_SCANCODE_DOWN, KEY_REPEAT_TIME))
    {
      ResetKeyHeldTime(input, SDL_SCANCODE_DOWN);
      gameplay->input_buffer[gameplay->input_buffer_write_count++ % gameplay->input_buffer_capacity] = {0,1}; // TODO - atm Down is up
    }


    // --- MOVING Entities using the ring buffer and ANIMATIONS ---
    bool are_entities_acting = false; // used to check if animation is done processing for the input
    Entity* entityBuffer = level->entityBuffer;

    // Check if entities are moving or rotating
    for (int i = 0; i < level->entityCount; i++){
      if(IsActing(&entityBuffer[i])){
        are_entities_acting = true;
        break;
      }
    }

    // GOAL CHECK
    // Update goals - blink timer
    // Only animate the goal timer if an entity is standing on it - and has done the action to move onto it
    for(int i = 0; i < level->goalCount; i++){
      Entity* entity = GetEntity(level, level->goals[i].x, level->goals[i].y);
      if(entity != nullptr && !IsActing(entity))
      {
        level->goals[i].blink_timer += dt;
      }
      else
      {
        level->goals[i].blink_timer = 0;
      }
    }

    // Check if all goals have players on them
    if(level->goalCount > 0)
    {
      // Count all goals reached
      int goals_reached = 0;
      for (int i = 0; i < level->goalCount; i++)
      {
        Goal goal = level->goals[i];
        Entity* entity = GetEntity(level, goal.x, goal.y);
        if(entity== nullptr || IsActing(entity)){ // is acting check lets animations finish before changing level
          break;
        }
        else if(HasBehaviour(entity, Behaviour::IS_PLAYER))
        {
          goals_reached++;
        }
      }
      
      // Go to next level if all goals reached
      if (goals_reached == level->goalCount)
      {
        if (gameplay->currentLevelIndex < gameplay->levelCount - 1)
        {
          gameplay->currentLevelIndex++;
          StartLevel(gameplay, arena_commands, arena_entities);// Level complete go to next
          PlaySFX(SFX_ID::SUCCESS);
          return;          
        }
        else
        {
          ChangeScene(data, SCENE_TYPES::CREDITS);
          return;
          // TODO change scene to credits or whatever at the end
          // gameplay->currentLevelIndex = 0;
          // StartLevel(gameplay, arena_commands, arena_entities);// Level complete go to next
          // ChangeScene(GameData *data, SCENE_TYPES new_scene)
        }
      }
    } // END GOAL check    



    // LOOP TROUGH ALL ENTITIES AND UPDATE ANIMATION
    for (int i = 0; i < level->entityCount; i++) // Loop trough all entities in the current level
    {
      Entity* entity = &entityBuffer[i]; // get the entity* from the current level enteties array
      if ( !entity->active ) continue; // go to next entity in the loop
      
      // UPDATE ANIMATION
      switch (entity->action)
      {
        case Actions::NONE:
          continue;
        case Actions::MOVING:
          entity->progress_01 += MOVE_SPEED * dt;
          break;
        case Actions::ROTATING:
          entity->progress_01 += 8.0f * dt; // TODO Rotation speed var or const
          break;
      }
      // RESET ANIMATION if overshot - Note on page 374 this is another for loop - having the exact same structure
      if(entity->progress_01 >= 1)
      {
        entity->x_prev = entity->x;
        entity->y_prev = entity->y;
        entity->facing_previous = entity->facing_current;
        entity->action = Actions::NONE;
        entity->progress_01 = 0;
        if(HasBehaviour(entity, Behaviour::IS_PUSHING))
        {
          RemoveBehaviour(entity, Behaviour::IS_PUSHING);
        }
      }
    } // - End of for loop Updating animations    


    // Find how many eligable entities we have then allocate that amount to the scrach arena
    int player_count = 0;
    for (int i = 0; i < level->entityCount; i++)
    {
      if( level->entityBuffer[i].active == false){
        continue;
      }
      if(HasBehaviour(&level->entityBuffer[i], (Behaviour)(IS_PLAYER)))
      {
        player_count++;
      }
    }
    // using the entities we found allocate memory on the scratch arena - We do this to newer have stale data of possible players
    // Entity** activePayerBuffer - an array of pointers of the type Entity*
    int index = 0;
    gameplay->activePlayerBuffer = ALLOC_ARRAY(arena_scratch, Entity*, player_count);
    for (int i = 0; i < level->entityCount; i++ )
    {
      if(level->entityBuffer[i].active == false){
        continue;
      }
      if(HasBehaviour(&level->entityBuffer[i], (Behaviour)(IS_PLAYER)))
      {
        gameplay->activePlayerBuffer[index++] = &level->entityBuffer[i];
      }
    }

    
    // Change Player - Key X
    if(are_entities_acting == false &&  KeyPressed(input, SDL_SCANCODE_X) && player_count > 0)
    {
      SwapActiveEntityCommand swap(&gameplay->activePlayerIndex, player_count);
      Push(gameplay->commandBuffer, swap, GetCurrentLevel(gameplay));
      gameplay->commandBuffer->timestamp += 1;
    }

    
    // Process next input in ring buffer if animations are done - If we are not acting - Moving or Rotating this frame-
    if ( are_entities_acting == true)
    {
      return;
    }
    
    // Check if all inputs in the buffer has been processed - return early from Update Loop
    if(gameplay->input_buffer_read_count == gameplay->input_buffer_write_count)
    {
      return; // return from Update Loop!
    }

    // Entity* entity = GetActiveEntity(gameplay); // Finns inte ??? TODO - p 376
    Entity* entity = gameplay->activePlayerBuffer[gameplay->activePlayerIndex]; // substitut p 372

    if( !HasBehaviour(entity, (Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))){ // If behaviors Respond to input and can move is false
      return;
    }

    if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){ // if petrified return
      return;
    }

    // Behaviour is dead ! - return - TODO - DEBUG
    
    int xDir = gameplay->input_buffer[gameplay->input_buffer_read_count % gameplay->input_buffer_capacity].x; // set xDir to Position.x from the buffer
    int yDir = gameplay->input_buffer[gameplay->input_buffer_read_count % gameplay->input_buffer_capacity].y;

    // Handel rotation commands
    Direction new_facing = DirectionFromXY(xDir, yDir);
    if(new_facing != entity->facing_current){
      RotateCommand rotate(entity, entity->facing_current, new_facing);
      Push(gameplay->commandBuffer, rotate, level);
      return;
    }

    // If Done Moving increment timestamps of commands
    if(!IsActing(entity))
    {
      // Try to move and if we move - play sound !
      bool moved = TryMove(entity, level, gameplay->commandBuffer, xDir, yDir,entity->strength, entity->health);
      if(moved){
        PlaySFX(SFX_ID::JUMP);
      }
      
      gameplay->commandBuffer->timestamp += 1;
      gameplay->input_buffer_read_count++; // increment the input buffer read count
    }
    
  }



  // UPDATE - MAIN -  gets called in main.cpp main loop!
  void Update(GameData* data, float dt)
  {
    *data->ticks_total += 1; // increment the frame count
    if ( -0.5f < data->time_in_scene){
      data->time_in_scene += dt; // increse time in scene
    }

    // FMOD Studio only queues our Play/Stop calls - this is what actually sends them off.
    // Kept at the very top so the early returns further down can never skip it.
    UpdateAudio(&data->audio);
    
    
    TitleScreen* titlescreen = &data->scenes.titleScreen;
    Gameplay* gameplay = &data->scenes.gameplay;
    EditorData* editorData = &data->editor_data;
    Transition* transition = &data->transition;
    
    // Editor code --- Level Editor ---
    if (KeyPressed(&data->input, SDL_SCANCODE_F2)){
      editorData->edit_level = !editorData->edit_level; // toggle editing on / off
      if(editorData->edit_level){
        SDL_Log("  --- Edit Mode --- ");
       }else{
        SDL_Log("  --- Normal Mode --- ");
       }
    }
    // if -- EDIT MODE --- UPDATE
    if(editorData->edit_level && data->scene_current == SCENE_TYPES::GAME){
      EDITOR::Update(&data->editor_data, &data->input, GetCurrentLevel(gameplay),gameplay->commandBuffer);
    }

    // Temp key for going to  --- Title Screen ---
    if (KeyPressed(&data->input, SDL_SCANCODE_5)) { // go to title screen if pressing 5 on keyboard
      ChangeScene(data, SCENE_TYPES::TITLESCREEN);
      return;
    }
    if (KeyPressed(&data->input, SDL_SCANCODE_6)){
      ChangeScene(data, SCENE_TYPES::CREDITS);
      return;
    }
    if (KeyPressed(&data->input, SDL_SCANCODE_4)){
      ChangeScene(data, SCENE_TYPES::MAINMENU);
      return;
    }
    if(KeyPressed(&data->input, SDL_SCANCODE_1)){
      ChangeScene(data, SCENE_TYPES::TEST_RULETILE);
      return;
    }

    // Temp key for saving the game
    
    if (KeyPressed(&data->input, SDL_SCANCODE_F5)){
      SaveGame(data);
    }
    
    // Level Transition code
    if(transition->state != Transition::Inactive){
      transition->fade_time_elapsed += dt;
      if(transition->fade_time_elapsed >= transition->fade_time_duration){
        transition->fade_time_elapsed = 0;
        switch (transition->state)
        {
          case Transition::Inactive:
            break;
          case Transition::FadeTo:
            transition->state = Transition::FadeFrom;
            break;
          case Transition::FadeFrom:
            transition->state = Transition::Inactive;
            break;
        }
      }
    }


    // Update the appropriate Update function
    switch (data->scene_current)
    {
      case SCENE_TYPES::TITLESCREEN:
        UpdateTitleScreen(titlescreen, dt);
        if(AnyKeyPressed(&data->input))
        {
          if(transition->state == Transition::FadeTo || transition->state == Transition::Inactive){
            ChangeScene(data, SCENE_TYPES::MAINMENU);
          }
        }
        break;
      case SCENE_TYPES::MAINMENU:
        UpdateMenu(data);
        break;
      case SCENE_TYPES::GAME:
        UpdateGame(gameplay, &data->input, data->arena_scratch, data->arena_commands, data->arena_entities, dt, data);
        break;
      case SCENE_TYPES::CREDITS:
        UpdateCredits(data);
        if(AnyKeyPressed(&data->input))
        {
          Credits* credits = &data->scenes.credits;
          if(credits->show_chars_count < credits->total_nr_charcters){
            credits->show_chars_count = credits->total_nr_charcters + 1;
          }
          else if(transition->state == Transition::FadeTo || transition->state == Transition::Inactive){
            ChangeScene(data, SCENE_TYPES::MAINMENU);
          }
        }
        break;
      case SCENE_TYPES::TEST_RULETILE:
        {
          UpdateRuleTile();
          break;
        }
      case SCENE_TYPES::NONE:
        assert(false);
        break;
    }
           
  }
}

  void DrawScene(GameData* data, SCENE_TYPES scene_type, SDL_Renderer* renderer)
  {
    switch (scene_type) {
      case SCENE_TYPES::TITLESCREEN:
      {
        Sprite* background = GetSprite(SPRITE_ID::titlescreen_background, data->spriteBuffer);
        float scale = (float)SCREEN_HEIGHT / ((float)background->height * UPSCALE_FACTOR); // scale to fill screen height
        RenderSprite_World(background, renderer, &data->camera, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT / 2.0f, scale);
        break;
      }
      case SCENE_TYPES::MAINMENU:
      {
        DrawMenu(&data->scenes.mainMenu, renderer, data->spriteBuffer, &data->input);
        // Test render text in menue
        // RenderText(&data->font, "Bring a towel on your adventure",
        //            renderer, &data->camera,SCREEN_WIDTH/2.0f,SCREEN_HEIGHT/2.0f,Aligment::Centered);
        break;
      }
      case SCENE_TYPES::GAME:
      {        
        RenderLevel(data, renderer);
        RenderEntities(data, renderer);
        break;
      }
      case SCENE_TYPES::CREDITS:
        {
          // TODO add bool for reseting animation
          
          DrawCredits(&data->scenes.credits, renderer, data->spriteBuffer, &data->input);
          break;
        }
      case SCENE_TYPES::TEST_RULETILE:
        {
          Walls* temp;
          DrawRuleTile(renderer, data->spriteBuffer, temp);
          break;
        }
      case SCENE_TYPES::NONE:
        assert(false); // Crash the program
        break;
    }
    
  }

  
  void Draw(GameData* data, SDL_Renderer* renderer)
  {
    #ifdef DEBUG

    DEV::PreDraw(data->imGui_context); // DEV TOOLS

    #endif        
    SDL_SetRenderDrawColor(renderer,30,30,30,255);  // sets the color and alpha to render
    SDL_RenderClear(renderer);  // Clears the renderer from everything drawn before and sets the color to the one specified above

    // We check the state of our transition and do accordingly
    switch (data->transition.state) {
      case Transition::Inactive:
        DrawScene(data,data->scene_current,renderer);
        break;
      case Transition::FadeTo:
      {
        DrawScene(data, data->scene_previous, renderer);
        float alpha = data->transition.fade_time_elapsed / data->transition.fade_time_duration;
        RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0.0f, 0.0f, SCREEN_WIDTH, alpha);
        break;
      }
      case Transition::FadeFrom:
      {
        DrawScene(data, data->scene_current, renderer);
        float alpha = 1 - data->transition.fade_time_elapsed / data->transition.fade_time_duration;
        RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0.0f, 0.0f, SCREEN_WIDTH, alpha);
        break;    
      }
    }
    
    #ifdef DEBUG

    DEV::Draw(data, renderer); // DEV TOOLS - has to happend before SDL_RenderPresent()
    // Draw everything we did to the pixels on screen with SDL-Renderer-Present
    // so take everything drawn to the backbuffer and put it out - Done once per frame

    #endif
    SDL_RenderPresent(renderer);  
  }

  void OnQuit(GameData* data, SDL_Renderer* renderer)
  {
    SaveGame(data);
    ShutdownAudioSystem(g_audioSystem); // unloads the banks and shuts the Studio system down
    SDL_DestroyRenderer(renderer);
  }
  

  // Recursive try to move to the next tile - 
  bool TryMove(Entity* mover, LevelData* level, CommandBuffer* cmd_buffer, int xDir, int yDir, int strength, int health)
  {
    // strenght is allowing for how many blocks we can affect by pushing
    if(strength < 0){
      return false;
    }
    

    if(health <= 0)
    {
     // player eller ice behvior removal
     // sprite change to dead eller remove
     // if ice remove fire
     return false; 
    }
  
    // if we try to move  - Exit clause
    if(HasBehaviour(mover, CAN_MOVE) == false )
    {
      return false;
    }

    // check if the tile is not an entity and is ground - Exit clause
    int test_x = mover->x + xDir;
    int test_y = mover->y + yDir;
    Entity* stepInto_entity = GetEntity(level ,test_x, test_y);
  
    if(stepInto_entity == nullptr )
    {
      if(IsWalkable(test_x, test_y, level))
      {
        MoveCommand mv(mover,xDir,yDir);   // Create a new move command - using a constructor
        Push(cmd_buffer, mv, level);     // Push the command to be executed and saved to the buffer
        return true;
      }
      return false;
    }

    if(HasBehaviour(stepInto_entity, (Behaviour)WATER))
    {
      // player restore to max health
      if(HasBehaviour(mover, (Behaviour)IS_PLAYER)){
        AnyCommand restoreHealth = ModifyHealthCommand(mover, mover->health, mover->maxHealth); // restore to max healthi
        Push(cmd_buffer, restoreHealth, level);
        AnyCommand removeWaterCMD = RemoveCommand(stepInto_entity);
        Push(cmd_buffer, removeWaterCMD, level);
        PlaySFX(SFX_ID::DROPLET);
      }
      // Add health to melt objects
      if(HasBehaviour(mover, (Behaviour)MELT) && mover->health < mover->maxHealth)
      {
        int newHealth = mover->health + 1;
        AnyCommand restoreHealth = ModifyHealthCommand(mover, mover->health, newHealth); // add health to melt objects
        Push(cmd_buffer, restoreHealth, level);
        AnyCommand removeWaterCMD = RemoveCommand(stepInto_entity);
        Push(cmd_buffer, removeWaterCMD, level);
        PlaySFX(SFX_ID::DROPLET);
      }
      MoveCommand mv(mover,xDir,yDir);   // Create a new move command - using a constructor
      Push(cmd_buffer, mv, level);     // Push the command to be executed and saved to the buffer
      return true;
    }

    if (HasBehaviour(stepInto_entity, Behaviour::BURN))
    {
      if (HasBehaviour(mover, Behaviour::MELT))
      {
        PlaySFX(SFX_ID::HISS);
        SDL_Log("try move melt into burn");
        // damage burning thing
        // damage ice block
        
        int moverNewHealth = mover->health - stepInto_entity->health; // ice block less health
        int burnHealth = stepInto_entity->health - mover->health; // whatever we have burning - health of melting entity
        
        AnyCommand moverHealthCMD = ModifyHealthCommand(mover,mover->health,moverNewHealth);
        Push(cmd_buffer, moverHealthCMD, level);
        
        AnyCommand burnHealtCMD = ModifyHealthCommand(stepInto_entity, stepInto_entity->health, burnHealth);
        Push(cmd_buffer, burnHealtCMD, level);

        // save stepInto pos for adding puddle multiple
        int pX = stepInto_entity->x;
        int pY = stepInto_entity->y;
        // save mover pos for adding puddle on last
        
        if(stepInto_entity->health <= 0)
        {
          // SDL_Log("Lava DEAD");
          AnyCommand remove = RemoveCommand(stepInto_entity);
          Push(cmd_buffer, remove, level);

          // in case we want water to be after each lava contact
          // AnyCommand addPuddle = AddCommand(pX, pY, ENTITY_ID::PUDDLE);
          // Push(cmd_buffer, addPuddle, level);
        }
        if(mover->health == 0) // if exacly 0 remove ice and add puddle
        {
          //SDL_Log("Ice remove");
          AnyCommand remove_ice = RemoveCommand(mover);
          Push(cmd_buffer, remove_ice, level);
          
          AnyCommand addPuddle = AddCommand(pX, pY, ENTITY_ID::PUDDLE);
          Push(cmd_buffer, addPuddle, level);
          return true;
        }
        else if(mover->health < 0) // if ice is less than 0 dont create a puddle
        {
          //SDL_Log("Ice remove");
          AnyCommand remove_ice = RemoveCommand(mover);
          Push(cmd_buffer, remove_ice, level);
        }
        
        MoveCommand mv(mover,xDir,yDir);   // Create a new move command - using a constructor
        Push(cmd_buffer, mv, level);     // Push the command to be executed and saved to the buffer

        // sliding Ice cubes
        // if(mover != nullptr && mover->health > 0)
        // {
        //   TryMove(mover, level, cmd_buffer, xDir, yDir, strength, health);
        // }
        
        return true;
      }
      
      if (HasBehaviour(mover, Behaviour::IS_PLAYER))
      {
        PlaySFX(SFX_ID::BURNSTEP);
        SDL_Log("try move player into burn");
        // damage player
        int moverNewHealth = mover->health - stepInto_entity->health; // player remove helth by fire health
        AnyCommand moverHealthCMD = ModifyHealthCommand(mover,mover->health,moverNewHealth);
        Push(cmd_buffer, moverHealthCMD, level);
        
        if(mover->health <= 0){
          SDL_Log("Player DEAD");
        }
        MoveCommand mv(mover,xDir,yDir);   // Create a new move command - using a constructor
        Push(cmd_buffer, mv, level);     // Push the command to be executed and saved to the buffer
        return true;
      }

      // Wooden things
      if(HasBehaviour(mover, Behaviour::WOOD))
      {
        PlaySFX(SFX_ID::BURNSTEP);
        SDL_Log("try move Wood into Burn");
        // damage Wood
        int moverNewHealth = mover->health - stepInto_entity->health; // remove helth by fire health
        AnyCommand moverHealthCMD = ModifyHealthCommand(mover,mover->health,moverNewHealth);
        Push(cmd_buffer, moverHealthCMD, level);
        
        if(mover->health <= 0){
          AnyCommand remove_wood = RemoveCommand(mover);
          Push(cmd_buffer, remove_wood, level);
        }
        return true;
      }
      
    } // end steped into burn

    // If there is an entety on the tile we try to step to and it can move we call TryMove Recurcive
    if(HasBehaviour(stepInto_entity, CAN_MOVE) && !HasBehaviour(stepInto_entity, UNPUSHABLE))
    {
      // call Try move again using the entity we steped on that can move
      // If it also can move it will return true
      // And we can move as well
      // strength gets decremented before we call the new function
      PlaySFX(SFX_ID::PUSH);
      if(TryMove(stepInto_entity, level, cmd_buffer, xDir, yDir, --strength, health)){
        MoveCommand mv(mover, xDir,yDir);
        AddBehaviour(mover, Behaviour::IS_PUSHING); // added behaviour for when we are pushing
        Push(cmd_buffer, mv, level);     // Push the command to be executed and saved to the buffer
        return true;
      }
    }
    // Cannot move
    return false;
  }

// ---------  extern C end bracket --------------------




// // ---- Old move logic!----
// // create a x y var for the new location we try to go
// int stepInto_x = entity->x + xChange;
// int stepInto_y = entity->y + yChange;
// // get the entity (if there is any) at the new location
// Entity* stepInto_entity = data->GetCurrentLevel()->GetEntity(stepInto_x, stepInto_y);
// uint8_t stepInto_id = data->GetCurrentLevel()->GetCellID(stepInto_x, stepInto_y);
// // if we did not step into an entity
// if(stepInto_entity == nullptr)
// {
//   // check what kind of id we steped into
//   if(stepInto_tile_id == (uint8_t)ID::GROUND)
//   {
//       entity->x = stepInto_x;
//       entity->y = stepInto_y;
//   }
// }

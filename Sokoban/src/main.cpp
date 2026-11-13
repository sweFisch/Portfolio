#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <propidl.h>
#include <windows.h>
#include <fileapi.h>
#include <cstdio>
#include <fstream>// for saving binary snapshot - gives access to ofstream and ifstream for reading and writing
#include <wingdi.h>

// best practice is to start with <> headers,
// then the ones we have not written our self
// and lastly our own code base

// cmake already includes the include folder making the full path unnessecary
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_log.h"

#include "SDL3/SDL_video.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "command.h"
#include "common.h"
#include "arena.h"
#include "entity.h"
#include "gameState.h"
#include "input.h"
#include "levels.h"
#include "spriteLibrary.h"
#include "tilesetLibrary.h"

// SDL  documentation : https://wiki.libsdl.org/SDL3/FrontPage

SDL_Window* window;
SDL_Renderer* renderer;

Uint64 NOW;
Uint64 PREV;

// constexpr makes the variable compiletime constant this implies that the pointer itself cannot change after initialization
// and that the compiler knows the value during compilation
// constexpr const means that the pointer and the memory is not allowed to change
// const char* point to the first character in a series of i chars,
// const also means that they are not allowed to change in memory 
constexpr const char* NAME_OF_DLL = "sokoban_iaf_game.dll";
constexpr const char* NAME_OF_TEMP_DLL = "sokoban_iaf_temp.dll";

// This is for accessing the we put into a dll (__declspec(dllexport) in our game.h file)
// it needs to be the exact same function signature as the function you try to point to
// This is function pointers
typedef void (*Function_Initialize) (GameData* data, SDL_Renderer* renderer, SDL_Window* window);
typedef bool (*Function_HandleEvents) (GameData* data, SDL_Event event);
typedef void (*Function_Update) (GameData* data, float dt);
typedef void (*Function_Draw) (GameData* data, SDL_Renderer* renderer);
typedef void (*Function_OnQuit) (GameData* data, SDL_Renderer* renderer);

// example of doing function pointer witout typedef?
// void (*initFunc)(GameData*data) = MyInitFunction
// using the typedef above we can do instead
// Function_Initialize initFunc = MyInitFunction;
// it has to be the exact names the defined functions in the game.h file
constexpr const char* NAME_OF_FUNC_INIT = "Initialize";
constexpr const char* NAME_OF_FUNC_HANDLE_EVENTS = "HandleEvents";
constexpr const char* NAME_OF_FUNC_UPDATE = "Update";
constexpr const char* NAME_OF_FUNC_DRAW = "Draw";
constexpr const char* NAME_OF_FUNC_ON_QUIT = "OnQuit";
// constexpr - means that this pointer can only point to this and never any other
// const - means that the memory there is not allowed to change

// collect all variables used for hotreload structure
struct DLL_INFO{
	HMODULE dll; // HMOUDLE - pointer to either a dll or an exe
	//filetime is used for checking if the file has been recompiled since last time
	FILETIME timestamp; // 64 bit value - re // 64 bit value - representing the time since jan 1, 1601, in 100 nanosecond intervalls

	// Setting up the Function pointers we declared above
	Function_Initialize initialize;
	Function_HandleEvents handleEvents;
	Function_Update update;
	Function_Draw draw;
	Function_OnQuit onQuit;
};

// find dll file and return timestamp
FILETIME GetTimestamp(){
	WIN32_FIND_DATA data;
	HANDLE handle = FindFirstFile(NAME_OF_DLL, &data);
	FILETIME time_of_last_change = data.ftLastWriteTime;
	FindClose(handle); // FindFirstFile allocates memory - this will free that memory
	return time_of_last_change;
}

// find the compiled dll
// create a temp copy of it
// then store the copy into our provided DLL_INFO struct (info)
// 
bool LoadDLL(DLL_INFO* info, int depth = 0){
	printf("- loading dll %i -", depth);

	// if we have tried and failed try again until depth of 20
	// recursive quit statement
	if (depth > 20){
		printf("-failed to write temp DLL--ERROR--");
		return false;
	}
	bool success = CopyFile(NAME_OF_DLL, NAME_OF_TEMP_DLL, false); // CopyFile is microsoft function

	// recursive call this function until a depth of 20 ,
	// if we dont succed in copying the file
	if(!success){
		Sleep(50);
		return LoadDLL(info,depth +1);
	}

	// load library into memory of info->dll
	info->dll = LoadLibrary(NAME_OF_TEMP_DLL);

	if(info->dll == nullptr){
		printf("could not load dll");
		return false;
	}
	
	// Set the function pointers to the functions in the dll
	// GetProcAddress - search for the function in the dll with the NAME_OF_FUNC_ xxx we defined
	info->initialize = (Function_Initialize)GetProcAddress(info->dll, NAME_OF_FUNC_INIT);
	info->handleEvents = (Function_HandleEvents)GetProcAddress(info->dll, NAME_OF_FUNC_HANDLE_EVENTS);
	info->update = (Function_Update)GetProcAddress(info->dll, NAME_OF_FUNC_UPDATE);
	info->draw = (Function_Draw)GetProcAddress(info->dll, NAME_OF_FUNC_DRAW);
	info->onQuit = (Function_OnQuit)GetProcAddress(info->dll, NAME_OF_FUNC_ON_QUIT);

	info->timestamp = GetTimestamp();

	return true;
}

void UnloadDLL(DLL_INFO* info){
	FreeLibrary(info->dll); // remove the memory here
	info->dll = nullptr; // make the pointer null
	DeleteFile(NAME_OF_TEMP_DLL); // 
}

// Check if timestamp is changed and unload old and load new
void DLL_CheckStatus(DLL_INFO* dll){
	FILETIME timestamp = GetTimestamp();
	bool is_timestamp_changed = CompareFileTime(&dll->timestamp, &timestamp) != 0;
	if(is_timestamp_changed){
		UnloadDLL(dll);
		LoadDLL(dll);
	}
}



void* AllocateGameMemory(){
	void* blob = malloc(GAME_MEMORY_ALLOWANCE);
	if (blob == nullptr) {
		printf("fatal error_ could not allocate memory");
		return nullptr;
	}
	printf("memory succesfully allocated");
	return blob;
}


void SDL_Setup(){
	SDL_SetAppMetadata("firstGame", "v0.01", "nfGame"); // Setting metadata - should be before SDL_Init
	// SDL_Init takes a bitwise (or flags / special power of 2 enums)
	// As input, se page 75, that makes so each enum can be added together and thus turn stuff on
	// multiple inputs to such function is written with the "Bitwise Or" -> "|"
	// SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) gives some number in bits example:  00010001 for the function
	SDL_Init(SDL_INIT_EVENTS); // if video or audio gets flagged event is included

	TTF_Init(); // Init the ttf library stuff for rendering text

	// https://wiki.libsdl.org/SDL3/SDL_CreateWindow
	window = SDL_CreateWindow("firstGame", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, NULL);
}

// passing the float as reference - syntactic sugar converts it to a pointer in the background
void CalculateDeltaTime(float& dt, float scaler){
	NOW = SDL_GetTicksNS();
	dt = NOW-PREV;
	dt = SDL_NS_TO_SECONDS(dt);
	dt *= scaler;
	PREV = NOW;
}
// not using the float& would look like this
// void CalculateDeltatime(float* dt){
// 	NOW = SDL_GetTicksNS();
// 	*dt = NOW-PREV;
// 	*dt = SDL_NS_TO_SECONDS(*dt);
// 	PREV = NOW;
// }



// Calculate remaining frame time to wait
void CalculateRemainingFrameTime_MS(double *milliseconds){
		// capping framerate to FRAME_TIME_MS calculated in common.h
		Uint64 frame_end_time_ns = SDL_GetTicksNS();
		double frame_time_spent_ns = frame_end_time_ns - PREV; // total time spent rendering
		double frame_time_spent_ms = frame_time_spent_ns / 1e6; // convert ns to ms frametime
		*milliseconds = FRAME_TIME_MS - frame_time_spent_ms;
}


// our memory arena uses unsigned char (1 byte) and the stream uses char(also 1 byte) thats why it safe to use the reinterpret_cast
// Saving whole memory space to .bin file in binary
void StoreGameState(Memory::Arena* arena){
	std::ofstream file("temp_state.bin",std::ios::binary);
	file.write(reinterpret_cast<const char*>(arena->base), arena->size);
	file.close();
}

// Loading the whole memory snapshot from a binary .bin file into memory
void RetriveGameState(Memory::Arena* arena){
	std::ifstream file("temp_state.bin",std::ios::binary);
	file.read(reinterpret_cast<char*>(arena->base), arena->size);
	file.close();
}


int main()
{
	// MEMORY ALLOCATION FOR GAME
	void* game_memory = AllocateGameMemory(); // Allocate the total game memory with malloc
	if(game_memory == nullptr){
		return 1; // quit application with error code 1
	}
	// MEMORY ARENA
	Memory::Arena* arena_main = new Memory::Arena();
	Memory::Initialize(arena_main, game_memory, GAME_MEMORY_ALLOWANCE);
	GameData* gameData = (GameData*)Memory::Allocate(arena_main, (sizeof(GameData))); // Allocate space in area_main for GameData
	gameData->arena_main = arena_main;
	gameData->ticks_total = ALLOC(arena_main, uint64_t); // variable for holding ticks counter since start of game
	// FPS variables DEV
	gameData->editor_data.fps_buffer_count = 256;
	gameData->editor_data.fps_buffer = ALLOC_ARRAY(arena_main, float, gameData->editor_data.fps_buffer_count);
	
	// MEMORY SUB ARENAS
	// Allocate space for arena_image - where the images will be (New Arena in the main_arena)
	int SPRITE_COUNT = 256;
	size_t IMAGE_ARENA_SIZE = MEGABYTES(1) ; //sizeof(Sprite) * SPRITE_COUNT; // allowes for 100 images to be loaded
	gameData->arena_images = Memory::CreateSubArena(arena_main, IMAGE_ARENA_SIZE);
	gameData->spriteBuffer = (Sprite*)Memory::Allocate(gameData->arena_images, sizeof(Sprite) * SPRITE_COUNT);
	gameData->tilesetBuffer = ALLOC_ARRAY(gameData->arena_images, Tileset, (int)TILESETS::COUNT); 
	// other sub arenas
	gameData->arena_levels = Memory::CreateSubArena(arena_main, MEGABYTES(3));
	gameData->arena_entities = Memory::CreateSubArena(gameData->arena_levels, MEGABYTES(1)); // sub area in levels - Entities
	gameData->arena_commands = Memory::CreateSubArena(gameData->arena_levels, MEGABYTES(1)); // sub area in levels - Commands
	gameData->arena_scratch = Memory::CreateSubArena(arena_main, KILOBYTES(256)); // scrach arena for temp memory
	
	
	// ALLOCATE LEVELS!
	gameData->scenes.gameplay.levelCount = 6;
	// gameData->levels = (LevelData*)Memory::Allocate(gameData->arena_levels, sizeof(LevelData) * gameData->levelCount);
	gameData->scenes.gameplay.levels = ALLOC_ARRAY(gameData->arena_levels, LevelData, gameData->scenes.gameplay.levelCount); // same function as above just using a macro

		
  // ALLOCATE INPUT gameData - Multiple bool arrays for keeping track of all the keys pressed down on the keyboard during gameplay
	// and a float array for the key held time
	size_t INPUT_ARENA_SIZE = 0;
	INPUT_ARENA_SIZE += sizeof(bool) * SDL_SCANCODE_COUNT * 2;
	INPUT_ARENA_SIZE += sizeof(float) * SDL_SCANCODE_COUNT;
	INPUT_ARENA_SIZE += 128; // add a bit of extra memory for padding - as our allocate function is a bit simple
	INPUT_ARENA_SIZE += sizeof(float) * 3; // mouse buttons held time
	gameData->arena_input = Memory::CreateSubArena(arena_main, INPUT_ARENA_SIZE);

	gameData->input.keys_current  = (bool*)Memory::Allocate(gameData->arena_input, sizeof(bool) * SDL_SCANCODE_COUNT);
	gameData->input.keys_previous = (bool*)Memory::Allocate(gameData->arena_input, sizeof(bool) * SDL_SCANCODE_COUNT);
	gameData->input.keys_held_time = (float*)Memory::Allocate(gameData->arena_input, sizeof(float) * SDL_SCANCODE_COUNT);

	int mouseButtonCount = 3;
	gameData->input.mouse_held_time = (float*)Memory::Allocate(gameData->arena_input, sizeof(float) * mouseButtonCount);
	

	Gameplay* gameplay = &gameData->scenes.gameplay; // store a pointer to gameplay struct for easier access
	
	// ALLOCATE RING BUFFER FOR INPUT
	gameplay->input_buffer_capacity = 50;  //gameData->scenes.gameplay.input_buffer_capacity = 50;
	size_t RING_BUFFER_SIZE = sizeof(Position) * gameplay->input_buffer_capacity;
	gameplay->input_buffer = (Position*)Memory::Allocate(gameData->arena_levels, RING_BUFFER_SIZE);

	// ALLOCATE COMMANDS - BUFFER POINTER - BUFFER ARRAY
	gameplay->commandBuffer = (CommandBuffer*)Memory::Allocate(arena_main, sizeof(CommandBuffer)); // Allocate command buffer pointer to main
	gameplay->commandBuffer->capacity = 20000; // set the max capacity of the command buffer
	size_t COMMAND_SIZE = sizeof(AnyCommand) * gameplay->commandBuffer->capacity; // calc the size of command buffer
	gameplay->commandBuffer->allCommands = (AnyCommand*)Memory::Allocate(gameData->arena_commands, COMMAND_SIZE); // Allocate the command buffer size
	// capacity can be set higher. Move Commands holds 2 integers and an pointer and an enum this is around 16-20 bytes.
	// With 1MB we can store around 50 000 commands
		
	// Check if we can increese the timer resolution, using winmm.lib in our CMAKE (cmakelist.txt)
	// This improves the granularity of sleep - for rendering frames in sync
	MMRESULT result = timeBeginPeriod(1);
	if (result == TIMERR_NOCANDO){
		printf("could not increase timer resolution");
		Sleep(2000);
		return 3;
	}

	// Load the dll file for hot reloading
	DLL_INFO dll;
	bool dll_successfully_loaded = LoadDLL(&dll); // we load the DLL_INFO dll by passing &dll

	if(dll_successfully_loaded == false){
		return 2; // exit application could not load game dll
	}
	

	// Load image - after SDL setup - else the renderer is not setup and cannot convert to texture
	SDL_Setup();
	
	// Load Images and make the setup
	dll.initialize(gameData,renderer,window); // run the inizialize function of the dll

	
	gameData->running = true;
	float dt;
	float dt_scaler = 1;
	gameData->dt = &dt; // set a pointer in gameData to the float dt memory
	gameData->dt_scaler = &dt_scaler;
	
	while(gameData->running) // ---- MAIN GAME LOOP -----
	{
		CalculateDeltaTime(dt, dt_scaler); // should be the first to run!
		DLL_CheckStatus(&dll); // this might be good to break out to a button press - it checks if the dll has changed (Hot reload)
		Reset(gameData->arena_scratch); // Resetting the scrach arena
		

		SDL_Event event;
		while(SDL_PollEvent(&event)){
			gameData->running = dll.handleEvents(gameData, event);
			if(gameData->running == false){
				break;
			}

			if(event.type == SDL_EVENT_KEY_DOWN){
				if(event.key.key == SDLK_F9){
					StoreGameState(arena_main);
				}
				if(event.key.key == SDLK_F10){
					RetriveGameState(arena_main);
				}
			}
		}

		// Read input before Update
		gameData->input.keys_current = SDL_GetKeyboardState(nullptr); // keyboard

		// Read mouse inputs and calculate mouse magnitude
		float* delta_x = &gameData->input.mouse_x_delta;
		float* delta_y = &gameData->input.mouse_y_delta;
		*delta_x = gameData->input.mouse_x; // store last frame variable x
		*delta_y = gameData->input.mouse_y; // stroe last frame variable y
		// Get new mouse Inputs for this frame
		gameData->input.mouse_current = SDL_GetMouseState(&gameData->input.mouse_x, &gameData->input.mouse_y); // mouse
		*delta_x = gameData->input.mouse_x - *delta_x;
		*delta_y = gameData->input.mouse_y - *delta_y;

		float dx = *delta_x;
		float dy = *delta_y;
		gameData->input.mouse_magnitude = std::sqrt(dx * dx + dy * dy); // pythagoras for magnitude

		// SDL_Log("x: %.4f y: %.4f \n", *delta_x, *delta_y);
		// SDL_Log(" %.4f \n", gameData->input.mouse_magnitude);
		
		dll.update(gameData,dt); // UPDATE game.cpp

    // Update inputdata - increment keyboard[key] - heldtime, store current_keys into previous_keys, same for mouse
		UpdateKeys(&gameData->input, dt); // keyboard
		UpdateMouse(&gameData->input, dt); // mouse

    
		dll.draw(gameData, renderer); // DRAW game.cpp

		// Calculate wait time for frame , trying to keep pogram running on a constant FPS
		double time_to_sleep_ms;
		CalculateRemainingFrameTime_MS(&time_to_sleep_ms);

		if(time_to_sleep_ms > 0){
			// this is a way to be more exact when trying to hit the frame
			if(time_to_sleep_ms > 1){
				SDL_Delay(time_to_sleep_ms - 1);// same as Sleep() but plattform independent
			} // this while loop will then run as fast as it can the last millisecond waiting more exact to start the next frame - more costly
			while (time_to_sleep_ms > 0){
				CalculateRemainingFrameTime_MS(&time_to_sleep_ms);
			}
		}
		else{
			printf("missed frame \n");
			printf("frametime: %f timeToSleep:%f \n", FRAME_TIME_MS,time_to_sleep_ms); // negative sleep means we missed the frame with that much milliseconds
		}
	}// --- END MAIN LOOP
	
	dll.onQuit(gameData, renderer);
	SDL_Quit(); // says in docs you should call it at the end of the program
	return 0;
}




// ========== Read mouse input example =========
void NfTestReadMouse(SDL_Event event){
			// Reading mouse position example
			switch(event.type)
			{
				case SDL_EVENT_MOUSE_MOTION:
					SDL_Log("We go a mouse event");
					SDL_Log("Current mouse Position is : (%f,%f)", event.motion.x, event.motion.y);
					break;
				default:
					SDL_Log("unhandled event!");
					break;
			}
}

// =========== POINTER EXAMPLE ===============
// using a pointer reference as input changes the number in memory
void AddOne(int* theNumber) // pointer input (pointer is just a memory adress)
{
	// dereference the pointer to become an int again (instead of a memory adress)
	*theNumber += 1;
}

// same as above just that the compiler handles the referencing and dereferencing for us
// memoryadress (pointer) here instead of at input (passing by reference)
void AddOneByRef(int& theNumber)
{ 
	theNumber += 1;
}

// would have to return the number and overrite it for changing the number
int AddOneNoPointer(int theNumber){
	theNumber += 1;
	return theNumber;
}

// showing the difference between pointer and copy input
void RefExample(){
	
	int number = 5;
	SDL_Log("before %i" , number);

	// convert the int to a memoryadress
	AddOne(&number);
	SDL_Log("after Add: Pointer reference -> %i", number);

	AddOneByRef(number);
	SDL_Log("after Add: Reference function -> %i", number);
	
  AddOneNoPointer(number); // this will not change the number in this scope
	SDL_Log("after Add: Normal -> %i", number);

	number = AddOneNoPointer(number); // override the number with the new value
	SDL_Log("after Add: After overriting from Normal: %i", number);
}

// ===========================================


// ==========  Struct example ===============
struct the_struct_name{
	int a_number;
	float a_decimal_number;
	bool a_bool;
}; // end with };

// access stuff by using the '.'  var_name.a_number
// when accesing it from a variable, if its in a pointer use '->' arrow operator 
// 
// void Some_function(the_struct_name* a_struct)
// {  as its passed as a pointer we use a arrow operator to get data from it
//    a_struct->a_number;
// }

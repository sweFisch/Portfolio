// audioSystem.cpp
//
// FMOD STUDIO version - see audioSystem.h for the Core -> Studio translation table.

#include "audioSystem.h"
#include "FMOD/fmod_studio.h"
#include "FMOD/fmod_studio_common.h"
#include "FMOD/fmod_common.h"
#include "FMOD/fmod_errors.h"
#include "SDL3/SDL_log.h"
#include "arena.h"
#include "common.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

// global - promise kept (extern in .h file)
AudioSystem* g_audioSystem;

// --- TWEAKABLES ---

// Live Update lets the FMOD Studio tool connect to the running game over the network,
// so you can tweak an event and hear the change without rebuilding. Very nice while authoring.
// It opens a TCP port (9264), so if Init starts failing, turn this back off first.
static constexpr bool USE_LIVE_UPDATE = false;

static constexpr float MUSIC_VOLUME = 0.3f;

// --- DATA TABLES ---
// Where the banks live at runtime. You build them in FMOD Studio (they land in
// _audio/banks/Desktop/) and CMake copies them next to the .exe under assets/audio/banks/,
// so these plain relative paths just work. Add a line here for every new bank you build.
static const BankDataEntry all_bank_data[]
{
  // The first two are required by Studio itself and must load before the rest.
  {BANK_ID::MASTER,         "assets/audio/banks/Master.bank"},
  {BANK_ID::MASTER_STRINGS, "assets/audio/banks/Master.strings.bank"},
  // Your own banks.
  {BANK_ID::SFX,            "assets/audio/banks/SFX.bank"},
  {BANK_ID::MUSIC,          "assets/audio/banks/Music.bank"},
  {BANK_ID::AMBIENCE,       "assets/audio/banks/Ambience.bank"},
};

// These are event paths inside the banks above, NOT files on disk.
// They have to match what you name the events in FMOD Studio, exactly, "event:/" prefix included.
// Nothing here exists yet - create the events in Studio, assign them to the SFX / Music bank,
// rebuild the banks, and they start working. Until then these lookups just log and stay silent.
static const SoundDataEntry all_sound_data[]
{
  {SFX_ID::FALLBACK, "event:/SFX/Fallback"},
  {SFX_ID::PUSH,     "event:/Push"},
  {SFX_ID::DROPLET,  "event:/Droplet"},
  {SFX_ID::BURNSTEP, "event:/BurnStep"},
  {SFX_ID::HISS,     "event:/WaterHiss"},
  {SFX_ID::PUSH,     "event:/Push"},
  {SFX_ID::JUMP,     "event:/Jump"},
  {SFX_ID::SUCCESS, "event:/Success"}
};

static const SongDataEntry all_song_data[]
{
  {SONG_ID::THEME, "event:/Music/Theme"},
};

static const AmbienceDataEntry all_ambience_data[]
{
  {AMBIENCE_ID::FIRESPARKS, "event:/fireSparkAmbience"},
};

// --- SMALL HELPERS ---

// Every fmod function returns a FMOD_RESULT that we can check against FMOD_OK to make sure all is well.
// This prints a readable reason instead of just a number, and hands the answer back so
// the caller can decide whether it is fatal or something we can live without.
static bool FMOD_Ok(FMOD_RESULT result, const char* what)
{
  if(result != FMOD_OK){
    SDL_Log("FMOD: %s failed - %s", what, FMOD_ErrorString(result));
    return false;
  }
  return true;
}

// Is the system up AND do we actually have banks to play from?
// Before you export your first bank this is false, and every play call quietly does nothing.
static bool AudioReady()
{
  return g_audioSystem != nullptr
      && g_audioSystem->initialized
      && g_audioSystem->banks_loaded;
}

// FMOD is picky about the memory pool we hand it: the address has to be aligned and the
// length has to be a whole number of 512 byte blocks. Our arena just hands out whatever
// byte comes next, so we walk the pointer forward to the next 512 boundary and pay for
// those skipped bytes out of the length, then round the length down to a multiple of 512.
// (Get this wrong and FMOD_Memory_Initialize returns FMOD_ERR_INVALID_PARAM.)
static void* AlignPoolForward(void* memory, size_t* size_in_out)
{
  const size_t ALIGNMENT = 512;
  uintptr_t address = (uintptr_t)memory;
  uintptr_t aligned = (address + (ALIGNMENT - 1)) & ~(uintptr_t)(ALIGNMENT - 1);
  size_t lost = (size_t)(aligned - address);

  assert(*size_in_out > lost);
  *size_in_out -= lost;
  *size_in_out -= (*size_in_out % ALIGNMENT);

  return (void*)aligned;
}

// --- SETUP ---

void InitializeAudioSystem(AudioSystem* audio, Memory::Arena* arena_main)
{
  assert(audio->initialized == false);

  audio->banks_loaded = false;
  audio->song_id = SONG_ID::NONE;
  audio->song_instance = nullptr;
  for(int i = 0; i < (int)AMBIENCE_ID::COUNT; i++){
    audio->ambience_instances[i] = nullptr;
  }
  audio->master_bus = nullptr;
  audio->core_system = nullptr;

  // Create a sub area where fmod lives
  size_t memory_size = AUDIO_MEMORY_ALLOWANCE;
  Memory::Arena* fmod_arena = Memory::CreateSubArena(arena_main, memory_size);
  // CreateSubArena gives us back the Arena bookkeeping struct, the usable bytes are in ->base.
  // We hand FMOD the bytes, not the bookkeeping, otherwise FMOD would scribble over the header.
  audio->fmod_memory = AlignPoolForward(fmod_arena->base, &memory_size);

  FMOD_RESULT memory_init_ok = FMOD_Memory_Initialize(audio->fmod_memory, (int)memory_size, nullptr, nullptr, nullptr, FMOD_MEMORY_ALL);
  FMOD_Ok(memory_init_ok, "Memory_Initialize");
  assert(memory_init_ok == FMOD_OK);

  // Create the Studio system. Studio creates a Core system for itself underneath.
  FMOD_RESULT system_creation_ok = FMOD_Studio_System_Create(&audio->studio_system, FMOD_VERSION);
  assert(system_creation_ok == FMOD_OK);

  // Two sets of flags here, because we are initializing two layers at once:
  //   FMOD_STUDIO_INIT_* for the Studio layer, FMOD_INIT_* for the Core layer underneath it.
  FMOD_STUDIO_INITFLAGS studio_flags = FMOD_STUDIO_INIT_NORMAL;
  if(USE_LIVE_UPDATE){
    studio_flags |= FMOD_STUDIO_INIT_LIVEUPDATE;
  }

  FMOD_RESULT system_init_ok = FMOD_Studio_System_Initialize(audio->studio_system, AudioSystem::CHANNEL_COUNT, studio_flags, FMOD_INIT_NORMAL, nullptr);
  assert(system_init_ok == FMOD_OK);

  // Grab the Core system too. We do not need it to play events, but it is the escape hatch
  // if we ever want to do something Studio does not expose.
  FMOD_Ok(FMOD_Studio_System_GetCoreSystem(audio->studio_system, &audio->core_system), "GetCoreSystem");

  g_audioSystem = audio; // set our global variable to be the AudioSystem struct that continans FMOD all setuped

  audio->initialized = true;
}

void ShutdownAudioSystem(AudioSystem* audio)
{
  if(audio == nullptr || audio->initialized == false){
    return;
  }

  StopSong();
  StopAllAmbience();

  // Releasing the Studio system unloads every bank and kills every instance for us,
  // so we do not have to walk the arrays by hand.
  FMOD_Ok(FMOD_Studio_System_Release(audio->studio_system), "System_Release");

  audio->studio_system = nullptr;
  audio->core_system = nullptr;
  audio->banks_loaded = false;
  audio->initialized = false;
  g_audioSystem = nullptr;
}

void UpdateAudio(AudioSystem* audio)
{
  // FMOD does not automatically keep working we have to update it each frame
  // double check that the variable is set - important for working with hotrealoading
  if(g_audioSystem == nullptr || g_audioSystem != audio){
    g_audioSystem = audio;
  }
  assert(audio->initialized);
  // This is also where Studio hands us the results of anything we asked for asynchronously,
  // so it has to run every frame even when nothing is playing.
  FMOD_Studio_System_Update(audio->studio_system);
}

// --- PLAYBACK ---

void PlaySong(SONG_ID id)
{
  if(!AudioReady()){
    return;
  }
  // already playing this one - do not restart it from the top
  if(g_audioSystem->song_id == id && g_audioSystem->song_instance != nullptr){
    return;
  }

  StopSong(); // if something is playing stop it

  g_audioSystem->song_id = id;
  if(id == SONG_ID::NONE){
    return;
  }

  FMOD_STUDIO_EVENTDESCRIPTION* description = g_audioSystem->song_descriptions[(int)id];
  if(description == nullptr){
    SDL_Log("FMOD: no event loaded for song %d", (int)id);
    return;
  }

  if(!FMOD_Ok(FMOD_Studio_EventDescription_CreateInstance(description, &g_audioSystem->song_instance), "CreateInstance (song)")){
    g_audioSystem->song_instance = nullptr;
    return;
  }

  // Looping is NOT set here anymore. In Studio you draw a loop region on the event's timeline,
  // which is why the old FMOD_Sound_SetLoopCount(FOREVER) call is gone.
  FMOD_Studio_EventInstance_SetVolume(g_audioSystem->song_instance, MUSIC_VOLUME);
  FMOD_Ok(FMOD_Studio_EventInstance_Start(g_audioSystem->song_instance), "Start (song)");
  // NOTE: we deliberately do NOT Release() here - we keep the handle so we can stop it later.
}

void StopSong()
{
  if(g_audioSystem == nullptr || g_audioSystem->song_instance == nullptr){
    return;
  }

  // ALLOWFADEOUT respects the fade out in the event.
  FMOD_Studio_EventInstance_Stop(g_audioSystem->song_instance, FMOD_STUDIO_STOP_ALLOWFADEOUT);
  // Release tells Studio "I am done with this handle, clean it up once it has finished stopping".
  FMOD_Studio_EventInstance_Release(g_audioSystem->song_instance);

  g_audioSystem->song_instance = nullptr;
  g_audioSystem->song_id = SONG_ID::NONE;
}

void PlayAmbience(AMBIENCE_ID id, float volume){

  assert(id != AMBIENCE_ID::COUNT);
  if(!AudioReady()){
    return;
  }

  FMOD_STUDIO_EVENTINSTANCE** instance_slot = &g_audioSystem->ambience_instances[(int)id];
  // this bed is already running - do not layer a second copy of it on top of itself
  if(*instance_slot != nullptr){
    return;
  }

  FMOD_STUDIO_EVENTDESCRIPTION* description = g_audioSystem->ambience_description[(int)id];
  if(description == nullptr){
    SDL_Log("FMOD: no event loaded for ambience %d", (int)id);
    return;
  }

  if(!FMOD_Ok(FMOD_Studio_EventDescription_CreateInstance(description, instance_slot), "CreateInstance (ambience)")){
    *instance_slot = nullptr;
    return;
  }

  // Same as music - the loop is a loop region drawn on the event timeline in Studio,
  // there is nothing to set here.
  FMOD_Studio_EventInstance_SetVolume(*instance_slot, volume);
  FMOD_Ok(FMOD_Studio_EventInstance_Start(*instance_slot), "Start (ambience)");
  // NOTE: no Release() - we keep the handle so StopAmbience can find it again.
}

void StopAmbience(AMBIENCE_ID id)
{
  assert(id != AMBIENCE_ID::COUNT);
  if(g_audioSystem == nullptr){
    return;
  }

  FMOD_STUDIO_EVENTINSTANCE** instance_slot = &g_audioSystem->ambience_instances[(int)id];
  if(*instance_slot == nullptr){
    return; // not playing
  }

  // ALLOWFADEOUT respects the fade out drawn on the event - IMMEDIATE would cut it dead.
  FMOD_Studio_EventInstance_Stop(*instance_slot, FMOD_STUDIO_STOP_ALLOWFADEOUT);
  FMOD_Studio_EventInstance_Release(*instance_slot);
  *instance_slot = nullptr;
}

void StopAllAmbience()
{
  for(int i = 0; i < (int)AMBIENCE_ID::COUNT; i++)
  {
    StopAmbience((AMBIENCE_ID)i);
  }
}

void PlaySFX(SFX_ID id, float volume)
{
  assert(id != SFX_ID::COUNT);
  if(!AudioReady()){
    return;
  }

  FMOD_STUDIO_EVENTDESCRIPTION* description = g_audioSystem->sfx_descriptions[(int)id];
  if(description == nullptr){
    if(id == SFX_ID::FALLBACK){
      return; // we tried to play fallback and that failed too - stay quiet rather than loop forever
    }
    PlaySFX(SFX_ID::FALLBACK, volume);
    return;
  }

  // This is the whole of the old channel hunting code. Studio owns the voices now:
  // it decides which instance gets a real channel and which one goes virtual,
  // based on the max instance / stealing rules you set on the event in the tool.
  FMOD_STUDIO_EVENTINSTANCE* instance = nullptr;
  if(!FMOD_Ok(FMOD_Studio_EventDescription_CreateInstance(description, &instance), "CreateInstance (sfx)")){
    return;
  }

  FMOD_Studio_EventInstance_SetVolume(instance, volume); // set volume of sound
  FMOD_Studio_EventInstance_Start(instance);
  // "Fire and forget": releasing straight after Start means Studio frees the instance
  // by itself the moment the sound finishes. We never have to think about it again.
  FMOD_Studio_EventInstance_Release(instance);
}




void SetMasterVolume(float volume)
{
  if(!AudioReady() || g_audioSystem->master_bus == nullptr){
    return;
  }
  FMOD_Studio_Bus_SetVolume(g_audioSystem->master_bus, volume);
}

namespace AssetManagement
{
  // Load every bank listed in all_bank_data.
  // A missing bank is a warning, not a crash - you have not built any yet.
  void LoadAllBanks(AudioSystem* audioSystem)
  {
    assert(audioSystem->initialized);

    bool master_ok = false;
    bool strings_ok = false;

    for (const BankDataEntry& bank_data : all_bank_data)
    {
      FMOD_STUDIO_BANK** bank_slot = &audioSystem->banks[(int)bank_data.id];

      FMOD_RESULT bank_load_ok = FMOD_Studio_System_LoadBankFile(
        audioSystem->studio_system,
        bank_data.path,
        FMOD_STUDIO_LOAD_BANK_NORMAL,
        bank_slot);

      if(bank_load_ok != FMOD_OK){
        *bank_slot = nullptr;
        SDL_Log("FMOD: could not load bank '%s' - %s (build it in FMOD Studio first)", bank_data.path, FMOD_ErrorString(bank_load_ok));
        continue;
      }

      if(bank_data.id == BANK_ID::MASTER)        { master_ok = true; }
      if(bank_data.id == BANK_ID::MASTER_STRINGS){ strings_ok = true; }
    }

    // Without the strings bank we can still play sound, but only by GUID -
    // every "event:/..." lookup in this file would fail. So we treat it as required.
    audioSystem->banks_loaded = master_ok && strings_ok;
    if(!audioSystem->banks_loaded){
      SDL_Log("FMOD: running silent - Master.bank and/or Master.strings.bank missing.");
      return;
    }

    // "bus:/" is the master bus and only exists once Master.bank is in.
    FMOD_Ok(FMOD_Studio_System_GetBus(audioSystem->studio_system, "bus:/", &audioSystem->master_bus), "GetBus (master)");
  }

  // Look up all events specified in the enums and paths in all_sound_data / all_song_data / all_ambience_data.
  // This replaces the old FMOD_System_CreateSound calls - nothing is read off disk here,
  // we are just asking the already loaded banks "do you know this event path?".
  void LoadAllEvents(AudioSystem* audioSystem)
  {
    if(!audioSystem->banks_loaded){
      SDL_Log("FMOD: skipping event lookup, no banks loaded.");
      return;
    }

    // using the more modern c++ style for loop - by reference - the compiler goes and gets it for us
    for (const SoundDataEntry& sound_data : all_sound_data)
    {
      FMOD_STUDIO_EVENTDESCRIPTION** description_slot = &audioSystem->sfx_descriptions[(int)sound_data.id];

      FMOD_RESULT event_found_ok = FMOD_Studio_System_GetEvent(audioSystem->studio_system, sound_data.event_path, description_slot);
      if(event_found_ok != FMOD_OK){
        *description_slot = nullptr;
        SDL_Log("FMOD: event '%s' not found - %s", sound_data.event_path, FMOD_ErrorString(event_found_ok));
        continue;
      }

      // Pull the samples into memory up front so the first PlaySFX does not hitch.
      // Studio would stream them in on demand otherwise.
      FMOD_Studio_EventDescription_LoadSampleData(*description_slot);
    }

    for (const SongDataEntry& song_data : all_song_data)
    {
      FMOD_STUDIO_EVENTDESCRIPTION** description_slot = &audioSystem->song_descriptions[(int)song_data.id];

      FMOD_RESULT event_found_ok = FMOD_Studio_System_GetEvent(audioSystem->studio_system, song_data.song_path, description_slot);
      if(event_found_ok != FMOD_OK){
        *description_slot = nullptr;
        SDL_Log("FMOD: event '%s' not found - %s", song_data.song_path, FMOD_ErrorString(event_found_ok));
      }
      // Music is usually long, so we let Studio stream it instead of calling LoadSampleData.
    }

    for (const AmbienceDataEntry& ambience_data : all_ambience_data)
    {
      FMOD_STUDIO_EVENTDESCRIPTION** description_slot = &audioSystem->ambience_description[(int)ambience_data.id];

      FMOD_RESULT event_found_ok = FMOD_Studio_System_GetEvent(audioSystem->studio_system, ambience_data.ambience_path, description_slot);
      if(event_found_ok != FMOD_OK){
        *description_slot = nullptr;
        SDL_Log("FMOD: event '%s' not found - %s", ambience_data.ambience_path, FMOD_ErrorString(event_found_ok));
      }
      // Long looping beds are streamed, so no LoadSampleData here - same reasoning as music.
    }
  }
}

// audioSystem.h
//
// FMOD STUDIO version.
//
// The Core API (fmod.h) thinks in "sounds" and "channels":
//   you load a .wav into an FMOD_SOUND and push it out through an FMOD_CHANNEL yourself.
//
// The Studio API (fmod_studio.h) thinks in "banks", "events" and "buses":
//   a sound designer builds .bank files in the FMOD Studio tool,
//   a bank contains events ("event:/SFX/Jump"),
//   an event is a little recipe (which samples, random pitch, looping, fades, volume...),
//   and we only say "play this event" - the recipe decides what actually comes out.
//
// The mapping from the old Core code to this file:
//   FMOD_SOUND    -> FMOD_STUDIO_EVENTDESCRIPTION (the template / recipe)
//   FMOD_CHANNEL  -> FMOD_STUDIO_EVENTINSTANCE    (one playing copy of that recipe)
//   PlaySound()   -> CreateInstance() + Start()
//   manual channel bookkeeping -> Studio does it for us

#pragma once
#include "FMOD/fmod_studio.h"        // the Studio C API - System / Bank / EventDescription / EventInstance
#include "FMOD/fmod_studio_common.h" // Studio types and flags
#include "FMOD/fmod_common.h"        // shared types - FMOD_RESULT, FMOD_VERSION ...
#include "FMOD/fmod_errors.h"        // FMOD_ErrorString() so we can print readable errors
#include <cassert>

// forward declare structs
namespace Memory{
  struct Arena;
}

// --- BANKS ---
// A bank is the file FMOD Studio exports. It is the container everything else lives inside.
// MASTER and MASTER_STRINGS are special:
//   Master.bank         holds the mixer (all the buses)
//   Master.strings.bank holds the human readable names, this is what lets us
//                       look events up by path ("event:/SFX/Jump") instead of by GUID.
// Both of those MUST be loaded before anything else works.
enum class BANK_ID{
  MASTER,
  MASTER_STRINGS,
  SFX,
  MUSIC,
  AMBIENCE,

  COUNT
};

// --- EVENTS ---
// enum for audio (the different kinds of sound we will have)
enum class SFX_ID{
  FALLBACK,
  JUMP,
  PUSH,
  DROPLET,
  HISS,
  BURNSTEP,
  SUCCESS,

  COUNT
};

enum class SONG_ID
{
  NONE,
  THEME,

  COUNT
};

enum class AMBIENCE_ID{
  FIRESPARKS,

  COUNT
};

// structs for setting up and storing the paths we use - the Load* functions walk these tables
struct BankDataEntry
{
  BANK_ID id;
  const char* path; // a path on disk, relative to the .exe
};

struct SoundDataEntry
{
  SFX_ID id;
  const char* event_path; // an FMOD Studio event path, NOT a file on disk
};

struct SongDataEntry{
  SONG_ID id;
  const char* song_path;
};

struct AmbienceDataEntry
{
  AMBIENCE_ID id;
  const char* ambience_path;
};

struct AudioSystem
{
  bool initialized;  // for cheking if its setup
  bool banks_loaded; // stays false until Master.bank + Master.strings.bank actually loaded.
                     // Everything guards on this, so the game runs silent (and does not crash)
                     // as long as you have not exported any banks from FMOD Studio yet.
  void* fmod_memory; // memory pointer for our FMOD to work with
  

  FMOD_STUDIO_SYSTEM* studio_system; // this is what we talk to - banks, events, buses
  FMOD_SYSTEM* core_system;          // the Core system Studio builds on top of.

  // Studio still needs to know how many virtual channels the Core mixer may use underneath.
  // A channel or voice is an audio thread responsible for streaming and pushing audio to our sound card
  // we cant have to many as that whould trottle our audio tread - 16-32 is usally a good number
  // Difference from the Core API: we no longer pick a channel by hand, Studio hands them out.
  static const int CHANNEL_COUNT = 32;

  // making the arrays in the way below will allocate them as soon as we make this struct as the memory size is known at compile time
  FMOD_STUDIO_BANK* banks[(int)BANK_ID::COUNT];

  // An EVENTDESCRIPTION is the loaded recipe. It does not make sound on its own,
  // we create an instance from it every time we want to hear it.
  FMOD_STUDIO_EVENTDESCRIPTION* sfx_descriptions[(int)SFX_ID::COUNT];
  FMOD_STUDIO_EVENTDESCRIPTION* song_descriptions[(int)SONG_ID::COUNT];
  
  // Ambience loops until we stop it, like music, so we keep the handles.
  // Unlike music several beds can run at once, so it is an array instead of one pointer.
  FMOD_STUDIO_EVENTDESCRIPTION* ambience_description[(int)AMBIENCE_ID::COUNT];
  FMOD_STUDIO_EVENTINSTANCE* ambience_instances[(int)AMBIENCE_ID::COUNT];
  // Music is the one thing we keep a handle to, because we need to stop it again later.
  // SFX are "fire and forget" - we release them immediately and never look at them again.
  SONG_ID song_id;
  FMOD_STUDIO_EVENTINSTANCE* song_instance;

  // A bus is a mixer strip inside Master.bank. "bus:/" is the master bus,
  // so this is our global volume knob.
  FMOD_STUDIO_BUS* master_bus;
};

// gloabl promise -
// we promise that this variable will exist and all files that use audioSystems.h will thus be able to access it
// the extern keyword promises that the actual pointer will be found during compilation from another file.
// We will keep this global pointer synced to the AudioSystem we put inside GameData (struct)
// So that any file can point to it and use our audio system - every program will by nature need some global state
extern AudioSystem* g_audioSystem;
// we will write a pointer with the exact same name in audioSsystem.cpp that will be the actual variable.
// our extern in a .h file is just a way of accessing this by #include

void InitializeAudioSystem(AudioSystem* audio, Memory::Arena* arena_main);
void ShutdownAudioSystem(AudioSystem* audio);
void UpdateAudio(AudioSystem* audio);

void PlaySong(SONG_ID id);
void StopSong();

void PlaySFX(SFX_ID id, float volume = 1.0f);
void SetMasterVolume(float volume);

void PlayAmbience(AMBIENCE_ID id, float volume = 1.0f);
void StopAmbience(AMBIENCE_ID);
void StopAllAmbience();

namespace AssetManagement{
  void LoadAllBanks(AudioSystem* audioSystem); // must run before LoadAllEvents
  void LoadAllEvents(AudioSystem* audioSystem);   // now looks up events inside the loaded banks
}

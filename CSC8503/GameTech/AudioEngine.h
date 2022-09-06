#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>
#include <iostream>
#include <fstream>
#include "Singleton.h"
#include "GameTechRenderer.h"
#include "InputManager.h"

struct CustomSound {
	GameObject* object = NULL;
	bool isChannelPlaying = false;
};

enum Audio {
	MENU_MUSIC, GAME_MUSIC, SHOOT_SOUND, ROCKET_FLYING_SOUND, EXPLOSION_SOUND, PROJECTILE_LAUNCH_SOUND, WALK_SOUND, PICKUP_COLLECTED_SOUND, VICTORY_SOUND, LOSS_SOUND, CAPTURE_AREA_SOUND, PAINT_FILL_SOUND, PROJECTILE_HIT_SOUND
};

class AudioEngine : public Singleton<AudioEngine> {
	friend class Singleton<AudioEngine>;

public:
	//constructor, initialises all variables automatically
	AudioEngine();

	//adds a sound to the system. For smaller files
	void Create2DSound(int index, const char* pFile);
	void Create3DSound(int index, const char* pFile, float minDist, float maxDist); //minDist is when attenuation starts, maxDist is when sound is inaudible 

	//makes the sound a stream. For larger files that may take a while to load
	//can't have multiple streams in 1 channel, mainly for music
	void Create2DStream(int index, const char* pFile);
	void Create3DStream(int index, const char* pFile, float minDist, float maxDist);

	//plays a certain sound, works for both sounds and streams
	void PlayASound(int index, bool loop = false, Vector3 position = { 0.f, 0.f, 0.f }, Vector3 velocity = { 0.f, 0.f, 0.f }, GameObject* go = NULL);

	//memory management, will delete the sound
	void ReleaseSound(int index);

	//pause all game sounds in the game
	void PauseGameSounds();
	void UnpauseGameSounds();

	//pause all music in the game
	void PauseMusic();
	void UnpauseMusic();

	//stops all sounds, not pauses
	void StopAllSounds();
	void StopAllFinishedSounds();
	void StopSound(int index, int delay);

	//sounds continue to play but muted
	void MuteAllSounds();
	void UnmuteAllSounds();

	//set volumes
	void SetMasterVolume(float f);
	void SetGameSoundsVolume(float f);
	void SetMusicVolume(float f);

	//added by Jeffery, get volumes
	float GetMasterVolume() { return masterVolume; }
	float GetGameSoundsVolume() { return gameSoundsVolume; }
	float GetMusicVolume() { return musicVolume; }

	//call each frame to update the audiosystem and pass in camera parameters
	void Update(Vector3 cameraPos, Vector3 cameraForward, Vector3 cameraUp, float dt);
	void Update();

	typedef unsigned int uint;

	uint GetSoundLength(int i) {
		uint length = NULL;
		sounds[i]->getLength(&length, FMOD_TIMEUNIT_MS);
		return length;
	}

	//destructor
	~AudioEngine();

private:
	//amount of sounds in the game
	const static int numSounds = 13;
	const static int numChannels = 64;

	//system that handles sounds and channels
	FMOD::System* audioEngine;

	//the actual sounds, includes music as well
	FMOD::Sound* sounds[numSounds];

	//channel the sound plays from
	FMOD::Channel* channels[numChannels];

	CustomSound freeChannels[numChannels];

	//camera variables for 3d sounds
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR listenerLastPos;
	FMOD_VECTOR listenerForward;
	FMOD_VECTOR listenerUp;
	FMOD_VECTOR listenerVelocity;

	//volume levels, can be controlled in options menu
	float masterVolume;
	float musicVolume;
	float gameSoundsVolume;

	//allows us to set volume levels for all sounds at once
	FMOD::ChannelGroup* gameSoundsGroup;
	FMOD::ChannelGroup* musicGroup;
	FMOD::ChannelGroup* masterGroup;

	//converts vec3 to FMODVec
	friend FMOD_VECTOR toFMODVector(Vector3 v);


	FMOD_RESULT result;
	std::ofstream myfile;
};
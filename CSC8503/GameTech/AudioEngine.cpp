#include "AudioEngine.h"
//constructor, initialises all variables automatically
AudioEngine::AudioEngine() {
	result = FMOD::System_Create(&audioEngine);
	int driverCount = 0;
	result = audioEngine->getNumDrivers(&driverCount);
	if (driverCount == 0) {
		std::cout << "No Audio Driver Detected!" << std::endl;
	}


	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerLastPos = { 0.0f, 0.0f, 0.0f };
	listenerForward = { 0.0f, 0.0f, 1.0f };
	listenerUp = { 0.0f, 1.0f, 0.0f };
	listenerVelocity = { 0.0f, 0.0f, 0.0f };

	masterVolume = 0.5f;
	gameSoundsVolume = 1.0f;
	musicVolume = 0.5f;

	result = audioEngine->init(numChannels, FMOD_INIT_3D_RIGHTHANDED, NULL);
	result = audioEngine->set3DSettings(1.0f, 1.0f, 1.0f);

	result = audioEngine->createChannelGroup("GameSoundsGroup", &gameSoundsGroup);
	result = audioEngine->createChannelGroup("MusicGroup", &musicGroup);

	result = audioEngine->getMasterChannelGroup(&masterGroup);
	result = masterGroup->addGroup(gameSoundsGroup);
	result = masterGroup->addGroup(musicGroup);

	SetMasterVolume(masterVolume);
	SetMusicVolume(musicVolume);
	SetGameSoundsVolume(gameSoundsVolume);
}

//destructor
AudioEngine::~AudioEngine() {
	StopAllSounds();
	for (int i = 0; i < numSounds; i++) {
		result = sounds[i]->release();
	}
	result = audioEngine->close();
	result = audioEngine->release();
}

//adds a sound to the system. For smaller files
void AudioEngine::Create2DSound(int index, const char* pFile) {
	FMOD_RESULT result;
	if (sounds[index] == NULL) {
		result = audioEngine->createSound(pFile, FMOD_2D, 0, &sounds[index]);
	}
	else {
		sounds[index]->release();
		result = audioEngine->createSound(pFile, FMOD_2D, 0, &sounds[index]);
	}
}

//3D sounds, attenuation is controlled with minDist and maxDist.
void AudioEngine::Create3DSound(int index, const char* pFile, float minDist, float maxDist) {
	if (sounds[index] == NULL) {
		result = audioEngine->createSound(pFile, FMOD_3D_LINEARROLLOFF, 0, &sounds[index]);
		sounds[index]->set3DMinMaxDistance(minDist, maxDist);
	}
	else {
		sounds[index]->release();
		result = audioEngine->createSound(pFile, FMOD_3D_LINEARROLLOFF, 0, &sounds[index]);
		sounds[index]->set3DMinMaxDistance(minDist, maxDist);
	}
}

//makes the sound a stream. For larger files that may take a while to load
void AudioEngine::Create2DStream(int index, const char* pFile) {
	if (sounds[index] == NULL) {
		result = audioEngine->createStream(pFile, FMOD_2D, 0, &sounds[index]);
	}
	else {
		sounds[index]->release();
		result = audioEngine->createStream(pFile, FMOD_2D, 0, &sounds[index]);
	}
}

//3D streams. May be a large file that needs to be streamed constantly in 3D space.
void AudioEngine::Create3DStream(int index, const char* pFile, float minDist, float maxDist) {
	if (sounds[index] == NULL) {
		result = audioEngine->createStream(pFile, FMOD_3D_LINEARROLLOFF, 0, &sounds[index]);
		sounds[index]->set3DMinMaxDistance(minDist, maxDist);
	}
	else {
		sounds[index]->release();
		result = audioEngine->createStream(pFile, FMOD_3D_LINEARROLLOFF, 0, &sounds[index]);
		sounds[index]->set3DMinMaxDistance(minDist, maxDist);
	}
}

//plays a certain sound, works for both sounds and streams
void AudioEngine::PlayASound(int index, bool loop, Vector3 position, Vector3 velocity, GameObject* go) {
	if (!loop)
		sounds[index]->setMode(FMOD_LOOP_OFF);
	else
	{
		sounds[index]->setMode(FMOD_LOOP_NORMAL);
		sounds[index]->setLoopCount(-1);
	}

	FMOD_VECTOR fPosition = toFMODVector(position);
	FMOD_VECTOR fVelocity = toFMODVector(velocity);

	int channelIndex = -1;
	for (int i = 0; i < numChannels; i++) {
		if (freeChannels[i].isChannelPlaying) {

		}
		else {
			channelIndex = i;
			break;
		}
	}

	if (channelIndex != -1) {
		result = audioEngine->playSound(sounds[index], 0, false, &channels[channelIndex]);
		if (index <= GAME_MUSIC) {
			channels[channelIndex]->setChannelGroup(musicGroup);
			channels[channelIndex]->set3DAttributes(&fPosition, &fVelocity);
			freeChannels[channelIndex].isChannelPlaying = true;
			freeChannels[channelIndex].object = go;
		}
		else {
			channels[channelIndex]->setChannelGroup(gameSoundsGroup);
			channels[channelIndex]->set3DAttributes(&fPosition, &fVelocity);
			freeChannels[channelIndex].isChannelPlaying = true;
			freeChannels[channelIndex].object = go;
		}
	}
}

//memory management, will delete the sound
void AudioEngine::ReleaseSound(int index) {
	result = sounds[index]->release();
}

//call each frame to update the audiosystem and pass in camera parameters
void AudioEngine::Update(Vector3 cameraPos, Vector3 cameraForward, Vector3 cameraUp, float dt) {
	StopAllFinishedSounds();

	listenerLastPos = listenerPos;
	listenerPos = toFMODVector(cameraPos);
	listenerForward = toFMODVector(cameraForward);
	listenerUp = toFMODVector(cameraUp);

	listenerVelocity.x = (listenerPos.x - listenerLastPos.x) * (dt / 1000);
	listenerVelocity.y = (listenerPos.y - listenerLastPos.y) * (dt / 1000);
	listenerVelocity.z = (listenerPos.z - listenerLastPos.z) * (dt / 1000);

	audioEngine->set3DListenerAttributes(0, &listenerPos, &listenerVelocity, &listenerForward, &listenerUp);
	audioEngine->update();
}

void AudioEngine::Update() {
	audioEngine->update();
}

//stop a specific sound, not pause
void AudioEngine::StopSound(int index, int delay) {
	freeChannels[index].object = NULL;
	unsigned long long parentClock;
	channels[index]->getDSPClock(NULL, &parentClock);
	channels[index]->addFadePoint(parentClock, 1.0f);
	channels[index]->addFadePoint(parentClock + delay, 0.0f);
	channels[index]->setDelay(0, parentClock + delay, true);
}

//stop all sounds, not pause
void AudioEngine::StopAllSounds() {
	result = masterGroup->stop();
	for (int i = 0; i < numChannels; i++) {
		freeChannels[i].object = NULL;
		freeChannels->isChannelPlaying = false;
	}
}

void AudioEngine::StopAllFinishedSounds() {
	bool channelPlaying;
	for (int i = 0; i < numChannels; i++) {
		channels[i]->isPlaying(&channelPlaying);
		if (!channelPlaying) {
			freeChannels[i].object = NULL;
			freeChannels[i].isChannelPlaying = false;
		}
	}
}

//sets the master volume
void AudioEngine::SetMasterVolume(float f) {
	masterVolume = f;
	result = masterGroup->setVolume(masterVolume);
}

//sets all game sounds volume
void AudioEngine::SetGameSoundsVolume(float f) {
	gameSoundsVolume = f;
	result = gameSoundsGroup->setVolume(gameSoundsVolume);
}

//sets all music volume
void AudioEngine::SetMusicVolume(float f) {
	musicVolume = f;
	result = musicGroup->setVolume(musicVolume);
}

//pause all game sounds in the game
void AudioEngine::PauseGameSounds() {
	result = gameSoundsGroup->setPaused(1);
}
//unpause all game sounds in the game
void AudioEngine::UnpauseGameSounds() {
	result = gameSoundsGroup->setPaused(0);
}

//pause all music in the game
void AudioEngine::PauseMusic() {
	result = musicGroup->setPaused(1);
}

//unpause all music in the game
void AudioEngine::UnpauseMusic() {
	result = musicGroup->setPaused(0);
}

//mutes all sounds, continue playing
void AudioEngine::MuteAllSounds() {
	result = masterGroup->setMute(1);
}

//unmutes all sounds
void AudioEngine::UnmuteAllSounds() {
	result = masterGroup->setMute(0);
}

//converts vec3 to FMODVec
FMOD_VECTOR toFMODVector(Vector3 v) {
	FMOD_VECTOR fm;
	fm.x = v.x;
	fm.y = v.y;
	fm.z = v.z;
	return fm;
}
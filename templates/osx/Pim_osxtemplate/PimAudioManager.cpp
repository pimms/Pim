#include "PimAudioManager.h"

#include "PimGameControl.h"
#include "PimAssert.h"
#include "PimRenderWindow.h"
#include "PimSound.h"

namespace Pim {
	AudioManager* AudioManager::singleton = NULL;

	/*
	=====================
	AudioManager::GetSingleton
	=====================
	*/
	AudioManager* AudioManager::GetSingleton() {
		return singleton;
	}

	/*
	=====================
	AudioManager::PrintOpenALErrors
	=====================
	*/
	void AudioManager::PrintOpenALErrors(string preceeding) {
		ALenum error = alGetError();

		if (error != AL_NO_ERROR) {
			printf("[OpenAL error (%s)]: ", preceeding.c_str());

			switch (error) {
			case AL_INVALID_NAME:
				printf("AL_INVALID_NAME");
				break;

			case AL_INVALID_ENUM:
				printf("AL_INVALID_ENUM");
				break;

			case AL_INVALID_VALUE:
				printf("AL_INVALID_VALUE");
				break;

			case AL_INVALID_OPERATION:
				printf("AL_INVALID_OPERATION");
				break;

			case AL_OUT_OF_MEMORY:
				printf("AL_OUT_OF_MEMORY");
				break;
			}

			printf("\n");
		}
	}

	/*
	=====================
	AudioManager::AudioManager
	=====================
	*/
	AudioManager::AudioManager() {
		device = alcOpenDevice(NULL);
		PimAssert(device != NULL, "Failed to find an audio device");

		context = alcCreateContext(device, NULL);
		PimAssert(context != NULL, "Failed to create an audio context");
		alcMakeContextCurrent(context);
	}

	/*
	=====================
	AudioManager::~AudioManager
	=====================
	*/
	AudioManager::~AudioManager() {
		if (context) {
			alcDestroyContext(context);
		}

		if (device) {
			alcCloseDevice(device);
		}
	}

	/*
	=====================
	AudioManager::InstantiateSingleton
	=====================
	*/
	void AudioManager::InstantiateSingleton() {
		PimAssert(singleton == NULL, "Error: AudioManager singleton instantiated twice.");
		singleton = new AudioManager;
	}

	/*
	=====================
	AudioManager::ClearSingleton
	=====================
	*/
	void AudioManager::ClearSingleton() {
		PimAssert(singleton != NULL, "Error: AudioManager was never initialized.");
		delete singleton;
		singleton = NULL;
	}

	/*
	=====================
	AudioManager::UpdateSoundBuffers

	Called every frame, calls Update on all 'sounds'.
	=====================
	*/
	void AudioManager::UpdateSoundBuffers() {
		PrintOpenALErrors("pre update");

		for (unsigned i=0; i<sounds.size(); i++) {
			sounds[i]->Update();
		}

		PrintOpenALErrors("post update");
	}

	/*
	==================
	AudioManager::AddSound
	==================
	*/
	void AudioManager::AddSound(Sound *sound) {
		sounds.push_back(sound);
	}

	/*
	==================
	AudioManager::RemoveSound
	==================
	*/
	void AudioManager::RemoveSound(Sound *sound) {
		for (unsigned i=0; i<sounds.size(); i++) {
			if (sounds[i] == sound) {
				sounds.erase(sounds.begin() + i);
				return;
			}
		}
	}
}
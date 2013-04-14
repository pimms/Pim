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
	AudioManager::CacheSound
	=====================
	*/
	bool AudioManager::CacheSound(string file) {
		/* Abort if the cache exists */
		if (singleton->cache.count(file)) {
			return true;
		}
		
		/* Open the file in binary mode */
		FILE *oggFile = NULL;
		OggVorbis_File *oggStream = NULL;
		if (!(oggFile = fopen(file.c_str(), "rb"))) {
			return false;
		}
		
		/* Attempt to open the file as Ogg */
		oggStream = new OggVorbis_File;
		
		int result;
		if ((result = ov_open(oggFile, oggStream, NULL, 0)) < 0) {
			fclose(oggFile);
			delete oggStream;
			return false;
		}
		
		/* Assign a new pair to the cache */
		if (singleton->cache.count(file)) {
			singleton->cache.erase(file);
		}
		singleton->cache[file] = pair<AudioData, vector<char>>();
		
		/* Point to the newly created pair<> */
		pair<AudioData, vector<char>> *cachePtr = &singleton->cache[file];
		
		/* Read the file information */
		vorbis_info *vorbisInfo;
		vorbisInfo = ov_info(oggStream, -1);
		
		if (vorbisInfo->channels == 1) {
			cachePtr->first.format = AL_FORMAT_MONO16;
		} else {
			cachePtr->first.format = AL_FORMAT_STEREO16;
		}
		
		cachePtr->first.frequency = vorbisInfo->rate;
		
		/* Read data */
		result = 1;
		while (result) {
			char buffer[1024];
			int s = 0;
			result = (int)ov_read(oggStream, buffer, 1024, 0, 2, 1, &s);
			
			cachePtr->second.insert(cachePtr->second.end(), buffer, buffer+result);
		}
		
		return true;
	}
	
	/*
	=====================
	AudioManager::RemoveCachedSound
	=====================
	*/
	bool AudioManager::RemoveCachedSound(string file) {
		if (singleton->cache.count(file)) {
			singleton->cache.erase(file);
			return true;
		}
		return false;
	}
	
	/*
	=====================
	AudioManager::RemoveAllCachedSounds
	=====================
	*/
	void AudioManager::RemoveAllCachedSounds() {
		singleton->cache.clear();
	}
	
	/*
	=====================
	AudioManager::GetCacheCount
	=====================
	*/
	int AudioManager::GetCacheCount() {
		return (int)singleton->cache.size();
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
#ifdef _DEBUG
		PrintOpenALErrors("pre update");
#endif
		for (unsigned i=0; i<sounds.size(); i++) {
			sounds[i]->Update();
		}
#ifdef _DEBUG
		PrintOpenALErrors("post update");
#endif
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
	
	/*
	==================
	AudioManager::GetCacheBytes
	==================
	*/
	vector<char>* AudioManager::GetCacheBytes(string file) {
		if (cache.count(file)) {
			return &cache[file].second;
		}
		
		PimWarning("This file was never cached!", file.c_str());
		return NULL;
	}
	
	/*
	==================
	AudioManager::GetCacheFormat
	==================
	*/
	AudioData AudioManager::GetCacheData(string file) {
		if (cache.count(file)) {
			return cache[file].first;
		}
		
		PimWarning("This file was never cached!", file.c_str());
		AudioData dd;
		dd.format = 0;
		dd.frequency = 0;
		return dd;
	}
}




























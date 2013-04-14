#pragma once

#include "PimInternal.h"
#include <map>
	
#define NUM_BUFFERS					32		// 32 simultaneous sounds
#define BUFFER_SIZE					32768	// 32k sound buffer

namespace Pim {
	/**
	 @class 	AudioManager
	 @brief 	Singleton object managing playback and buffering of sounds.
	 @details 	Pim::Sound objects are managed by this class. 
	 
				This class also directly interacts with OpenAL, so you don't have to.
	 
	 @todo 		Support sound caching.
	 */
	
	
	class GameControl;
	class Sound;

	
	/* Data used by Sounds for playback */
	struct AudioData {
		ALenum 			format;
		long			frequency;
	};
	

	class AudioManager {
	protected:
		friend class GameControl;

	private:
		friend class Sound;

	public:
		ALCdevice				*device;
		ALCcontext				*context;
		
		static AudioManager*	GetSingleton();
		static void				PrintOpenALErrors(string preceeding);
		static bool 			CacheSound(string file);
		static bool 			RemoveCachedSound(string file);
		static void				RemoveAllCachedSounds();
		static int 				GetCacheCount();

	protected:
								AudioManager();
								~AudioManager();
		static void				InstantiateSingleton();
		static void				ClearSingleton();
		void					UpdateSoundBuffers();

	private:
		static AudioManager		*singleton;
		vector<Sound*>			sounds;
		
		/* string: The file used in the buffer 
		 * AudioData: The format of the audio
		 * vector: The audio-bytes
		 */
		map<string, pair<AudioData,vector<char>>>	cache;

		void					AddSound(Sound *sound);
		void					RemoveSound(Sound *sound);
		vector<char>*			GetCacheBytes(string file);
		AudioData 				GetCacheData(string file);
	};
	
	/**
	 @fn		AudioManager::GetSingeton
	 @brief		Returns the singleton object.
	 */
	
	/**
	 @fn		AudioManager::CacheSound
	 @brief 	Load a short ogg-file into memory for future ease of play.
	 */
	
	/**
	 @fn 		AudioManager::PrintOpenALErrors
	 @brief 	Checks for, and prints any relevant OpenAL errors.
	 @param 	preceeding
	 			The error message is preceeded by this string if there was an error.
	 */
	
	/**
	 @fn 		AudioManager::UpdateSoundBuffers
	 @brief 	Updates the buffers of the sound objects if needed.
	 @details 	Currently, only streaming audio from files is supported.
	 */
}
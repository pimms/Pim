#pragma once

#include "PimInternal.h"
#include <map>

#define NUM_BUFFERS 32		// 32 simultaneous sounds
#define BUFFER_SIZE 32768	// 32k sound buffer

namespace Pim
{
	// Forward declarations
	class GameControl;
	class Sound;

	// Structure used for wav files
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

	struct SoundCache
	{
		std::vector<char> buf;
		long size;
		long rate;
		int channels;
	};

	// The main audio manager, used as an interface to play sound internally.
	// In order to play sound in your game, use Pim::Sound objects.
	class AudioManager
	{
	public:
		static AudioManager* getSingleton();

		// Load an ogg-file into the cache. This should only be done for 
		// relatively small files ( < 30 seconds), and is only beneficial if
		// the sound is to be played multiple times. Music and ambience should
		// be streamed normally.
		bool cacheOgg(std::string id, char *file);

		// Delete a cached sound by ID
		void deleteCache(std::string id);

	private:
		friend class GameControl;
		friend class Sound;

		AudioManager();
		~AudioManager();

		static void instantiateSingleton();
		static void clearSingleton();

		bool loadWav(const char *file, Sound*);
		bool loadOgg(const char *file, Sound*);

		void fillBuffer(Sound *s, int section);

		// Ogg files require an update roughly every 0.5 second. 
		void oggUpdate();
		void scheduleOggUpdate(Sound*);
		void unscheduleOggUpdate(Sound*);

		// Rewinding of ogg streams takes slightly more effort
		void rewindOgg(Sound*);

		// Create a new IDirectSoundBuffer8 (for parallel sounds)
		IDirectSoundBuffer8* createBuffer(WAVEFORMATEX*, DSBUFFERDESC*);

		std::vector<Sound*>		oggSounds;

		static AudioManager		*singleton;

		IDirectSound8			*m_DirectSound;

		// Pre-cached sounds 
		std::map<std::string, SoundCache*> cache;
	};
}
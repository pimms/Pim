#pragma once

#include "PimInternal.h"

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

	// The main audio manager, used as an interface to play sound internally.
	// In order to play sound in your game, use Pim::Sound objects.
	class AudioManager
	{
	public:
		static AudioManager* getSingleton();

	private:
		friend class GameControl;
		friend class Sound;

		AudioManager();
		~AudioManager();

		static void instantiateSingleton();
		static void clearSingleton();

		bool loadWav(const char *file, Sound*);
		bool loadOgg(const char *file, Sound*);

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
	};
}
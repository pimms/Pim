#pragma once

#include "Stdafx.h"

#define NUM_BUFFERS 32		// 32 simultaneous sounds
#define BUFFER_SIZE 32768	// 32k sound buffer

namespace Pim
{
	// Forward declarations
	class GameControl;

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

	private:
		friend class GameControl;

		AudioManager();
		~AudioManager();

		static void instantiateSingleton();
		static void clearSingleton();

		bool loadWav(char *file, IDirectSoundBuffer8**);
		void shutdownWav(IDirectSoundBuffer8**);
		bool playWav();


		static AudioManager *singleton;

		IDirectSound8 *m_DirectSound;
		IDirectSoundBuffer *m_PrimaryBuffer;
		IDirectSoundBuffer8 *m_SecondaryBuffer1;
	};
}
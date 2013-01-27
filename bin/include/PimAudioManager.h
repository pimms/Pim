#pragma once

#include "PimInternal.h"
#include <map>
	
#define NUM_BUFFERS					32		// 32 simultaneous sounds
#define BUFFER_SIZE					32768	// 32k sound buffer

namespace Pim {
	class GameControl;
	class Sound;

	struct SoundCache {
		vector<char>				buf;
		long						size;
		long						rate;
		int							channels;
	};

	class AudioManager {
	private:
		friend class GameControl;
		friend class Sound;

	public:
		static AudioManager*		GetSingleton();
		bool						CacheOgg(const string id, char *file);
		void						DeleteCache(const string id);

	private:
		vector<Sound*>				oggSounds;
		static AudioManager			*singleton;
		IDirectSound8				*m_DirectSound;
		map<string, SoundCache*>	cache;

									AudioManager();
									~AudioManager();
		static void					InstantiateSingleton();
		static void					ClearSingleton();
		bool						LoadWav(const char *file, Sound*);
		bool						LoadOgg(const char *file, Sound*);
		void						FillBuffer(Sound *s, const int section);
		void						OggUpdate();
		void						ScheduleOggUpdate(Sound*);
		void						UnscheduleOggUpdate(Sound*);
		void						RewindOgg(Sound*);
		IDirectSoundBuffer8*		CreateBuffer(WAVEFORMATEX*, DSBUFFERDESC*);
	};
}
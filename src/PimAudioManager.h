#pragma once

#include "PimInternal.h"
#include <map>
	
#define NUM_BUFFERS					32		// 32 simultaneous sounds
#define BUFFER_SIZE					32768	// 32k sound buffer

namespace Pim {
	class GameControl;
	class Sound;

	class AudioManager {
	protected:
		friend class GameControl;

	private:
		friend class Sound;

	public:
		ALCdevice					*device;
		ALCcontext					*context;
		
		static AudioManager*		GetSingleton();
		static void					PrintOpenALErrors(string preceeding);

	protected:
									AudioManager();
									~AudioManager();
		static void					InstantiateSingleton();
		static void					ClearSingleton();
		void						UpdateSoundBuffers();

	private:
		static AudioManager			*singleton;
		vector<Sound*>				sounds;

		void						AddSound(Sound *sound);
		void						RemoveSound(Sound *sound);
	};
}
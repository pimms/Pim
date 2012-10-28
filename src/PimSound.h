#pragma once

#include "PimAudioManager.h"

namespace Pim
{
	class Sound
	{
	public:
		Sound(std::string file);
		Sound();
		~Sound();

		// Play the currently loaded audio file
		void play();

		// Play the currently loaded audio file in an infinite loop
		void loop();

		// Pause the current sound
		void pause();

		// Rewind and play
		void replay();
		
		// No sound: 0.0 
		// Max volume: 1.0
		void setVolume(float volume);

		// all left: -1.0
		// all right: 1.0
		void setPan(float pan);

		// Retreive the buffer. Try not to do anything stupid with it.
		IDirectSoundBuffer8* getBuffer() { return buffer; }

	protected:
		IDirectSoundBuffer8 *buffer;
	};
}
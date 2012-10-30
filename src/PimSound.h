#pragma once

#include "PimInternal.h"

namespace Pim
{
	// Forward declaration
	class AudioManager;

	class Sound
	{
	public:
		Sound(std::string file);
		Sound();
		~Sound();

		// Load either a WAV or an OGG file.
		void loadFile(std::string file);

		// Play the currently loaded audio file
		void play();

		// Play the currently loaded audio file in an infinite loop
		void loop();

		// Rewind and loop
		void reloop();

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
		friend class AudioManager;

		// The buffer object used for all sound formats.
		IDirectSoundBuffer8 *buffer;

		// The vorbis-file used for .ogg formatted files
		OggVorbis_File *oggFile;
		
	private:
		bool audioStream;		// Is the audio streamed?

		bool isLoop;			// Stream looping?
		bool almostDone;		// Stream almost done?
		bool done;				// Stream done?
		bool rewindNext;		// Rewind stream next update

		int lastSection;		// Which half of the buffer was filled?
		int curSection;			// Which half of the buffer IS filled?
	};
}
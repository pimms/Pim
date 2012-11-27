#pragma once

#include "PimInternal.h"

/*

	Pim supports WAV and OGG file formats.
	WAV files are loaded into memory in it's entirety, while OGG files are streamed
	16kb at a time. 



*/

namespace Pim
{
	// Forward declaration
	class AudioManager;

	class Sound
	{
	public:
		Sound(std::string file);
		~Sound();

		// Load either a WAV or an OGG file.
		void loadFile(std::string file);

		// Play the currently loaded audio file
		void play();

		// Rewind and play
		void replay();

		// Play the currently loaded audio file in an infinite loop
		void loop();

		// Rewind and loop
		void reloop();

		// Pause the current sound
		void pause();
		
		// No sound: 0.0 
		// Max volume: 1.0
		void setVolume(float volume);

		// all left: -1.0
		// all right: 1.0
		void setPan(float pan);

		// Returns the current position in the sound in seconds
		float position();

		// Seek to a position in the file in seconds
		int position(float time);

		// Play the currently streamed (OGG ONLY) file in parallel.
		// It's stupidly important to notice that ONLY OGG FILES CAN BE PLAYED IN PARALLEL.
		// Pass true to this method in order to let the AudioManager clean it up for you.
		Sound* playParallel(bool deleteWhenDone = false);

		// Retreive the buffer. Try not to do anything stupid with it.
		IDirectSoundBuffer8* getBuffer() { return buffer; }


		bool deleteWhenDone;	// Delete sound when done playing?

	protected:
		friend class AudioManager;

		WAVEFORMATEX wfm;
		DSBUFFERDESC desc;

		// The buffer object used for all sound formats.
		IDirectSoundBuffer8 *buffer;

		// The vorbis-file used for .ogg formatted files
		OggVorbis_File *oggFile;
		
	private:
		/*
			All following member variables are only used when playing OGG-files, as they
			are streamed from memory.
		*/

		std::string filename;

		bool audioStream;		// Is the audio streamed?

		bool isLoop;			// Stream looping?
		bool almostDone;		// Stream almost done?
		bool done;				// Stream done?

		int lastSection;		// Which half of the buffer was played?
		int curSection;			// Which half of the buffer IS played?

		bool isParallel;		// Is this a parallel sound?
	};
}
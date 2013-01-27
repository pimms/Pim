#pragma once

#include "PimInternal.h"
#include "PimAudioManager.h"

/*

	Pim supports WAV and OGG file formats.
	WAV files are loaded into memory in it's entirety, while OGG files are streamed
	16kb at a time.
*/

namespace Pim {
	class AudioManager;
	struct SoundCache;

	class Sound {
	protected:
		friend class AudioManager;

	public:
		bool						deleteWhenDone;			// Cleanup when audio is done?

									Sound(const string file);
									Sound();
									~Sound();
		void						LoadFile(const string file);// OGG format ONLY!
		void						UseCache(const string id);	// Load cache
		void						Play();					
		void						Replay();				// Rewind and play
		void						Loop();					// Infinite loop
		void						Reloop();				// Rewind and loop
		void						Pause();
		void						SetVolume(float volume);// In the range [0 to 1]
		void						SetPan(float pan);		// In the range [-1 to 1]
		float						Position();				// Current position in seconds
		int							Position(float time);	// Go to position in seconds
		Sound*						PlayParallel(const bool deleteWhenDone = false);
		const IDirectSoundBuffer8*	GetBuffer() const;

	protected:
		WAVEFORMATEX				wfm;
		DSBUFFERDESC				desc;
		IDirectSoundBuffer8			*buffer;
		OggVorbis_File				*oggFile;

	private:
		string						filename;
		bool						audioStream;		// Is the audio streamed?
		SoundCache*					cache;				// Pointer to the cache
		long						cachePosition;		// Otherwise, it's probably cached
		bool						isLoop;				// Stream looping?
		bool						almostDone;			// Stream almost done?
		bool						done;				// Stream done?
		int							lastSection;		// Which half of the buffer was played?
		int							curSection;			// Which half of the buffer IS played?
		bool						isParallel;			// Is this a parallel sound?
	};
}
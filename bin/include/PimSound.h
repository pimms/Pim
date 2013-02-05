#pragma once

#include "PimInternal.h"
#include "PimAudioManager.h"


namespace Pim {
	class AudioManager;
	class Vec2;
	struct SoundCache;


	enum PlaybackMethod {
		PLAYBACK_UNDEFINED,
		PLAYBACK_STREAM, 
		PLAYBACK_CACHE,
	};


	class Sound {
	protected:
		friend class AudioManager;

	public:
		static string			OggErrorString(int errCode);
								Sound(string file, PlaybackMethod method);
								Sound();
								~Sound();
		bool					Stream(string file);
		bool					Cache(string file);
		bool					IsPlaying();
		bool					Play();
		bool					Loop();
		bool					Pause();
		bool					Rewind();
		void					SetLoop(bool flag);
		bool					GetLoop();
		void					SetVolume(float volume);	// Range: [0.0 - 1.0]
		void					SetSourcePosition(float x, float y, float z);
		void					SetSourcePosition(Vec2 pos);
		float					GetTime();					// Returns the current position in seconds
		PlaybackMethod			GetPlaybackMethod();

	protected:
		FILE					*oggFile;
		OggVorbis_File			*oggStream;
		vorbis_info				*vorbisInfo;
		ALuint					buffers[2];
		ALuint					source;
		ALenum					format;
		PlaybackMethod			pbMethod;
		bool					requiresInitialFill;	// The buffers must be filled at first Play()
		unsigned long			bytePos;
		bool					loop;

		bool					Update();
		bool					FillBuffer(ALuint buffer);
		bool					Init(string file);
		bool					InitialFill();
		void					Clear();
	};
}
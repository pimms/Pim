#pragma once

#include "PimInternal.h"
#include "PimAudioManager.h"


namespace Pim {
	/**
	 @class			Sound
	 @brief 		Interface for playing a sound.
	 @details 		Play a sound. Only OGG files are supported.
	 
	 				As of 0.9, it is only possible to stream audio from 
					file, but eventually caching (as existed in 0.7) will
	 				be reimplemented for OpenAL.
	 */
	
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
								Sound(string audioSource, PlaybackMethod method);
								Sound();
								~Sound();
		bool					Stream(string file);
		bool					Cache(string source);
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
	
	/**
	 @fn 			Sound(string file, PlaybackMethod method)
	 @brief 		Constructor defining a file and a playback method
	 @param 		audioSource
	 				If the playback method is @e PLAYBACK_STREAM, the audioSource
	 				parameter must be an OGG-file.
	 
	 				If the playback method is @e PLAYBACK_CACHE, the audioSource
					parameter must be the unique identifier of a sound loaded 
	 				into the AudioManager's sound cache.
	 */
	
	/**
	 @fn 			Stream
	 @brief 		Stream audio from a file.
	 */
	
	/**
	 @fn 			Cache
	 @brief 		Cache a sound file identified by a unique id string @e source.
	 */
	
	/**
	 @fn 			SetLoop
	 @brief			Enable or disable looping. Looping is idefinate, and must be
	 				stopped manually.
	 */
	
	/**
	 @fn			SetVolume
	 @brief 		Provide a value between 0 and 1, and the volume will abide your
	 				numerical value.
	 */
	
	/**
	 @fn			SetSourcePosition
	 @brief			[NOT WORKING] Set the OpenAL source at position [X,Y,Z].
	 */
	
	/**
 	 @fn 			GetTime
 	 @brief			The current position in the audio file in seconds.
	 */
}
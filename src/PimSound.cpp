#include "PimSound.h"
#include "PimAudioManager.h"
#include "PimVec2.h"
#include "PimAssert.h"

namespace Pim {

	/*
	==================
	Sound::OggErrorString
	==================
	*/
	string Sound::OggErrorString(int errCode) {
		switch(errCode)
		{
			case OV_EREAD:
				return string("Read from media.");
			case OV_ENOTVORBIS:
				return string("Not Vorbis data.");
			case OV_EVERSION:
				return string("Vorbis version mismatch.");
			case OV_EBADHEADER:
				return string("Invalid Vorbis header.");
			case OV_EFAULT:
				return string("Internal logic fault (bug or heap/stack corruption.");
			default:
				return string("Unknown Ogg error.");
		}
	}

	/*
	==================
	Sound::Sound
	==================
	*/
	Sound::Sound(string file, PlaybackMethod method) {
		oggFile				= NULL;
		oggStream			= NULL;
		vorbisInfo			= NULL;
		
		bytePos				= 0;
		cacheBuffer 		= NULL;
		
		buffers[0]			= 0;
		buffers[1]			= 0;
		source				= 0;
		audioData.format 	= 0;
		audioData.frequency	= 0;
		pbMethod			= PLAYBACK_UNDEFINED;
		requiresInitialFill	= true;
		loop				= false;
		deleteWhenDone 		= true;

		AudioManager::GetSingleton()->AddSound(this);

		PimAssert(method != PLAYBACK_UNDEFINED, "Error: Must provide valid playback method!");

		if (method == PLAYBACK_CACHE) {
			Cache(file);
		} else if (method == PLAYBACK_STREAM) {
			Stream(file);
		} 
	}

	/*
	==================
	Sound::Sound
	==================
	*/
	Sound::Sound() {
		oggFile				= NULL;
		oggStream			= NULL;
		vorbisInfo			= NULL;
		
		bytePos				= 0;
		cacheBuffer			= NULL;
		
		buffers[0]			= 0;
		buffers[1]			= 0;
		source				= 0;
		audioData.format	= 0;
		audioData.frequency	= 0;
		pbMethod			= PLAYBACK_UNDEFINED;
		requiresInitialFill	= true;
		loop				= false;
		deleteWhenDone 		= true;

		AudioManager::GetSingleton()->AddSound(this);
	}

	/*
	==================
	Sound::~Sound
	==================
	*/
	Sound::~Sound() {
		Clear();

		AudioManager::GetSingleton()->RemoveSound(this);
	}

	/*
	==================
	Sound::Stream

	Stream a sound from file. If the track will EVER be played two or more times
	simultaneously, use Cache instead.  
	==================
	*/
	bool Sound::Stream(string file) {
		/* Stream cannot be called twice on the same object */
		if (pbMethod != PLAYBACK_UNDEFINED) {
			return false;
		}

		pbMethod = PLAYBACK_STREAM;

		if (!Init(file)) {
			return false;
		}

		return true;
	}

	/*
	==================
	Sound::Cache

	Play a sound from a pre-loaded cache.
	==================
	*/
	bool Sound::Cache(string file) {
		/* Cache cannot be called twice on the same object */
		if (pbMethod != PLAYBACK_UNDEFINED) {
			return false;
		}

		pbMethod = PLAYBACK_CACHE;
		
		/* Cache the sound */
		if (!AudioManager::CacheSound(file)) {
			return false;
		}

		if (!Init(file)) {
			return false;
		}

		return true;
	}

	/*
	==================
	Sound::IsPlaying
	==================
	*/
	bool Sound::IsPlaying() {
		ALenum state;

		alGetSourcei(source, AL_SOURCE_STATE, &state);

		return (state == AL_PLAYING);
	}

	/*
	==================
	Sound::Play
	==================
	*/
	bool Sound::Play() {
		if (IsPlaying()) {
			return true;
		}

		if (requiresInitialFill) {
			InitialFill();
		}

		alSourcePlay(source);

		return true;
	}

	/*
	==================
	Sound::Loop
	==================
	*/
	bool Sound::Loop() {
		SetLoop(true);
		return Play();
	}
	
	/*
	==================
	Sound::Pause
	==================
	*/
	bool Sound::Pause() {
		if (IsPlaying()) {
			alSourcePause(source);
			return true;
		}
		return false;
	}

	/*
	==================
	Sound::Rewind
	==================
	*/
	bool Sound::Rewind() {
		alSourceStop(source);
	
		if (pbMethod == PLAYBACK_STREAM) {
			if (ov_pcm_seek(oggStream, 0) < 0) {
				return false;
			}
		} else {
			bytePos = 0;
		}

		alSourceUnqueueBuffers(source, 2, buffers);

		if (!FillBuffer(buffers[0])) {
			return false;
		}

		if (!FillBuffer(buffers[1])) {
			return false;
		}

		alSourceQueueBuffers(source, 2, buffers);

		alSourceRewind(source);

		return true;
	}

	/*
	==================
	Sound::SetLoop
	==================
	*/
	void Sound::SetLoop(bool flag) {
		loop = flag;
	}

	/*
	==================
	Sound::GetLoop
	==================
	*/
	bool Sound::GetLoop() {
		return loop;
	}
	
	/*
	==================
	Sound::SetDeleteWhenDone
	==================
	*/
	void Sound::SetDeleteWhenDone(bool flag) {
		deleteWhenDone = flag;
	}
	
	/*
	==================
	Sound::GetDeleteWhenDone
	==================
	*/
	bool Sound::GetDeleteWhenDone() {
		return deleteWhenDone;
	}

	/*
	==================
	Souns::SetVolume
	==================
	*/
	void Sound::SetVolume(float volume) {
		alSourcef(source, AL_GAIN, volume);
	}

	/*
	==================
	Sound::SetSourcePosition
	==================
	*/
	void Sound::SetSourcePosition(float x, float y, float z) {
		alSource3f(source, AL_POSITION, x, y, z);
	}

	/*
	==================
	Sound::SetSourcePosition
	==================
	*/
	void Sound::SetSourcePosition(Vec2 pos) {
		SetSourcePosition(pos.x, pos.y, 0.f);
	}

	/*
	==================
	Sound::GetTime
	==================
	*/
	float Sound::GetTime() {
		int curBytes = 0;
		alGetSourcei(source, AL_BYTE_OFFSET, &curBytes);

		float pos = float(bytePos + curBytes) / float(vorbisInfo->rate) / 2.f;

		if (audioData.format == AL_FORMAT_STEREO16) {
			pos /= 2.f;
		}

		return pos;
	}

	/*
	==================
	Sound::GetPlaybackMethod
	==================
	*/
	PlaybackMethod Sound::GetPlaybackMethod() {
		return pbMethod;
	}

	/*
	==================
	Sound::Update
	==================
	*/
	bool Sound::Update() {
		if (!IsPlaying()) {
			/* Nothing went wrong, so there's no need to alert AudioManager */
			return true;
		}

		int	processed = 0;
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

		while (processed--) {
			/*
				There's no need to initialize the 'buffer' variable, as it
				is set to the relevant buffer when calling alSourceUnqueueBuffers.
			*/
			ALuint buffer;
			alSourceUnqueueBuffers(source, 1, &buffer);

			if (!FillBuffer(buffer)) {
				/* Either something went wrong, or we've reached
				 * the end of the sound-buffer. The O-So-Mighty-AudioManager will
				 * delete this sound if (deleteWhenDone && !loop)
				 */
				return false;
			}
			
			alSourceQueueBuffers(source, 1, &buffer);
		}

		return true;
	}

	/*
	==================
	Sound::FillBuffer

	Fills the buffer by streaming from an appropriate buffer.
	 
	If PLAYBACK_CACHE:
	 	The vector<char> *cacheBuffer is used to fill the buffer.
	
	if PLAYBACK_STREAM:
	 	The OggVorbis_File *oggStream is used to fill the buffer.
	==================
	*/
	bool Sound::FillBuffer(ALuint buffer) {
		char	data[BUFFER_SIZE];
		int		size	= 0;
		int		section = 0;
		long	result	= 0;
		
		while (size < BUFFER_SIZE) {
			if (pbMethod == PLAYBACK_STREAM) {
				result = ov_read(oggStream, data+size, BUFFER_SIZE-size, 0, 2, 1, &section);
			} else if (pbMethod == PLAYBACK_CACHE) {
				/* Calculate how much we can copy from the cache */
				long remainding = cacheBuffer->size() - bytePos;
				result = min<long>(remainding, BUFFER_SIZE-size);
				
				/* Perform the copy */
				if (result > 0) {
					copy(cacheBuffer->begin()+bytePos,
						 cacheBuffer->begin()+bytePos+result,
						 data+size);
					bytePos += result;
				}
			} else {
				PimAssert(false, "FillBuffer on PLAYBACK_UNDEFINED sound-object");
			}

			if (result > 0) {
				size += result;
			} else if (result < 0) {
				/* An error occurred */
				if (pbMethod == PLAYBACK_STREAM) {
					printf("OggVorbis error: %s\n", OggErrorString((int)result).c_str());
				}
				return false;
			} else {
				/* We've reached the end of the buffer */
				if (loop) {
					if (pbMethod == PLAYBACK_STREAM) {
						ov_pcm_seek(oggStream, 0);
					} else {
						bytePos = 0;
					}
				} else {
					/* We're not looping, and the end has beed reached */
					if (!size) {
						return false;
					} else {
						/* Fill the buffer with what we've got */
						break;
					}
				}
			}
		}
		
		/* Fill the OpenAL buffer with the retrieved data */
		alBufferData(buffer, audioData.format, data, size, (int)audioData.frequency);

		return true;
	}

	/*
	==================
	Sound::Init

	Initiates the file handles, sources and buffers.
	==================
	*/
	bool Sound::Init(string file) {
		Clear();

		if (pbMethod == PLAYBACK_STREAM) {
			if (!(oggFile = fopen(file.c_str(), "rb"))) {
				return false;
			}
			
			/* Attempt to read from the file as an Ogg-file */
			oggStream = new OggVorbis_File;
			
			int result;
			if ((result = ov_open(oggFile, oggStream, NULL, 0)) < 0) {
				fclose(oggFile);
				
				delete oggStream;
				oggStream = NULL;
				
				return false;
			}
			
			/* Read format data from the file */
			vorbisInfo = ov_info(oggStream, -1);

			if (vorbisInfo->channels == 1) {
				audioData.format = AL_FORMAT_MONO16;
			} else {
				audioData.format = AL_FORMAT_STEREO16;
			}
			
			audioData.frequency = vorbisInfo->rate;
		} else if (pbMethod == PLAYBACK_CACHE) {
			cacheBuffer = AudioManager::GetSingleton()->GetCacheBytes(file);
			PimAssert(cacheBuffer != NULL, "Failed to retrieve cache buffer");
			
			audioData = AudioManager::GetSingleton()->GetCacheData(file);
			PimAssert(audioData.format != 0, "Failed to retrieve cache format");
			PimAssert(audioData.frequency != 0, "Failed to retrieve cache frequency");
		} else {
			PimAssert(0, "Init called on PLAYBACK_UNDEFINED Sound object");
			return false;
		}

		alGenBuffers(2, buffers);
		alGenSources(1, &source);

		alSource3f(source, AL_POSITION,			0.f, 0.f, 0.f);
		alSource3f(source, AL_VELOCITY,			0.f, 0.f, 0.f);
		alSource3f(source, AL_DIRECTION,		0.f, 0.f, 0.f);
		alSourcef (source, AL_ROLLOFF_FACTOR,	0.f			 );
		alSourcei (source, AL_SOURCE_RELATIVE,	AL_TRUE		 );
		
		return true;
	}

	/*
	==================
	Sound::InitialFill

	When first starting playing, the buffers needs to be filled.
	==================
	*/
	bool Sound::InitialFill() {
		PimAssert(requiresInitialFill, "Internal sound error");
		
		if (!FillBuffer(buffers[0])) {
			return false;
		}
		
		if (!FillBuffer(buffers[1])) {
			return false;
		}

		alSourceQueueBuffers(source, 2, buffers);
		requiresInitialFill = false;

		return true;
	}

	/*
	==================
	Sound::Clear
	==================
	*/
	void Sound::Clear() {
		cacheBuffer = NULL;
		bytePos = 0;
		
		if (source) {
			alSourceStop(source);
		}

		if (buffers[0]) {
			alDeleteBuffers(1, &buffers[0]);	
		}

		if (buffers[1]) {
			alDeleteBuffers(1, &buffers[1]);
		}

		if (source) {
			alDeleteSources(1, &source);
		}

		if (oggFile) {
			fclose(oggFile);
		}

		if (oggStream) {
			ov_clear(oggStream);
			delete oggStream;
		}
	}
}
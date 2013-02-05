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
		buffers[0]			= 0;
		buffers[1]			= 0;
		source				= 0;
		format				= 0;
		pbMethod			= PLAYBACK_UNDEFINED;
		requiresInitialFill	= true;
		bytePos				= 0;
		loop				= false;

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
		buffers[0]			= 0;
		buffers[1]			= 0;
		source				= 0;
		format				= 0;
		pbMethod			= PLAYBACK_UNDEFINED;
		requiresInitialFill	= true;
		bytePos				= 0;
		loop				= false;

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
		if (pbMethod != PLAYBACK_UNDEFINED) {
			return false;
		}

		pbMethod = PLAYBACK_CACHE;

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

			alSourceUnqueueBuffers(source, 2, buffers);

			if (!FillBuffer(buffers[0])) {
				return false;
			}

			if (!FillBuffer(buffers[1])) {
				return false;
			}

			alSourceQueueBuffers(source, 2, buffers);
		}

		alSourceRewind(source);

		bytePos = 0;

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

		if (format == AL_FORMAT_STEREO16) {
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
			return false;
		}

		int	processed = 0;

		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

		while (processed--) {
			bytePos += BUFFER_SIZE;

			/*
				There's no need to initialize the 'buffer' variable, as it
				is set to the relevant buffer when calling alSourceUnqueueBuffers.
			*/
			ALuint buffer;
			alSourceUnqueueBuffers(source, 1, &buffer);

			if (!FillBuffer(buffer)) {
				return false;	// There is nothing more to fill
			}

			alSourceQueueBuffers(source, 1, &buffer);
		}

		return true;
	}

	/*
	==================
	Sound::FillBuffer

	Fills the buffer by streaming from file
	==================
	*/
	bool Sound::FillBuffer(ALuint buffer) {
		char	data[BUFFER_SIZE];
		int		size	= 0;
		int		section = 0;
		int		result	= 0;

		while (size < BUFFER_SIZE) {
			result = ov_read(oggStream, data+size, BUFFER_SIZE-size, 0, 2, 1, &section);

			if (result > 0) {
				size += result;
			} else if (result < 0) {
				#ifdef _DEBUG
				printf("OggVorbis error: %s\n", OggErrorString(result).c_str());
				#endif
				return false;
			} else {
				if (pbMethod == PLAYBACK_STREAM) {
					if (loop) {
						ov_pcm_seek(oggStream, 0);
					}
				}
			}
		}

		if (!size) {
			return false;
		}

		alBufferData(buffer, format, data, size, vorbisInfo->rate);

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

		int result;

		if (!(oggFile = fopen(file.c_str(), "rb"))) {
			return false;
		}

		oggStream = new OggVorbis_File;

		if ((result = ov_open(oggFile, oggStream, NULL, 0)) < 0) {
			fclose(oggFile);
			return false;
		}

		vorbisInfo = ov_info(oggStream, -1);

		if (vorbisInfo->channels == 1) {
			format = AL_FORMAT_MONO16;
		} else {
			format = AL_FORMAT_STEREO16;
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
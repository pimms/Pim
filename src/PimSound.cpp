#include "PimSound.h"
#include "PimAudioManager.h"
#include "PimAssert.h"

namespace Pim {
	/*
	=====================
	Sound::Sound
	=====================
	*/
	Sound::Sound(string file) {
		oggFile			= NULL;
		buffer			= NULL;
		isLoop			= false;
		almostDone		= false;
		done			= false;
		audioStream		= false;
		isParallel		= false;
		deleteWhenDone	= false;
		cachePosition	= 0;

		LoadFile(file);
	}

	/*
	=====================
	Sound::Sound
	=====================
	*/
	Sound::Sound() {
		oggFile			= NULL;
		buffer			= NULL;
		isLoop			= false;
		almostDone		= false;
		done			= false;
		audioStream		= false;
		isParallel		= false;
		deleteWhenDone	= false;
		cachePosition	= 0;
	}

	/*
	=====================
	Sound::~Sound
	=====================
	*/
	Sound::~Sound() {
		if (buffer) {
			buffer->Release();
		}

		if (oggFile && !isParallel) {
			ov_clear(oggFile);
			delete oggFile;
		}

		// No matter what happens, unschedule to be safe.
		AudioManager::GetSingleton()->UnscheduleOggUpdate(this);
	}

	/*
	=====================
	Sound::LoadFile
	=====================
	*/
	void Sound::LoadFile(const string file) {
		PimAssert(file.length() >= 4, "Error: invalid file");

		filename = file;

		string format = file.substr(file.length()-4, 4);

		if (format == ".ogg") {
			oggFile = new OggVorbis_File;
			curSection = 0;
			lastSection = 1;
			audioStream = true;

			if (!AudioManager::GetSingleton()->LoadOgg(file.c_str(), this)) {
				cout<<"ERROR: Failed to load file: " <<file <<"\n";
			} else {
				AudioManager::GetSingleton()->ScheduleOggUpdate(this);
			}
		} else {
			cout<<"ERROR! PIM DOES NOT SUPPORT THE FOLLOWING AUDIO FORMAT: "
					 <<file <<"\n";
		}
	}

	/*
	=====================
	Sound::UseCache
	=====================
	*/
	void Sound::UseCache(const string id) {
		PimAssert(AudioManager::GetSingleton()->cache.count(id) != 0,
				  "Error: file is not cached!");

		cache				= AudioManager::GetSingleton()->cache[id];
		curSection			= 0;
		lastSection			= 1;

		memset(&wfm, 0, sizeof(wfm));
		wfm.cbSize			= sizeof(wfm);
		wfm.nChannels		= cache->channels;
		wfm.wBitsPerSample	= 16;
		wfm.nSamplesPerSec	= cache->rate;
		wfm.nAvgBytesPerSec	= wfm.nSamplesPerSec * wfm.nChannels * 2;
		wfm.nBlockAlign		= 2 * wfm.nChannels;
		wfm.wFormatTag		= 1;

		// Prepare the buffer
		desc.dwSize			= sizeof(desc);
		desc.dwFlags		= DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN;
		desc.lpwfxFormat	= &wfm;
		desc.dwReserved		= 0;
		desc.dwBufferBytes	= BUFFER_SIZE * 2;

		buffer = AudioManager::GetSingleton()->CreateBuffer(&wfm, &desc);

		AudioManager::GetSingleton()->ScheduleOggUpdate(this);
		AudioManager::GetSingleton()->FillBuffer(this, 0);
	}

	/*
	=====================
	Sound::Play
	=====================
	*/
	void Sound::Play() {
		if (done) {
			Replay();
		} else {
			buffer->Play(0,0,DSBPLAY_LOOPING);
		}
	}

	/*
	=====================
	Sound::Replay
	=====================
	*/
	void Sound::Replay() {
		if (audioStream) {
			AudioManager::GetSingleton()->RewindOgg(this);
		} else {
			cachePosition = 0;
			buffer->SetCurrentPosition(0);

			curSection = 0;
			lastSection = 0;

			AudioManager::GetSingleton()->FillBuffer(this, 0);
		}

		Play();
	}

	/*
	=====================
	Sound::Loop
	=====================
	*/
	void Sound::Loop() {
		buffer->Play(0,0,DSBPLAY_LOOPING);
		isLoop = true;
	}

	/*
	=====================
	Sound::Reloop
	=====================
	*/
	void Sound::Reloop() {
		if (audioStream) {
			AudioManager::GetSingleton()->RewindOgg(this);
		} else {
			cachePosition = 0;
			buffer->SetCurrentPosition(0);

			curSection = 0;
			lastSection = 0;

			AudioManager::GetSingleton()->FillBuffer(this, 0);
		}

		Loop();
	}

	/*
	=====================
	Sound::Pause
	=====================
	*/
	void Sound::Pause() {
		buffer->Stop();
	}
	
	/*
	=====================
	Sound::SetVolume
	=====================
	*/
	void Sound::SetVolume(float vol) {
		buffer->SetVolume(LONG(-8000 * (1.f-vol)));
	}

	/*
	=====================
	Sound::SetPan
	=====================
	*/
	void Sound::SetPan(float pan) {
		buffer->SetPan(LONG(pan * 3500.f));
	}

	/*
	=====================
	Sound::Position
	=====================
	*/
	float Sound::Position() {
		if (audioStream) {
			return (float)ov_time_tell(oggFile);
		} else {
			return (float)cachePosition / (float)cache->rate;
		}
	}

	/*
	=====================
	Sound::Position
	=====================
	*/
	int Sound::Position(float time) {
		if (audioStream) {
			int ret = ov_time_seek(oggFile, (double)time);

			if (!ret) {
				AudioManager::GetSingleton()->FillBuffer(this, 0);
				buffer->SetCurrentPosition(0);
			}

			return ret;
		}

		return 1;
	}
	
	/*
	=====================
	Sound::PlayParallel
	=====================
	*/
	Sound* Sound::PlayParallel(bool delWhenDone) {
		Sound *s = new Sound(filename);
		s->deleteWhenDone = delWhenDone;
		s->Play();
		return s;
	}

	/*
	=====================
	Sound::GetBuffer
	=====================
	*/
	const IDirectSoundBuffer8* Sound::GetBuffer() const {
		return buffer;
	}
}
#include "PimAudioManager.h"

#include "PimGameControl.h"
#include "PimAssert.h"
#include "PimRenderWindow.h"
#include "PimSound.h"

namespace Pim {
	AudioManager* AudioManager::singleton = NULL;

	/*
	=====================
	AudioManager::GetSingleton
	=====================
	*/
	AudioManager* AudioManager::GetSingleton() {
		return singleton;
	}

	/*
	=====================
	AudioManager::AudioManager
	=====================
	*/
	AudioManager::AudioManager() {
		m_DirectSound = 0;

		HWND hWnd = GameControl::GetSingleton()->GetRenderWindow()->GetHwnd();

		DirectSoundCreate8(NULL, &m_DirectSound, NULL);
		m_DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	}

	/*
	=====================
	AudioManager::~AudioManager
	=====================
	*/
	AudioManager::~AudioManager() {
		// Delete scheduled, delete-when-done Sounds.
		for (unsigned int i=0; i<oggSounds.size(); i++) {
			if (oggSounds[i]->deleteWhenDone) {
				delete oggSounds[i];
			}
		}

		// Release all cached objects
		for (auto it=cache.begin(); it!=cache.end(); it++) {
			it->second->buf.clear();
			delete it->second;
		}

		if (m_DirectSound) {
			m_DirectSound->Release();
		}
	}

	/*
	=====================
	AudioManager::InstantiateSingleton
	=====================
	*/
	void AudioManager::InstantiateSingleton() {
		PimAssert(singleton == NULL, "Error: AudioManager singleton instantiated twice.");
		singleton = new AudioManager;
	}

	/*
	=====================
	AudioManager::ClearSingleton
	=====================
	*/
	void AudioManager::ClearSingleton() {
		PimAssert(singleton != NULL, "Error: AudioManager was never initialized.");
		delete singleton;
		singleton = NULL;
	}

	/*
	=====================
	AudioManager::CacheOgg
	=====================
	*/
	bool AudioManager::CacheOgg(const string id, char *filename) {
		DeleteCache(id);

		OggVorbis_File *file = new OggVorbis_File;
		FILE *f;

		if (fopen_s(&f, filename, "rb")) {
			return false;
		}

		if (ov_open(f, file, NULL, 0)) {
			fclose(f);
			return false;
		}

		vorbis_info *vi = ov_info(file, -1);

		cache[id] = new SoundCache;
		cache[id]->size = 0;
		cache[id]->rate = vi->rate;
		cache[id]->channels = vi->channels;

		int ret = 1;
		int sec = 0;

		while (ret) {
			char b[512];
			ret = ov_read(file, b, 512, 0, 2, 1, &sec);
			cache[id]->buf.insert(cache[id]->buf.end(), b, b+ret);
			cache[id]->size += ret;
		}

		ov_clear(file);
		delete file;

		return true;
	}

	/*
	=====================
	AudioManager::DeleteCache
	=====================
	*/
	void AudioManager::DeleteCache(const string id) {
		if (cache.count(id)) {
			delete cache[id];
			cache.erase(id);
		}
	}

	/*
	=====================
	AudioManager::LoadOgg
	=====================
	*/
	bool AudioManager::LoadOgg(const char *filename, Sound *s) {
		int error;
		FILE *f;

		error = fopen_s(&f, filename, "rb");
		if (error != 0) {
			return false;
		}

		if (ov_open(f, s->oggFile, NULL, 0) != 0) {
			fclose(f);
			return false;
		}

		vorbis_info *vi = ov_info(s->oggFile, -1);

		// Wave format
		memset(&s->wfm, 0, sizeof(s->wfm));
		s->wfm.cbSize			= sizeof(s->wfm);
		s->wfm.nChannels		= vi->channels;
		s->wfm.wBitsPerSample	= 16;
		s->wfm.nSamplesPerSec	= vi->rate;
		s->wfm.nAvgBytesPerSec	= s->wfm.nSamplesPerSec * s->wfm.nChannels * 2;
		s->wfm.nBlockAlign		= 2 * s->wfm.nChannels;
		s->wfm.wFormatTag		= 1;

		// Prepare the buffer
		s->desc.dwSize			= sizeof(s->desc);
		s->desc.dwFlags			= DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN;
		s->desc.lpwfxFormat		= &s->wfm;
		s->desc.dwReserved		= 0;
		s->desc.dwBufferBytes	= BUFFER_SIZE * 2;

		s->buffer = CreateBuffer(&s->wfm, &s->desc);

		FillBuffer(s, 0);

		return true;
	}

	/*
	=====================
	AudioManager::FillBuffer
	=====================
	*/
	void AudioManager::FillBuffer(Sound *s, int section) {
		DWORD size = BUFFER_SIZE;
		char *buf;

		// Fill the section we just left
		s->buffer->Lock(section*BUFFER_SIZE, size, (LPVOID*)&buf, &size, 0, 0, 0);

		DWORD	pos = 0;
		int		sec = 0;
		int		ret = 1;

		if (s->audioStream) {
			while (ret && pos < size) {
				ret = ov_read(s->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
				pos += ret;
			}

			// If we reached the end of the file but we're looping, rewind to the
			// beginning and start filling from there.
			if (!ret && s->isLoop) {
				ret = 1;
				ov_pcm_seek(s->oggFile, 0);
				while (ret && pos<size) {
					ret = ov_read(s->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
					pos += ret;
				}
			}

			// We've reached the end of the file, and we're not looping. Nullify the
			// remaining part of the buffer.
			else if (!ret && !s->isLoop) {
				while (pos < size) {
					*(buf+pos++) = 0;
				}
				s->almostDone = true;
			}
		} else {
			// Don't copy more than the cache holds
			int toCopy = size;
			unsigned remainding = s->cache->size - s->cachePosition;
			if (remainding < size) {
				toCopy = remainding;
			}

			// Copy the remainding into the buffer
			memcpy(buf, &s->cache->buf[0]+s->cachePosition, toCopy);
			s->cachePosition += toCopy;

			// If we're looping and has reached the end of the cache
			if (remainding < size && s->isLoop) {
				toCopy = size - toCopy;

				memcpy(buf, &s->cache->buf[0], toCopy);
				s->cachePosition = toCopy;
			}

			// We've reached the end, but we're not looping
			else if (remainding < size && !s->isLoop) {
				// Null the rest of the buffer
				memset(buf, 0, size-toCopy);
				s->cachePosition = s->cache->size;

				s->almostDone = true;
			}
		}

		s->buffer->Unlock(buf, size, NULL, NULL);
	}

	/*
	=====================
	AudioManager::OggUpdate
	=====================
	*/
	void AudioManager::OggUpdate() {
		// Thanks to some dude on the internet for the stream functionality.

		DWORD pos;

		vector<Sound*> soundsToDelete;

		for each (Sound *s in oggSounds) {
			s->buffer->GetCurrentPosition(&pos, NULL);
			s->curSection = (pos < BUFFER_SIZE) ? (0) : (1);

			if (s->curSection != s->lastSection) {
				if (s->done && s->isLoop) {
					s->Pause();
				}

				if (s->almostDone && !s->isLoop) {
					s->done = true;
				}

				FillBuffer(s, s->lastSection);
				s->lastSection = s->curSection;
			}

			if (s->deleteWhenDone && s->done && !s->isLoop) {
				soundsToDelete.push_back(s);
			}
		}

		for each (Sound *s in soundsToDelete) {
			delete s;
		}
	}

	/*
	=====================
	AudioManager::ScheduleOggUpdate
	=====================
	*/
	void AudioManager::ScheduleOggUpdate(Sound *sound) {
		oggSounds.push_back(sound);
	}

	/*
	=====================
	AudioManager::UnscheduleOggUpdate
	=====================
	*/
	void AudioManager::UnscheduleOggUpdate(Sound *sound) {
		for (unsigned int i=0; i<oggSounds.size(); i++) {
			if (oggSounds[i] == sound) {
				oggSounds.erase(oggSounds.begin() + i);
			}
		}
	}

	/*
	=====================
	AudioManager::RewindOgg
	=====================
	*/
	void AudioManager::RewindOgg(Sound *sound) {
		DWORD size = BUFFER_SIZE;
		char *buf;
		DWORD	pos = 0;
		int		sec = 0;
		int		ret = 1;

		sound->curSection = 0;
		sound->lastSection = 1;
		sound->done = false;
		sound->almostDone = false;

		sound->buffer->Lock(sound->lastSection*BUFFER_SIZE, size, (LPVOID*)&buf, &size, 0, 0, 0);

		// Clear the entire buffer
		memset(buf, 0, BUFFER_SIZE);

		ret = 1;
		ov_pcm_seek(sound->oggFile, 0);
		while (ret && pos<size) {
			ret = ov_read(sound->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
			pos += ret;
		}

		sound->buffer->Unlock(buf, size, NULL, NULL);
		sound->buffer->SetCurrentPosition(0);
		sound->lastSection = sound->curSection;
	}

	/*
	=====================
	AudioManager::CreateBuffer
	=====================
	*/
	IDirectSoundBuffer8* AudioManager::CreateBuffer(WAVEFORMATEX *wfm, DSBUFFERDESC *desc) {
		IDirectSoundBuffer *tempBuffer;

		m_DirectSound->CreateSoundBuffer(desc, &tempBuffer, NULL);

		IDirectSoundBuffer8 *ret = 0;
		tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*&ret);
		tempBuffer->Release();

		return ret;
	}
}
#include "PimAudioManager.h"

#include "PimGameControl.h"
#include "PimException.h"
#include "PimRenderWindow.h"
#include "PimSound.h"

namespace Pim
{
	AudioManager* AudioManager::singleton = NULL;

	AudioManager* AudioManager::getSingleton()
	{
		return singleton;
	}

	AudioManager::AudioManager()
	{
		m_DirectSound = 0;

		HWND hWnd = GameControl::getSingleton()->getRenderWindow()->getHwnd();

		DirectSoundCreate8(NULL, &m_DirectSound, NULL);
		m_DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	}
	AudioManager::~AudioManager()
	{
		// Delete scheduled, delete-when-done Sounds.
		for (unsigned int i=0; i<oggSounds.size(); i++)
		{
			if (oggSounds[i]->deleteWhenDone)
			{
				delete oggSounds[i];
			}
		}

		// Release all cached objects
		for (auto it=cache.begin(); it!=cache.end(); it++)
		{
			it->second->buf.clear();
			delete it->second;
		}
		cache.clear();

		if (m_DirectSound)
		{
			m_DirectSound->Release();
		}
	}

	void AudioManager::instantiateSingleton()
	{
#ifdef _DEBUG
		PimAssert(singleton == NULL, "Error: AudioManager singleton instantiated twice.");
#endif
		singleton = new AudioManager;
	}
	void AudioManager::clearSingleton()
	{
#ifdef _DEBUG
		PimAssert(singleton != NULL, "Error: AudioManager was never initialized.");
#endif
		delete singleton;
		singleton = NULL;
	}

	bool AudioManager::cacheOgg(std::string id, char *filename)
	{
		deleteCache(id);

		OggVorbis_File *file = new OggVorbis_File;
		FILE *f;

		if (fopen_s(&f, filename, "rb"))
		{
			return false;
		}

		if (ov_open(f, file, NULL, 0))
		{
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

		while (ret)
		{
			char b[512];
			ret = ov_read(file, b, 512, 0, 2, 1, &sec);
			cache[id]->buf.insert(cache[id]->buf.end(), b, b+ret);
			cache[id]->size += ret;
		}

		ov_clear(file);
		delete file;

		return true;
	}
	void AudioManager::deleteCache(std::string id)
	{
		if (cache.count(id))
		{
			delete cache[id];
			cache.erase(id);
		}
	}

	/* THE WAVE FORMAT IS NO LONGER SUPPORTED */
	/*
	bool AudioManager::loadWav(const char *filename, Sound *s)
	{
		// Thanks to Raster Tek (rastertek.com) for the following code.
		FILE *filePtr;
		unsigned int count;
		WaveHeaderType waveFileHeader;
		HRESULT result;
		IDirectSoundBuffer *tempBuffer;
		unsigned char *waveData;
		unsigned char *bufferPtr;
		unsigned long bufferSize;

		// Open the wave file in binary.
		fopen_s(&filePtr, filename, "rb");
 
		// Read in the wave file header.
		fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);

		// Set the wave format of secondary buffer that this wave file will be loaded onto.
		s->wfm.wFormatTag = WAVE_FORMAT_PCM;
		s->wfm.nSamplesPerSec = 44100;
		s->wfm.wBitsPerSample = 16;
		s->wfm.nChannels = 2;
		s->wfm.nBlockAlign = (s->wfm.wBitsPerSample / 8) * s->wfm.nChannels;
		s->wfm.nAvgBytesPerSec = s->wfm.nSamplesPerSec * s->wfm.nBlockAlign;
		s->wfm.cbSize = 0;
 
		// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
		s->desc.dwSize = sizeof(DSBUFFERDESC);
		s->desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN;
		s->desc.dwBufferBytes = waveFileHeader.dataSize;
		s->desc.dwReserved = 0;
		s->desc.lpwfxFormat = &s->wfm;
		s->desc.guid3DAlgorithm = GUID_NULL;

		s->buffer = createBuffer(&s->wfm, &s->desc);

		// Move to the beginning of the wave data which starts at the end of the data chunk header.
		fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);
 
		// Create a temporary buffer to hold the wave file data.
		waveData = new unsigned char[waveFileHeader.dataSize];
 
		// Read in the wave file data into the newly created buffer.
		fread(waveData, 1, waveFileHeader.dataSize, filePtr);
 
		fclose(filePtr);
 
		s->buffer->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr,
			(DWORD*)&bufferSize, NULL, 0, 0);

		memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

		s->buffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);

		delete [] waveData;
		waveData = 0;
 
		return true;
	}
	*/

	bool AudioManager::loadOgg(const char *filename, Sound *s)
	{
		int error;
		FILE *f;

		error = fopen_s(&f, filename, "rb");
		if (error != 0)
			return false;

		if (ov_open(f, s->oggFile, NULL, 0) != 0)
		{
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
		
		s->buffer = createBuffer(&s->wfm, &s->desc);

		fillBuffer(s, 0);

		return true;
	}

	void AudioManager::fillBuffer(Sound *s, int section)
	{
		DWORD size = BUFFER_SIZE;
		char *buf;

		// Fill the section we just left
		s->buffer->Lock(section*BUFFER_SIZE, size, (LPVOID*)&buf, &size, 0, 0, 0);

		DWORD	pos = 0;
		int		sec = 0;
		int		ret = 1;

		if (s->audioStream)
		{
			while (ret && pos < size)
			{
				ret = ov_read(s->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
				pos += ret;
			}

			// If we reached the end of the file but we're looping, rewind to the 
			// beginning and start filling from there.
			if (!ret && s->isLoop)
			{
				ret = 1;
				ov_pcm_seek(s->oggFile, 0);
				while (ret && pos<size)
				{
					ret = ov_read(s->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
					pos += ret;
				}
			}

			// We've reached the end of the file, and we're not looping. Nullify the 
			// remaining part of the buffer.
			else if (!ret && !s->isLoop)
			{
				while (pos < size)
				{
					*(buf+pos++) = 0;
				}
				s->almostDone = true;
			}
		}
		else
		{
			// Don't copy more than the cache holds
			int toCopy = size;
			int remainding = s->cache->size - s->cachePosition;
			if (remainding < size)
			{
				toCopy = remainding;
			}
			
			// Copy the remainding into the buffer
			memcpy(buf, &s->cache->buf[0]+s->cachePosition, toCopy);
			s->cachePosition += toCopy;

			// If we're looping and has reached the end of the cache
			if (remainding < size && s->isLoop)
			{
				toCopy = size - toCopy;

				memcpy(buf, &s->cache->buf[0], toCopy);
				s->cachePosition = toCopy;
			}

			// We've reached the end, but we're not looping
			else if (remainding < size && !s->isLoop)
			{
				// Null the rest of the buffer
				memset(buf, 0, size-toCopy);
				s->cachePosition = s->cache->size;

				s->almostDone = true;
			}
		}

		s->buffer->Unlock(buf, size, NULL, NULL);
	}

	void AudioManager::oggUpdate()
	{
		// Thanks to some dude on the internet for the stream functionality.

		DWORD pos;

		std::vector<Sound*> soundsToDelete;

		for each (Sound *s in oggSounds)
		{
			s->buffer->GetCurrentPosition(&pos, NULL);
			s->curSection = (pos < BUFFER_SIZE) ? (0) : (1);

			if (s->curSection != s->lastSection)
			{
				if (s->done && s->isLoop)
					s->pause();

				if (s->almostDone && !s->isLoop)
					s->done = true;

				fillBuffer(s, s->lastSection);
				
				s->lastSection = s->curSection;
			}

			if (s->deleteWhenDone && s->done && !s->isLoop)
				soundsToDelete.push_back(s);
		}

		for each (Sound *s in soundsToDelete)
			delete s;
	}
	void AudioManager::scheduleOggUpdate(Sound *sound)
	{
		oggSounds.push_back(sound);
	}
	void AudioManager::unscheduleOggUpdate(Sound *sound)
	{
		for (unsigned int i=0; i<oggSounds.size(); i++)
			if (oggSounds[i] == sound)
				oggSounds.erase(oggSounds.begin() + i);
	}

	void AudioManager::rewindOgg(Sound *sound)
	{
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
		while (ret && pos<size)
		{
			ret = ov_read(sound->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
			pos += ret;
		}

		sound->buffer->Unlock(buf, size, NULL, NULL);

		sound->buffer->SetCurrentPosition(0);

		sound->lastSection = sound->curSection;
	}

	IDirectSoundBuffer8* AudioManager::createBuffer(WAVEFORMATEX *wfm, DSBUFFERDESC *desc)
	{
		IDirectSoundBuffer *tempBuffer;

		m_DirectSound->CreateSoundBuffer(desc, &tempBuffer, NULL); 

		IDirectSoundBuffer8 *ret = 0;
		tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*&ret);
		tempBuffer->Release();

		return ret;
	}
}
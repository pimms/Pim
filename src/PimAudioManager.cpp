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

		HWND hWnd = *Pim::GameControl::getSingleton()->getRenderWindow()->getHwnd();

		DirectSoundCreate8(NULL, &m_DirectSound, NULL);
		m_DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	}
	AudioManager::~AudioManager()
	{
		if (m_DirectSound)
			m_DirectSound->Release();
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

	bool AudioManager::loadWav(const char *filename, IDirectSoundBuffer8 **buffer)
	{
		// Thanks to Raster Tek (rastertek.com) for the following code.

		int error;
		FILE *filePtr;
		unsigned int count;
		WaveHeaderType waveFileHeader;
		WAVEFORMATEX waveFormat;
		DSBUFFERDESC bufferDesc;
		HRESULT result;
		IDirectSoundBuffer *tempBuffer;
		unsigned char *waveData;
		unsigned char *bufferPtr;
		unsigned long bufferSize;

		// Open the wave file in binary.
		error = fopen_s(&filePtr, filename, "rb");
		if(error != 0)
			return false;
 
		// Read in the wave file header.
		count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
		if(count != 1)
			return false;
 
		// Check that the chunk ID is the RIFF format.
		if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
		   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
			return false;
 
		// Check that the file format is the WAVE format.
		if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
			return false;
 
		// Check that the sub chunk ID is the fmt format.
		if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
			return false;
 
		// Check that the audio format is WAVE_FORMAT_PCM.
		if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
			return false;
 
		// Check that the wave file was recorded in stereo format.
		if(waveFileHeader.numChannels != 2)
			return false;
 
		// Check that the wave file was recorded at a sample rate of 44.1 KHz.
		if(waveFileHeader.sampleRate != 44100)
			return false;
 
		// Ensure that the wave file was recorded in 16 bit format.
		if(waveFileHeader.bitsPerSample != 16)
			return false;
 
		// Check for the data chunk header.
		if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
			return false;

		// Set the wave format of secondary buffer that this wave file will be loaded onto.
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nChannels = 2;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;
 
		// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN;
		bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = &waveFormat;
		bufferDesc.guid3DAlgorithm = GUID_NULL;

		// Create a temporary sound buffer with the specific buffer settings.
		result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
		if(FAILED(result))
			return false;
 
		// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
		result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*buffer);
		if(FAILED(result))
			return false;
 
		// Release the temporary buffer.
		tempBuffer->Release();
		tempBuffer = 0;

		// Move to the beginning of the wave data which starts at the end of the data chunk header.
		fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);
 
		// Create a temporary buffer to hold the wave file data.
		waveData = new unsigned char[waveFileHeader.dataSize];
		if(!waveData)
			return false;
 
		// Read in the wave file data into the newly created buffer.
		count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
		if(count != waveFileHeader.dataSize)
			return false;
 
		// Close the file once done reading.
		error = fclose(filePtr);
		if(error != 0)
		{
			return false;
		}
 
		// Lock the secondary buffer to write wave data into it.
		result = (*buffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
		if(FAILED(result))
			return false;
 
		// Copy the wave data into the buffer.
		memcpy(bufferPtr, waveData, waveFileHeader.dataSize);
 
		// Unlock the secondary buffer after the data has been written to it.
		result = (*buffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
		if(FAILED(result))
			return false;
	
		// Release the wave data since it was copied into the secondary buffer.
		delete [] waveData;
		waveData = 0;
 
		return true;
	}
	bool AudioManager::loadOgg(const char *filename, IDirectSoundBuffer8 **buffer, 
		OggVorbis_File *oggFile)
	{
		int error;
		FILE *f;

		error = fopen_s(&f, filename, "rb");
		if (error != 0)
			return false;

		ov_open(f, oggFile, NULL, 0);
		vorbis_info *vi = ov_info(oggFile, -1);

		// Wave format
		WAVEFORMATEX wfm;
		memset(&wfm, 0, sizeof(wfm));
		wfm.cbSize			= sizeof(wfm);
		wfm.nChannels		= vi->channels;
		wfm.wBitsPerSample	= 16;
		wfm.nSamplesPerSec	= vi->rate;
		wfm.nAvgBytesPerSec	= wfm.nSamplesPerSec * wfm.nChannels * 2;
		wfm.nBlockAlign		= 2 * wfm.nChannels;
		wfm.wFormatTag		= 1;

		// Prepare the buffer
		DSBUFFERDESC desc;
		desc.dwSize			= sizeof(desc);
		desc.dwFlags		= DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN;
		desc.lpwfxFormat	= &wfm;
		desc.dwReserved		= 0;
		desc.dwBufferBytes	= BUFFER_SIZE * 2;
		
		IDirectSoundBuffer *tempBuffer;
		m_DirectSound->CreateSoundBuffer(&desc, &tempBuffer, NULL);

		tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*buffer);

		tempBuffer->Release();
		tempBuffer = 0;

		DWORD	pos = 0;
		int		sec = 0;
		int		ret = 1;
		DWORD	size = BUFFER_SIZE * 2;
		char	*buf;

		(*buffer)->Lock(0, size, (LPVOID*)&buf, &size, NULL, NULL, DSBLOCK_ENTIREBUFFER);

		while (ret && pos<size)
		{
			ret = ov_read(oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
			pos += ret;
		}

		(*buffer)->Unlock(buf, size, NULL, NULL);

		return true;
	}

	void AudioManager::oggUpdate()
	{
		DWORD pos;

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

				DWORD size = BUFFER_SIZE;
				char *buf;

				// Fill the section we just left
				s->buffer->Lock(s->lastSection*BUFFER_SIZE, size, (LPVOID*)&buf, &size, 0, 0, 0);

				DWORD	pos = 0;
				int		sec = 0;
				int		ret = 1;

				while (ret && pos < size)
				{
					ret = ov_read(s->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
					pos += ret;
				}

				if ((!ret && s->isLoop) || s->rewindNext)
				{
					ret = 1;
					ov_pcm_seek(s->oggFile, 0);
					while (ret && pos<size)
					{
						ret = ov_read(s->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
						pos += ret;
					}

					s->rewindNext = false;
				}
				else if (!ret && !s->isLoop)
				{
					while (pos < size)
					{
						*(buf+pos) = 0;
						pos++;
					}
					s->almostDone = true;
				}

				s->buffer->Unlock(buf, size, NULL, NULL);
				s->lastSection = s->curSection;
			}
		}
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

		sound->buffer->Lock(sound->lastSection*BUFFER_SIZE, size, (LPVOID*)&buf, &size, 0, 0, 0);

		ret = 1;
		ov_pcm_seek(sound->oggFile, 0);
		while (ret && pos<size)
		{
			ret = ov_read(sound->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
			pos += ret;
		}

		sound->rewindNext = false;

		sound->buffer->Unlock(buf, size, NULL, NULL);

		sound->buffer->SetCurrentPosition(sound->curSection*BUFFER_SIZE);

		sound->lastSection = sound->curSection;
	}
}
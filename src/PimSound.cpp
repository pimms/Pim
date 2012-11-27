#include "PimSound.h"
#include "PimAudioManager.h"
#include "PimException.h"

namespace Pim
{
	Sound::Sound(std::string file)
	{		
		oggFile			= NULL;
		buffer			= NULL;
		isLoop			= false;
		almostDone		= false;
		done			= false;
		audioStream		= false;
		isParallel		= false;
		deleteWhenDone	= false;
		cachePosition	= 0;

		loadFile(file);
	}
	Sound::Sound()
	{
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
	Sound::~Sound()
	{
		if (buffer)
		{
			buffer->Release();
		}
		if (oggFile && !isParallel)
		{
			ov_clear(oggFile);
			delete oggFile;
		}

		// No matter what happens, unschedule to be safe.
		AudioManager::getSingleton()->unscheduleOggUpdate(this);
	}

	void Sound::loadFile(std::string file)
	{
#ifdef _DEBUG
		PimAssert(file.length() >= 4, "Error: invalid file");
#endif
		
		filename = file;

		std::string format = file.substr(file.length()-4, 4);
		
		/* THE WAVE FORMAT IS NO LONGER SUPPORTED */
		/*
		if (format == ".wav")
		{
			if (!AudioManager::getSingleton()->loadWav(file.c_str(), this))
			{
				std::cout<<"ERROR: Failed to load file: " <<file <<"\n";
			}
		}
		*/

		if (format == ".ogg")
		{
			oggFile = new OggVorbis_File;
			curSection = 0;
			lastSection = 1;
			audioStream = true;

			if (!AudioManager::getSingleton()->loadOgg(file.c_str(), this))
			{
				std::cout<<"ERROR: Failed to load file: " <<file <<"\n";
			}
			else
			{
				AudioManager::getSingleton()->scheduleOggUpdate(this);
			}
		}
		else
		{
			std::cout<<"ERROR! PIM DOES NOT SUPPORT THE FOLLOWING AUDIO FORMAT: "
					 <<file <<"\n";
		}
	}
	void Sound::useCache(std::string id)
	{
		PimAssert(AudioManager::getSingleton()->cache.count(id) != 0,
			"Error: file is not cached!");
			
		cache = AudioManager::getSingleton()->cache[id];

		curSection = 0;
		lastSection = 1;

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
		
		buffer = AudioManager::getSingleton()->createBuffer(&wfm, &desc);

		AudioManager::getSingleton()->scheduleOggUpdate(this);
		AudioManager::getSingleton()->fillBuffer(this, 0);
	}

	void Sound::play()
	{
		if (done)
		{
			replay();
		}
		else
		{
			buffer->Play(0,0,DSBPLAY_LOOPING);
		}
	}
	void Sound::replay()
	{
		if (audioStream)
		{
			AudioManager::getSingleton()->rewindOgg(this);
		}
		else
		{
			cachePosition = 0;
			buffer->SetCurrentPosition(0);
			
			curSection = 0;
			lastSection = 0;

			AudioManager::getSingleton()->fillBuffer(this, 0);
		}
			
		play();
	}

	void Sound::loop()
	{
		buffer->Play(0,0,DSBPLAY_LOOPING);
		isLoop = true;
	}
	void Sound::reloop()
	{
		if (audioStream)
		{
			AudioManager::getSingleton()->rewindOgg(this);
		}
		else
		{
			cachePosition = 0;
			buffer->SetCurrentPosition(0);
			
			curSection = 0;
			lastSection = 0;

			AudioManager::getSingleton()->fillBuffer(this, 0);
		}

		loop();
	}

	void Sound::pause()
	{
		buffer->Stop();
	}

	void Sound::setVolume(float vol)
	{
		buffer->SetVolume(-8000 * (1.f-vol));
	}
	void Sound::setPan(float pan)
	{
		buffer->SetPan(pan * 3500);
	}

	float Sound::position()
	{
		if (audioStream)
		{
			return (float)ov_time_tell(oggFile);
		}
		else
		{
			return cachePosition / cache->rate;
		}
	}
	int Sound::position(float time)
	{
		if (audioStream)
		{
			int ret = ov_time_seek(oggFile, (double)time);

			if (!ret)
			{
				AudioManager::getSingleton()->fillBuffer(this, 0);
				buffer->SetCurrentPosition(0);
			}

			return ret;
		}

		return 1;
	}

	Sound* Sound::playParallel(bool delWhenDone)
	{
		Sound *s = new Sound(filename);
		s->deleteWhenDone = delWhenDone;
		s->play();
		return s;
	}
}
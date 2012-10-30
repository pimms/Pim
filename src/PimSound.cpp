#include "PimSound.h"
#include "PimAudioManager.h"
#include "PimException.h"

namespace Pim
{
	Sound::Sound(std::string file)
	{		
		oggFile		= NULL;
		buffer		= NULL;
		isLoop		= false;
		almostDone	= false;
		done		= false;
		audioStream = false;
		isParallel  = false;

		loadFile(file);
	}
	Sound::Sound()
	{
		oggFile		= NULL;
		buffer		= NULL;
		isLoop		= false;
		almostDone	= false;
		done		= false;
		audioStream = false;
		isParallel  = false;
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

		std::string format = file.substr(file.length()-4, 4);

		if (format == ".wav")
		{
			if (!AudioManager::getSingleton()->loadWav(file.c_str(), this))
				std::cout<<"ERROR: Failed to load file: " <<file <<"\n";
		}
		else if (format == ".ogg")
		{
			oggFile = new OggVorbis_File;
			if (!AudioManager::getSingleton()->loadOgg(file.c_str(), this))
				std::cout<<"ERROR: Failed to load file: " <<file <<"\n";
			else
				AudioManager::getSingleton()->scheduleOggUpdate(this);

			curSection = 1;
			lastSection = 0;
			audioStream = true;
		}
		else
		{
			std::cout<<"ERROR! PIM DOES NOT SUPPORT THE FOLLOWING AUDIO FORMAT: "
					 <<file <<"\n";
		}
	}

	void Sound::play()
	{
		if (audioStream)
			buffer->Play(0,0,DSBPLAY_LOOPING);
		else
			buffer->Play(0,0,0);
	}
	void Sound::loop()
	{
		buffer->Play(0,0,DSBPLAY_LOOPING);
		isLoop = true;
	}
	void Sound::reloop()
	{
		if (audioStream)
			AudioManager::getSingleton()->rewindOgg(this);
		else
			buffer->SetCurrentPosition(0);
			
		loop();
	}

	void Sound::pause()
	{
		buffer->Stop();
	}
	void Sound::replay()
	{
		if (audioStream)
			AudioManager::getSingleton()->rewindOgg(this);
		else
			buffer->SetCurrentPosition(0);
			
		play();
	}

	void Sound::setVolume(float vol)
	{
		buffer->SetVolume(-7000 * (1.f-vol));
	}
	void Sound::setPan(float pan)
	{
		buffer->SetPan(pan * 3500);
	}

	Sound* Sound::playParallel()
	{
		// Set default values for parallel play
		Sound *s = new Sound;
		s->isParallel = true;
		s->oggFile = oggFile;
		s->audioStream = true;
		s->curSection = 1;
		s->lastSection = 0;

		// Create a parallel buffer
		s->buffer = AudioManager::getSingleton()->createBuffer(&wfm, &desc);

		// Schedule the parallel sound for ogg updates
		AudioManager::getSingleton()->scheduleOggUpdate(s);

		// Read into the parallel sound
		DWORD	pos = 0;
		int		sec = 0;
		int		ret = 1;
		DWORD	size = BUFFER_SIZE * 2;
		char	*buf;

		s->buffer->Lock(0, size, (LPVOID*)&buf, &size, NULL, NULL, DSBLOCK_ENTIREBUFFER);

		while (ret && pos<size)
		{
			ret = ov_read(s->oggFile, buf+pos, size-pos, 0, 2, 1, &sec);
			pos += ret;
		}

		s->buffer->Unlock(buf, size, NULL, NULL);

		return s;
	}
}
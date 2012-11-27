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

		loadFile(file);
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

		if (format == ".wav")
		{
			if (!AudioManager::getSingleton()->loadWav(file.c_str(), this))
			{
				std::cout<<"ERROR: Failed to load file: " <<file <<"\n";
			}
		}
		else if (format == ".ogg")
		{
			oggFile = new OggVorbis_File;
			if (!AudioManager::getSingleton()->loadOgg(file.c_str(), this))
			{
				std::cout<<"ERROR: Failed to load file: " <<file <<"\n";
			}
			else
			{
				AudioManager::getSingleton()->scheduleOggUpdate(this);
			}

			curSection = 0;
			lastSection = 1;
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
		else
		{
			buffer->Play(0,0,0);
		}
	}
	void Sound::replay()
	{
		if (audioStream)
			AudioManager::getSingleton()->rewindOgg(this);
		else
			buffer->SetCurrentPosition(0);
			
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
			AudioManager::getSingleton()->rewindOgg(this);
		else
			buffer->SetCurrentPosition(0);
			
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
		return (float)ov_time_tell(oggFile);
	}
	int Sound::position(float time)
	{
		int ret = ov_time_seek(oggFile, (double)time);

		if (!ret)
		{
			AudioManager::getSingleton()->fillBuffer(this, 0);
			buffer->SetCurrentPosition(0);
		}

		return ret;
	}

	Sound* Sound::playParallel(bool delWhenDone)
	{
		Sound *s = new Sound(filename);
		s->deleteWhenDone = delWhenDone;
		s->play();
		return s;
	}
}
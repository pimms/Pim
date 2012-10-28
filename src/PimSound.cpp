#include "PimSound.h"
#include "PimException.h"

namespace Pim
{
	Sound::Sound(std::string file)
	{		
#ifdef _DEBUG
		PimAssert(file.length() >= 4, "Error: invalid file");
#endif

		std::string format = file.substr(file.length()-4, 4);

		if (format == ".wav")
		{
			AudioManager::getSingleton()->loadWav(file.c_str(), &buffer);
		}
		else
		{
			std::cout<<"ERROR! PIM DOES NOT SUPPORT THE FOLLOWING AUDIO FORMAT: "
					 <<file <<"\n";
		}
	}
	Sound::Sound()
	{
		buffer = 0;
	}
	Sound::~Sound()
	{
		if (buffer)
		{
			buffer->Release();
		}
	}

	void Sound::play()
	{
		buffer->Play(0,0,0);
	}
	void Sound::loop()
	{
		buffer->Play(0,0,DSBPLAY_LOOPING);
	}

	void Sound::pause()
	{
		buffer->Stop();
	}
	void Sound::replay()
	{
		buffer->SetCurrentPosition(0);
		play();
	}

	void Sound::setVolume(float vol)
	{
		buffer->SetVolume(DSBVOLUME_MIN * (1.f-vol));
	}
}
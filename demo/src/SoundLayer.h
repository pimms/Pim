#pragma once
#include "Pim.h"

class PlayButton : public Pim::Button
{
public:
							PlayButton(Pim::Sprite *norm, Pim::Sprite *press, Pim::Font *font);
	void					SetText(std::string txt);
	void					MakeNormalCurrent();
	void					MakePressedCurrent();

private:
	Pim::Label*				label;
};

class SoundLayer : public Pim::Layer, public Pim::ButtonCallback, public Pim::SliderCallback
{
public:
	Pim::Sound*				music;
	bool					musicPlaying;
	Pim::Font*				font;
	Pim::SpriteBatchNode*	buttonSheet;
	PlayButton*				playButton;
	Pim::Slider*			volumeSlider;
	Pim::Slider*			panSlider;

							~SoundLayer();
	void					LoadResources();
	void					LoadMusic();
	void					LoadButton();
	void					LoadSliders();
	void					SliderValueChanged(Pim::Slider *s, float val);
	void					ButtonReleased(Pim::Button *button);
};


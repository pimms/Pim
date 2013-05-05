#include "SoundLayer.h"


/*
==================
PlayButton::PlayButton
==================
*/
PlayButton::PlayButton(Pim::Sprite *norm, Pim::Sprite *press, Pim::Font *font)
: Pim::Button(norm, NULL, press, NULL) {
	label = new Pim::Label(font);
	label->color = Pim::Color(0.f, 0.f, 0.f, 1.f);
	label->SetTextAlignment(Pim::Label::TEXT_CENTER);
	label->SetZOrder(10);
	label->position = Pim::Vec2(0.f, 3.f);
	AddChild(label);
}

/*
==================
PlayButton::SetText
==================
*/
void PlayButton::SetText(const string txt) {
	label->SetText(txt);
}

/*
==================
PlayButton::MakeNormalCurrent
==================
*/
void PlayButton::MakeNormalCurrent() {
	Pim::Button::MakeNormalCurrent();
	label->position = Pim::Vec2(0.f, 3.f);
}

/*
==================
PlayButton::MakePressedCurrent
==================
*/
void PlayButton::MakePressedCurrent() {
	Pim::Button::MakePressedCurrent();
	label->position = Pim::Vec2(0.f, 0.f);
}



/*
==================
SoundLayer::~SoundLayer
==================
*/
SoundLayer::~SoundLayer() {
	delete music;
	delete font;
}

/*
==================
SoundLayer::LoadResources
==================
*/
void SoundLayer::LoadResources() {
	buttonSheet = new Pim::SpriteBatchNode("soundbuttons.png");
	AddChild(buttonSheet);

	font = new Pim::Font("arial.ttf", 15);

	LoadButton();
	LoadSliders();
	LoadMusic();
}

/*
==================
SoundLayer::LoadMusic
==================
*/
void SoundLayer::LoadMusic() {
	music = new Pim::Sound;
	music->Stream("rh.ogg");
	music->SetLoop(true);
	musicPlaying = false;
	
	Pim::Sound *background = new Pim::Sound;
	background->Stream("space.ogg");
	background->SetDeleteWhenDone(true);
	background->Play();
}

/*
==================
SoundLayer::LoadButton
==================
*/
void SoundLayer::LoadButton() {
	Pim::Sprite *norm = new Pim::Sprite;
	norm->rect = Pim::Rect(0,60,80,30);
	norm->UseBatchNode(buttonSheet);
	
	Pim::Sprite *press = new Pim::Sprite;
	press->UseBatchNode(buttonSheet);
	press->rect = Pim::Rect(0,30,80,30);

	playButton = new PlayButton(norm, press, font);
	playButton->position = Pim::Vec2(400,400);
	AddChild(playButton);

	playButton->SetText("play");
	playButton->SetCallback(this);
}

/*
==================
SoundLayer::LoadSliders
==================
*/
void SoundLayer::LoadSliders() {
	Pim::Sprite *back;
	Pim::Sprite *nrm;
	Pim::Sprite *prs;
	Pim::Label  *lbl;

	// VOLUME SLIDER
	back = new Pim::Sprite;
	back->UseBatchNode(buttonSheet);
	back->rect = Pim::Rect(0,0,120,30);

	lbl = new Pim::Label(font);
	lbl->SetTextAlignment(Pim::Label::TEXT_LEFT);
	lbl->color = Pim::Color(0.f, 0.f, 0.f, 1.f);
	lbl->position = Pim::Vec2(70.f, 0.f);
	lbl->SetText("Volume");
	back->AddChild(lbl);

	nrm = new Pim::Sprite;
	nrm->UseBatchNode(buttonSheet);
	nrm->rect = Pim::Rect(81,30,11,38);

	prs = new Pim::Sprite;
	prs->UseBatchNode(buttonSheet);
	prs->rect = Pim::Rect(92,30,11,38);

	volumeSlider = new Pim::Slider(
		Pim::Vec2(-45,0), Pim::Vec2(45,0),
		back, nrm, NULL, prs, NULL
		);

	volumeSlider->position = Pim::Vec2(400, 350);
	volumeSlider->SetMinMaxValues(0.f, 1.f);
	volumeSlider->SetCallback(this);
	AddChild(volumeSlider);

	// PAN SLIDER
	back = new Pim::Sprite;
	back->UseBatchNode(buttonSheet);
	back->rect = Pim::Rect(0,0,120,30);

	lbl = new Pim::Label(font);
	lbl->SetTextAlignment(Pim::Label::TEXT_LEFT);
	lbl->color = Pim::Color(0.f, 0.f, 0.f, 1.f);
	lbl->position = Pim::Vec2(70.f, 0.f);
	lbl->SetText("Pan");
	back->AddChild(lbl);

	nrm = new Pim::Sprite;
	nrm->UseBatchNode(buttonSheet);
	nrm->rect = Pim::Rect(81,30,11,38);

	prs = new Pim::Sprite;
	prs->UseBatchNode(buttonSheet);
	prs->rect = Pim::Rect(92,30,11,38);

	panSlider = new Pim::Slider(
		Pim::Vec2(-45,0), Pim::Vec2(45,0),
		back, nrm, NULL, prs, NULL
		);

	panSlider->position = Pim::Vec2(400, 300);
	panSlider->SetMinMaxValues(-1.f, 1.f);
	panSlider->SetHandlePosition(0.f);
	panSlider->SetCallback(this);
	AddChild(panSlider);
}

/*
==================
SoundLayer::SliderValueChanged
==================
*/
void SoundLayer::SliderValueChanged(Pim::Slider *s, float val) {
	if (s == volumeSlider) {
		music->SetVolume(val);
	} else if (s == panSlider) {
		music->SetSourcePosition(val*10000.f, 0.f, 0.f);
	}
}

/*
==================
SoundLayer::ButtonReleased
==================
*/
void SoundLayer::ButtonReleased(Pim::Button *button) {
	if (button == playButton) {
		musicPlaying = !musicPlaying;

		if (musicPlaying) {
			playButton->SetText("rewind");
			music->Play();
		} else {
			playButton->SetText("play");
			music->Rewind();
		}
	}
}


#pragma once

// TO BE IMPLEMENTED AT A FUTURE DATE.

/*
	Animation is a brief class that provides you with the rectangle to use, 
	provided that you have an animation for a sprite in a sprite sheet.

	The usage is quite simple. For this example, I have a spritesheet containing 8 sprites
	which together makes up an animation. Five of them are on the top row, and three on the
	lower row. Each frame is 10x15 pixels. Each frame shall last 0.2 seconds.

	Animation anim = new Animation(10,15,5,8,0.2f);

	Then in the update method:

	rect = anim->update(dt);

	And that is it.
		
*/

namespace Pim
{

	class Animation
	{
	public:
		Animation();
		~Animation(void);
	};

}
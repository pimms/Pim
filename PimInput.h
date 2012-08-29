#pragma once

/*
	Input is a singleton object.

	In order to optimize rebinding of keys, there are two different ways
	of checking wether or not a key is pressed. 

	1. Querying for the key directly (K_W)
	2. Querying for the string associated with the button ("jump").

	In order to accomodate method #2, you are able to pass a string and a key-code to
	the bindKey(std::string,Pim::Key). Binding "jump" to K_SPACE, will result in the query
	keyStatus("jump") to return the equivalent value of keyStatus(K_SPACE).

	Whenever an input action occurs, the singleton is flagged as dirty. In the game loop,
	the Input object is queried for dirtyness - a "true" will dispatch a notification to all
	GameNodes scheduled for listening.
*/

#include <string>
#include <map>

namespace Pim
{
	namespace Key
	{
		typedef enum KeyCode
		{
			K_ENTER = 13,
			K_SPACE = 32,
			K_LEFT = 37,K_UP,K_RIGHT,K_DOWN,
			K_0 = 48,K_1,K_2,K_3,K_4,K_5,K_6,K_7,K_8,K_9,
			K_A = 65,K_B,K_C,K_D,K_E,K_F,K_G,K_H,K_I,K_J,K_K,K_L,K_M,K_N,K_O,K_P,K_Q,K_R,K_S,K_T,K_U,K_V,K_W,K_X,K_Y,K_Z,
			K_F1 = 112,K_F2,K_F3,K_F4,K_F5,	K_F6,K_F7,K_F8,K_F9,K_F10,K_F11,K_F12,
		};
	}
	namespace Mouse
	{
		typedef enum MouseButton
		{
			MBTN_LEFT,
			MBTN_RIGHT,
		};
	}

	// Forward declarations
	class Vec2;
	class GameControl;

	class Input
	{
	public:
		static Input* getSingleton() { return Input::singleton; }

		void bindKey(std::string id, Key::KeyCode key);
		void unbindKey(std::string id);

		bool keyStatus(Key::KeyCode key);
		bool keyStatus(std::string id);
		bool mouseStatus(Mouse::MouseButton key);
		Vec2 mousePosition();
		Vec2 mouseRelPosition();

		// Methods have to be manual in order for the WINAPI callback
		// function to reach them. ight make these inaccessible at a
		// future date.
		// Manual use of these methods will cause bugs. ///////////////////
		void lostFocus();							    // DON'T USE >:( //
		void gainedFocus();							    // DON'T USE >:( //
		void keyPressed(int);						    // DON'T USE >:( //
		void keyReleased(int);						    // DON'T USE >:( //
		void mouseMoved(int,int);					    // DON'T USE >:( //
		void mousePressed(int);							// DON'T USE >:( //
		void mouseReleased(int);						// DON'T USE >:( //
		///////////////////////////////////////////////////////////////////

	private:
		friend class GameControl;

		Input();
		Input(const Input&) {}
		static void instantiateSingleton();
		static void clearSingleton();

		void mouseRelUpdate() { mousePosLastFrame = mousePos; } // Called post-dispatch
		bool isMouseDirty() { return mouseDirty; }
		bool isKeyDirty()   { return keyDirty;   }

		std::map<std::string,Key::KeyCode>	boundKeys;
		static Input						*singleton;
		bool								keys[256];
		bool								mouse[2];
		Vec2								mousePos;
		Vec2								mousePosLastFrame;
		bool								keyDirty;
		bool								mouseDirty;
	};
}
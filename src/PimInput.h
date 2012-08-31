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
	class Input;
	class Vec2;
	class GameControl;

	// Class containing all key event data.
	// One instance is kept by Input, and is passed to listeners as const.
	class KeyEvent
	{
	public:
		inline bool isKeyDown(Key::KeyCode k)
			{ return keys[k]; }
		inline bool isKeyFresh(Key::KeyCode k)
			{ return keys[k] && fresh[k]; }
			
		inline bool isKeyDown(std::string str)
			{ return keys[binds[str]]; }
		inline bool isKeyFresh(std::string str)
			{ return keys[binds[str]] && fresh[binds[str]]; }

		inline int keyCount()
			{ return count; }

	protected:
		friend class Input;

		int count;
		bool keys[256];
		bool fresh[256];
		std::map<std::string,Key::KeyCode>	binds;

		KeyEvent()
			{ _reset(); }
		KeyEvent(const KeyEvent&)
			{ _reset(); }
		inline void _reset()
			{ count=0; for (int i=0; i<256; i++) { keys[i]=false; fresh[i]=false; } }
		inline void _unfresh()
			{ for (int i=0; i<256; i++) { fresh[i]=false; } }

		// Binding keys by string
		void bindKey(std::string &str, Key::KeyCode k)
			{ binds[str] = k; }
		void unbindKey(std::string &str)
			{ binds.erase(str); }
	};

	// Class containing all mouse event data.
	// One instance is kept by Input, and is passed to listeners as const.
	class MouseEvent
	{
	public:
		inline bool isKeyDown(Mouse::MouseButton mb)
			{ return keys[mb]; }
		inline bool isKeyFresh(Mouse::MouseButton mb)
			{ return keys[mb] && fresh[mb]; }
		inline Vec2 getPosition()
			{ return position; }
		inline Vec2 getRelative()
			{ return relPosition; }

	private:
		friend class Input;

		bool dirty;
		bool keys[2];
		bool fresh[2];
		Vec2 position;
		Vec2 relPosition;

		MouseEvent()
			{ _reset(); }
		MouseEvent(const MouseEvent&)
			{ _reset(); }
		inline void _reset()
			{ position = Vec2(0.f,0.f); relPosition = Vec2(0.f,0.f); dirty=false; keys[0]=false;keys[1]=false; fresh[0]=false;fresh[0]=false; }
		inline void _unfresh()
			{ dirty=false; relPosition=Vec2(0.f, 0.f); fresh[0]=false;fresh[1]=false; }
		inline void _mouseMoved(Vec2 pos)
			{ relPosition = pos; position = pos; }
	};

	class Input
	{
	public:
		static Input* getSingleton() { return Input::singleton; }

		void bindKey(std::string id, Key::KeyCode key);
		void unbindKey(std::string id);

		// Methods have to be manual in order for the WINAPI callback
		// function to reach them. ight make these inaccessible at a
		// future date.
		// Manual use of these methods will cause bugs. ///////////////////
		void _lostFocus();							    // DON'T USE >:( //
		void _gainedFocus();							    // DON'T USE >:( //
		void _keyPressed(int);						    // DON'T USE >:( //
		void _keyReleased(int);						    // DON'T USE >:( //
		void _mouseMoved(int,int);					    // DON'T USE >:( //
		void _mousePressed(int);							// DON'T USE >:( //
		void _mouseReleased(int);						// DON'T USE >:( //
		///////////////////////////////////////////////////////////////////

	private:
		friend class GameControl;

		Input();
		Input(const Input&) {}
		static void instantiateSingleton();
		static void clearSingleton();

		void addKeyListener(GameNode* n);
		void removeKeyListener(GameNode* n);

		void addMouseListener(GameNode* n);
		void removeMouseListener(GameNode* n);

		// Dispatches all events to listeners
		void _dispatch();

		static Input						*singleton;
		std::vector<GameNode*>				kl;				// key listeners
		std::vector<GameNode*>				ml;				// mouse listeners

		// The events containing input data
		KeyEvent keyEvent;
		MouseEvent mouseEvent;
	};

}
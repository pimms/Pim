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
	// Forward declarations
	class Input;
	class Vec2;
	class GameControl;

	// Class containing all key event data.
	// One instance is kept by Input, and is passed to listeners.
	class KeyEvent
	{
	public:
		enum KeyCode
		{
			K_BACKSPACE = 8, K_ENTER = 13, K_LCTRL = 17, K_ESC = 27, K_SPACE = 32,
			K_LEFT = 37,K_UP,K_RIGHT,K_DOWN,
			K_0 = 48,K_1,K_2,K_3,K_4,K_5,K_6,K_7,K_8,K_9,
			K_A = 65,K_B,K_C,K_D,K_E,K_F,K_G,K_H,K_I,K_J,K_K,K_L,K_M,K_N,K_O,K_P,K_Q,K_R,K_S,K_T,K_U,K_V,K_W,K_X,K_Y,K_Z,
			K_F1 = 112,K_F2,K_F3,K_F4,K_F5,	K_F6,K_F7,K_F8,K_F9,K_F10,K_F11,K_F12,
		};

		bool isKeyDown(KeyCode k);
		bool isKeyFresh(KeyCode k);
			
		bool isKeyDown(std::string str);
		bool isKeyFresh(std::string str);

		inline int keyCount();

	protected:
		friend class Input;

		bool activePrevFrame;
		int count;
		bool keys[256];
		bool fresh[256];
		std::map<std::string,KeyCode>	binds;

		KeyEvent();
		KeyEvent(const KeyEvent&);
		inline void _reset();
		inline void _unfresh();

		// Binding keys by string
		void bindKey(std::string &str, KeyCode k);
		void unbindKey(std::string &str);
	};

	// Class containing all mouse event data.
	// One instance is kept by Input, and is passed to listeners.
	class MouseEvent
	{
	public:
		enum MouseButton
		{
			MBTN_LEFT,
			MBTN_RIGHT,
		};

		bool isKeyDown(MouseButton mb);
		bool isKeyFresh(MouseButton mb);
		Vec2 getPosition();
		Vec2 getRelative();

	private:
		friend class Input;

		bool dirty;
		bool keys[2];
		bool fresh[2];
		Vec2 position;
		Vec2 relPosition;

		MouseEvent();
		MouseEvent(const MouseEvent&);
		void _reset();
		void _unfresh();
		void _mouseMoved(Vec2 pos);
	};

	class Input
	{
	public:
		static Input* getSingleton() { return Input::singleton; }

		void bindKey(std::string id, KeyEvent::KeyCode key);
		void unbindKey(std::string id);

		// Methods have to be manual in order for the WINAPI callback
		// function to reach them. Might make these inaccessible at a
		// future date.
		// Manual use of these methods WILL cause bugs. ///////////////////////
		void _lostFocus();									// DON'T USE >:( //
		void _gainedFocus();							    // DON'T USE >:( //
		void _keyPressed(int);								// DON'T USE >:( //
		void _keyReleased(int);								// DON'T USE >:( //
		void _mouseMoved(int,int);							// DON'T USE >:( //
		void _mousePressed(int);							// DON'T USE >:( //
		void _mouseReleased(int);							// DON'T USE >:( //
		///////////////////////////////////////////////////////////////////////

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
		void dispatch();

		// Dispatches all events to all children of the passed node
		void dispatchPaused(GameNode *l);
		void recursiveDispatch(GameNode *n);

		static Input						*singleton;
		std::vector<GameNode*>				kl;				// key listeners
		std::vector<GameNode*>				ml;				// mouse listeners

		// The events containing input data
		KeyEvent keyEvent;
		MouseEvent mouseEvent;
	};

}
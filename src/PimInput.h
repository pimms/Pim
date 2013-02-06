#pragma once

/*
	Input is a singleton object.

	In order to optimize rebinding of keys, there are two different ways
	of checking wether or not a key is pressed.

	1. Querying for the key directly (K_W)
	2. Querying for the string associated with the button ("jump").

	In order to accomodate method #2, you are able to pass a string and a key-code to
	the bindKey(string,Pim::Key). Binding "jump" to K_SPACE, will result in the query
	keyStatus("jump") to return the equivalent value of keyStatus(K_SPACE).

	Whenever an input action occurs, the singleton is flagged as dirty. In the game loop,
	the Input object is queried for dirtyness - a "true" will dispatch a notification to all
	GameNodes scheduled for listening.
*/

#include <string>
#include <map>

#include <Xinput.h>

namespace Pim {
	class Input;
	class Vec2;
	class GameControl;

	class KeyEvent {
	private:
		friend class Input;

	public:
		enum KeyCode {
			K_BACKSPACE = 8, K_ENTER = 13, K_SHIFT = 16, K_CTRL = 17, K_ESC = 27, K_SPACE = 32,
			K_LEFT = 37,K_UP,K_RIGHT,K_DOWN,
			K_0 = 48,K_1,K_2,K_3,K_4,K_5,K_6,K_7,K_8,K_9,
			K_A = 65,K_B,K_C,K_D,K_E,K_F,K_G,K_H,K_I,K_J,K_K,K_L,K_M,K_N,K_O,K_P,K_Q,K_R,K_S,K_T,K_U,K_V,K_W,K_X,K_Y,K_Z,
			K_F1 = 112,K_F2,K_F3,K_F4,K_F5,	K_F6,K_F7,K_F8,K_F9,K_F10,K_F11,K_F12,
		};

		bool							IsKeyDown(const KeyCode k) const;
		bool							IsKeyFresh(const KeyCode k) const;
		bool							IsKeyDown(const string str) const;
		bool							IsKeyFresh(const string str) const;
		int								KeyCount() const;

	private:
		bool							activePrevFrame;
		int								count;
		bool							keys[256];
		bool							fresh[256];
		map<string,KeyCode>				binds;

										KeyEvent();
										KeyEvent(const KeyEvent&);
		void							_Reset();
		void							_Unfresh();
		void							BindKey(const string &str, const KeyCode k);
		void							UnbindKey(const string &str);
	};

	class MouseEvent {
	private:
		friend class Input;

	public:
		enum MouseButton {
										MBTN_LEFT,
										MBTN_RIGHT,
		};

		bool							IsKeyDown(const MouseButton mb) const;
		bool							IsKeyFresh(const MouseButton mb) const;
		Vec2							GetPosition() const;
		Vec2							GetRelative() const;

	private:
		bool							dirty;
		bool							keys[2];
		bool							fresh[2];
		Vec2							position;
		Vec2							relPosition;
		Vec2							lastPosition;

										MouseEvent();
										MouseEvent(const MouseEvent&);
		void							_Reset();
		void							_Unfresh();
		void							_MouseMoved(Vec2 pos);
	};

	class ControllerEvent {
	private:
		friend class Input;

	public:
		enum Xbox {
			X_A						= XINPUT_GAMEPAD_A,
			X_B						= XINPUT_GAMEPAD_B,
			X_X						= XINPUT_GAMEPAD_X,
			X_Y						= XINPUT_GAMEPAD_Y,
			X_DUP					= XINPUT_GAMEPAD_DPAD_UP,
			X_DRIGHT				= XINPUT_GAMEPAD_DPAD_RIGHT,
			X_DDOWN					= XINPUT_GAMEPAD_DPAD_DOWN,
			X_DLEFT					= XINPUT_GAMEPAD_DPAD_LEFT,
			X_BACK					= XINPUT_GAMEPAD_BACK,
			X_START					= XINPUT_GAMEPAD_START,
			X_LB					= XINPUT_GAMEPAD_LEFT_SHOULDER,
			X_RB					= XINPUT_GAMEPAD_RIGHT_SHOULDER,
			X_LS					= XINPUT_GAMEPAD_LEFT_THUMB,
			X_RS					= XINPUT_GAMEPAD_RIGHT_THUMB
		};

		bool						IsKeyDown(Xbox x) const;
		bool						IsKeyFresh(Xbox x) const;
		Vec2						LeftStick() const;		// Returns a unit vector of the stick
		Vec2						RightStick() const;		// Returns a unit vector of the stick
		float						LeftTrigger() const;		// Returns the presedness of LT (0.0 - 1.0)
		float						RightTrigger() const;		// Returns the pressedness of RT (0.0 - 1.0)
		bool						Connected();

	private:
		WORD						curBtnState;
		WORD						prevBtnState;
		XINPUT_STATE				xinputState;

									ControllerEvent();
		void						GetStates();
		void						Vibrate(float l, float r);
	};

	class Input {
	private:
		friend class GameControl;

	public:
		static Input*				GetSingleton();
		void						BindKey(const string id, const KeyEvent::KeyCode key);
		void						UnbindKey(const string id);
		void						VibrateXbox(float leftVib, float rightVib);
		void						_LostFocus();	
		void						_GainedFocus();		
		void						_KeyPressed(int);		
		void						_KeyReleased(int);		
		void						_MouseMoved(int,int);	
		void						_MousePressed(int);			
		void						_MouseReleased(int);			

	private:
		static Input				*singleton;
		vector<GameNode*>			kl;				// key listeners
		vector<GameNode*>			ml;				// mouse listeners
		vector<GameNode*>			cl;				// control listeners
		KeyEvent					keyEvent;
		MouseEvent					mouseEvent;
		ControllerEvent				contEvent;
        
									Input();
									Input(const Input&) {}
		static void					InstantiateSingleton();
		static void					ClearSingleton();
		void						AddKeyListener(GameNode* n);
		void						RemoveKeyListener(GameNode* n);
		void						AddMouseListener(GameNode* n);
		void						RemoveMouseListener(GameNode* n);
		void						AddControlListener(GameNode *n);
		void						RemoveControlListener(GameNode *n);
		void						DispatchPaused(GameNode *l);
		void						Dispatch();
		void						Dispatch_r(GameNode *n, bool controller);
	};
}
#pragma once

#include <string>
#include <map>

namespace Pim {
	class Input;
	class Vec2;
	class GameControl;
	
	
	/**
	 @class 		KeyEvent
	 @brief 		Passed on key presses / releases to Key-listeners.
	 @details 		The KeyEvent class supports storing data about up to 512
	 				unique keys, as defined by the SDL key-event values. The state
	 				of the buttons are stored in two 512-bit bitfields - one for
	 				the absolute status of the keys (on or off), and one for tracking
	 				the 'freshness' of the keys. Fresh is defined as a button that
					was pressed down @e this frame.
	 */
	class KeyEvent {
	private:
		friend class Input;

	public:
		enum KeyCode {
			K_BACKSPACE = 8, K_ENTER = 13, K_SHIFT = 16, K_CTRL = 17, K_ESC = 27, K_SPACE = 32,
			K_LEFT = 37,K_UP,K_RIGHT,K_DOWN,
			K_0 = 48,K_1,K_2,K_3,K_4,K_5,K_6,K_7,K_8,K_9,
			K_A = 97,K_B,K_C,K_D,K_E,K_F,K_G,K_H,K_I,K_J,K_K,K_L,K_M,K_N,K_O,K_P,K_Q,K_R,K_S,K_T,K_U,K_V,K_W,K_X,K_Y,K_Z,
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
		//bool							keys[256];
		//bool							fresh[256];	
		unsigned						keyField[16];	// Holds 512 bits (512 keys)
		unsigned						freshField[16];	
		map<string,KeyCode>				binds;

										KeyEvent();
										KeyEvent(const KeyEvent&);
		void							Reset();
		void							Unfresh();
		void							BindKey(const string &str, const KeyCode k);
		void							UnbindKey(const string &str);
	};
	
	
	/**
	 @class 		MouseEvent
	 @brief 		Passed on mouse movement to Mouse-Listeners.
	 @details 		Contains data on the mouse position (in the screen, based on
	 				the coordinate values you initiated your CreationData-object
	 				with) and the status of the mouse buttons. 
	 
	 				Currently, only 7 unique buttons are supported. See MouseEvent::MouseButton
	 				for more details.
	 */
	class MouseEvent {
	private:
		friend class Input;

	public:
		enum MouseButton {				MBTN_LEFT		= 0,
										MBTN_MIDDLE		= 1,
										MBTN_RIGHT		= 2,
										MBTN_WHEEL_UP	= 3,
										MBTN_WHEEL_DOWN = 4,
										MBTN_BACK		= 5,
										MBTN_FORWARD	= 6,


		};

		bool							IsKeyDown(const MouseButton mb) const;
		bool							IsKeyFresh(const MouseButton mb) const;
		Vec2							GetPosition() const;
		Vec2							GetRelative() const;

	private:
		bool							dirty;
		bool							keys[7];
		bool							fresh[7];
		Vec2							position;
		Vec2							relPosition;
		Vec2							lastPosition;

										MouseEvent();
										MouseEvent(const MouseEvent&);
		void							Reset();
		void							Unfresh();
		void							MouseMoved(Vec2 pos);
	};
	
	/**
	 @class 		ControllerEvent
	 @brief 		Passed to Controller-Listeners.
	 @details 		Contains data on buttons and analog sticks. 
	 */
	class ControllerEvent {
	private:
		friend class Input;

	public:
		enum Xbox {
			X_A			= 1 << 1,		
			X_B			= 1 << 2,		
			X_X			= 1 << 3,		
			X_Y			= 1 << 4,
			X_LBUMP		= 1 << 5,	
			X_RBUMP		= 1 << 6,
			X_BACK		= 1 << 7,			
			X_START		= 1 << 8,
			X_LSTICK	= 1 << 9,	
			X_RSTICK	= 1 << 10,

			X_DUP		= 1 << 11,		
			X_DRIGHT	= 1 << 12,	
			X_DDOWN		= 1 << 13,	
			X_DLEFT		= 1 << 14,
		};

		enum XboxAxes {
			XAXIS_LEFT_X,		XAXIS_LEFT_Y,		XAXIS_LEFT_TRIGGER,
			XAXIS_RIGHT_X,		XAXIS_RIGHT_Y,		XAXIS_RIGHT_TRIGGER,
			XAXIS_AXIS_MAX		= 32768,	
			XAXIS_AXIS_MIN		= 8000,
		};

		bool							IsKeyDown(Xbox x) const;
		bool							IsKeyFresh(Xbox x) const;

		Vec2							LeftStick() const;		// Unit vector
		Vec2							RightStick() const;		// Unit vector
		float							LeftTrigger() const;	// 0.0  -  1.0
		float							RightTrigger() const;	// 0.0  -  1.0

	private:
		SDL_Joystick					*joystick;
		bool							dirty;
		short							prevButtons;
		short							buttons;				// Bitfield 
		float							axes[6];				// LS(2), RS(2), LT, RT

										ControllerEvent();
		void							Reset();
		void							Unfresh();
		bool							IsConnected();
		void							Vibrate(float l, float r);
	};
	
	
	/**
	 @class 		Input
	 @brief 		The singleton managing user input (keyboard, mouse, controller)
	 @details 		Input is a singleton object.
	 
					In order to optimize rebinding of keys, there are two different ways
					of checking wether or not a key is pressed.
					1. Querying for the key directly (K_W)
					2. Querying for the string associated with the button ("jump").
	 
					In order to accomodate method #2, you are able to pass a string and a key-code to
					the bindKey(string,Pim::Key). Binding "jump" to K_SPACE, will result in the query
					keyStatus("jump") to return the equivalent value of keyStatus(K_SPACE).
	 				This functionality is currently @e only supported for keyboard input.
	 
					Whenever an input action occurs, the singleton is flagged as dirty. In the game loop,
					the Input object is queried for dirtyness - a "true" will dispatch a notification to all
					GameNodes scheduled for listening.
		 */
	class Input {
	private:
		friend class GameControl;

	public:
		static Input*					GetSingleton();
		void							BindKey(const string id, const KeyEvent::KeyCode key);
		void							UnbindKey(const string id);
		void							VibrateXbox(float leftVib, float rightVib);		

	private:
		static Input					*singleton;
		vector<GameNode*>				kl;						// key listeners
		vector<GameNode*>				ml;						// mouse listeners
		vector<GameNode*>				cl;						// control listeners
		KeyEvent						keyEvent;
		MouseEvent						mouseEvent;
		ControllerEvent					contEvent;
        
										Input();
										Input(const Input&) {}
	
		void							KeyPressed(int button);		
		void							KeyReleased(int button);		
		void							MouseMoved(int xpos, int ypos);	
		void							MousePressed(int button);				
		void							MouseReleased(int button);	
		void							ControllerButtonPressed(int button);
		void							ControllerButtonReleased(int button);
		void							ControllerAxisMoved(int axis, int value);

		static void						InstantiateSingleton();
		static void						ClearSingleton();
		void							AddKeyListener(GameNode* n);
		void							RemoveKeyListener(GameNode* n);
		void							AddMouseListener(GameNode* n);
		void							RemoveMouseListener(GameNode* n);
		void							AddControlListener(GameNode *n);
		void							RemoveControlListener(GameNode *n);
		void							DispatchPaused(GameNode *l);
		void							Dispatch();
		void							Dispatch_r(GameNode *n, bool controller);
	};
	
	/**
	 @fn			Input::Dispatch_r
	 @brief 		Recursive dispatch of input. Used when the game is paused.
	 @param 		n 
	 				The node on which to dispatch
	 @param 		controller
	 				Flag indicating whether a controller is connected or not.
	 */
}
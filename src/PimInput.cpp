#include "PimInternal.h"

#include "PimVec2.h"
#include "PimGameNode.h"
#include "PimAssert.h"
#include "PimGameControl.h"

#include "PimInput.h"

#include <iostream>

namespace Pim {
	// --- KeyEvent ---
	/*
	=====================
	KeyEvent::KeyEvent:
	=====================
	*/
	KeyEvent::KeyEvent() {
		activePrevFrame = false;
		Reset();
	}

	/*
	=====================
	KeyEvent::KeyEvent
	=====================
	*/
	KeyEvent::KeyEvent(const KeyEvent&) {
		Reset();
	}

	/*
	=====================
	KeyEvent::Reset
	=====================
	*/
	void KeyEvent::Reset() {
		count=0;
		for (int i=0; i<16; i++) {
			keyField[i]		= 0;
			freshField[i]   = 0;
		}
	}

	/*
	=====================
	KeyEvent::Unfresh
	=====================
	*/
	void KeyEvent::Unfresh() {
		for (int i=0; i<16; i++) {
			freshField[i] = 0;
		}
	}

	/*
	=====================
	KeyEvent::IsKeyDown
	=====================
	*/
	bool KeyEvent::IsKeyDown(const KeyCode k) const {
		// Return the flag of the kth bit in keyField
		char idx = k / 32;
		char bit = k % 32;

		return (keyField[idx] & (1 << bit)) != 0;
	}

	/*
	=====================
	KeyEvent::IsKeyFresh
	=====================
	*/
	bool KeyEvent::IsKeyFresh(const KeyCode k) const {
		// Return the flag of the kth bit in keyField AND freshField
		char idx = k / 32;
		char bit = k % 32;

		return	(keyField[idx]	 & (1 << bit)) != 0 
			&&	(freshField[idx] & (1 << bit)) != 0;
	}

	/*
	=====================
	KeyEvent::IsKeyDown
	=====================
	*/
	bool KeyEvent::IsKeyDown(const string str) const {
		return IsKeyDown(binds.find(str)->second);
	}

	/*
	=====================
	KeyEvent::IsKeyFresh
	=====================
	*/
	bool KeyEvent::IsKeyFresh(const string str) const {
		return IsKeyFresh(binds.find(str)->second);
	}

	/*
	=====================
	KeyEvent::KeyCount
	=====================
	*/
	int KeyEvent::KeyCount() const {
		return count;
	}

	/*
	=====================
	KeyEvent::BindKey
	=====================
	*/
	void KeyEvent::BindKey(const string &str, const KeyCode k) {
		binds[str] = k;
	}

	/*
	=====================
	KeyEvent::UnbindKey
	=====================
	*/
	void KeyEvent::UnbindKey(const string &str) {
		binds.erase(str);
	}


	// --- MouseEvent ---
	/*
	=====================
	MouseEvent::MouseEvent
	=====================
	*/
	MouseEvent::MouseEvent() {
		Reset();
	}

	/*
	=====================
	MouseEvent::MouseEvent
	=====================
	*/
	MouseEvent::MouseEvent(const MouseEvent&) {
		Reset();
	}

	/*
	=====================
	MouseEvent::Reset
	=====================
	*/
	void MouseEvent::Reset() {
		position		= Vec2(0.f,0.f);
		lastPosition	= Vec2(0.f,0.f);
		dirty			= false;
		
		for (int i=0; i<7; i++) {
			keys[i]		= false;
			fresh[i]	= false;
		}
	}

	/*
	=====================
	MouseEvent::Unfresh
	=====================
	*/
	void MouseEvent::Unfresh() {
		
		for (int i=0; i<7; i++) {
			fresh[i] = false;

			if (keys[i]) {
				dirty = true;
			}
		}

		lastPosition = position;

	}

	/*
	=====================
	MouseEvent::MouseMoved
	=====================
	*/
	void MouseEvent::MouseMoved(Vec2 pos) {
		position = pos;
	}

	/*
	=====================
	MouseEvent::IsKeyDown
	=====================
	*/
	bool MouseEvent::IsKeyDown(MouseButton mb) const {
		return keys[mb];
	}

	/*
	=====================
	MouseEvent::IsKeyFresh
	=====================
	*/
	bool MouseEvent::IsKeyFresh(MouseButton mb) const {
		return keys[mb] && fresh[mb];
	}

	/*
	=====================
	MouseEvent::GetPosition
	=====================
	*/
	Vec2 MouseEvent::GetPosition() const {
		Vec2 v(
			position.x,
			GameControl::GetWindowHeight() + GameControl::GetMouseOffset().y - position.y
		);

		v -= GameControl::GetSingleton()->LowerLeftCorner();
		v *= GameControl::GetSingleton()->GetCoordinateFactor();

		return v;
	}

	/*
	=====================
	MouseEvent::GetRelative
	=====================
	*/
	Vec2 MouseEvent::GetRelative() const {
		return (position - lastPosition) 
				* GameControl::GetSingleton()->GetCoordinateFactor()
				* Vec2(1.f, -1.f);
	}


	// --- ControllerEvent ---
	/*
	=====================
	ControllerEvent::ControllerEvent
	=====================
	*/
	ControllerEvent::ControllerEvent() {
		joystick = NULL;
		Reset();
	}

	/*
	=====================
	ControllerEvent::IsKeyDown
	=====================
	*/
	bool ControllerEvent::IsKeyDown(Xbox x) const {
		return (buttons & x) != 0;
	}

	/*
	=====================
	ControllerEvent::IsKeyFresh
	=====================
	*/
	bool ControllerEvent::IsKeyFresh(Xbox x) const {
		return ((buttons & x) & (prevButtons & x)) == 0;
	}

	/*
	=====================
	ControllerEvent::LeftStick
	=====================
	*/
	Vec2 ControllerEvent::LeftStick() const {
		return Vec2(
			(axes[XAXIS_LEFT_X] < XAXIS_AXIS_MIN) ? 0.f : axes[XAXIS_LEFT_X],
			(axes[XAXIS_LEFT_Y] < XAXIS_AXIS_MIN) ? 0.f : axes[XAXIS_LEFT_Y]
		);
	}

	/*
	=====================
	ControllerEvent::RightStick
	=====================
	*/
	Vec2 ControllerEvent::RightStick() const {
		return Vec2(
			(axes[XAXIS_RIGHT_X] < XAXIS_AXIS_MIN) ? 0.f : axes[XAXIS_RIGHT_X],
			(axes[XAXIS_RIGHT_Y] < XAXIS_AXIS_MIN) ? 0.f : axes[XAXIS_RIGHT_Y]
		);
	}

	/*
	=====================
	ControllerEvent::LeftTrigger
	=====================
	*/
	float ControllerEvent::LeftTrigger() const {
		return axes[XAXIS_LEFT_TRIGGER];
	}

	/*
	=====================
	ControllerEvent::RightTrigger
	=====================
	*/
	float ControllerEvent::RightTrigger() const {
		return axes[XAXIS_RIGHT_TRIGGER];
	}

	/*
	=====================
	ControllerEvent::Reset
	=====================
	*/
	void ControllerEvent::Reset() {
		dirty = false;

		prevButtons = 0;
		buttons		= 0;

		for (int i=0; i<6; i++) {
			axes[i] = 0.f;
		}
	}

	/*
	=====================
	ControllerEvent::Unfresh
	=====================
	*/
	void ControllerEvent::Unfresh() {
		prevButtons = buttons;
		
		if (buttons != 0) {
			dirty = true;
		} else {
			for (int i=0; i<6; i++) {
				if (axes[i] < XAXIS_AXIS_MIN) {
					dirty = true;
				}
			}
		}
	}

	/*
	=====================
	ControllerEvent::IsConnected
	=====================
	*/
	bool ControllerEvent::IsConnected() {
		if (!joystick) {
			joystick = SDL_JoystickOpen(0);
		}

		return (joystick != NULL);
	}

	/*
	=====================
	ControllerEvent::Vibrate
	=====================
	*/
	void ControllerEvent::Vibrate(float l, float r) {
		
	}


	// --- Input ---
	Input* Input::singleton = NULL;

	/*
	=====================
	Input::GetSingleton
	=====================
	*/
	Input* Input::GetSingleton() {
		return singleton;
	}

	/*
	=====================
	Input::Input
	=====================
	*/
	Input::Input() {
		PimAssert(singleton == NULL, "Input singleton is already initialized");
	}

	/*
	=====================
	Input::InstantiateSingleton
	=====================
	*/
	void Input::InstantiateSingleton() {
		singleton = new Input;
	}

	/*
	=====================
	Input::ClearSingleton
	=====================
	*/
	void Input::ClearSingleton() {
		if (singleton) {
			delete singleton;
			singleton = NULL;
		}
	}

	/*
	=====================
	Input::BindKey
	=====================
	*/
	void Input::BindKey(string id, KeyEvent::KeyCode key) {
		PimAssert(key >= 0 && key < 256, "Key out of range.");
		keyEvent.BindKey(id, key);

		#ifdef _DEBUG
		cout<<"Bound key - " <<id <<": " <<keyEvent.binds[id] <<"\n";
		#endif /* _DEBUG */
	}

	/*
	=====================
	Input::UnbindKey
	=====================
	*/
	void Input::UnbindKey(string id) {
		keyEvent.UnbindKey(id);
	}

	/*
	=====================
	Input::AddKeyListener
	=====================
	*/
	void Input::AddKeyListener(GameNode *node) {
		kl.push_back(node);
	}

	/*
	=====================
	Input::RemoveKeyListener
	=====================
	*/
	void Input::RemoveKeyListener(GameNode *node) {
		for (unsigned int i=0; i<kl.size(); i++) {
			if (kl[i] == node) {
				kl.erase(kl.begin() + i);
			}
		}
	}

	/*
	=====================
	Input::AddMouseListener
	=====================
	*/
	void Input::AddMouseListener(GameNode *node) {
		ml.push_back(node);
	}

	/*
	=====================
	Input::RemoveMouseListener
	=====================
	*/
	void Input::RemoveMouseListener(GameNode *node) {
		for (unsigned int i=0; i<ml.size(); i++) {
			if (ml[i] == node) {
				ml.erase(ml.begin() + i);
			}
		}
	}

	/*
	=====================
	Input::AddControlListener
	=====================
	*/
	void Input::AddControlListener(GameNode *node) {
		cl.push_back(node);
	}

	/*
	=====================
	Input::RemoveControlListener
	=====================
	*/
	void Input::RemoveControlListener(GameNode *node) {
		for (unsigned int i=0; i<cl.size(); i++) {
			if (cl[i] == node) {
				cl.erase(cl.begin() + i);
			}
		}
	}

	/*
	=====================
	Input::VibrateXbox
	=====================
	*/
	void Input::VibrateXbox(float leftVib, float rightVib) {
		contEvent.Vibrate(leftVib, rightVib);
	}

	/*
	=====================
	Input::KeyPressed
	=====================
	*/
	void Input::KeyPressed(int key) {
		// Key is a number between 0 and 512. 
		// The key-th bit in the bit fields should be flagged to 1.
		char idx = key / 32;
		char bit = key % 32;

		keyEvent.keyField[idx]		|= (1 << bit);
		keyEvent.freshField[idx]	|= (1 << bit);
		
		keyEvent.count++;
	}

	/*
	=====================
	Input::KeyReleased
	=====================
	*/
	void Input::KeyReleased(int key) {
		// Key is a number between 0 and 512.
		// the key-th bit in keyField should be flagged to 0.
		char idx = key / 32;
		char bit = key % 32;

		keyEvent.keyField[idx]	&= ~(1 << bit);

		keyEvent.count--;
		keyEvent.activePrevFrame = true;
	}

	/*
	=====================
	Input::MouseMoved
	=====================
	*/
	void Input::MouseMoved(int x, int y) {
		mouseEvent.MouseMoved(Vec2((float)x, (float)y));
		mouseEvent.dirty = true;
	}

	/*
	=====================
	Input::MousePressed
	=====================
	*/
	void Input::MousePressed(int id) {
		mouseEvent.keys[id] = true;
		mouseEvent.fresh[id] = true;
		mouseEvent.dirty = true;
	}

	/*
	=====================
	Input::MouseReleased
	=====================
	*/
	void Input::MouseReleased(int id) {
		mouseEvent.keys[id] = false;
		mouseEvent.dirty = true;
	}

	/*
	=====================
	Input::ControllerButtonPressed

	=====================
	*/
	void Input::ControllerButtonPressed(int button) {
		contEvent.buttons |= 1 << button;
		contEvent.dirty = true;
	}

	/*
	=====================
	Input::ControllerButtonReleased
	=====================
	*/
	void Input::ControllerButtonReleased(int button) {
		contEvent.buttons &= ~(1 << button);
		contEvent.dirty = true;
	}

	/*
	=====================
	Input::ControllerAxisMoved
	=====================
	*/
	void Input::ControllerAxisMoved(int axis, int value) {
		contEvent.axes[axis] = value / ControllerEvent::XAXIS_AXIS_MAX;

		if (contEvent.axes[axis] > ControllerEvent::XAXIS_AXIS_MIN) {
			contEvent.dirty = true;
		}
	}

	/*
	=====================
	Input::Dispatch
	=====================
	*/
	void Input::Dispatch() {
		// dispatch keys..
		if (keyEvent.count || keyEvent.activePrevFrame) {
			for (unsigned int i=0; i<kl.size(); i++) {
				kl[i]->OnKeyEvent(keyEvent);
			}

			keyEvent.activePrevFrame = false;
		}
		keyEvent.Unfresh();

		// dispatch mouse..
		if (mouseEvent.dirty) {
			for (unsigned int i=0; i<ml.size(); i++) {
				ml[i]->OnMouseEvent(mouseEvent);
			}
		}
		mouseEvent.Unfresh();

		// dispatch control...
		if (cl.size() && contEvent.IsConnected()) {
			for (unsigned int i=0; i<cl.size(); i++) {
				cl[i]->OnControllerEvent(contEvent);
			}
		}
		contEvent.Unfresh();
	}

	/*
	=====================
	Input::DispatchPaused
	=====================
	*/
	void Input::DispatchPaused(GameNode *n) {
		bool controller = contEvent.IsConnected();

		// Dispatch to the pause-layer regardless of what has occured
		Dispatch_r(n, controller);

		keyEvent.activePrevFrame = false;
		keyEvent.Unfresh();
		mouseEvent.Unfresh();
		contEvent.Unfresh();
	}

	/*
	=====================
	Input::Dispatch_r
	=====================
	*/
	void Input::Dispatch_r(GameNode *n, bool controller) {
		n->OnKeyEvent(keyEvent);
		n->OnMouseEvent(mouseEvent);

		if (controller) {
			n->OnControllerEvent(contEvent);
		}

		for (unsigned int i=0; i<n->children.size(); i++) {
			Dispatch_r(n->children[i], controller);
		}
	}
}
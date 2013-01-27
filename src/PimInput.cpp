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
		_Reset();
	}

	/*
	=====================
	KeyEvent::KeyEvent
	=====================
	*/
	KeyEvent::KeyEvent(const KeyEvent&) {
		_Reset();
	}

	/*
	=====================
	KeyEvent::_Reset
	=====================
	*/
	void KeyEvent::_Reset() {
		count=0;
		for (int i=0; i<256; i++) {
			keys[i]=false;
			fresh[i]=false;
		}
	}

	/*
	=====================
	KeyEvent::_Unfresh
	=====================
	*/
	void KeyEvent::_Unfresh() {
		for (int i=0; i<256; i++) {
			fresh[i]=false;
		}
	}

	/*
	=====================
	KeyEvent::IsKeyDown
	=====================
	*/
	bool KeyEvent::IsKeyDown(const KeyCode k) const {
		return keys[k];
	}

	/*
	=====================
	KeyEvent::IsKeyFresh
	=====================
	*/
	bool KeyEvent::IsKeyFresh(const KeyCode k) const {
		return keys[k] && fresh[k];
	}

	/*
	=====================
	KeyEvent::IsKeyDown
	=====================
	*/
	bool KeyEvent::IsKeyDown(const string str) const {
		return keys[binds.find(str)->second];
	}

	/*
	=====================
	KeyEvent::IsKeyFresh
	=====================
	*/
	bool KeyEvent::IsKeyFresh(const string str) const {
		return keys[binds.find(str)->second] && fresh[binds.find(str)->second];
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
		_Reset();
	}

	/*
	=====================
	MouseEvent::MouseEvent
	=====================
	*/
	MouseEvent::MouseEvent(const MouseEvent&) {
		_Reset();
	}

	/*
	=====================
	MouseEvent::_Reset
	=====================
	*/
	void MouseEvent::_Reset() {
		position = Vec2(0.f,0.f);
		relPosition = Vec2(0.f,0.f);
		dirty=false;
		keys[0]=false;
		keys[1]=false;
		fresh[0]=false;
		fresh[0]=false;
	}

	/*
	=====================
	MouseEvent::_Unfresh
	=====================
	*/
	void MouseEvent::_Unfresh() {
		dirty		= keys[0] || keys[1];
		fresh[0]	= false;
		fresh[1]	= false;
	}

	/*
	=====================
	MouseEvent::_MouseMoved
	=====================
	*/
	void MouseEvent::_MouseMoved(Vec2 pos) {
		relPosition = pos;
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
		return (Vec2(position.x, GameControl::GetWindowHeight()-position.y)
				- GameControl::GetSingleton()->LowerLeftCorner())
			   * GameControl::GetSingleton()->GetCoordinateFactor();
	}

	/*
	=====================
	MouseEvent::GetRelative
	=====================
	*/
	Vec2 MouseEvent::GetRelative() const {
		return relPosition;
	}


	// --- ControllerEvent ---

	/*
	=====================
	ControllerEvent::ControllerEvent
	=====================
	*/
	ControllerEvent::ControllerEvent() {
		curBtnState = 0;
		prevBtnState = 0;

		if (Connected()) {
			GetStates();
		}
	}

	/*
	=====================
	ControllerEvent::Connected
	=====================
	*/
	bool ControllerEvent::Connected() {
		ZeroMemory(&xinputState, sizeof(XINPUT_STATE));
		return !XInputGetState(0, &xinputState);
	}

	/*
	=====================
	ControllerEvent::GetStates
	=====================
	*/
	void ControllerEvent::GetStates() {
		prevBtnState = curBtnState;
		curBtnState = xinputState.Gamepad.wButtons;
	}

	/*
	=====================
	ControllerEvent::Vibrate
	=====================
	*/
	void ControllerEvent::Vibrate(float l, float r) {
		if (Connected()) {
			// Create a Vibraton State
			XINPUT_VIBRATION vibration;

			// Zeroise the Vibration
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

			// Set the Vibration Values
			vibration.wLeftMotorSpeed = WORD(l * 65535);
			vibration.wRightMotorSpeed = WORD(r * 65535);

			// Vibrate the controller
			XInputSetState(0, &vibration);
		}
	}

	/*
	=====================
	ControllerEvent::IsKeyDown
	=====================
	*/
	bool ControllerEvent::IsKeyDown(Xbox x) const {
		return (curBtnState & x) != 0;
	}

	/*
	=====================
	ControllerEvent::IsKeyFresh
	=====================
	*/
	bool ControllerEvent::IsKeyFresh(Xbox x) const {
		return ((curBtnState & x) != 0) && ((prevBtnState & x) == 0);
	}

	/*
	=====================
	ControllerEvent::LeftStick
	=====================
	*/
	Vec2 ControllerEvent::LeftStick() const {
		SHORT lx = xinputState.Gamepad.sThumbLX;
		if (abs(lx) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
			lx = 0;
		}

		SHORT ly = xinputState.Gamepad.sThumbLY;
		if (abs(ly) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
			ly = 0;
		}

		return Pim::Vec2((float)lx/32767.f, (float)ly/32767.f);
	}

	/*
	=====================
	ControllerEvent::RightStick
	=====================
	*/
	Vec2 ControllerEvent::RightStick() const {
		SHORT rx = xinputState.Gamepad.sThumbRX;
		if (abs(rx) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
			rx = 0;
		}

		SHORT ry = xinputState.Gamepad.sThumbRY;
		if (abs(ry) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
			ry = 0;
		}

		return Pim::Vec2((float)rx/32767.f, (float)ry/32767.f);
	}

	/*
	=====================
	ControllerEvent::LeftTrigger
	=====================
	*/
	float ControllerEvent::LeftTrigger() const {
		return (float)xinputState.Gamepad.bLeftTrigger / 255.f;
	}

	/*
	=====================
	ControllerEvent::RightTrigger
	=====================
	*/
	float ControllerEvent::RightTrigger() const {
		return (float)xinputState.Gamepad.bRightTrigger / 255.f;
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

	// DO NOT CALL THE UNDERLINED METHODS MANUALLY. I'M SERIOUS YO. DON'T.

	/*
	=====================
	Input::_LostFocus
	=====================
	*/
	void Input::_LostFocus() {
		keyEvent._Reset();
		mouseEvent._Reset();
		Dispatch();
	}

	/*
	=====================
	Input::_GainedFocus
	=====================
	*/
	void Input::_GainedFocus() {
		// TODO: Dispatch a NULL message to all listeners
	}

	/*
	=====================
	Input::_KeyPressed
	=====================
	*/
	void Input::_KeyPressed(int key) {
		if (!keyEvent.keys[key]) {
			keyEvent.keys[key] = true;
			keyEvent.fresh[key] = true;
			keyEvent.count++;
		}
	}

	/*
	=====================
	Input::_KeyReleased
	=====================
	*/
	void Input::_KeyReleased(int key) {
		keyEvent.keys[key] = false;
		keyEvent.count--;
		keyEvent.activePrevFrame = true;
	}

	/*
	=====================
	Input::_MouseMoved
	=====================
	*/
	void Input::_MouseMoved(int x, int y) {
		mouseEvent._MouseMoved(Vec2((float)x, (float)y));
		mouseEvent.dirty = true;
	}

	/*
	=====================
	Input::_MousePressed
	=====================
	*/
	void Input::_MousePressed(int id) {
		mouseEvent.keys[id] = true;
		mouseEvent.fresh[id] = true;
		mouseEvent.dirty = true;
	}

	/*
	=====================
	Input::_MouseReleased
	=====================
	*/
	void Input::_MouseReleased(int id) {
		mouseEvent.keys[id] = false;
		mouseEvent.dirty = true;
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
		keyEvent._Unfresh();

		// dispatch mouse..
		if (mouseEvent.dirty) {
			mouseEvent.relPosition = mouseEvent.position - mouseEvent.lastPosition;
			mouseEvent.lastPosition = mouseEvent.position;

			for (unsigned int i=0; i<ml.size(); i++) {
				ml[i]->OnMouseEvent(mouseEvent);
			}
		}
		mouseEvent._Unfresh();

		// dispatch control...
		if (cl.size() && contEvent.Connected()) {
			contEvent.GetStates();

			for (unsigned int i=0; i<cl.size(); i++) {
				cl[i]->OnControllerEvent(contEvent);
			}
		}
	}

	/*
	=====================
	Input::DispatchPaused
	=====================
	*/
	void Input::DispatchPaused(GameNode *n) {
		mouseEvent.relPosition = mouseEvent.position - mouseEvent.lastPosition;
		mouseEvent.lastPosition = mouseEvent.position;

		bool cont = contEvent.Connected();
		if (cont) {
			contEvent.GetStates();
		}

		// Dispatch to the pause-layer regardless of what has occured
		Dispatch_r(n, cont);

		keyEvent.activePrevFrame = false;
		keyEvent._Unfresh();
		mouseEvent._Unfresh();
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
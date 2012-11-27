#include "PimInternal.h"

#include "PimVec2.h"
#include "PimGameNode.h"
#include "PimException.h"
#include "PimGameControl.h"

#include "PimInput.h"

#include <iostream>

namespace Pim
{
	// --- KeyEvent ---
	KeyEvent::KeyEvent()
	{ 
		activePrevFrame = false;
		_reset(); 
	}
	KeyEvent::KeyEvent(const KeyEvent&)
	{ 
		_reset(); 
	}
	void KeyEvent::_reset()
	{ 
		count=0; 
		for (int i=0; i<256; i++) 
		{
			keys[i]=false; fresh[i]=false; 
		}
	}
	void KeyEvent::_unfresh()
	{ 
		for (int i=0; i<256; i++) 
			fresh[i]=false; 
	}

	bool KeyEvent::isKeyDown(KeyCode k)
	{ 
		return keys[k]; 
	}
	bool KeyEvent::isKeyFresh(KeyCode k)
	{ 
		return keys[k] && fresh[k]; 
	}
	bool KeyEvent::isKeyDown(std::string str)
	{ 
		return keys[binds[str]]; 
	}
	bool KeyEvent::isKeyFresh(std::string str)
	{ 
		return keys[binds[str]] && fresh[binds[str]]; 
	}
	int KeyEvent::keyCount()
	{ 
		return count; 
	}
	void KeyEvent::bindKey(std::string &str, KeyCode k)
	{ 
		binds[str] = k; 
	}
	void KeyEvent::unbindKey(std::string &str)
	{
		binds.erase(str); 
	}


	// --- MouseEvent ---
	MouseEvent::MouseEvent()
	{ 
		_reset(); 
	}
	MouseEvent::MouseEvent(const MouseEvent&)
	{ 
		_reset(); 
	}
	void MouseEvent::_reset()
	{ 
		position = Vec2(0.f,0.f); 
		relPosition = Vec2(0.f,0.f); 
		dirty=false; 
		keys[0]=false;
		keys[1]=false; 
		fresh[0]=false;
		fresh[0]=false; 
	}
	void MouseEvent::_unfresh()
	{ 
		dirty		= keys[0] || keys[1]; 
		fresh[0]	= false;
		fresh[1]	= false;
	}
	void MouseEvent::_mouseMoved(Vec2 pos)
	{ 
		relPosition = pos; 
		position = pos; 
	}

	bool MouseEvent::isKeyDown(MouseButton mb)
	{ 
		return keys[mb]; 
	}
	bool MouseEvent::isKeyFresh(MouseButton mb)
	{
		return keys[mb] && fresh[mb]; 
	}
	Vec2 MouseEvent::getPosition()
	{
		return (Vec2(position.x, GameControl::getWindowHeight()-position.y)
				- GameControl::getSingleton()->lowerLeftCorner())
				* GameControl::getSingleton()->coordinateFactor(); 
	}
	Vec2 MouseEvent::getRelative()
	{
		return relPosition;
	}


	// --- ControllerEvent ---
	ControllerEvent::ControllerEvent()
	{
		curBtnState = 0;
		prevBtnState = 0;

		if (connected())
		{
			getStates();
		}
	}

	bool ControllerEvent::connected()
	{
		ZeroMemory(&xinputState, sizeof(XINPUT_STATE));
		return !XInputGetState(0, &xinputState);
	}
	void ControllerEvent::getStates()
	{
		prevBtnState = curBtnState;
		curBtnState = xinputState.Gamepad.wButtons;
	}
	void ControllerEvent::vibrate(float l, float r)
	{
		if (connected())
		{
			// Create a Vibraton State
			XINPUT_VIBRATION vibration;

			// Zeroise the Vibration
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

			// Set the Vibration Values
			vibration.wLeftMotorSpeed = l * 65535;
			vibration.wRightMotorSpeed = r * 65535;

			// Vibrate the controller
			XInputSetState(0, &vibration);
		}
	}

	bool ControllerEvent::isKeyDown(Xbox x)
	{
		return (bool)(curBtnState & x);
	}
	bool ControllerEvent::isKeyFresh(Xbox x)
	{
		return (bool)(curBtnState & x) && !(bool)(prevBtnState & x);
	}
	Vec2 ControllerEvent::leftStick()
	{
		SHORT lx = xinputState.Gamepad.sThumbLX;
		if (abs(lx) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			lx = 0;
		}

		SHORT ly = xinputState.Gamepad.sThumbLY;
		if (abs(ly) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			ly = 0;
		}

		return Pim::Vec2((float)lx/32767.f, (float)ly/32767.f);
	}
	Vec2 ControllerEvent::rightStick()
	{
		SHORT rx = xinputState.Gamepad.sThumbRX;
		if (abs(rx) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			rx = 0;
		}

		SHORT ry = xinputState.Gamepad.sThumbRY;
		if (abs(ry) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			ry = 0;
		}

		return Pim::Vec2((float)rx/32767.f, (float)ry/32767.f);
	}
	float ControllerEvent::leftTrigger()
	{
		return (float)xinputState.Gamepad.bLeftTrigger / 255.f;
	}
	float ControllerEvent::rightTrigger()
	{
		return (float)xinputState.Gamepad.bRightTrigger / 255.f;
	}


	// --- Input ---
	Input* Input::singleton = NULL;
	
	Input::Input()
	{
		PimAssert(singleton == NULL, "Input singleton is already initialized");
	}
	void Input::instantiateSingleton()
	{
		singleton = new Input;
	}
	void Input::clearSingleton()
	{
		if (singleton)
		{
			delete singleton;
			singleton = NULL;
		}
	}

	void Input::bindKey(std::string id, KeyEvent::KeyCode key)
	{
		PimAssert(key >= 0 && key < 256, "Key out of range.");
		keyEvent.bindKey(id, key);

#ifdef _DEBUG
		std::cout<<"Bound key - " <<id <<": " <<keyEvent.binds[id] <<"\n";
#endif
	}
	void Input::unbindKey(std::string id)
	{
		keyEvent.unbindKey(id);
	}

	// Called by GameControl upon registration. Use GameControl::add__listener.
	void Input::addKeyListener(GameNode *node)
	{
		kl.push_back(node);
	}
	void Input::removeKeyListener(GameNode *node)
	{
		for (unsigned int i=0; i<kl.size(); i++)
			if (kl[i] == node)
				kl.erase(kl.begin() + i);
	}

	void Input::addMouseListener(GameNode *node)
	{
		ml.push_back(node);
	}
	void Input::removeMouseListener(GameNode *node)
	{
		for (unsigned int i=0; i<ml.size(); i++)
		{
			if (ml[i] == node)
			{
				ml.erase(ml.begin() + i);
			}
		}
	}

	void Input::addControlListener(GameNode *node)
	{
		cl.push_back(node);
	}
	void Input::removeControlListener(GameNode *node)
	{
		for (unsigned int i=0; i<cl.size(); i++)
		{
			if (cl[i] == node)
			{
				cl.erase(cl.begin() + i);
			}
		}
	}

	void Input::vibrateXbox(float leftVib, float rightVib)
	{
		contEvent.vibrate(leftVib, rightVib);
	}
	
	// DO NOT CALL THESE METHODS MANUALLY. I'M SERIOUS YO. DON'T.
	void Input::_lostFocus()
	{
		// TODO: Dispatch a NULL message to all
		keyEvent._reset();
		mouseEvent._reset();
		dispatch();
	}
	void Input::_gainedFocus()
	{

	}
	void Input::_keyPressed(int key)
	{
		if (!keyEvent.keys[key])
		{
			keyEvent.keys[key] = true;
			keyEvent.fresh[key] = true;
			keyEvent.count++;
		}
	}
	void Input::_keyReleased(int key)
	{
		keyEvent.keys[key] = false;
		keyEvent.count--;
		keyEvent.activePrevFrame = true;
	}
	void Input::_mouseMoved(int x, int y)
	{
		mouseEvent._mouseMoved(Vec2((float)x, (float)y));
		mouseEvent.dirty = true;
	}
	void Input::_mousePressed(int id)
	{
		mouseEvent.keys[id] = true;
		mouseEvent.fresh[id] = true;
		mouseEvent.dirty = true;
	}
	void Input::_mouseReleased(int id)
	{
		mouseEvent.keys[id] = false;
		mouseEvent.dirty = true;
	}

	void Input::dispatch()
	{
		// dispatch keys..
		if (keyEvent.count || keyEvent.activePrevFrame)
		{
			for (unsigned int i=0; i<kl.size(); i++)
			{
				kl[i]->keyEvent(keyEvent);
			}

			keyEvent.activePrevFrame = false;
		}
		keyEvent._unfresh();

		// dispatch mouse..
		if (mouseEvent.dirty)
		{
			mouseEvent.relPosition = mouseEvent.position - mouseEvent.lastPosition;
			mouseEvent.lastPosition = mouseEvent.position;

			for (unsigned int i=0; i<ml.size(); i++)
			{
				ml[i]->mouseEvent(mouseEvent);
			}
		}
		mouseEvent._unfresh();

		// dispatch control...
		if (cl.size() && contEvent.connected())
		{
			contEvent.getStates();

			for (unsigned int i=0; i<cl.size(); i++)
			{
				cl[i]->controllerEvent(contEvent);
			}
		}
	}
	
	void Input::dispatchPaused(GameNode *n)
	{
		mouseEvent.relPosition = mouseEvent.position - mouseEvent.lastPosition;
		mouseEvent.lastPosition = mouseEvent.position;

		bool cont = contEvent.connected();
		if (cont)
		{
			contEvent.getStates();
		}

		// Dispatch to the pause-layer regardless of what has occured
		recursiveDispatch(n, cont);

		keyEvent.activePrevFrame = false;
		keyEvent._unfresh();
		mouseEvent._unfresh();
	}
	void Input::recursiveDispatch(GameNode *n, bool controller)
	{
		n->keyEvent(keyEvent);
		n->mouseEvent(mouseEvent);

		if (controller)
		{
			n->controllerEvent(contEvent);
		}

		for each (GameNode *child in n->children)
		{
			recursiveDispatch(child, controller);
		}
	}
}
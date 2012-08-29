#include "Stdafx.h"

#include "Pim.h"

#include "PimInput.h"
#include "PimVec2.h"
#include "PimException.h"

namespace Pim
{
	Input* Input::singleton = NULL;
	
	Input::Input()
	{
		PimAssert(singleton == NULL, "Input singleton is already initialized");

		for (int i=0; i<256; i++) keys[i] = false;
		mouse[0] = false; mouse[1] = false;
		mouseDirty	= false;
		keyDirty	= false; 
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

	void Input::bindKey(std::string id, Key::KeyCode key)
	{
		PimAssert(key >= 1 && key < 256, id.append(": Key out of range."));
		boundKeys[id] = key;
	}
	void Input::unbindKey(std::string id)
	{
		boundKeys.erase(id);
	}
	
	bool Input::keyStatus(Key::KeyCode key)
	{
		return keys[key];
	}
	bool Input::keyStatus(std::string id)
	{
		return keys[boundKeys[id]];
	}
	bool Input::mouseStatus(Mouse::MouseButton key)
	{
		return mouse[key];
	}
	Vec2 Input::mousePosition()
	{
		return mousePos;
	}
	Vec2 Input::mouseRelPosition()
	{
		return mousePos - mousePosLastFrame;
	}
	
	// DO NOT CALL THESE METHODS MANUALLY
	void Input::lostFocus()
	{
		// TODO: Dispatch a NULL message to all
	}
	void Input::gainedFocus()
	{

	}
	void Input::keyPressed(int key)
	{
		keyDirty = true;
		keys[key] = true;
	}
	void Input::keyReleased(int key)
	{
		keyDirty = true;
		keys[key] = false;
	}
	void Input::mouseMoved(int x, int y)
	{
		mouseDirty = true;
		mousePos.x = x;
		mousePos.y = y;
	}
	void Input::mousePressed(int id)
	{
		mouseDirty = true;
		mouse[id] = true;
	}
	void Input::mouseReleased(int id)
	{
		mouseDirty = true;
		mouse[id] = false;
	}
}
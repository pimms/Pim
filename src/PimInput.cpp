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

	bool KeyEvent::isKeyDown(Key::KeyCode k)
	{ 
		return keys[k]; 
	}
	bool KeyEvent::isKeyFresh(Key::KeyCode k)
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
	void KeyEvent::bindKey(std::string &str, Key::KeyCode k)
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
		dirty=false; 
		relPosition=Vec2(0.f, 0.f); 
		fresh[0]=false;
		fresh[1]=false;
	}
	void MouseEvent::_mouseMoved(Vec2 pos)
	{ 
		relPosition = pos; 
		position = pos; 
	}

	bool MouseEvent::isKeyDown(Mouse::MouseButton mb)
	{ 
		return keys[mb]; 
	}
	bool MouseEvent::isKeyFresh(Mouse::MouseButton mb)
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

	void Input::bindKey(std::string id, Key::KeyCode key)
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
			if (ml[i] == node)
				ml.erase(ml.begin() + i);
	}
	
	// DO NOT CALL THESE METHODS MANUALLY. I'M SERIOUS YO. DON'T.
	void Input::_lostFocus()
	{
		// TODO: Dispatch a NULL message to all
		keyEvent._reset();
		mouseEvent._reset();
		_dispatch();
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

	void Input::_dispatch()
	{
		// dispatch keys..
		if (keyEvent.count)
		{
			for (unsigned int i=0; i<kl.size(); i++)
				kl[i]->keyEvent(keyEvent);
		}
		keyEvent._unfresh();

		// dispatch mouse..
		if (mouseEvent.dirty)
		{
			for (unsigned int i=0; i<ml.size(); i++)
				ml[i]->mouseEvent(mouseEvent);
		}
		mouseEvent._unfresh();
	}
}
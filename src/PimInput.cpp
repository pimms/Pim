#include "Stdafx.h"

#include "Pim.h"
#include "PimInput.h"
#include "PimVec2.h"
#include "PimException.h"

#include <iostream>

namespace Pim
{
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

		std::cout<<id <<"\n";
		std::cout<<"Bound key - " <<id <<": " <<keyEvent.binds[id] <<"\n";
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
	
	// DO NOT CALL THESE METHODS MANUALLY
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
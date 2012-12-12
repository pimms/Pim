#include <stdio.h>
#include "PimAction.h"
#include "PimSprite.h"
#include "PimAssert.h"

// Upon implementing the various Action subclasses, I noticed that most of the 
// code in the update-methods were structurally identical. The following macros
// may seem large, inuntuitive and nasty (and frankly they are), but the amount
// of duplicated code avoided by this method is in my opinion a worthy tradeoff.

// 
//	ACTION_UPDATE_RELATIVE: 
//		Update macro for [verb]ToActions
//
//	PARAMETERS:
//		_PARENT_TYPE:	The type of the parent; Sprite, GameNode, etc.
//		_MEMBER:		The member variable to change; position, scale, etc
//		_UPDATE_SET:	Must be of type _MEMBER. Look at any usage for reference.
//		_FINAL_SET:		Must be of type _MEMBER. Set the final value of _MEMBER.
#define ACTION_UPDATE_STATIC(_PARENT_TYPE,_MEMBER,_UPDATE_SET,_FINAL_SET)	\
timer -= dt;																\
if (timer > 0.f )													\
{																			\
	((_PARENT_TYPE*)getParent())->_MEMBER = _UPDATE_SET;					\
}																			\
else																		\
{																			\
	((_PARENT_TYPE*)getParent())->_MEMBER = _FINAL_SET;						\
	cleanup();																\
}	

//
//	ACTION_UPDATE_RELATIVE:
//		Update macro for [verb]ByActions
//
//	PARAMETERS:
//		_PARENT_TYPE:	The type of the parent; Sprite, GameNode, etc.
//		_MEMBER:		The member variable to change; position, scale, color, etc.
//						Note that the _MEMBER-type ABSOLUTELY MUST override the "+=" operator.
//		_UPDATE_VAR:	Must be of type _MEMBER. Only pass the variable, it's multiplied by
//						'dt' and '(1.f/dur)' in the macro.
#define ACTION_UPDATE_RELATIVE(_PARENT_TYPE,_MEMBER,_UPDATE_VAR)			\
bool ovride = false;														\
if (timer > 0.f && timer - dt <= 0.f)										\
{																			\
	float tmp = timer;														\
	timer -= dt;															\
	dt = tmp;																\
	ovride = true;															\
}																			\
else																		\
{																			\
	timer -= dt;															\
}																			\
if (timer > 0.f || ovride)													\
{																			\
	((_PARENT_TYPE*)getParent())->_MEMBER += _UPDATE_VAR *dt*(1.f/dur);		\
	if (ovride) /* override means we're done */								\
	{																		\
		cleanup();															\
	}																		\
}																			\
else																		\
{																			\
	cleanup();																\
}																			




namespace Pim
{
	/* VIRTUAL Base Action */
	BaseAction::BaseAction(float duration)
	{
		done					= false;
		inQueue					= false;
		notifyOnCompletion		= false;
		queue					= NULL;
		notificationCallback	= NULL;
		dur						= duration;
		timer					= duration;
		initialDur				= duration;
	}
	void BaseAction::cleanup()
	{
		if (inQueue)
		{
			done = true;
			unlistenFrame();

			if (notifyOnCompletion)
			{
				notificationCallback->actionCompleted(this);
			}

			queue->activateNext();
		}
		else
		{
			// Make this node an orphan
			getParent()->removeChild(this, false);

			// Notify the parent if required - note that this
			// call may call "removeAllChildren"
			if (notifyOnCompletion)
			{
				notificationCallback->actionCompleted(this);
			}

			delete this;
		}
	}


	/*	
		###########################################
		#####          NODE ACTIONS		      #####
		###########################################
	*/
	/* Action */
	void Action::activate()
	{
		PimAssert(getParent() != NULL, "Action is orphan");
		listenFrame();

		if (!notificationCallback)
		{
			notificationCallback = getParent();
		}
	}

	/* Move To Action */
	MoveToAction::MoveToAction(Vec2 destination, float duration)
		: Action(duration)
	{
		dest		= destination;
	}
	void MoveToAction::activate()
	{
		Action::activate();
		start = getParent()->position;
	}
	void MoveToAction::update(float dt)
	{
		ACTION_UPDATE_STATIC(
			GameNode,
			position,
			Vec2(
				dest.x + (start.x-dest.x) * (timer/dur),
				dest.y + (start.y-dest.y) * (timer/dur)
			),
			dest
		);
	}

	/* Move By Action */
	MoveByAction::MoveByAction(Vec2 relative, float duration)
		: Action(duration)
	{
		rel		= relative;
	}
	void MoveByAction::activate()
	{
		Action::activate();
	}
	void MoveByAction::update(float dt)
	{
		ACTION_UPDATE_RELATIVE(GameNode, position, rel);
	}

	/* Rotate By Action */
	RotateByAction::RotateByAction(float angle, float duration)
		: Action(duration)
	{
		total		= angle;
		dir			= angle / abs(angle);
	}
	void RotateByAction::activate()
	{
		Action::activate();
		remainding = total;
	}
	void RotateByAction::update(float dt)
	{
		ACTION_UPDATE_RELATIVE(GameNode, rotation, total);
	}

	/* Delay Action */
	DelayAction::DelayAction(float duration)
		: Action(duration)
	{
	}
	void DelayAction::activate()
	{
		Action::activate();
	}
	void DelayAction::update(float dt)
	{
		timer -= dt;

		if (timer <= 0.f)
		{
			cleanup();
		}
	}

	/*	
		###########################################
		#####         SPRITE ACTIONS		  #####
		###########################################
	*/
	/* Sprite Action */
	void SpriteAction::activate()
	{
		PimAssert(getParent() != NULL, "Action is orphan");
		PimAssert(dynamic_cast<Sprite*>(getParent()) != NULL, 
			"Cannot add a Sprite-action to a non-sprite node!");

		listenFrame();

		if (!notificationCallback)
		{
			notificationCallback = getParent();
		}
	}

	/* Tint Action */
	TintAction::TintAction(Color fadeTo, float duration)
		: SpriteAction(duration)
	{
		dest	= fadeTo;
	}
	void TintAction::activate()
	{
		SpriteAction::activate();
		source = ((Sprite*)getParent())->color;
	}
	void TintAction::update(float dt)
	{
		ACTION_UPDATE_STATIC(
			Sprite,
			color,
			Color(
				dest.r + (source.r-dest.r) * (timer/dur),
				dest.g + (source.g-dest.g) * (timer/dur),
				dest.b + (source.b-dest.b) * (timer/dur),
				dest.a + (source.a-dest.a) * (timer/dur)
			),
			dest
		);
	}

	/* Scale To Action */
	ScaleToAction::ScaleToAction(Vec2 factor, float duration)
		: SpriteAction(duration)
	{
		dest = factor;
	}
	void ScaleToAction::activate()
	{
		SpriteAction::activate();
		source = ((Sprite*)getParent())->scale;
	}
	void ScaleToAction::update(float dt)
	{
		ACTION_UPDATE_STATIC(
			Sprite,
			scale,
			Vec2(
				dest.x + (source.x-dest.x) * (timer/dur),
				dest.y + (source.y-dest.y) * (timer/dur)
			),
			dest
		);
	}

	/* Scale By Action */
	ScaleByAction::ScaleByAction(Vec2 factor, float duration)
		: SpriteAction(duration)
	{
		remainding = factor;
	}
	void ScaleByAction::activate()
	{
		SpriteAction::activate();
	}
	void ScaleByAction::update(float dt)
	{
		ACTION_UPDATE_RELATIVE(Sprite, scale, remainding);
	}


	/*	
		###########################################
		#####          ACTION QUEUE	 	      #####
		###########################################
	*/
	/* Action Queue */
	ActionQueue::ActionQueue(int numAct, BaseAction *act1, ...)
		: Action(0.f)
	{
		PimAssert(numAct != 0, "No actions / invalid num provided to ActionQueue");
		PimAssert(numAct < 32, "ActionQueues does not support more than 32 actions");

		curAct = NULL;

		actions.push_back(act1);
		act1->inQueue = true;
		act1->queue = this;

		va_list	argp;
		va_start(argp, act1);

		for (int i=1; i<numAct; i++)
		{
			actions.push_back(va_arg(argp, BaseAction*));
			actions[i]->inQueue = true;
			actions[i]->queue = this;
		}

		va_end(argp);
	}
	ActionQueue::~ActionQueue()
	{
		// Delete unplayed actions
		for (int i=0; i<actions.size(); i++)
		{
			delete actions[i];
		}

		actions.clear();
	}
	void ActionQueue::activate()
	{
		PimAssert(getParent() != NULL, "Action is orphan");

		listenFrame();
		activateNext();
	}
	void ActionQueue::update(float dt)
	{
		if (done)
		{
			getParent()->removeChild(this);
		}
	}
	void ActionQueue::activateNext()
	{
		if (actions.size() != 0)
		{
			float excess = 0.f;

			if (curAct)
			{
				excess = curAct->timer;
				getParent()->removeChild(curAct);
			}

			curAct = actions[0];
			actions.pop_front();
			
			curAct->dur += excess;
			curAct->timer += excess;

			getParent()->addChild(curAct);
			curAct->activate();
		}
		else
		{
			curAct = NULL;
			done = true;
		}
	}

	/* Action Queue Repeat */
	ActionQueueRepeat::ActionQueueRepeat(unsigned int repNum, int numAct, BaseAction *act1, ...)
	{
		// Code duplication from ActionQueue::ActionQueue(...).
		// Variable arguments proved difficult to pass on.
		PimAssert(numAct != 0, "No actions / invalid num provided to ActionQueue");
		PimAssert(numAct < 32, "ActionQueues does not support more than 32 actions");

		curAct = NULL;

		actions.push_back(act1);
		act1->inQueue = true;
		act1->queue = this;

		va_list	argp;
		va_start(argp, act1);

		for (int i=1; i<numAct; i++)
		{
			actions.push_back(va_arg(argp, BaseAction*));
			actions[i]->inQueue = true;
			actions[i]->queue = this;
		}

		va_end(argp);

		// Custom init
		actionIdx		= -1;		// gets incremented in activateNext()
		remaindingLoops = repNum;
		infinite		= false;
	}
	ActionQueueRepeat::~ActionQueueRepeat()
	{
		if (curAct)
		{
			// Everything in the actions-deque is deleted in ActionQueue's 
			// destructor - to avoid the ActionQueue AND the parent of curAct
			// attempting to delete it, it's removed from the actions-deque.
			for (int i=0; i<actions.size(); i++)
			{
				if (actions[i] == curAct)
				{
					actions.erase(actions.begin() + i);
				}
			}
		}
	}
	void ActionQueueRepeat::activateNext()
	{
		// Update the action counters
		if (++actionIdx >= actions.size())
		{
			actionIdx = 0;

			if (!infinite)
			{
				remaindingLoops--;
			}
		}	

		if (infinite || remaindingLoops > 0)
		{
			float excess = 0.f;

			if (curAct)
			{
				excess = curAct->timer;

				// The action is not deleted.
				getParent()->removeChild(curAct, false);
				curAct->unlistenFrame();
			}

			// Prepare the next action
			curAct = actions[actionIdx];

			curAct->timer	= curAct->initialDur + excess;
			curAct->dur		= curAct->initialDur + excess;
			curAct->done	= false;

			// Run the next action
			getParent()->addChild(curAct);
			curAct->activate();

			//curAct->update(-excess);
		}
		else
		{
			done = true;
		}
	}
}
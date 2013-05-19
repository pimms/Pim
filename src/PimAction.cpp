#include <stdio.h>
#include "PimAction.h"
#include "PimSprite.h"
#include "PimAssert.h"

namespace Pim {
	/*
	=====================
	BaseAction::BaseAction
	=====================
	*/
	BaseAction::BaseAction(float duration) {
		done					= false;
		inQueue					= false;
		notifyOnCompletion		= false;
		queue					= NULL;
		notificationCallback	= NULL;
		dur						= duration;
		timer					= duration;
		initialDur				= duration;
	}

	/*
	=====================
	BaseAction::Cleanup
	=====================
	*/
	void BaseAction::Cleanup() {
		if (notifyOnCompletion) {
			notificationCallback->OnActionCompleted(this);
		}

		if (inQueue) {
			done = true;
			UnlistenFrame();

			queue->ActivateNext();
		} else {
			if (GetParent()) {
				GetParent()->RemoveChild(this);
			}
		}
	}



	/*
	=====================
	Action::Action
	=====================
	*/
	void Action::Activate() {
		PimAssert(GetParent() != NULL, "Action is orphan");
		ListenFrame();

		if (!notificationCallback) {
			notificationCallback = GetParent();
		}
	}

	

	/*
	=====================
	MoveToAction::MoveToAction
	=====================
	*/
	MoveToAction::MoveToAction(Vec2 destination, float duration)
		: Action(duration) {
		dest = destination;
	}

	/*
	=====================
	MoveToAction::Activate
	=====================
	*/
	void MoveToAction::Activate() {
		Action::Activate();
		start = GetParent()->position;
	}

	/*
	=====================
	MoveToAction::Update
	=====================
	*/
	void MoveToAction::Update(float dt) {
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

	

	/*
	=====================
	MoveByAction::MoveByAction
	=====================
	*/
	MoveByAction::MoveByAction(Vec2 relative, float duration)
		: Action(duration) {
		rel		= relative;
	}

	/*
	=====================
	MoveByAction::Activate
	=====================
	*/
	void MoveByAction::Activate() {
		Action::Activate();
	}

	/*
	=====================
	MoveByAction::Update
	=====================
	*/
	void MoveByAction::Update(float dt) {
		ACTION_UPDATE_RELATIVE(GameNode, position, rel);
	}

	

	/*
	=====================
	RotateByAction::RotateByAction
	=====================
	*/
	RotateByAction::RotateByAction(float angle, float duration)
		: Action(duration) {
		total		= angle;
		dir			= angle / abs(angle);
	}

	/*
	=====================
	RotateByAction::Activate
	=====================
	*/
	void RotateByAction::Activate() {
		Action::Activate();
		remainding = total;
	}

	/*
	=====================
	RotateByAction::Update
	=====================
	*/
	void RotateByAction::Update(float dt) {
		ACTION_UPDATE_RELATIVE(GameNode, rotation, total);
	}

	

	/*
	=====================
	DelayAction::DelayAction
	=====================
	*/
	DelayAction::DelayAction(float duration)
		: Action(duration) {
	}

	/*
	=====================
	DelayAction::Activate
	=====================
	*/
	void DelayAction::Activate() {
		Action::Activate();
	}

	/*
	=====================
	DelayAction::Update
	=====================
	*/
	void DelayAction::Update(float dt) {
		timer -= dt;

		if (timer <= 0.f) {
			Cleanup();
		}
	}


	
	/*
	=====================
	SpriteAction::Activate
	=====================
	*/
	void SpriteAction::Activate() {
		PimAssert(GetParent() != NULL, "Action is orphan");
		PimAssert(dynamic_cast<Sprite*>(GetParent()) != NULL,
				  "Cannot add a Sprite-action to a non-sprite node!");

		ListenFrame();

		if (!notificationCallback) {
			notificationCallback = GetParent();
		}
	}

	

	/*
	=====================
	TintAction::TintAction
	=====================
	*/
	TintAction::TintAction(Color fadeTo, float duration)
		: SpriteAction(duration) {
		dest	= fadeTo;
	}

	/*
	=====================
	TintAction::Activate
	=====================
	*/
	void TintAction::Activate() {
		SpriteAction::Activate();
		source = ((Sprite*)GetParent())->color;
	}

	/*
	=====================
	TintAction::Update
	=====================
	*/
	void TintAction::Update(float dt) {
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

	

	/*
	=====================
	ScaleToAction::ScaleToAction
	=====================
	*/
	ScaleToAction::ScaleToAction(Vec2 factor, float duration)
		: SpriteAction(duration) {
		dest = factor;
	}

	/*
	=====================
	ScaleToAction::Activate
	=====================
	*/
	void ScaleToAction::Activate() {
		SpriteAction::Activate();
		source = ((Sprite*)GetParent())->scale;
	}

	/*
	=====================
	ScaleToAction::Update
	=====================
	*/
	void ScaleToAction::Update(float dt) {
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




	/*
	=====================
	ScaleByAction::ScaleByAction
	=====================
	*/
	ScaleByAction::ScaleByAction(Vec2 factor, float duration)
		: SpriteAction(duration) {
		remainding = factor;
	}

	/*
	=====================
	ScaleByAction::Activate
	=====================
	*/
	void ScaleByAction::Activate() {
		SpriteAction::Activate();
	}

	/*
	=====================
	ScaleByAction::Update
	=====================
	*/
	void ScaleByAction::Update(float dt) {
		ACTION_UPDATE_RELATIVE(Sprite, scale, remainding);
	}



	/*
	=====================
	ActionQueue::ActionQueue
	=====================
	*/
	ActionQueue::ActionQueue(int numAct, BaseAction *act1, ...)
		: Action(0.f) {
		PimAssert(numAct != 0, "No actions / invalid num provided to ActionQueue");
		PimAssert(numAct < 32, "ActionQueues does not support more than 32 actions");

		curAct = NULL;

		actions.push_back(act1);
		act1->inQueue = true;
		act1->queue = this;

		va_list	argp;
		va_start(argp, act1);

		for (int i=1; i<numAct; i++) {
			actions.push_back(va_arg(argp, BaseAction*));
			actions[i]->inQueue = true;
			actions[i]->queue = this;
		}

		va_end(argp);
	}

	/*
	=====================
	ActionQueue::~ActionQueue
	=====================
	*/
	ActionQueue::~ActionQueue() {
		// Delete unplayed actions
		for (unsigned i=0; i<actions.size(); i++) {
			delete actions[i];
		}

		actions.clear();
	}

	/*
	=====================
	ActionQueue::Activate
	=====================
	*/
	void ActionQueue::Activate() {
		PimAssert(GetParent() != NULL, "Action is orphan");

		ListenFrame();
		ActivateNext();
	}

	/*
	=====================
	ActionQueue::Update
	=====================
	*/
	void ActionQueue::Update(float dt) {
		if (done) {
			GetParent()->RemoveChild(this);
		}
	}

	/*
	=====================
	ActionQueue::GetTotalTime
	=====================
	*/
	float ActionQueue::GetTotalTime() {
		float ret = 0.f;

		for (int i=0; i<actions.size(); i++) {
			ret += actions[i]->initialDur;
		}

		return ret;
	}

	/*
	=====================
	ActionQueue::ActivateNext
	=====================
	*/
	void ActionQueue::ActivateNext() {
		if (actions.size() != 0) {
			float excess = 0.f;

			if (curAct) {
				excess = curAct->timer;

				if (parent) {
					parent->RemoveChild(curAct);
				}
			}

			curAct = actions[0];
			actions.pop_front();

			curAct->dur += excess;
			curAct->timer += excess;

			if (parent) {
				parent->AddChild(curAct);
				curAct->Activate();
			}
		} else {
			curAct = NULL;
			done = true;
		}
	}



	/*
	=====================
	ActionQueueRepeat::ActionQueueRepeat
	=====================
	*/
	ActionQueueRepeat::ActionQueueRepeat(unsigned int repNum, int numAct, BaseAction *act1, ...) {
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

		for (int i=1; i<numAct; i++) {
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

	/*
	=====================
	ActionQueueRepeat::~ActionQueueRepeat
	=====================
	*/
	ActionQueueRepeat::~ActionQueueRepeat() {
		if (curAct) {
			// Everything in the actions-deque is deleted in ActionQueue's
			// destructor - to avoid the ActionQueue AND the parent of curAct
			// attempting to delete it, it's removed from the actions-deque.
			for (unsigned i=0; i<actions.size(); i++) {
				if (actions[i] == curAct) {
					actions.erase(actions.begin() + i);

					if (parent) {
						parent->RemoveChild(curAct);
					}
				}
			}
		}
	}

	/*
	=====================
	ActionQueueRepeat::ActivateNext
	=====================
	*/
	void ActionQueueRepeat::ActivateNext() {
		if (willDelete) return;

		// Update the action counters
		if (unsigned(++actionIdx) >= actions.size()) {
			actionIdx = 0;

			if (!infinite) {
				remaindingLoops--;
			}
		}

		if (infinite || remaindingLoops > 0) {
			float excess = 0.f;

			if (curAct) {
				excess = curAct->timer;
				float total = GetTotalTime();

				while (excess < total) {
					excess += total;
					if (!infinite) {
						remaindingLoops--;
					}
				}
				if (excess > 0.f) excess = 0.f;

				// The action is not deleted.
				if (parent) {
					GetParent()->RemoveChild(curAct, false);
				}

				curAct->UnlistenFrame();
			}

			// Prepare the next action
			curAct = actions[actionIdx];

			curAct->timer	= curAct->initialDur + excess;
			curAct->dur		= curAct->initialDur + excess;
			curAct->done	= false;

			// Run the next action
			if (parent) {
				GetParent()->AddChild(curAct);
				curAct->Activate();
			}

			curAct->Update(-excess);
		} else {
			done = true;
		}
	}
}
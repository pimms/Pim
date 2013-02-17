#pragma once

#include "PimGameNode.h"
#include <queue>

namespace Pim {
	class ActionQueue;
	class ActionQueueRepeat;

	/* The VIRTUAL BaseAction class */
	/* All derivatives of BaseAction works as such:
	   The action must be passed to the "runAction(..)" method on a
	   GameNode (or any subclass). The node then calls "action->activate()",
	   which initializes the action. The action receives update(float) calls
	   until it's criteria of completion is met, at which point it is removed
	   from the parent and deleted.

	   ActionQueueRepeat does not delete the actions until it's own deletion, as
	   the actions are needed several times.									*/
	class BaseAction : public GameNode {
	protected:
		friend class ActionQueueInfinite;
		friend class ActionQueueRepeat;
		friend class ActionQueue;
		friend class GameNode;

	public:
		// If true, the node running the action will be notified via the
		// GameNode::actionCompleted(BaseAction*) overrideable method.
		bool					notifyOnCompletion;

		// By default, the parent of the action is notified.
		// You may override this by setting a callback-node yourself.
		GameNode*				notificationCallback;
		
								BaseAction(float duration);
		virtual void			Update(float)	= 0;
		float					TimeRemaining() { return timer; }

	protected:
		bool					done;
		bool					inQueue;
		ActionQueue*			queue;
		float					dur;
		float					timer;
		float					initialDur;  // Initial value of timer

		virtual void			Activate() = 0;
		void					Cleanup();
	};


	/* Action class */
	/* Can be run on any GameNode */
	/* Implemented Actions:
			- MoveToAction			Moves the node to a specific position
			- MoveByAction			Moves the node relatively by a vector
			- RotateByAction		Rotates the node a certain amount
			- DelayAction			Wait for X seconds						*/
	class Action : public BaseAction {
	protected:
		friend class GameNode;
	public:
								Action(float duration) : BaseAction(duration) {}
	protected:
		virtual void			Update(float) {}
		virtual void			Activate();
	};

	/*
		###########################################
		#####          NODE ACTIONS		      #####
		###########################################
	*/
	/* Move To Action */
	class MoveToAction : public Action {
	public:
								MoveToAction(Vec2 destination, float duration);
		void					Update(float dt);

	protected:
		Vec2					start;
		Vec2					dest;

		void					Activate();
	};

	/* Move By Action */
	class MoveByAction : public Action {
	public:
								MoveByAction(Vec2 relative, float duration);
		void					Update(float dt);

	protected:
		Vec2					rel;

		void					Activate();
	};

	/* Rotate By Action */
	class RotateByAction : public Action {
	public:
								RotateByAction(float angle, float duration);
		void					Update(float dt);

	protected:
		float					total;
		float					remainding;
		float					dir;

		void					Activate();
	};

	/* Delay Action */
	class DelayAction : public Action {
	public:
								DelayAction(float duration);
		void					Update(float dt);

	protected:
		void					Activate();
	};

	/*
		###########################################
		#####         SPRITE ACTIONS		  #####
		###########################################
	*/
	/* Sprite Action class */
	/* Can only be run on Sprites */
	/* Implemented SpriteActions:
			- TintAction			Tints the sprite to a certain color over time
			- ScaleToAction			Scales the sprite to a provided value
			- ScaleByAction			Scales the sprite relatively to the provided factors  */
	class SpriteAction : public BaseAction {
	protected:
		friend class Sprite;

	public:
								SpriteAction(float duration) : BaseAction(duration) {}

	protected:
		virtual void			Update(float) {}
		virtual void			Activate();
	};

	/* Tint Action */
	class TintAction : public SpriteAction {
	public:
								TintAction(Color fadeTo, float duration);
		void					Update(float dt);

	protected:
		Color					source;		// Initial color of parent sprite
		Color					dest;		// The color to which we're tinting

		void					Activate();
	};

	/* Scale To Action */
	class ScaleToAction : public SpriteAction {
	public:
								ScaleToAction(Vec2 factor, float duration);
		void					Update(float);

	protected:
		Vec2					source;		// Initial scale of the sprite
		Vec2					dest;		// The scale destination

		void					Activate();
	};

	/* Scale By Action */
	class ScaleByAction : public SpriteAction {
	public:
								ScaleByAction(Vec2 factor, float duration);
		void					Update(float);

	protected:
		Vec2					remainding;

		void					Activate();
	};


	/*
		###########################################
		#####          ACTION QUEUES	 	  #####
		###########################################
	*/
	/* Run several actions in succession
	   There's a limit of 32 actions.	*/
	class ActionQueue : public Action {
	protected:
		friend class BaseAction;
		friend class GameNode;
		friend class Sprite;

	public:
								ActionQueue(int numActions, BaseAction *action1, ...);
								~ActionQueue();
		void					Update(float);
		BaseAction*				GetCurrentAction() {
			return curAct;
		}

	protected:
		deque<BaseAction*>		actions;
		BaseAction*				curAct;

								ActionQueue() : Action(0.f) {}
		void					Activate();
		virtual void			ActivateNext();
	};

	/* ActionQueueRepeat */
	/* Run several actions in succession, repeated N times.
	   The bool 'infinite' can be flagged to true to repeat the action
	   sequence indefinitely. Also note that the uint 'remaindingLoops' is public,
	   so the amount of loops to run can be altered. As with all actions, the
	   ActionQueueRepeat and all it's actions are deleted upon completion.		*/
	class ActionQueueRepeat : public ActionQueue {
	public:
		unsigned int			remaindingLoops; // Initialized at n, decremented for each loop
		bool					infinite;

								ActionQueueRepeat(unsigned int repeatNum, 
											int numActions, BaseAction *action1, ...);
								~ActionQueueRepeat();

	private:
		int						actionIdx; // The index of the running action

		void					ActivateNext();
	};
}

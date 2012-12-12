#pragma once

#include "PimGameNode.h"
#include <queue>

namespace Pim
{
	// Forward declarations
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
	class BaseAction : public GameNode
	{
	public:
		BaseAction(float duration);
		virtual void update(float)	= 0;

		// If true, the node running the action will be notified via the
		// GameNode::actionCompleted(BaseAction*) overrideable method.
		bool			notifyOnCompletion;

		// By default, the parent of the action is notified.
		// You may override this by setting a callback-node yourself.
		GameNode		*notificationCallback;

	protected:
		friend class ActionQueueInfinite;
		friend class ActionQueueRepeat;
		friend class ActionQueue;
		friend class GameNode;

		virtual void activate()		= 0;
		void cleanup();

		bool			done;

		bool			inQueue;
		ActionQueue		*queue;

		float			dur;
		float			timer;

		// Used by ActionQueueRepeat and ActionQueueInfinite, as the
		// dur and timer-variables are adjusted for preceeding actions exceeding
		// their original timer
		float			initialDur;
	};


	/* Action class */
	/* Can be run on any GameNode */
	/* Implemented Actions:
			- MoveToAction			Moves the node to a specific position
			- MoveByAction			Moves the node relatively by a vector
			- RotateByAction		Rotates the node a certain amount
			- DelayAction			Wait for X seconds						*/
	class Action : public BaseAction
	{ 
	public:
		Action(float duration) : BaseAction(duration){}
	protected:
		friend class GameNode;
		virtual void update(float)	{}
		virtual void activate();
	};

	/*	
		###########################################
		#####          NODE ACTIONS		      #####
		###########################################
	*/
	/* Move To Action */
	class MoveToAction : public Action
	{
	public:
		MoveToAction(Vec2 destination, float duration);
		void update(float dt);

	protected:
		void activate();

		Vec2	start;
		Vec2	dest;
	};

	/* Move By Action */
	class MoveByAction : public Action
	{
	public:
		MoveByAction(Vec2 relative, float duration);
		void update(float dt);

	protected:
		void activate();

		Vec2  rel;
	};

	/* Rotate By Action */
	class RotateByAction : public Action
	{
	public:
		RotateByAction(float angle, float duration);
		void update(float dt);

	protected:
		void activate();

		float	total;
		float	remainding;
		float	dir;
	};

	/* Delay Action */
	class DelayAction : public Action
	{
	public:
		DelayAction(float duration);
		void update(float dt);

	protected:
		void activate();
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
	class SpriteAction : public BaseAction
	{ 
	public:
		SpriteAction(float duration) : BaseAction(duration){}
	protected:
		friend class Sprite;
		virtual void update(float)	{}
		virtual void activate();
	};

	/* Tint Action */
	class TintAction : public SpriteAction
	{
	public:
		TintAction(Color fadeTo, float duration);
		void update(float dt);

	protected:
		void activate();

		Color	source;		// Initial color of parent sprite
		Color	dest;		// The color to which we're tinting
	};

	/* Scale To Action */
	class ScaleToAction : public SpriteAction
	{
	public:
		ScaleToAction(Vec2 factor, float duration);
		void update(float);

	protected:
		void activate();

		Vec2	source;		// Initial scale of the sprite
		Vec2	dest;		// The scale destination
	};

	/* Scale By Action */
	class ScaleByAction : public SpriteAction
	{
	public:
		ScaleByAction(Vec2 factor, float duration);
		void update(float);

	protected:
		void activate();

		Vec2	remainding;	
	};


	/*	
		###########################################
		#####          ACTION QUEUES	 	  #####
		###########################################
	*/
	/* Run several actions in succession 
	   There's a limit of 32 actions.	*/
	class ActionQueue : public Action
	{
	public:
		ActionQueue(int numActions, BaseAction *action1, ...);
		~ActionQueue();

		void update(float);

	protected:
		friend class BaseAction;
		friend class GameNode;
		friend class Sprite;

		// Declared for subclassing purposes only
		ActionQueue() : Action(0.f) {}

		void activate();
		virtual void activateNext();

		std::deque<BaseAction*>		actions;
		BaseAction					*curAct;
	};

	/* ActionQueueRepeat */
	/* Run several actions in succession, repeated N times.
	   The bool 'infinite' can be flagged to true to repeat the action
	   sequence indefinitely. Also note that the uint 'remaindingLoops' is public,
	   so the amount of loops to run can be altered. As with all actions, the
	   ActionQueueRepeat and all it's actions are deleted upon completion.		*/
	class ActionQueueRepeat : public ActionQueue
	{
	public:
		ActionQueueRepeat(unsigned int repeatNum, int numActions, BaseAction *action1, ...);
		~ActionQueueRepeat();

		unsigned int	remaindingLoops;	// Initialized at n, decremented for each loop
		bool			infinite;

	private:
		void activateNext();

		int				actionIdx;			// The index of the running action
	};
}

#pragma once

#include "PimGameNode.h"
#include <queue>

namespace Pim {
	class ActionQueue;
	class ActionQueueRepeat;

	
	/**
	 @class 	BaseAction
	 @brief		Virtual base class for all Action-classes.
	 @details 	All derivatives of BaseAction works as such:
	 			The action must be passed to the @e RunAction(..)  method on a
				GameNode (or any subclass). The node then calls @e action->Activate(),
				which initializes the action. The action receives @e Update(float) calls
	 			until it's criteria of completion is met, at which point it is removed
	 			from the parent and deleted.
	 */
	class BaseAction : public GameNode {
	protected:
		friend class ActionQueueInfinite;
		friend class ActionQueueRepeat;
		friend class ActionQueue;
		friend class GameNode;

	public:
		// If true, the node running the action will be notified via the
		// virtual GameNode::ActionCompleted(BaseAction*) method.
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

	
	/**
	 @class 	Action
	 @brief 	Can be run on any GameNode.
	 @details	It is a criteria for all Action-derivatives that they do @b not
	 			expect the parent (the node on which they're run) to have any 
				attributes not listed in the GameNode class.
	 
	 			Implemented Actions:
	 				- MoveToAction			Moves the node to a specific position
					- MoveByAction			Moves the node relatively by a vector
				 	- RotateByAction		Rotates the node a certain amount
	 				- DelayAction			Wait for X seconds
	 */
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
	
	/**
	 @class 	MoveToAction
	 @brief 	Moves a node to a specific position over a certain amount of time.
	 @details 	The position to which the object is moved is @e absolute, and
	 			modifying the node's position while this action is running will
	 			cause some nasty behaviour.
	 */
	class MoveToAction : public Action {
	public:
								MoveToAction(Vec2 destination, float duration);
		void					Update(float dt);

	protected:
		Vec2					start;
		Vec2					dest;

		void					Activate();
	};

	
	/**
	 @class 	MoveByAction
	 @brief 	Moves a node relatively over a certain amount of time.
	 @details 	The node's position is linearly incremented over a certain amount
	 			of time until the MoveByAction has moved the object far enough.
	 */
	class MoveByAction : public Action {
	public:
								MoveByAction(Vec2 relative, float duration);
		void					Update(float dt);

	protected:
		Vec2					rel;

		void					Activate();
	};

	
	/**
	 @class 	RotateByAction
	 @brief 	The object is rotated a certain amount of degrees in a certain 
	 			amount of time.
	 */
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

	
	/**
	 @class 	DelayAction
	 @brief 	Litteraly does nothing. Best used in ActionQueue's as a filler.
	 */
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
	/**
	 @class 	SpriteAction
	 @brief 	Defines the base for actions that may be run on Sprites.
	@details 	Implemented SpriteActions:
	 				- TintAction			Tints the sprite to a certain color over time
	 				- ScaleToAction			Scales the sprite to a provided value
	 				- ScaleByAction			Scales the sprite relatively to the provided factors
	 */
	class SpriteAction : public BaseAction {
	protected:
		friend class Sprite;

	public:
								SpriteAction(float duration) : BaseAction(duration) {}

	protected:
		virtual void			Update(float) {}
		virtual void			Activate();
	};

	
	/**
	 @class 	TintAction
	 @brief		Tints the Sprite to a certain color value.
	 */
	class TintAction : public SpriteAction {
	public:
								TintAction(Color fadeTo, float duration);
		void					Update(float dt);

	protected:
		Color					source;		// Initial color of parent sprite
		Color					dest;		// The color to which we're tinting

		void					Activate();
	};

	
	/**
	 @class 	ScaleToAction
	 @brief 	Scales the Sprite to a certain size.
	 */
	class ScaleToAction : public SpriteAction {
	public:
								ScaleToAction(Vec2 factor, float duration);
		void					Update(float);

	protected:
		Vec2					source;		// Initial scale of the sprite
		Vec2					dest;		// The scale destination

		void					Activate();
	};

	/**
	 @class 	ScaleByAction
	 @brief 	Scales the Sprite relatively.
	 */
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
	
	/**
	 @class 	ActionQueue
	 @brief 	Runs up to 32 actions in succession.
	 @details	Example usage:
	 @code
	 			Action *a1 = ...;
	 			Action *a2 = ...;
	 			ActionQueue *aq = new ActionQueue(2, a1, a2);
	 			RunActionQueue(aq);
	 @endcode
	 */
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
	   		*/
	
	/**
	 @class 	ActionQueueRepeat
	 @brief 	Runs several actions in succession, repeated N times. 
	 @details 	The attribute @e infinite can be flagged to true to repeat the action
	 			sequence indefinitely. Also note that the attribute @e remaindingLoops 
	 			is public, so the amount of loops to run can be altered. As with all 
	 			actions, the ActionQueueRepeat and all it's actions are deleted upon completion.
	 */
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

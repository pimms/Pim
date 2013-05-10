#pragma once
/*
	GameNode is the base node used in pim.

	GameNode in itself is not renderable. Use Pim::Sprite for images. GameNodes
	can however draw natively using OpenGL by itself.

	GameNodes can be created at will. However, for a GameNode (sprite) to be rendered, it
	MUST be added to a parent (another game node). Most nodes will be attached
	directly to the Layer.

	A node can only be attached to one parent at a time, and an exception will be
	thrown if a node with a parent is attempted added to a node.
*/

#include "PimInternal.h"
#include "PimVec2.h"
#include "PimConsoleReader.h"

namespace Pim {
	/**
	 @class 	GameNode
	 
	 @brief 	The base of all nodes in the Pim hierarchial tree.
	 
	 @details	GameNode is the base class used by nearly all classes in Pim.
	 
				A GameNode can (and should be) added to a parent, also of type
	 			GameNode. A node can only have one parent at a time, but can have
	 			an infinite amount of children attached to it.
	 
				A GameNode object in itself is not drawable, and should only be
	 			used in it's raw form for relative positioning of drawable child
	 			nodes.
	 
	 			Notable derivatives of GameNode includes:
	 				- Layer
	 				- Sprite
	 				- Label
	 */
	
	
	class LightingSystem;
	class PolygonShape;
	class Vec2;
	class Input;
	class MouseEvent;
	class KeyEvent;
	class ControllerEvent;
	class Layer;
	class Scene;
	class ActionQueue;
	class BaseAction;
	class Action;

	class GameNode : public ConsoleListener {
	protected:
		friend class GameControl;
		friend class LightingSystem;
		friend class Scene;
		friend class Layer;

	public:
		float				rotation;
		Vec2				position;
		vector<GameNode*>	children;
		string				identifier;
		void*				userData;

							GameNode();
		virtual				~GameNode();
		virtual void		Update(float dt)							{}
		virtual void		OnMouseEvent(MouseEvent &evt)				{}
		virtual void		OnKeyEvent(KeyEvent &evt)					{}
		virtual void		OnControllerEvent(ControllerEvent &evt)		{}
		virtual void		OnActionCompleted(BaseAction *act)			{}
		virtual void		OnParentChange(GameNode *newParent)			{}
		virtual void		OnChildRemove(GameNode *oldChild)			{}
		GameNode*			GetParent() const;	
		virtual Scene*		GetParentScene() const;
		virtual Layer*		GetParentLayer();
		virtual void		AddChild(GameNode *ch);
		virtual void		RemoveChild(GameNode *ch, bool cleanup=true);
		virtual void		RemoveAllChildren(bool cleanup=true);
		virtual void 		RemoveFromParent(bool cleanup=true);
		int 				ChildCount();
		void				ListenInput();
		void				UnlistenInput();
		void				ListenKeys();
		void				UnlistenKeys();
		void				ListenMouse();
		void				UnlistenMouse();
		void				ListenController();
		void				UnlistenController();
		void				ListenFrame();
		void				UnlistenFrame();
		virtual Vec2		GetWorldPosition() const;
		virtual Vec2		GetLayerPosition() const;
		virtual float		GetWorldRotation() const;
		virtual float		GetLayerRotation() const;
		virtual void		Draw();
		virtual void		BatchDraw();
		virtual void		SetZOrder(const int z);
		int					GetZOrder() const;
		void				OrderChildren();
		void				RunAction(Action *a);
		void				RunActionQueue(ActionQueue *queue);
		void				RemoveAllActions();
		void				SetShadowShape(Vec2 vertices[], const int vertexCount);
		void				SetShadowShapeDebugDraw(const bool flag);
		PolygonShape*		GetShadowShape() const;
		virtual void		ReloadTextures();
	protected:
		bool				dirtyZOrder;
		PolygonShape		*shadowShape;
		bool				dbgShadowShape;
		GameNode			*parent;
		int					zOrder;
		bool				willDelete;

	private:
		void				PrepareDeletion();
	};
	
	/**
	 @fn		GameNode::Update
	 @brief 	Called each frame on frame listening nodes.
	 @details 	A node can schedule itself to receive Update-calls each frame
	 			by calling @e ListenFrame() at any point in it's existance.
	 
	 			A node can unschedule Update-calls by calling @e UnlistenFrame().
	 */
	
	/**
	 @fn 		GameNode::OnMouseEvent
	 @brief 	Called when the mouse is moved / pressed on mouse listeners.
	 @details 	A node can schedule itself to receive OnMouseEvent-calls each
	 			time the mouse is either moved or pressed by calling @e ListenMouse()
	 			or @e ListenInput() at any point in it's existance.
	 
	 			A node can unschedule OnMouseEvent-calls by calling @e UnlistenMouse
	 			or @e UnlistenInput().
	 */
	
	/**
	 @fn 		GameNode::OnKeyEvent
	 @brief 	Called when a key is pressed or released on keyboard listeners.
	 @details 	A node can schedule itself to receive OnKeyEvent-calls every time
	 			a keyboard button is pressed or released by calling @e ListenKeys()
	 			or @e ListenInput(). 
	 
	 			A node can unschedule OnKeyEvent-calls by calling @e UnlistenKeys()
	 			or @e UnlistenInput().
	 */
	
	/**
	 @fn 		GameNode::OnControllerEvent
	 @brief 	Called when a button or stick is moved on a connected 360-controller.
	 @details 	Listen to controller events by calling @e ListenController().
	 
	 			Unschedule the node by calling @e UnlistenController().
	 
	 			Note that currently only the 360-pad is tested and supported.
	 */
	
	/**
	 @fn 		GameNode::OnActionCompleted
	 @brief 	Called when an action has completed if explicitly stated.
	 
	 @details 	When running an Action, you may specify that you want a callback
	 			when it has completed. 
	 
	 			Example usage:
	 @code
				Pim::Action *act = new Pim::DelayAction(1.5f);
	 			act->notifyOnCompletion = true;
	 			RunAction(act);
	 
	 			// In your class:
	 			void MySuperNode::OnActionCompleted(Pim::BaseAction *action) {
	 				std::cout << "One and a half second has passed! WOW!";
	 			}
	 @endcode
	 */

	/**
	 @fn		GameNode::OnParentChange
	 @brief		Called on a node when it has been added or removed from another node.
				The parameter 'newParent' CAN BE NULL!
	 @details	This method is not responsible for anything. The 'parent' member
				variable has already been set when this method is called.
	*/
	
	/**
	 @fn 		GameNode::GetParent
	 @brief 	Returns the parent of this node.
	 */
	
	/**
	 @fn 		GameNode::GetParentScene
	 @brief 	Returns the parent scene of this object.
	 @details 	As there is only one active scene active at any given point, you
	 			may also use GameControl::GetScene() to get a pointer to your scene.
	 */
	
	/**
	 @fn 		GameNode::GetParentLayer
	 @brief 	Returns the @e first layer above this node in the hierarchy. If
	 			this method is called on a Layer, @e this is returned.
	 */
	
	/**
	 @fn 		GameNode::AddChild
	 @brief 	Adds a node as a child to this node.
	 @details 	The added node is added to the @e children vector, and will be
	 			cleaned up automatically unless you manually remove it.
	 */
	
	/**
	 @fn 		GameNode::RemoveChild
	 @brief 	Remove a child node from this object.
	 
	 @details	If the node passed is a child of this node, it is removed from the
				hierarchy.
	 @param		cleanup
	 			If yes, the object is added to GameControl's delete-queue.
	 			It is then deleted at a safe point in the future, but it will
	 			no longer be treated as an active member of the hierarchy.
	 */
	
	/**
	 @fn 		GameNode::RemoveAllChildren
	 @brief 	Removes all children from this node.
	 @param 	cleanup
	 			If yes, all relevant nodes are added to the GameControl's delete-queue.
	 			They are not deleted instantly, but at a safe point in the 
	 			game-loop where no access violations are prone.
	 */
	
	/**
	 @fn 		GameNode::ListenInput
	 @brief 	Receive @e OnMouseEvent and @e OnKeyEvent calls.
	 */
	
	/**
	 @fn 		GameNode::UnlistenInput
	 @brief 	Stop recieving @e OnMouseEvent and @e OnKeyEvent calls.
	 */
	
	/**
	 @fn 		GameNode::ListenKeys
	 @brief 	Receive @e OnKeyEvent calls.
	 */
	
	/**
	 @fn 		GameNode::UnlistenKeys
	 @brief 	Stop receiving @e OnKeyEvent calls.
	 */
	
	/**
	 @fn 		GameNode::ListenMouse
	 @brief 	Receive @e OnMouseEvent calls.
	 */
	
	/**
	 @fn 		GameNode::UnlistenMouse
	 @brief 	Stop recieving @e OnMouseEvent calls.
	 */
	
	/**
	 @fn 		GameNode::ListenController
	 @brief 	Receive @e OnControllerEvent calls.
	 */
	
	/**
	 @fn 		GameNode::UnlistenController
	 @brief 	Stop receiving @e OnControllerEvent calls.
	 */
	
	/**
	 @fn		GameNode::ListenFrame
	 @brief 	Receive @e Update calls each frame.
	 */
	
	/**
	 @fn 		GameNode::UnlistenFrame
	 @brief 	Stop receiving @e Update calls each frame.
	 */
	
	/**
	 @fn 		GameNode::GetWorldPosition
	 @brief 	Returns the position of this node relative to the @e absolute origin.
	 @details 	Sums the positions of all nodes upwards in the hierarchy.
	 */
	
	/**
	 @fn 		GameNode::GetWorldRotation
	 @brief 	Returns the absolute rotation of this node.
	 @details 	Sums the rotations of all nodes upwards in the hierarchy.
	 */
	
	/**
	 @fn 		GameNode::GetLayerPosition
	 @brief 	Returns the position of this node relative to the parent layer.
	 */
	
	/**
	 @fn 		GameNode::Draw
	 @brief 	Calls @e Draw() on this node's children.
	 @details 	GameNodes does not draw anything. However, their children are 
	 			being drawn relative to them. 
	 */
	
	/**
	 @fn 		GameNode::BatchDraw
	 @brief 	Calls @e BatchDraw() on this node's children.
	 @details 	BatchDraw() is used when a Sprite is added to a SpriteBatchNode.
	 			The batch node loads the texture once, and all Sprite's added as
	 			children to it uses this texture.
	 */
	
	/**
	 @fn 		GameNode::SetZOrder
	 @brief 	The Z-order determines in which order they sprites are drawn. 
	 			The lowest Z-ordered nodes are drawn first.
	 */
	
	/**
	 @fn 		GameNode::OrderChildren
	 @brief 	Orders children based on their Z-order values.
	 */
	
	/**
	 @fn 		GameNode::RunAction
	 @brief 	Run an action on a GameNode. 
	 @param 	a
	 			Absolutely must @b NOT be of SpriteAction-type. Exceptions will
	 			be trown if you attemt to run a SpriteAction on a GameNode.
	 */
	
	/**
	 @fn 		GameNode::RunActionQueue
	 @brief 	Runs several actions in succession on a GameNode.
	 @param 	queue
	 			Must consist solely of Action-derived actions, none of which can
	 			be of type SpriteAction.
	 */
	
	/**
	 @fn 		GameNode::SetShadowShape
	 @brief 	Set a shadow shape for this GameNode based on @e n vertices.
	 @param 	vertices
	 			An array of vertices that must be @b convex, and the vertices
	 			must be wound @e counter-clockwise.
	 
	 			Note that this method must be called manually (by you) before
	 			the node is added as a shadow caster to a Layer.
	 */
	
	/**
	 @fn 		GameNode::SetShadowShapeDebugDraw
	 @brief 	Toggle debug drawing of the shadow shape.
	 */
	
	/**
	 @fn 		GameNode::ReloadTextures
	 @brief 	Calls @e ReloadTextures() on all children.
	 @details 	Upon window resize, the OpenGL context is destroyed by SDL. As such,
	 			all textures, shaders and FBO's must be re-created.
	 */
	
	/**
	 @fn 		GameNode::PrepareDeletion
	 @brief 	Prepares the node for it's inevitable demise.
	 @details 	Called as the GameNode is added to GameControl's delete-queue.
				The GameNode tidily cleans up after itself and removes all it's
	 			children and associated callbacks.
	 */
}
#pragma once
/*
	GameNode is the base node used in pim.

	GameNode in itself is not renderable. Use Pim::Sprite for images. GameNodes
	can however draw natively using OpenGL by itself.

	GameNodes can be created at will. However, for a GameNode to be rendered, it
	MUST be added to a parent (another game node). Most nodes will be attached
	directly to the Layer.

	A node can only be attached to one parent at a time, and an exception will be
	thrown if a node with a parent is attempted added to a node.

	Nodes are deleted automatically if "true" is passed to removeChild(GameNode,bool),
	or by passing "true" to removeAllChildren(bool), or upon the parent's deletion.
*/

#include "PimInternal.h"

#include "PimVec2.h"

#include "PimConsoleReader.h"

namespace Pim {
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
		virtual void		OnMouseEvent(MouseEvent &)					{}
		virtual void		OnKeyEvent(KeyEvent &)						{}
		virtual void		OnControllerEvent(ControllerEvent&)			{}
		virtual void		OnActionCompleted(BaseAction*)				{}
		GameNode*			GetParent() const;	
		virtual Scene*		GetParentScene() const;
		virtual Layer*		GetParentLayer();
		virtual void		AddChild(GameNode *ch);
		virtual void		RemoveChild(GameNode *ch, const bool cleanup=true);
		virtual void		RemoveAllChildren(const bool cleanup=true);
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
		virtual float		GetWorldRotation() const;
		virtual Vec2		GetLayerPosition() const;
		virtual void		Draw();
		virtual void		BatchDraw();
		virtual void		SetZOrder(const int z);
		int					GetZOrder() const;
		void				OrderChildren();
		void				RunAction(Action *a);
		void				RunActionQueue(ActionQueue *queue);
		void				SetShadowShape(Vec2 vertices[], const int vertexCount);
		void				SetShadowShapeDebugDraw(const bool flag);
		PolygonShape*		GetShadowShape() const;

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
}
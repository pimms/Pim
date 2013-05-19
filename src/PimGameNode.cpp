#include "PimInternal.h"

#include "PimVec2.h"
#include "PimGameNode.h"
#include "PimAssert.h"
#include "PimInput.h"
#include "PimGameControl.h"
#include "PimLayer.h"
#include "PimPolygonShape.h"
#include "PimScene.h"
#include "PimLightingSystem.h"
#include "PimAction.h"

#include <iostream>

namespace Pim {
	/*
	=====================
	GameNode::GameNode
	=====================
	*/
	GameNode::GameNode() {
		parent					= NULL;
		rotation				= 0.f;
		zOrder					= 0;
		dirtyZOrder				= false;
		willDelete				= false;
		shadowShape				= NULL;
		dbgShadowShape			= false;
		userData				= NULL;
	}

	/*
	=====================
	GameNode::~GameNode
	=====================
	*/
	GameNode::~GameNode() {
		if (!willDelete) {
			PrepareDeletion();
		}

		RemoveAllChildren();

		UnlistenFrame();
		UnlistenInput();
		UnlistenController();
	}

	/*
	=====================
	GameNode::PrepareDeletion
	=====================
	*/
	void GameNode::PrepareDeletion() {
		if (GetParentLayer()) {
			GetParentLayer()->RemoveLight(this);
			GetParentLayer()->RemoveShadowCaster(this);
		}

		if (shadowShape) {
			delete shadowShape;
		}

		RemoveAllChildren();

		UnlistenInput();
		UnlistenController();

		parent = NULL;
		willDelete = true;
	}

	/*
	=====================
	GameNode::AddChild
	=====================
	*/
	void GameNode::AddChild(GameNode *ch) {
		PimAssert(!ch->GetParent(), "Node already has a parent");

		Layer *layer = dynamic_cast<Layer*>(ch);
		if (layer) {
			layer->LoadResources();
		}

		ch->parent = this;
		children.push_back(ch);

		ch->OnParentChange(this);

		dirtyZOrder = true;
	}

	/*
	=====================
	GameNode::RemoveChild
	=====================
	*/
	void GameNode::RemoveChild(GameNode *ch, bool cleanup) {
		for (unsigned int i=0; i<children.size(); i++) {
			if (children[i] == ch) {
				children.erase(children.begin() + i);

				if (cleanup) {
					GameControl::GetSingleton()->AddNodeToDelete(ch);
				}
				
				ch->parent = NULL;
				ch->OnParentChange(NULL);
				OnChildRemove(ch);
			}
		}
	}

	/*
	=====================
	GameNode::RemoveAllChildren
	=====================
	*/
	void GameNode::RemoveAllChildren(bool cleanup) {
		// Delete all if required
		for (unsigned i=0; i<children.size(); i++) {
			if (cleanup) {
				GameControl::GetSingleton()->AddNodeToDelete(children[i]);
			}

			children[i]->parent = NULL;
			children[i]->OnParentChange(NULL);
			OnChildRemove(children[i]);
		}

		// THEN clear the array
		children.clear();
	}
	
	/*
	=====================
	GameNode::RemoveFromParent
	=====================
	*/
	void GameNode::RemoveFromParent(bool cleanup) {
		if (parent) {
			parent->RemoveChild(this, cleanup);
		} else {
			PimWarning("Cannot remove orphan from parent.", "Logical error");
		}
	}

	/*
	=====================
	GameNode::ChildCount
	
	Sums up the child count and ChildCount() of all children.
	=====================
	*/
	int GameNode::ChildCount() {
		int count = (int)children.size();
		
		for (unsigned i=0; i<children.size(); i++) {
			count += children[i]->ChildCount();
		}
		
		return count;
	}
	
	/*
	=====================
	GameNode::GetParent
	===================== 
	*/
	GameNode* GameNode::GetParent() const {
		return parent;
	}

	/*
	=====================
	GameNode::GetParentScene
	=====================
	*/
	Scene* GameNode::GetParentScene() const {
		if (parent) {
			return parent->GetParentScene();
		}
		return NULL;
	}

	/*
	=====================
	GameNode::GetParentLayer
	=====================
	*/
	Layer* GameNode::GetParentLayer() {
		if (parent) {
			return parent->GetParentLayer();
		}
		return NULL;
	}

	/*
	=====================
	GameNode::ListenInput
	=====================
	*/
	void GameNode::ListenInput() {
		GameControl::GetSingleton()->AddKeyListener(this);
		GameControl::GetSingleton()->AddMouseListener(this);
	}

	/*
	=====================
	GameNode::UnlistenInput
	=====================
	*/
	void GameNode::UnlistenInput() {
		GameControl::GetSingleton()->RemoveKeyListener(this);
		GameControl::GetSingleton()->RemoveMouseListener(this);
	}

	/*
	=====================
	GameNode::ListenKeys
	=====================
	*/
	void GameNode::ListenKeys() {
		GameControl::GetSingleton()->AddKeyListener(this);
	}

	/*
	=====================
	GameNode::UnlistenKeys
	=====================
	*/
	void GameNode::UnlistenKeys() {
		GameControl::GetSingleton()->RemoveKeyListener(this);
	}

	/*
	=====================
	GameNode::ListenMouse
	=====================
	*/
	void GameNode::ListenMouse() {
		GameControl::GetSingleton()->AddMouseListener(this);
	}

	/*
	=====================
	GameNode::UnlistenMouse
	=====================
	*/
	void GameNode::UnlistenMouse() {
		GameControl::GetSingleton()->RemoveMouseListener(this);
	}

	/*
	=====================
	GameNode::ListenController
	=====================
	*/
	void GameNode::ListenController() {
		GameControl::GetSingleton()->AddControlListener(this);
	}

	/*
	=====================
	GameNode::UnlistenController
	=====================
	*/
	void GameNode::UnlistenController() {
		GameControl::GetSingleton()->RemoveControlListener(this);
	}

	/*
	=====================
	GameNode::ListenFrame
	=====================
	*/
	void GameNode::ListenFrame() {
		GameControl::GetSingleton()->AddFrameListener(this);
	}

	/*
	=====================
	GameNode::UnlistenFrame
	=====================
	*/
	void GameNode::UnlistenFrame() {
		GameControl::GetSingleton()->RemoveFrameListener(this);
	}

	/*
	=====================
	GameNode::GetWorldPosition
	=====================
	*/
	Vec2 GameNode::GetWorldPosition() const {
		if (parent) {
			return position.RotateAroundPoint(Vec2(0.f, 0.f), parent->GetWorldRotation()) +
					parent->GetWorldPosition();
		}
		return position;
	}

	/*
	=====================
	GameNode::GetLayerPosition
	=====================
	*/
	Vec2 GameNode::GetLayerPosition() const {
		if (parent) {
			return position.RotateAroundPoint(Vec2(0.f, 0.f), parent->GetLayerRotation()) +
					parent->GetLayerPosition();
		}
		return position;
	}

	/*
	=====================
	GameNode::GetWorldRotation
	=====================
	*/
	float GameNode::GetWorldRotation() const {
		if (parent) {
			return rotation + parent->GetWorldRotation();
		}
		return rotation;
	}

	/*
	==================
	GameNode::GetLayerRotation
	==================
	*/
	float GameNode::GetLayerRotation() const {
		if (parent) {
			return rotation + parent->GetLayerRotation();
		}
		return rotation;
	}

	/*
	=====================
	GameNode::Draw
	=====================
	*/
	void GameNode::Draw() {
		glPushMatrix();
		Vec2 fac = GameControl::GetSingleton()->GetCoordinateFactor();

		glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		if (shadowShape && dbgShadowShape) {
			glPushMatrix();

			fac = GameControl::GetSingleton()->GetWindowScale();
			glScalef(fac.x, fac.y, 1.f);

			shadowShape->DebugDraw();

			glPopMatrix();
		}

		OrderChildren();

		for (unsigned int i=0; i<children.size(); i++) {
			children[i]->Draw();
		}

		glPopMatrix();
	}

	/*
	=====================
	GameNode::BatchDraw
	=====================
	*/
	void GameNode::BatchDraw() {
		glPushMatrix();
		Vec2 fac = GameControl::GetSingleton()->GetCoordinateFactor();

		glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		if (shadowShape && dbgShadowShape) {
			glPushMatrix();

			fac = GameControl::GetSingleton()->GetWindowScale();
			glScalef(fac.x, fac.y, 1.f);

			shadowShape->DebugDraw();

			glPopMatrix();
		}

		OrderChildren();

		for (unsigned int i=0; i<children.size(); i++) {
			// This is the only difference from GameNode::draw(): batchDraw is called
			// on the node's children.
			children[i]->BatchDraw();
		}

		glPopMatrix();
	}

	/*
	=====================
	GameNode::SetZOrder
	=====================
	*/
	void GameNode::SetZOrder(const int z) {
		if (parent) {
			parent->dirtyZOrder = true;
		}

		zOrder = z;
	}

	/*
	=====================
	GameNode::GetZOrder
	=====================
	*/
	int GameNode::GetZOrder() const {
		return zOrder;
	}

	/*
	=====================
	GameNode::OrderChildren
	=====================
	*/
	void GameNode::OrderChildren() {
		if (!dirtyZOrder || children.size() <= 1) {
			return;
		}

		// Insertion sorting - the children should be somewhat sorted already.
		for (int j=1; j<children.size(); j++) {
			GameNode *key = children[j];
			int i = j - 1;

			while (i >= 0 && children[i]->zOrder > key->zOrder) {
				children[i+1] = children[i];
				i--;
			}
			children[i+1] = key;
		}

		dirtyZOrder = false;
	}

	/*
	=====================
	GameNode::RunAction
	=====================
	*/
	void GameNode::RunAction(Action *a) {
		AddChild(a);
		a->Activate();
	}

	/*
	=====================
	GameNode::RunActionQueue
	=====================
	*/
	void GameNode::RunActionQueue(ActionQueue *aq) {
		AddChild(aq);
		aq->Activate();
	}

	/*
	==================
	GameNode::RemoveAllActions
	==================
	*/
	void GameNode::RemoveAllActions() {
		for (int i=0; i<children.size(); i++) {
			BaseAction *act = NULL;

			act = dynamic_cast<BaseAction*>(children[i]);
			if (act) {
				RemoveChild(act);
				i--;
			}
		}
	}

	/*
	=====================
	GameNode::SetShadowShape
	=====================
	*/
	void GameNode::SetShadowShape(Vec2 vertices[], const int vertexCount) {
		if (shadowShape) {
			delete shadowShape;
		}

		shadowShape = new PolygonShape(vertices, vertexCount, this);
	}

	/*
	=====================
	GameNode::SetShadowShapeDebugDraw
	=====================
	*/
	void GameNode::SetShadowShapeDebugDraw(const bool flag) {
		dbgShadowShape = flag;
	}

	/*
	=====================
	GameNode::GetShadowShape
	=====================
	*/
	PolygonShape* GameNode::GetShadowShape() const {
		return shadowShape;
	}

	/*
	==================
	GameNode::ReloadTextures
	==================
	*/
	void GameNode::ReloadTextures() {
		for (unsigned i=0; i<children.size(); i++) {
			children[i]->ReloadTextures();
		}
	}
}

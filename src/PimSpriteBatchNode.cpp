#include "PimInternal.h"

#include "PimSpriteBatchNode.h"
#include "PimGameNode.h"

#include <functional>

namespace Pim {
	/*
	=====================
	SpriteBatchNode::SpriteBatchNode
	=====================
	*/
	SpriteBatchNode::SpriteBatchNode(string file)
		: Sprite(file) {
		// Using Pim::Sprite's init method
	}

	/*
	=====================
	SpriteBatchNode::SpriteBatchNode
	=====================
	*/
	SpriteBatchNode::SpriteBatchNode()
		: Sprite() {
		// Using Pim::Sprite's default init method
	}

	/*
	=====================
	SpriteBatchNode::~SpriteBatchNode
	=====================
	*/
	SpriteBatchNode::~SpriteBatchNode(void) {
		// Using Pim::Sprite's destruction method
	}

	/*
	=====================
	SpriteBatchNode::AddChild
	=====================
	*/
	void SpriteBatchNode::AddChild(GameNode *ch) {
		GameNode::AddChild(ch);

		if (Sprite *s = dynamic_cast<Sprite*>(ch)) {
			s->UseBatchNode(this);
		}
	}

	/*
	=====================
	SpriteBatchNode::Draw
	=====================
	*/
	void SpriteBatchNode::Draw() {
		BatchDraw();
	}

	/*
	=====================
	SpriteBatchNode::BatchDraw
	=====================
	*/
	void SpriteBatchNode::BatchDraw() {
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texID);

		OrderChildren();

		for (unsigned int i=0; i<children.size(); i++) {
			children[i]->BatchDraw();
		}

		glPopMatrix();
	}
}
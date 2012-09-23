// TestGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Pim.h"
#include "GameLayer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Pim::GameControl *gc = new Pim::GameControl;

	Pim::WinStyle::CreationData cd;
	
	cd.forcedAspectRatio = true;
	cd.aspectRatio = 1920.f / 1080.f;
	
	cd.forcedCoordinateSystem = true;
	cd.coordinateSystem = Pim::Vec2(192.f, 108.f);

	cd.forcedRenderResolution = true;
	cd.renderResolution = Pim::Vec2(192.f, 108.f);

	cd.defaultResolution = Pim::Vec2(192.f*6, 108.f*6);

	cd.width = 192 * 6;
	cd.height = 108 * 6;

	cd.winTitle = "TestGame";

	//cd.winStyle = Pim::WinStyle::FULLSCREEN;

	gc->go(new GameLayer, cd);

	return 0;
}


Pim::Vec2 toPim(const b2Vec2 v)
{
	return Pim::Vec2(v.x, v.y);
}
b2Vec2 tob2(const Pim::Vec2 v)
{
	return b2Vec2(v.x, v.y);
}

void debugDrawShape(b2Body *body, b2Vec2 offset)
{
	b2Vec2 pos = body->GetPosition() + offset;
	float rot = body->GetAngle();
	b2PolygonShape *shape = (b2PolygonShape*)body->GetFixtureList()->GetShape();

	Pim::Vec2 fac = Pim::GameControl::getSingleton()->windowScale();

	glPushMatrix();
	glScalef(fac.x, fac.y, 1.f);
	glTranslatef(pos.x, pos.y, 0.f);

	glDisable(GL_TEXTURE_2D);

	glColor4ub(255,0,0,100);
	glBegin(GL_TRIANGLE_FAN);

	for (int i=0; i<shape->GetVertexCount(); i++)
	{
		b2Vec2 v = shape->GetVertex(i);
		glVertex2f(v.x, v.y);
	}

	glEnd();

	glEnable(GL_TEXTURE_2D);

	glPopMatrix();
}

#pragma once

#include "pim.h"

using namespace Pim;

class NormalMap : public Sprite {
public:
	vector<Vec2>	lpos;
	
					NormalMap(string spriteFile, string normalFile);
					NormalMap();
	void 			UpdateUniforms();
	void 			Draw();
	
private:
	GLuint 			normalTex;
	
	void 			CreateShader();
};
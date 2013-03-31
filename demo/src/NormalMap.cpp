//
//  NormalMap.cpp
//  Pim_osxtemplate
//
//  Created by Joakim Stien on 3/30/13.
//  Copyright (c) 2013 Joakim Stien. All rights reserved.
//

#include "NormalMap.h"

NormalMap::NormalMap(string spriteFile, string normalFile) : Sprite(spriteFile) {
	CreateShader();
	
	LoadSprite(normalFile);
	
	normalTex = texID;
	texID = 0;
	
	LoadSprite(spriteFile);
	
	return;
	
	if (!shader) {
		return;
	}
	
	/* Profile how long it takes to set 1 million uniform vec2's 
	 *
	 *		Results:
	 *			MacBook Pro 13" (early 2012) - Intel i5 @ 2.3Ghz
	 *				1.04 - 1.12 (avg ~1.09) seconds
	 */
	clock_t s = clock();
	
	for (unsigned i=0; i<250000; i++) {
		//shader->SetUniform2f("lpos", lpos.x, lpos.y);
		shader->SetUniform2f("dims", rect.width, rect.height);
		shader->SetUniform2f("wpos", GetLayerPosition().x, GetLayerPosition().y);
		shader->SetUniform2f("anchor", anchor.x, anchor.y);
	}
	
	clock_t e = clock();
	
	printf("PROFILE:\nTime to set 1 million uniform 2f:\n");
	printf("\t%f\n\n", float(e-s) / CLOCKS_PER_SEC);
	
	/* Profile how long it takes to bind two textures 1 million times
	 *
	 *		Results:
	 *			MacBook Pro 13" (early 2012) - Intel i5 @ 2.3Ghz
	 *				0.051 seconds (rough average)
	 */
	s = clock();
	
	glEnable(GL_TEXTURE_2D);
	for (unsigned i=0; i<1000000; i++) {
		glBindTexture(GL_TEXTURE_2D, texID);
		glBindTexture(GL_TEXTURE_2D, normalTex);
	}
	
	e = clock();
	printf("PROFILE:\nTime to bind 2 million textures:\n");
	printf("\t%f\n\n", float(e-s) / CLOCKS_PER_SEC);
}

void NormalMap::UpdateUniforms() {
	scale = Vec2(0.5f, 0.5f);
	
	if (shader) {
		shader->SetUniform2f("dims", rect.width*scale.x, rect.height*scale.y);
		shader->SetUniform2f("wpos", GetLayerPosition().x, GetLayerPosition().y);
		shader->SetUniform2f("anchor", anchor.x, anchor.y);
		shader->SetUniform1i("numLights", (int)lpos.size());

		shader->EnableShader();
		
		GLfloat lv[20] = {0.f};
		for (unsigned i=0; i<lpos.size() && i<10; i++) {
			lv[i*2+0] = lpos[i].x;
			lv[i*2+1] = lpos[i].y;
		}
		glUniform2fv(shader->GetUniformLocation("lpos"), 20, lv);
		
		shader->DisableShader();
	}
}

void NormalMap::Draw() {
	UpdateUniforms();
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTex);
	glActiveTexture(GL_TEXTURE0);
	
	Sprite::Draw();
}

void NormalMap::CreateShader() {
	shader = ShaderManager::GetSingleton()->GetShader("_NormalMap_");
	if (!shader) {
		string vert =
			"void main() {"
		 	"	gl_Position = ftransform();"
			"	gl_TexCoord[0] = gl_MultiTexCoord0;"
			"}";
		
		string frag =
			"uniform sampler2D 	tex0;" 		// The sprite texture
			"uniform sampler2D 	tex1;"		// The normal texture
			"uniform vec2 		dims;"		// The dimension of the texture
			"uniform vec2 		wpos;"		// The world position of the sprite
			"uniform vec2 		anchor;"	// The anchor of the Sprite
			"uniform int		numLights;"	// The amount of lights
			"uniform vec2 		lpos[10];"	// The light positions
			"uniform float 		lrad[10];"	// The light radius'
		
			"void main() {"
			"	vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);"
		
				/* Iterate over all the colors */
			" 	for (int i=0; i<numLights; i++) {"
		
					/* Get the normal of the normal-map */
			"		vec4 color = texture2D(tex1, gl_TexCoord[0].xy);"
			"		vec3 norm = normalize((color.xyz * 2.0) + vec3(-1.0));"
		
					/* Find the relative position of the light */
			"		vec2 nlpos = lpos[i] - wpos + (anchor * dims) - gl_TexCoord[0].xy * dims;"
			
					/* Set up a distance-factor */
			"		vec2 diff = nlpos - gl_TexCoord[0].xy;"
			"		float len = length(diff);"
			"		len /=  400.0;"
			//"		len = cos(len);"
			"		if (len > 1.0) len = 1.0;"
			"		if (len < 0.0) len = 0.0;"
			"		len = (1.0 - len);"
		
					/* Get the color of the image texture */
			"		color = texture2D(tex0, gl_TexCoord[0].xy);"
			
					/* Calculate the final color */
			"		float fac = dot(norm, normalize(vec3(nlpos, 100.0)));"
			"		fac *= len;"
			"		color.rgb *= fac;"
			"		finalColor += color;"
			"	}"
			"	gl_FragColor = finalColor;"
			"}";
		
		shader = ShaderManager::GetSingleton()->AddShader(frag, vert, "_NormalMap_");
		if (!shader) {
			printf("Failed to create shader.\n");
		} else {
			SetShader(shader);
			shader->SetUniform1i("tex0", 0);
			shader->SetUniform1i("tex1", 1);
			
			shader->EnableShader();
			
			GLfloat rads[10] = {400.f};

			GLint loc = shader->GetUniformLocation("lrad");
			glUniform1fv(loc, 10, rads);
			GameControl::GetSingleton()->GetRenderWindow()->PrintOpenGLErrors("rads");
			
			shader->DisableShader();
		}
	}
}
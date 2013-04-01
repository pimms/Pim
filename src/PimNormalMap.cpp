#include "PimNormalMap.h"
#include "PimLayer.h"
#include "PimAssert.h"
#include "PimGameControl.h"
#include "PimShaderManager.h"

namespace Pim {
	/*
	==================
	NormalMap::NormalMap
	==================
	*/
	NormalMap::NormalMap(string spriteFile, string normalFile) : Sprite(spriteFile) {
		LoadSprite(normalFile);
	
		normalTex = texID;
		texID = 0;
	
		LoadSprite(spriteFile);
	}

	/*
	==================
	NormalMap::OnParentChange
	==================
	*/
	void NormalMap::OnParentChange(GameNode *newParent) {
		if (newParent) {
			Layer *layer = GetParentLayer();

			if (!layer->GetLightingSystem()) {
				PimWarning(	"NormalMap added to layer with no lighting system!!\n"
							"The lighting system has been created by the NormalMap.", 
							"Error!"
				);

				layer->CreateLightingSystem(
					GameControl::GetSingleton()->GetCreationData().renderResolution
				);
			}

			SetShader(layer->GetLightingSystem()->GetNormalMapShader());
		}
	}

	/*
	==================
	NormalMap::Draw
	==================
	*/
	void NormalMap::Draw() {
		UpdateShaderUniforms();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalTex);
		glActiveTexture(GL_TEXTURE0);
	
		Sprite::Draw();
	}

	/*
	==================
	NormalMap::UpdateShaderUniforms
	==================
	*/
	void NormalMap::UpdateShaderUniforms() {
		if (shader) {
			Vec2 lpos = GetLayerPosition();

			glUseProgram(shader->GetProgram());
		
			glUniform2f(shader->GetUniformLocation("dims"), rect.width*scale.x, rect.height*scale.y);
			glUniform2f(shader->GetUniformLocation("wpos"), lpos.x, lpos.y);
			glUniform2f(shader->GetUniformLocation("anchor"), anchor.x, anchor.y);

			glUseProgram(0);
		}
	}
}
#pragma once

#include "PimInternal.h"
#include "PimSprite.h"

namespace Pim {
	class NormalMap : public Sprite {
	public:
						NormalMap(string spriteFile, string normalFile);
		void			OnParentChange(GameNode *newParent);
		void 			Draw();
		
	protected:
		void			UpdateShaderUniforms();

	private:
		GLuint 			normalTex;
	};
}
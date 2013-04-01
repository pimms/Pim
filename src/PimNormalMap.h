#pragma once

#include "PimInternal.h"
#include "PimSprite.h"

namespace Pim {
	/**
	 @class			NormalMap
	 @brief			A Sprite object using a second texture for normal mapping.
	 @details		This class must be used in conjunction with a LightingSystem
					and lights (see LightingSystem for more details). 

					There is no requirement that the two sprites are in any
					way related, this must be handled by you. 

					See pim_demo/LightLayer.cpp for details on how to use.

					@b Todo:
					Allow usage of sprite sheets (SpriteBatchNode) with both
					the image texture and the normal texture.
	*/

	class NormalMap : public Sprite {
	public:
						NormalMap(string spriteFile, string normalFile);
		
	protected:
		void			OnParentChange(GameNode *newParent);
		void 			Draw();
		void			UpdateShaderUniforms();

	private:
		GLuint 			normalTex;
	};
}
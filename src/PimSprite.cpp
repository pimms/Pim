#include "PimInternal.h"

#include "PimVec2.h"
#include "PimSprite.h"
#include "PimAssert.h"
#include "PimShaderManager.h"
#include "PimPolygonShape.h"
#include "PimGameControl.h"
#include "PimSpriteBatchNode.h"
#include "PimAction.h"

namespace Pim {
	/*
	=====================
	Sprite::Sprite
	=====================
	*/
	Sprite::Sprite(string file) {
		anchor			= Vec2(0.5f, 0.5f);
		scale			= Vec2(1.f, 1.f);
		color			= Color(1.f, 1.f, 1.f, 1.f);
		texID			= -1;
		shader			= NULL;
		hidden			= false;
		_usebatch		= false;

		LoadSprite(file);
	}

	/*
	=====================
	Sprite::Sprite
	=====================
	*/
	Sprite::Sprite() {
		anchor			= Vec2(0.5f, 0.5f);
		scale			= Vec2(1.f, 1.f);
		color			= Color(1.f, 1.f, 1.f, 1.f);
		texID			= -1;
		shader			= NULL;
		hidden			= false;
		_usebatch		= false;
	}

	/*
	=====================
	Sprite::~Sprite
	=====================
	*/
	Sprite::~Sprite() {
		if (!_usebatch) {
			glDeleteTextures(1, &texID);
		}
	}

	/*
	=====================
	Sprite::LoadSprite
	=====================
	*/
	void Sprite::LoadSprite(string file) {
		png_structp		png_ptr;
		png_infop		info_ptr;
		unsigned int	sig_read = 0;
		FILE *fp;

		fp = fopen(file.c_str(), "rb");
		PimAssert(fp != NULL, file.append(": Does not exist!").c_str());

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (!png_ptr) {
			fclose(fp);
			PimAssert(false, "Error: failed instantiating png reading");
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			fclose(fp);
			PimAssert(false, "Error: failed instantioating png info");
		}

		if (setjmp(png_jmpbuf(png_ptr))) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			fclose(fp);
			PimAssert(false, "Error in loading png: something went wrong.");
		}

		// Init complete, init for real
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, sig_read);
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);


		// Get the size
		_tw = png_get_image_width(png_ptr, info_ptr);
		_th = png_get_image_height(png_ptr, info_ptr);

		// Get the color format
		png_byte byte = png_get_color_type(png_ptr, info_ptr);
		switch (byte) {
		case PNG_COLOR_TYPE_RGB:
			_a = false;
			break;
		case PNG_COLOR_TYPE_RGBA:
			_a = true;
			break;
		default:
			PimAssert(false,"Image format not supported");
		}

		// Allocate the texture
		unsigned long row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		GLubyte *texture = (GLubyte*)malloc(_th * row_bytes);

		// Read the image into the texture
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

		for (unsigned int i=0; i<_th; i++) {
			memcpy(texture+(row_bytes * (_th-1-i)), row_pointers[i], row_bytes);
		}

		// Cleanup
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);

		// Default rect is the texture size. Crop at free!
		rect.width = _tw;
		rect.height = _th;

		// Create the texture
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		// The loading is complete, now let's bind some textures!
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, _a?4:3, _tw, _th, 0, _a?GL_RGBA:GL_RGB,
					 GL_UNSIGNED_BYTE, texture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		free(texture);
	}

	/*
	=====================
	Sprite::Draw
	=====================
	*/
	void Sprite::Draw() {
		glPushMatrix();

		// Update view matrix
		Vec2 fac = GameControl::GetSingleton()->GetCoordinateFactor();

		glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		glPushMatrix();

		fac = GameControl::GetSingleton()->GetWindowScale();
		glScalef(scale.x * fac.x, scale.y * fac.y, 1.f);
		glColor4f(color.r, color.g, color.b, color.a);

		glBindTexture(GL_TEXTURE_2D, texID);

		if (shader) {
			shader->SetUniform1i("texture", 0);
			glUseProgram(shader->GetProgram());
		}

		if (!hidden) {
			glBegin(GL_QUADS);

				// Bottom left
				glTexCoord2f((float)rect.x / (float)_tw, 
							 (float)rect.y / (float)_th);
				glVertex2f(-anchor.x * rect.width, -anchor.y * rect.height);

				// Bottom right
				glTexCoord2f((float)rect.x/(float)_tw + (float)rect.width/(float)_tw, 
							 (float)rect.y / (float)_th);
				glVertex2f((1.f-anchor.x) * rect.width, -anchor.y * rect.height);

				// Top right
				glTexCoord2f((float)rect.x/(float)_tw + (float)rect.width/(float)_tw, 
							 (float)rect.y/(float)_th + (float)rect.height/(float)_th);
				glVertex2f((1.f-anchor.x) * rect.width, (1.f-anchor.y) * rect.height);

				// Top left
				glTexCoord2f((float)rect.x/(float)_tw, 
							 (float)rect.y/(float)_th + (float)rect.height/(float)_th);
				glVertex2f(-anchor.x * rect.width, (1.f-anchor.y) * rect.height);

			glEnd();
		}

		glUseProgram(0);

		// Debug draw shadow shape if flagged to do so
		if (shadowShape && dbgShadowShape) {
			shadowShape->DebugDraw();
		}

		// Children are unaffected by their parent's scale. Restore.
		glPopMatrix();

		OrderChildren();

		for (unsigned int i=0; i<children.size(); i++) {
			children[i]->Draw();
		}

		// Restore this parent's view matrix
		glPopMatrix();
	}

	/*
	=====================
	Sprite::BatchDraw
	=====================
	*/
	void Sprite::BatchDraw() {
		glPushMatrix();

		// Update view matrix
		Vec2 fac = GameControl::GetSingleton()->GetCoordinateFactor();

		glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		// Push matrix before scaling
		glPushMatrix();

		fac = GameControl::GetSingleton()->GetWindowScale();
		glScalef(scale.x * fac.x, scale.y * fac.y, 1.f);
		glColor4f(color.r, color.g, color.b, color.a);

		if (shader) {
			shader->SetUniform1i("texture", 0);
			glUseProgram(shader->GetProgram());
		}

		if (!hidden) {
			glBegin(GL_QUADS);

				// Bottom left
				glTexCoord2f((float)rect.x / (float)_tw, 
							 (float)rect.y / (float)_th);
				glVertex2f(-anchor.x * rect.width, -anchor.y * rect.height);

				// Bottom right
				glTexCoord2f((float)rect.x/(float)_tw + (float)rect.width/(float)_tw, 
							 (float)rect.y / (float)_th);
				glVertex2f((1.f-anchor.x) * rect.width, -anchor.y * rect.height);

				// Top right
				glTexCoord2f((float)rect.x/(float)_tw + (float)rect.width/(float)_tw, 
							 (float)rect.y/(float)_th + (float)rect.height/(float)_th);
				glVertex2f((1.f-anchor.x) * rect.width, (1.f-anchor.y) * rect.height);

				// Top left
				glTexCoord2f((float)rect.x/(float)_tw, 
							 (float)rect.y/(float)_th + (float)rect.height/(float)_th);
				glVertex2f(-anchor.x * rect.width, (1.f-anchor.y) * rect.height);

			glEnd();
		}

		glUseProgram(0);

		// Debug draw shadow shape if flagged to do so
		if (shadowShape && dbgShadowShape) {
			shadowShape->DebugDraw();
		}

		// Children are unaffected by their parent's scale. Restore.
		glPopMatrix();

		OrderChildren();

		for (unsigned int i=0; i<children.size(); i++) {
			children[i]->BatchDraw();
		}

		// Restore this parent's view matrix
		glPopMatrix();
	}

	/*
	=====================
	Sprite::RunAction
	=====================
	*/
	void Sprite::RunAction(SpriteAction *a) {
		AddChild(a);
		a->Activate();
	}

	/*
	=====================
	Sprite::RunAction
	=====================
	*/
	void Sprite::RunAction(Action *a) {
		GameNode::RunAction(a);
	}

	/*
	=====================
	Sprite::SetShader
	=====================
	*/
	void Sprite::SetShader(Shader *s) {
		shader = s;
	}

	/*
	=====================
	Sprite::GetShader
	=====================
	*/
	Shader* Sprite::GetShader() const {
		return shader;
	}

	/*
	=====================
	Sprite::UseBatchNode
	=====================
	*/
	void Sprite::UseBatchNode(const SpriteBatchNode *batch) {
		// Assign alpha and texture dimensions
		_a    = batch->_a;
		_tw   = batch->_tw;
		_th	  = batch->_th;
		texID = batch->texID;

		_usebatch = true;
		_batchNode = batch;
	}
}
#include "PimInternal.h"
#include "PimLabel.h"
#include "PimFont.h"
#include "PimGameControl.h"
#include "PimAssert.h"

namespace Pim {
	/*
	=====================
	Label::Label
	=====================
	*/
	Label::Label(const Font *pfont) {
		PimAssert(pfont != NULL, "Error: cannot pass NULL-font!");
		fontOwner	= false;
		anchor		= Vec2(0.5f, 0.5f);
		scale		= Vec2(1.f, 1.f);
		color		= Color(1.f, 1.f, 1.f, 1.f);
		linePadding = 0;
		font		= pfont;
	}

	/*
	=====================
	Label::Label
	=====================
	*/
	Label::Label(const Font *pfont, const string ptext) {
		PimAssert(pfont != NULL, "Error: cannot pass NULL-font!");
		fontOwner	= false;
		anchor		= Vec2(0.5f, 0.5f);
		scale		= Vec2(1.f, 1.f);
		color		= Color(1.f, 1.f, 1.f, 1.f);
		linePadding = 0;
		font		= pfont;
		SetText(ptext);
	}

	/*
	=====================
	Label::~Label
	=====================
	*/
	Label::~Label() {
		if (fontOwner) {
			delete font;
		}
	}

	/*
	=====================
	Label::SetFont
	=====================
	*/
	void Label::SetFont(const Font *pfont) {
		PimAssert(pfont != NULL, "Error: cannot pass NULL-font!");
		font = pfont;
		CalculateDimensions();
	}

	/*
	=====================
	Label::SetText
	=====================
	*/
	void Label::SetText(const string ptext) {
		lines.clear();

		int lastend = 0;
		for (unsigned int i=0; i<ptext.size(); i++) {
			if (ptext[i] == '\n') {
				lines.push_back(ptext.substr(lastend,i-lastend));
				lastend = i+1;
			}
		}
		lines.push_back(ptext.substr(lastend,ptext.length()-lastend));

		CalculateDimensions();
	}

	/*
	=====================
	Label::SetTextWithFormat
	=====================
	*/
	void Label::SetTextWithFormat(const char *ptext, ...) {
		va_list args;

		va_start(args, ptext);
		SetTextWithFormat(ptext, args);
		va_end(args);
	}
	
	/*
	=====================
	Label::SetTextWithFormat
	=====================
	*/
	void Label::SetTextWithFormat(const char *ptext, va_list args) {
		lines.clear();

		char text[256];

		if (!ptext) {
			*text = NULL;
		} else {
			vsprintf(text, ptext, args);
		}

		const int len = strlen(text);
		const char *startLine = text;
		for (const char *c = text; c-text <=len; c++) {
			if (*c == '\n' || *c == '\0') {
				string line;
				for (const char *n=startLine; n<c; n++)  {
					line.append(1,*n);
				}

				lines.push_back(line);
				startLine = c+1;
			}
		}

		if (startLine && !lines.size()) {
			string line(ptext);
			lines.push_back(line);
		}

		CalculateDimensions();
	}

	/*
	=====================
	Label::SetTextAlignment
	=====================
	*/
	void Label::SetTextAlignment(const TextAlignment align) {
		if (align == TEXT_LEFT) {
			anchor = Vec2(0.f, 0.5);
		} else if (align == TEXT_CENTER) {
			anchor = Vec2(0.5f, 0.5f);
		} else if (align == TEXT_RIGHT) {
			anchor = Vec2(1.f, 0.5f);
		}
	}

	/*
	=====================
	Label::SetLinePadding
	=====================
	*/
	void Label::SetLinePadding(const int pad) {
		linePadding = pad;
		CalculateDimensions();
	}

	/*
	=====================
	Label::CalculateDimensions
	=====================
	*/
	void Label::CalculateDimensions() {
		lineWidth.clear();

		int lon = -1;
		for (unsigned j=0; j<lines.size(); j++) {
			int cur = 0;
			for (unsigned int i=0; i<lines[j].length(); i++) {
				cur += font->GetCharacterWidth(lines[j][i]);
			}

			if (cur > lon) {
				lon = cur;
			}

			lineWidth.push_back(cur);
		}

		dim.x = (float)lon;
		dim.y = (float)(font->size + (font->size + linePadding) * (lines.size()-1));
	}

	/*
	=====================
	Label::getDimensions
	=====================
	*/
	Vec2 Label::GetDimensions() const {
		return dim * scale;
	}

	/*
	=====================
	Label::Draw
	=====================
	*/
	void Label::Draw() {
		Vec2 fac = GameControl::GetSingleton()->GetCoordinateFactor();
		Vec2 pos = Vec2(position.x / fac.x, position.y / fac.y);

		glPushMatrix();

		glTranslatef(pos.x, pos.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		fac = GameControl::GetSingleton()->GetWindowScale();
		glScalef(scale.x * fac.x, scale.y * fac.y, 1.f);
		glColor4f(color.r, color.g, color.b, color.a);

		glTranslatef(0.f, dim.y/2 - font->size, 0.f);

		glListBase(font->listBase);

		// Render each line in the label individually
		for (unsigned int i=0; (unsigned)i<lines.size(); i++) {
			glPushMatrix();

			float listX = -(anchor.x) * lineWidth[i];
			float listY = float(i*(font->size+linePadding));
			listY *= -1.0f;

			glTranslatef(listX, listY, 0.f);
			glCallLists((int)lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
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
	Label::BatchDraw
	=====================
	*/
	void Label::BatchDraw() {
		Draw();
	}

	/*
	=====================
	Label::giveOwnershipOfFont
	=====================
	*/
	void Label::GiveOwnershipOfFont() {
		PimAssert(font != NULL, "Error: No font to take ownership over.");
		fontOwner = true;
	}
}
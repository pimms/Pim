#include "PimInternal.h"
#include "PimLabel.h"
#include "PimFont.h"
#include "PimGameControl.h"
#include "PimException.h"

namespace Pim
{
	Label::Label(Font *pfont)
	{
		PimAssert(pfont, "Error: cannot pass NULL-font!");

		fontOwner	= false;
		anchor		= Vec2(0.5f, 0.5f);
		scale		= Vec2(1.f, 1.f);
		color		= Color(1.f, 1.f, 1.f, 1.f);
		linePadding = 0;

		font = pfont;
	}
	Label::Label(Font *pfont, std::string ptext)
	{
		PimAssert(pfont, "Error: cannot pass NULL-font!");

		fontOwner	= false;
		anchor		= Vec2(0.5f, 0.5f);
		scale		= Vec2(1.f, 1.f);
		color		= Color(1.f, 1.f, 1.f, 1.f);
		linePadding = 0;

		font = pfont;
		setText(ptext);
	}
	Label::~Label()
	{
		if (fontOwner)
			delete font;
	}

	void Label::setFont(Font *pfont)
	{
		PimAssert(pfont, "Error: cannot pass NULL-font!");

		font = pfont;
		calculateDimensions();
	}
	void Label::setText(std::string ptext)
	{
		lines.clear();

		int lastend = 0;
		for (unsigned int i=0; i<ptext.size(); i++)
		{
			if (ptext[i] == '\n')
			{
				lines.push_back(ptext.substr(lastend,i-lastend));
				lastend = i+1;
			}
		}
		lines.push_back(ptext.substr(lastend,ptext.length()-lastend));

		calculateDimensions();
	}
	void Label::setTextWithFormat(const char *ptext, ...)
	{
		lines.clear();

		char text[256];
		va_list ap;

		if (!ptext)
		{
			*text = NULL;
		}
		else 
		{
			// Get the arguments, write to the text
			va_start(ap,ptext);
			vsprintf_s(text, ptext, ap);
			va_end(ap);
		}

		const char *startLine = text;
		for (const char *c = text; *c; c++)
		{
			if (*c == '\n' || *c == '\0')
			{
				std::string line;
				for (const char *n=startLine; n<c; n++) line.append(1,*n);
				lines.push_back(line);
				startLine = c+1;
			}
		}

		if (startLine && !lines.size())
		{
			std::string line(ptext);
			lines.push_back(line);
		}

		calculateDimensions();
	}

	void Label::setTextAlignment(TextAlignment align)
	{
		if (align == TEXT_LEFT) 
			anchor = Vec2(0.f, 0.5);
		else if (align == TEXT_CENTER)
			anchor = Vec2(0.5f, 0.5f);
		else if (align == TEXT_RIGHT)
			anchor = Vec2(1.f, 0.5f);
	}
	void Label::setLinePadding(int pad)
	{
		linePadding = pad;
		calculateDimensions();
	}

	void Label::calculateDimensions()
	{
		lineWidth.clear();

		int lon = -1;
		for each (std::string line in lines)
		{
			int cur = 0;
			for (unsigned int i=0; i<line.length(); i++)
			{
				cur += font->getCharacterWidth(line[i]);
			}
			if (cur > lon)
				lon = cur;

			lineWidth.push_back(cur);
		}

		dim.x = lon;
		dim.y = font->size + (font->size + linePadding) * (lines.size()-1);
	}
	Vec2 Label::getDimensions()
	{
		return dim * scale;
	}

	void Label::draw()
	{
		// Update view matrix
		Vec2 fac = GameControl::getSingleton()->coordinateFactor();

		Vec2 pos;
		if (allowMidPixelPosition)
			pos = Vec2(position.x / fac.x, position.y / fac.y);
		else
			pos = Vec2(floor(position.x) / fac.x, floor(position.y) / fac.y);

		// Push matrix before scaling and overlaying color
		glPushMatrix();

		glTranslatef(pos.x, pos.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		// scale and apply color
		fac = GameControl::getSingleton()->windowScale();
		glScalef(scale.x * fac.x, scale.y * fac.y, 1.f);
		glColor4f(color.r, color.g, color.b, color.a);

		glTranslatef(0.f, dim.y/2 - font->size, 0.f);

		glListBase(font->listBase);

		// Render each line in the label individually
		for (unsigned int i=0; i<lines.size(); i++)
		{
			glPushMatrix();

			int x = -(anchor.x) * lineWidth[i];
			int y = -i*(font->size+linePadding);
			glTranslatef((float)x, (float)y, 0.f);
			
			glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
			
			glPopMatrix();
		}

		orderChildren();
		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}

		glPopMatrix();
	}
	void Label::batchDraw()
	{
		// Draw normally.
		draw();
	}

	void Label::giveOwnershipOfFont()
	{
		PimAssert(font != NULL, "Error: No font to take ownership over.");
		fontOwner = true;
	}
}
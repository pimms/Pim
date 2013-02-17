#pragma once
#include "PimGameNode.h"


namespace Pim {
	class Font;

	class Label : public GameNode {
	public:
		enum TextAlignment {
			TEXT_LEFT,
			TEXT_CENTER,
			TEXT_RIGHT,
		};

		const Font						*font;
		Vec2							scale;
		Color							color;

										Label(const Font *pfont);
										Label(const Font *pfont, const string ptext);
										~Label();
		void							SetFont(const Font *pfont);
		void							SetText(const string ptext);
		void							SetTextWithFormat(const char *ptext, ...);
		void							SetTextAlignment(const TextAlignment align);
		void							SetLinePadding(const int pad); // Line distance
		void							CalculateDimensions();
		Vec2							GetDimensions() const;
		void							Draw();
		void							BatchDraw();
		void							GiveOwnershipOfFont();

	private:
		unsigned int					linePadding;
		Vec2							anchor;
		Vec2							dim;
		vector<string>					lines;
		vector<int>						lineWidth;
		bool							fontOwner;
	};
}
